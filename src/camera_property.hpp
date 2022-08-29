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
#include <Object.hpp>

#include <libuvc/libuvc.h>

namespace godot {

class CameraProperty {

public:
    virtual ~CameraProperty() = 0;

    virtual String get_name();

    virtual uvc_error_t set_value(int) = 0;
    virtual int get_value() = 0;

    virtual Variant get_display_value() = 0;

    virtual int get_maximum();
    virtual int get_minimum();
    virtual int get_default();

protected:
    String name;
    uvc_device_handle_t* device_handle;

    int maximum_value;
    int minimum_value;
    int default_value;
};

template<typename data_type>
class RangeCameraProperty : public CameraProperty {

public:
    RangeCameraProperty(String name,
                            uvc_device_handle_t* device_handle,
                            uvc_error_t (*getter)(uvc_device_handle_t*, data_type*, uvc_req_code),
                            uvc_error_t (*setter)(uvc_device_handle_t*, data_type)) {
        this->name = name;
        this->device_handle = device_handle;
        this->getter = getter;
        this->setter = setter;

        this->default_value = request_uvc_control(uvc_req_code::UVC_GET_DEF);
        this->maximum_value = request_uvc_control(uvc_req_code::UVC_GET_MAX);
        this->minimum_value = request_uvc_control(uvc_req_code::UVC_GET_MIN);
    }

    ~RangeCameraProperty() {

    }

    uvc_error_t set_value(int value) {
        uvc_error_t res = setter(device_handle, static_cast<data_type>(value));
        return res;
    }

    int get_value() {
        return request_uvc_control(uvc_req_code::UVC_GET_CUR);
    }

    Variant get_display_value() {
        Variant display_value = get_value();
        return display_value;
    }

private:
    uvc_error_t (*getter)(uvc_device_handle_t*, data_type*, uvc_req_code);
    uvc_error_t (*setter)(uvc_device_handle_t*, data_type);

    int request_uvc_control(uvc_req_code req_code) {
        data_type control_value;
        getter(device_handle, &control_value, req_code);

        return static_cast<int>(control_value);
    }
};

class BooleanCameraProperty : public CameraProperty {

public:
    BooleanCameraProperty(String name,
                            uvc_device_handle_t* device_handle,
                            uvc_error_t (*getter)(uvc_device_handle_t*, uint8_t*, uvc_req_code),
                            uvc_error_t (*setter)(uvc_device_handle_t*, uint8_t));
    ~BooleanCameraProperty();

    uvc_error_t set_value(int);
    int get_value();

    Variant get_display_value();

private:
    uvc_error_t (*getter)(uvc_device_handle_t*, uint8_t*, uvc_req_code);
    uvc_error_t (*setter)(uvc_device_handle_t*, uint8_t);
};

class EnumCameraProperty : public CameraProperty {

public:
    EnumCameraProperty(String name,
                            uvc_device_handle_t* device_handle,
                            uvc_error_t (*getter)(uvc_device_handle_t*, uint8_t*, uvc_req_code),
                            uvc_error_t (*setter)(uvc_device_handle_t*, uint8_t),
                            Array values,
                            Array display_values);
    ~EnumCameraProperty();

    uvc_error_t set_value(int);
    int get_value();

    Variant get_display_value();

private:
    uvc_error_t (*getter)(uvc_device_handle_t*, uint8_t*, uvc_req_code);
    uvc_error_t (*setter)(uvc_device_handle_t*, uint8_t);

    Array values;
    Array display_values;
};
}