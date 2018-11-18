extends Node

signal connected
signal disconnected
signal data_received

var bluetooth
var strips = []
var matrixes = []
func _ready():
	if(Engine.has_singleton("GodotBluetooth")):
		bluetooth = Engine.get_singleton("GodotBluetooth")
		bluetooth.init(get_instance_id(), true)
	pass

#GodotBluetooth Callbacks
func _on_connected(device_name, device_adress):
	emit_signal("connected")
	pass

func _on_disconnected():
	emit_signal("disconnected")
	pass

func _on_data_received(data_received):
	print("DATA:"+str(data_received))
	emit_signal("data_received", data_received)
	pass
func getStrips():
	return strips
func getMatrix():
	return matrixes
func addStrip(val):
	strips.push_back(val)
func remStrip(val):
	strips.remove(val)
func addMatrix(val):
	matrixes.push_back(val)
func remMatrix(val):
	matrixes.remove(val)