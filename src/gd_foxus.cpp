/*************************************************************************/
/* Copyright (c) 2022 Nolan Consulting Limited.                          */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "gd_foxus.hpp"

#include <libusb-1.0/libusb.h>

#include <Engine.hpp>

#include <string>
#include <stdexcept>

#ifdef __ANDROID__
const bool is_android = true;
#else
const bool is_android = false;
#endif

using namespace godot;

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
    Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
    Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
    Godot::nativescript_init(handle);
    register_class<godot::GDFoxus>();
}

void GDFoxus::_register_methods() {
    register_property<GDFoxus, int>("colorspace",
            &GDFoxus::set_colorspace,
            &GDFoxus::get_colorspace,
            0,
            GODOT_METHOD_RPC_MODE_DISABLED,
            GODOT_PROPERTY_USAGE_DEFAULT,
            GODOT_PROPERTY_HINT_ENUM,
            "RGB,YUV");

    register_method("_init", &GDFoxus::_ready);
    register_method("_ready", &GDFoxus::_ready);
    register_method("_process", &GDFoxus::_process);

    register_method("on_usb_permission_received", &GDFoxus::on_usb_permission_received);

    register_method("set_camera_property", &GDFoxus::set_camera_property);
    register_method("get_camera_property", &GDFoxus::get_camera_property);
    register_method("get_camera_property_display_value", &GDFoxus::get_camera_property_display_value);
    register_method("reset_camera_property_to_default", &GDFoxus::reset_camera_property_to_default);

    register_signal<GDFoxus>("frame_ready", "image", GODOT_VARIANT_TYPE_OBJECT);

    register_signal<GDFoxus>("camera_property_changed",
            "property_name", GODOT_VARIANT_TYPE_STRING,
            "current_value", GODOT_VARIANT_TYPE_INT,
            "minimum_value", GODOT_VARIANT_TYPE_INT,
            "maximum_value", GODOT_VARIANT_TYPE_INT);

    register_signal<GDFoxus>("camera_status_changed", "camera_status", GODOT_VARIANT_TYPE_INT);
    register_signal<GDFoxus>("colorspace_changed", "colorspace", GODOT_VARIANT_TYPE_INT);

}

GDFoxus::GDFoxus() {

}

GDFoxus::~GDFoxus() {

}

void GDFoxus::_init() {
    create_decoder();
}

void GDFoxus::_ready() {
    if (Engine::get_singleton()->has_singleton("EiffelCamera")) {
        eiffelcamera_singleton = Engine::get_singleton()->get_singleton("EiffelCamera");
        eiffelcamera_singleton->connect("usb_permission_received", this, "on_usb_permission_received");
    }

    if (!is_android) {
        camera_connection_status = CAMERA_CONNECTION_STATUS::ATTACHED;
        emit_signal("camera_status_changed", (int)camera_connection_status);

        connect_to_camera();
    }
}

void GDFoxus::_process(float delta) {

    check_if_camera_is_just_attached();

    if (camera_connection_status == CAMERA_CONNECTION_STATUS::ATTACHED && !waiting_for_permission) {
        connect_to_camera();
    }

    if (camera_connection_status != CAMERA_CONNECTION_STATUS::CONNECTED) {
        Godot::print("ERROR: Camera is not connected");
        return;
    }

    try {
        get_frame();

        image_decoder->decode_image((unsigned char*) uvc_data.frame->data,
                                    uvc_data.frame->data_bytes);

        emit_signal("frame_ready", image_decoder->get_image_data());

    } catch (std::runtime_error& error) {
        Godot::print(String("ERROR: ") + error.what());
    }
}

void GDFoxus::check_if_camera_is_just_attached() {
    if (is_android && camera_connection_status == CAMERA_CONNECTION_STATUS::DETACHED) {
        bool is_camera_just_attached = eiffelcamera_singleton->call("isCameraAttached", vendor_id, product_id);

        if (is_camera_just_attached) {
            camera_connection_status = CAMERA_CONNECTION_STATUS::ATTACHED;
            emit_signal("camera_status_changed", (int)camera_connection_status);
        }
    }
}

