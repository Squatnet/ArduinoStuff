extends Control

onready var btn_connect = get_node("ConnBtn")
var StripScene = load("res://Scenes/LEDString.tscn")
var bt
var matrixes = []
var strips = []
func _ready():
	set_signals()
	addDevice("Setting","s")
	if BTHandler.bluetooth:
		bt = true
	
#GodotBluetooth Methods
func _on_connect_pressed():
	if bt:
		BTHandler.bluetooth.getPairedDevices(true)
		OS.delay_msec(100)
	else:
		OS.alert("Module not initialized!")
	pass

#GodotBluetooth Callbacks
func _on_connected():
	btn_connect.set_text("Disconnect")
	$ScrollContainer.show()
	addDevice("Setting","s")
	addDevice("LED","A")
	addDevice("LED","B")
	addDevice("MAT","A")

func _on_disconnected():
	btn_connect.set_text("Connect")
	$LEDString.hide()
	$Term.hide()
	$Strip.hide()
	$Terminal.hide()
	pass

func set_signals():
	BTHandler.connect("connected", self, "_on_connected")
	BTHandler.connect("disconnected", self, "_on_disconnected")
	BTHandler.connect("data_received", self, "_on_data_received")
	btn_connect.connect("pressed", self, "_on_connect_pressed")
	
func addDevice(type,id):
	$ScrollContainer.addBtn(type+str(id))
	$PageWrapper.addPage(type+str(id))