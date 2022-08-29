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

const slider_scene = preload("res://scenes/Slider.tscn")

var gd_foxus : Node
var sliders : Dictionary

func _enter_tree():
	gd_foxus = get_node("/root/Scene/GDFoxus")
	gd_foxus.connect("camera_property_changed", self, "on_camera_property_changed")

func on_camera_property_changed(name, value, minimum, maximum):
	
	if sliders.has(name):
		sliders[name].update_slider(name, value, minimum, maximum)
	else:
		add_new_slider(name, value, minimum, maximum)

func add_new_slider(name, value, minimum, maximum):
	var instance = slider_scene.instance()

	instance.name = name
	add_child(instance)
	
	sliders[name] = instance

	instance.update_slider(name, value, minimum, maximum)