void GDFoxus::check_if_camera_is_detached() {
    if (is_android) {
        bool is_camera_attached = eiffelcamera_singleton->call("isCameraAttached", vendor_id, product_id);
        if (!is_camera_attached) {
            camera_connection_status = CAMERA_CONNECTION_STATUS::DETACHED;
            emit_signal("camera_status_changed", (int)camera_connection_status);
        }
    }
}

void GDFoxus::get_frame() {
    uvc_error_t uvc_ret = uvc_stream_get_frame(uvc_data.streamh, &uvc_data.frame, -1);

    if (uvc_ret != UVC_SUCCESS) {
        check_if_camera_is_detached();
        throw std::runtime_error(std::string("Unable to get frame (") + uvc_strerror(uvc_ret) + ")");
    }

    if (uvc_data.frame == nullptr) {
        check_if_camera_is_detached();
        throw std::runtime_error(std::string("Unable to get frame"));
    }
}

String itos(int n) {
    return String(std::to_string(n).c_str());
}

void GDFoxus::on_usb_permission_received(int fd) {

    waiting_for_permission = false;

    if (fd < 0) {
        Godot::print("ERROR: Invalid file descriptor received with usb permission. FD: " + itos(fd));
        return;
    }

    if (camera_connection_status == CAMERA_CONNECTION_STATUS::CONNECTED) {
        Godot::print("ERROR: Camera already running");
        return;
    }

    int libusb_res = libusb_set_option(NULL, LIBUSB_OPTION_NO_DEVICE_DISCOVERY, NULL);
    if (libusb_res != LIBUSB_SUCCESS) {
        Godot::print(String("ERROR: Failed to set usb option (") + String(libusb_strerror(libusb_res)) + ")");
        return;
    }

    uvc_error_t uvc_res = uvc_init(&uvc_data.ctx, NULL);
    if (uvc_res != UVC_SUCCESS) {
        Godot::print(String("ERROR: Failed to initialise UVC (") + String(uvc_strerror(uvc_res)) + ")");
        return;
    }

    uvc_res = uvc_wrap(fd, uvc_data.ctx, &uvc_data.dev_handle);
    if (uvc_res != UVC_SUCCESS) {
        Godot::print(String("ERROR: cannot find Eiffel Camera device (") + String(uvc_strerror(uvc_res)) + ")");
        return;
    } else {
        Godot::print("INFO: Eiffel Camera Device found");
        uvc_data.dev = uvc_get_device(uvc_data.dev_handle);
    }

    start_streaming();
    initialize_camera_properties();
}

void GDFoxus::initialize_camera_properties() {
    Godot::print("INFO: Setting ae mode to aperture priority");
    uvc_set_ae_mode(uvc_data.dev_handle, 0); // turn off auto exposure

    Godot::print("INFO: Try to approximate linear color space");
    uvc_set_white_balance_temperature_auto(uvc_data.dev_handle, 0);
    uvc_set_white_balance_temperature(uvc_data.dev_handle, 32000);
}

