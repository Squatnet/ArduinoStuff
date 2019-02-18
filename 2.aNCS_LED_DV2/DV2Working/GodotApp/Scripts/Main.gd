extends Control
var TheDict = {}
var TypeNums = []
func _ready():
	set_signals()
	
func set_signals():
	GS.connect("connected", self, "_on_connected")
	GS.connect("disconnected", self, "_on_disconnected")
	GS.connect("data_received", self, "_on_data_received")

func _on_connected():
	print("MAIN - Connected")
	$Connect.set_text("Disconnect")
	GS.BT.sendData("App,Init")

func _on_disconnected():
	$Connect.set_text("Connect")

func _on_data_received(data_received):
	print("MAIN - "+data_received)
	if data_received.begins_with("Nums"):
		data_received.erase(0,data_received.find(",")+1)
		print(data_received)
		TypeNums = Array(data_received)
		for i in TypeNums:
			print(i)

func _on_Connect_pressed():
	if GS.BT:
		GS.BT.getPairedDevices(true)
	else:
		print("Module not initialized!")