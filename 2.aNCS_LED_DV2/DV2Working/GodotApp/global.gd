extends Node

signal connected
signal disconnected
signal data_received
signal reg_finished

var BT

var devNums = [0,0,0,0]
var knownDevs = {"Str":{},
				"Mat":{},
				"Ter":{},
				"Rtr":{}
				}
var registerStep = 0
var regStepsArray = []
func addDvc(typ,dev):
	var sPos = dev[0]
	var sName = dev[1]
	var sId = dev[2]
	print(knownDevs[typ])
	knownDevs[typ][str(sPos)] = [sName,sId]
func doRegStep():
	if regStepsArray.size() == 0:
		print("GS: doRegStep: regStepsArray Empty!!")
		registerStep += 1
	if registerStep == 1:
		var t = regStepsArray.pop_front()
		print("GS: doRegStep: Sending "+str(t))
		BT.sendData(str(t))
	if registerStep == 2:
		print("GS: doRegStep: Register Queue Finished")
		print("GS: knownDevices: "+str(knownDevs))
		emit_signal("reg_finished")
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
	knownDevs.clear()
	knownDevs = {"Str":{},"Mat":{},"Ter":{},"Rtr":{}}
	regStepsArray.clear()
	registerStep = 0
	pass

func _on_data_received(data_received):
	print("GS - Got data "+str(data_received))
	emit_signal("data_received", data_received)
	pass