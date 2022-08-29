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

extends VBoxContainer

var gd_foxus : Node

var name_label : Label
var display_value_label : Label

var slider : HSlider

func _enter_tree():
	gd_foxus = get_node("/root/Scene/GDFoxus")
	slider = get_node("HBox/HSlider")
	display_value_label = get_node("Labels/DisplayValue")
	name_label = get_node("Labels/Name")

func update_slider(name, value, minimum, maximum):
	name_label.text = name
	
	slider.min_value = minimum
	slider.max_value = maximum
	slider.value = value

	display_value_label.text = str(gd_foxus.get_camera_property_display_value(name))


func on_slider_value_changed(value):
	gd_foxus.set_camera_property(name_label.text, value)
	display_value_label.text = str(gd_foxus.get_camera_property_display_value(name))

func on_reset_pressed():
	gd_foxus.reset_camera_property_to_default(name_label.text)
	display_value_label.text = str(gd_foxus.get_camera_property_display_value(name))
	slider.value = gd_foxus.get_camera_property(name)
