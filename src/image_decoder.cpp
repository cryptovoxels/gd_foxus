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

#include "image_decoder.hpp"

#include <stdexcept>
#include <string>

using namespace godot;

ImageDecoder::ImageDecoder(int width, int height, int channels) {
    this->width = width;
    this->height = height;

    image_data.resize(width * height * channels);

    tj_handle = tjInitDecompress();
}

ImageDecoder::~ImageDecoder() {
    tjDestroy(tj_handle);
}

PoolByteArray ImageDecoder::get_image_data() {
    return image_data;
}

String ImageDecoder::get_error() {
    return String(tjGetErrorStr2(tj_handle));
}

ImageDecoderYUV::~ImageDecoderYUV() {

}

void ImageDecoderYUV::decode_image(unsigned char* frame_data, size_t frame_size) {

    PoolByteArray::Write image_data_write = image_data.write();

    int result = tjDecompressToYUV(tj_handle, frame_data, frame_size, image_data_write.ptr(), TJFLAG_FASTDCT | TJFLAG_NOREALLOC);

    if (result != 0) {
//        throw std::runtime_error(std::string("Failed to decode image: ") + tjGetErrorStr2(tj_handle));
    }
}

ImageDecoderRGB::~ImageDecoderRGB() {

}

void ImageDecoderRGB::decode_image(unsigned char* frame_data, size_t frame_size) {

    PoolByteArray::Write image_data_write = image_data.write();

    int result = tjDecompress2(tj_handle, frame_data, frame_size, image_data_write.ptr(), width, 0, height, TJPF_RGB, 0);

    if (result != 0) {
        throw std::runtime_error(std::string("Failed to decode image ") + tjGetErrorStr2(tj_handle));
    }

}