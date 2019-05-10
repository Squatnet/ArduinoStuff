extends Node

signal connected
signal disconnected
signal data_received

var BT

func _ready():
	if(Engine.has_singleton("GodotBluetooth")):
		BT = Engine.get_singleton("GodotBluetooth")
		BT.init(get_instance_id(), true)
	pass

#GodotBluetooth Callbacks
func _on_connected(device_name, device_adress):
	print("GS - Connected")
	emit_signal("connected")
	pass

func _on_disconnected():
	print("GS - Disconnected")
	emit_signal("disconnected")
	pass

func _on_data_received(data_received):
	print("GS - Got data "+data_received)
	emit_signal("data_received", data_received)
	pass