void GDFoxus::start_streaming() {

    uvc_error_t uvc_res = uvc_get_stream_ctrl_format_size(uvc_data.dev_handle, &uvc_data.ctrl,
                                    UVC_FRAME_FORMAT_COMPRESSED, stream_width, stream_height, stream_fps);

    if (uvc_res != UVC_SUCCESS) {
        Godot::print("ERROR: Cannot get requested stream size (" + String(uvc_strerror(uvc_res)) + ")");
        return;
    }

    uvc_print_stream_ctrl(&uvc_data.ctrl, stderr);

    uvc_res = uvc_stream_open_ctrl(uvc_data.dev_handle, &uvc_data.streamh, &uvc_data.ctrl);
    if (uvc_res != UVC_SUCCESS) {
        Godot::print(String("ERROR: Unable to open stream ctrl (") + String(uvc_strerror(uvc_res)) + ")");
        return;
    }

    uvc_res = uvc_stream_start(uvc_data.streamh, NULL, NULL, 0);
    if (uvc_res != UVC_SUCCESS) {
        Godot::print(String("ERROR: Unable to start streaming (") + String(uvc_strerror(uvc_res)) + ")");
        return;
    }

    camera_connection_status = CAMERA_CONNECTION_STATUS::CONNECTED;
    emit_signal("camera_status_changed", (int)camera_connection_status);

    refresh_camera_properties();
}

void GDFoxus::refresh_camera_properties() {
    camera_properties["brightness"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<int16_t>(
        "brightness", uvc_data.dev_handle, &uvc_get_brightness, &uvc_set_brightness));

    camera_properties["contrast"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "contrast", uvc_data.dev_handle, &uvc_get_contrast, &uvc_set_contrast));

    camera_properties["saturation"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "saturation", uvc_data.dev_handle, &uvc_get_saturation, &uvc_set_saturation));

    camera_properties["hue"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<int16_t>(
        "hue", uvc_data.dev_handle, &uvc_get_hue, &uvc_set_hue));

    camera_properties["white_balance_temperature_auto"] = std::unique_ptr<CameraProperty>(new BooleanCameraProperty(
        "white_balance_temperature_auto", uvc_data.dev_handle, &uvc_get_white_balance_temperature_auto, &uvc_set_white_balance_temperature_auto));

    camera_properties["gamma"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "gamma", uvc_data.dev_handle, &uvc_get_gamma, &uvc_set_gamma));

    camera_properties["gain"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "gain", uvc_data.dev_handle, &uvc_get_gain, &uvc_set_gain));

    camera_properties["white_balance_temperature"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "white_balance_temperature", uvc_data.dev_handle, &uvc_get_white_balance_temperature, &uvc_set_saturation));

    camera_properties["sharpness"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "sharpness", uvc_data.dev_handle, &uvc_get_sharpness, &uvc_set_sharpness));

    camera_properties["backlight_compensation"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "backlight_compensation", uvc_data.dev_handle, &uvc_get_backlight_compensation, &uvc_set_backlight_compensation));

    camera_properties["exposure_abs"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint32_t>(
        "exposure_abs", uvc_data.dev_handle, &uvc_get_exposure_abs, &uvc_set_exposure_abs));

    camera_properties["focus_abs"] = std::unique_ptr<CameraProperty>(new RangeCameraProperty<uint16_t>(
        "focus_abs", uvc_data.dev_handle, &uvc_get_focus_abs, &uvc_set_focus_abs));

    camera_properties["focus_auto"] = std::unique_ptr<CameraProperty>(new BooleanCameraProperty(
        "focus_auto", uvc_data.dev_handle, &uvc_get_focus_auto, &uvc_set_focus_auto));

    Array power_line_frequency_values, power_line_frequency_display_values;

    power_line_frequency_values.append(0);
    power_line_frequency_values.append(1);
    power_line_frequency_values.append(2);

    power_line_frequency_display_values.append("Disabled");
    power_line_frequency_display_values.append("50 Hz");
    power_line_frequency_display_values.append("60 Hz");

    camera_properties["power_line_frequency"] = std::unique_ptr<CameraProperty>(new EnumCameraProperty(
        "power_line_frequency", uvc_data.dev_handle, &uvc_get_power_line_frequency, &uvc_set_power_line_frequency, power_line_frequency_values, power_line_frequency_display_values));


    Array ae_mode_values, ae_mode_display_values;

    ae_mode_values.append(1);
    ae_mode_values.append(8);

    ae_mode_display_values.append("Manual Mode");
    ae_mode_display_values.append("Aperture Priority Mode");

    camera_properties["ae_mode"] = std::unique_ptr<CameraProperty>(new EnumCameraProperty(
        "ae_mode", uvc_data.dev_handle, &uvc_get_ae_mode, &uvc_set_ae_mode, ae_mode_values, ae_mode_display_values));

    emit_camera_property_changed_signals();
}

