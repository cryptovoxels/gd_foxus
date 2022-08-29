#
# Copyright (c) 2022 Nolan Consulting Limited.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
#  the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

extends Node

const RGB = 0
const YUV = 1

const WIDTH = 2560
const HEIGHT = 960

var gd_foxus : Node = null
var mesh_instance : MeshInstance = null

func _enter_tree():
	gd_foxus = get_node("/root/Scene/GDFoxus")
	mesh_instance = get_node("ARVROrigin/ARVRCamera/MeshInstance")

	gd_foxus.connect("frame_ready", self, "on_frame_ready")
	gd_foxus.connect("colorspace_changed", self, "on_colorspace_changed")
	
	mesh_instance.get_active_material(0).set_shader_param("colorspace", gd_foxus.colorspace)
	
func on_colorspace_changed(colorspace):
	mesh_instance.get_active_material(0).set_shader_param("colorspace", colorspace)
		
func on_frame_ready(image_data):
	var image = Image.new()
	var image_texture = ImageTexture.new()
	
	if gd_foxus.colorspace == YUV:
		image.create_from_data(WIDTH, HEIGHT * 2, false, Image.FORMAT_L8, image_data)
	elif gd_foxus.colorspace == RGB:
		image.create_from_data(WIDTH, HEIGHT, false, Image.FORMAT_RGB8, image_data)
	else:
		return

	image_texture.create_from_image(image)
	mesh_instance.get_active_material(0).set_shader_param("image", image_texture)
