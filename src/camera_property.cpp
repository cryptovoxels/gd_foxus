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

#include "camera_property.hpp"

using namespace godot;

CameraProperty::~CameraProperty() {

}

String CameraProperty::get_name() {
    return name;
}

int CameraProperty::get_maximum() {
    return maximum_value;
}

int CameraProperty::get_minimum() {
    return minimum_value;
}

int CameraProperty::get_default() {
    return default_value;
}

BooleanCameraProperty::BooleanCameraProperty(String name,
                            uvc_device_handle_t* device_handle,
                            uvc_error_t (*getter)(uvc_device_handle_t*, uint8_t*, uvc_req_code),
                            uvc_error_t (*setter)(uvc_device_handle_t*, uint8_t)) {

    this->name = name;
    this->device_handle = device_handle;
    this->getter = getter;
    this->setter = setter;

    this->minimum_value = 0;
    this->maximum_value = 1;

    uint8_t default_value;
    getter(device_handle, &default_value, uvc_req_code::UVC_GET_DEF);

    this->default_value = static_cast<int>(default_value);
}

BooleanCameraProperty::~BooleanCameraProperty() {

}

uvc_error_t BooleanCameraProperty::set_value(int value) {
    uvc_error_t uvc_res = setter(device_handle, static_cast<uint8_t>(value));
    return uvc_res;
}

int BooleanCameraProperty::get_value() {
    uint8_t result;
    getter(device_handle, &result, uvc_req_code::UVC_GET_CUR);

    return static_cast<int>(result);
}

Variant BooleanCameraProperty::get_display_value() {
    Variant display_value = get_value();
    return display_value;
}

EnumCameraProperty::EnumCameraProperty(String name,
                            uvc_device_handle_t* device_handle,
                            uvc_error_t (*getter)(uvc_device_handle_t*, uint8_t*, uvc_req_code),
                            uvc_error_t (*setter)(uvc_device_handle_t*, uint8_t),
                            Array values,
                            Array display_values) {

    this->name = name;
    this->device_handle = device_handle;
    this->getter = getter;
    this->setter = setter;
    this->values = values;
    this->display_values = display_values;

    this->minimum_value = 0;
    this->maximum_value = values.size() - 1;

    uint8_t default_value;
    getter(device_handle, &default_value, uvc_req_code::UVC_GET_DEF);

    int converted_default_value = static_cast<int>(default_value);
    int default_value_index = values.find(converted_default_value);

    this->default_value = default_value_index;
}

EnumCameraProperty::~EnumCameraProperty() {

}

uvc_error_t EnumCameraProperty::set_value(int index) {
    int value = values[index];

    uvc_error_t uvc_res = setter(device_handle, static_cast<uint8_t>(value));
    return uvc_res;
}

int EnumCameraProperty::get_value() {
    uint8_t control_value;
    getter(device_handle, &control_value, uvc_req_code::UVC_GET_CUR);

    int value_index = static_cast<int>(control_value);

    return values.find(value_index);
}

Variant EnumCameraProperty::get_display_value() {
    Variant display_value_index = get_value();
    return display_values[display_value_index];
}