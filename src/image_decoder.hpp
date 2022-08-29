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

#include <turbojpeg.h>

#include <jpeglib.h>
#include <jerror.h>

namespace godot {

class ImageDecoder {

public:

    ImageDecoder(int width, int height, int channels);

    virtual ~ImageDecoder() = 0;

    virtual void decode_image(unsigned char* frame_data, size_t frame_size) = 0;

    PoolByteArray get_image_data();

    String get_error();

protected:
    PoolByteArray image_data;
    tjhandle tj_handle;

    int width;
    int height;
};

class ImageDecoderYUV : public ImageDecoder {

public:
    ~ImageDecoderYUV();

    ImageDecoderYUV(int width, int height, int channels) : ImageDecoder(width, height, channels) {}

    void decode_image(unsigned char* frame_data, size_t frame_size);

};

class ImageDecoderRGB : public ImageDecoder {

public:
    ~ImageDecoderRGB();

    ImageDecoderRGB(int width, int height, int channels) : ImageDecoder(width, height, channels) {}

    void decode_image(unsigned char* frame_data, size_t frame_size);
};

}