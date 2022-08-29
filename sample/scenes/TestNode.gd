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

func _ready():
	check_eiffelcamera()

	if (OS.get_name() == "Android"):
		var ovr_init_config = preload("res://addons/godot_ovrmobile/OvrInitConfig.gdns").new()
		var ovr_performance = preload("res://addons/godot_ovrmobile/OvrPerformance.gdns").new()

		var interface = ARVRServer.find_interface("OVRMobile")
		if interface:
			ovr_init_config.set_render_target_size_multiplier(1)
			if interface.initialize():
				get_viewport().arvr = true

func check_eiffelcamera():
	var singleton = null
	if Engine.has_singleton("EiffelCamera"):
		singleton = Engine.get_singleton("EiffelCamera")
  
	if singleton == null:
		print("EiffelCamera singleton not found")
	else:
		print("Found EiffelCamera singleton")
