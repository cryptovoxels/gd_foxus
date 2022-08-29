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

#pragma once

#include <Godot.hpp>
#include <Node.hpp>

#include <map>
#include <memory>

#include <libusb-1.0/libusb.h>
#include <libuvc/libuvc.h>

#include "camera_property.hpp"
#include "image_decoder.hpp"

namespace godot {

struct uvc_values {
    uvc_context_t* ctx;
    uvc_device_t* dev;
    uvc_device_handle_t* dev_handle;
    uvc_stream_ctrl_t ctrl;
    uvc_stream_handle_t* streamh;
    uvc_frame_t* frame;
};

enum class CAMERA_COLORSPACE {
    RGB,
    YUV
};

enum class CAMERA_CONNECTION_STATUS {
    DETACHED,
    ATTACHED,
    CONNECTED
};

class GDFoxus : public Node {
    GODOT_CLASS(GDFoxus, Node)

public:
    static void _register_methods();

    GDFoxus();
    ~GDFoxus();

    void _init();
    void _ready();
    void _process(float delta);

    void on_usb_permission_received(int fd);
    void connect_to_camera();

    void set_camera_property(String property, Variant value);
    int get_camera_property(String property);
    void reset_camera_property_to_default(String property);
    Variant get_camera_property_display_value(String property);

    int get_colorspace() {
        return (int)colorspace;
    }

    void set_colorspace(int colorspace) {
        this->colorspace = (CAMERA_COLORSPACE)colorspace;
        emit_signal("colorspace_changed", colorspace);
        create_decoder();
    }

private:
    void start_streaming();
    void initialize_camera_properties();
    void refresh_camera_properties();
    void emit_camera_property_changed_signals();

    void connect_to_camera_on_android();
    void connect_to_camera_on_host();
    void check_if_camera_is_detached();
    void check_if_camera_is_just_attached();

    void create_decoder();

    void get_frame();

    Object* eiffelcamera_singleton = nullptr;

    CAMERA_CONNECTION_STATUS camera_connection_status = CAMERA_CONNECTION_STATUS::DETACHED;

    uvc_values uvc_data;

    const int stream_width = 2560;
    const int stream_height = 960;
    const int stream_fps = 60;

    const uint32_t vendor_id = 0x32e4;
    const uint32_t product_id = 0x9750;

    std::map<String, std::unique_ptr<CameraProperty>> camera_properties;
    std::unique_ptr<ImageDecoder> image_decoder;

    CAMERA_COLORSPACE colorspace = CAMERA_COLORSPACE::YUV;

    bool waiting_for_permission = false;
};
}