void GDFoxus::emit_camera_property_changed_signals() {
    for (const auto& entry : camera_properties) {
        emit_signal("camera_property_changed",
            entry.second->get_name(),
            entry.second->get_value(),
            entry.second->get_minimum(),
            entry.second->get_maximum()
        );
    }
}

void GDFoxus::connect_to_camera() {
    if (is_android) {
        waiting_for_permission = true;
        connect_to_camera_on_android();
    } else {
        connect_to_camera_on_host();
    }
}

void GDFoxus::connect_to_camera_on_android() {
    // If we are running on Android, we must call our Android Plugin that makes
    // the necessary Java API calls to open the device and retrieve the
    // file descriptor that we can wrap with libuvc

    Godot::print("INFO: Trying to connect camera on Android");

    int libusb_FD = -1;

    Godot::print("INFO: Calling connectCamera (vid=" + itos(vendor_id) + ", pid=" + itos(product_id) + ")");
    bool permission_requested = eiffelcamera_singleton->call("connectCamera", vendor_id, product_id);

    if (permission_requested) {
        Godot::print("INFO: Successfully requested usb permission");
    } else {
        Godot::print("ERROR: Failed to request usb permissions");
    }
}

void GDFoxus::connect_to_camera_on_host() {
    //We are running on OSX
    Godot::print("INFO: Running on host");

    uvc_error_t uvc_res = uvc_init(&uvc_data.ctx, NULL);

    if (uvc_res < 0) {
        Godot::print(String("ERROR: uvc_init error! (") + uvc_strerror(uvc_res) + ")");
        return;
    }

    uvc_res = uvc_find_device(uvc_data.ctx, &uvc_data.dev, vendor_id, product_id, NULL); // filter devices: vendor_id, product_id, "serial_num"
    if (uvc_res < 0) {
        Godot::print(String("ERROR: cannot find Eiffel Camera device (") + uvc_strerror(uvc_res) + ")");
        return;
    }

    Godot::print("INFO: Eiffel Camera Device found");
    uvc_res = uvc_open(uvc_data.dev, &uvc_data.dev_handle);

    if (uvc_res < 0) {
        Godot::print(String("ERROR: Cannot open UVC device (") + uvc_strerror(uvc_res) + ")");
        return;
    }

    Godot::print("INFO: Eiffel Camera Device opened\n");

    uvc_print_diag(uvc_data.dev_handle, stderr);

    start_streaming();
}

void GDFoxus::create_decoder() {
    if (colorspace == CAMERA_COLORSPACE::YUV) {
        image_decoder = std::unique_ptr<ImageDecoder>(new ImageDecoderYUV(stream_width, stream_height * 2, 1));
    } else if (colorspace == CAMERA_COLORSPACE::RGB) {
        image_decoder = std::unique_ptr<ImageDecoder>(new ImageDecoderRGB(stream_width, stream_height, 3));
    }
}

void GDFoxus::set_camera_property(String property, Variant value) {
    if (camera_properties.count(property)) {
        camera_properties[property]->set_value(value);
    }
}

int GDFoxus::get_camera_property(String property) {
    if (camera_properties.count(property)) {
        return camera_properties[property]->get_value();
    }

    return 0;
}

Variant GDFoxus::get_camera_property_display_value(String property) {
    if (camera_properties.count(property)) {
        return camera_properties[property]->get_display_value();
    }

    return Variant();
}

void GDFoxus::reset_camera_property_to_default(String property) {
    if (camera_properties.count(property)) {
        camera_properties[property]->set_value(camera_properties[property]->get_default());
    }
}