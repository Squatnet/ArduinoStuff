extends Control

onready var btn_connect = get_node("ConnBtn")
var StripScene = load("res://Scenes/LEDString.tscn")
var bt
var knownDevices = {
					"Strips":{},
					"Matrix":{}
					}

var activeScenes = []
var recievedEvent = []
var terminalLog = []
func _ready():	
	set_signals()
	if BTHandler.bluetooth:
		bt = true
	
#GodotBluetooth Methods
func _on_connect_pressed():
	if bt:
		BTHandler.bluetooth.getPairedDevices(true)
		OS.delay_msec(100)
		BTHandler.bluetooth.sendData("POLL")
		OS.delay_msec(100)
	else:
		OS.alert("Module not initialized!")
	pass

#GodotBluetooth Callbacks
func _on_connected():
	btn_connect.set_text("Disconnect")
	$Strip.show()
	$Term.show()
	poll()
	pass

func _on_disconnected():
	btn_connect.set_text("Connect")
	$LEDString.hide()
	$Term.hide()
	$Strip.hide()
	$Terminal.hide()
	pass

func _on_data_received(data_received):
	print("INCOMING : "+str(data_received))
	recievedEvent = data_received.split(",",false)
	recievedEvent = Array(recievedEvent)
	var type = recievedEvent.pop_front()
	if type == "STR":
		var id = recievedEvent.pop_front()
		var new
		if knownDevices.Strips.has(id):
			new = false
		else:
			new = true
		knownDevices.Strips[id] = {"numStrips":recievedEvent.pop_front(),
										"color":Color(recievedEvent.pop_front(),recievedEvent.pop_front(),recievedEvent.pop_front()),
										"pattern":recievedEvent.pop_front(),
										"auto":recievedEvent.pop_front(),
										"timeout":recievedEvent.pop_front()
										}
		if new:
			var t = StripScene.instance()
			t.rect_position = Vector2(30,280)
			t.rect_size = Vector2(1020,1400)
			t.setup(id,knownDevices.Strips[id])
			t.set_name("STRIP"+str(id))
			t.hide()
			var b = Button.new()
			b.text = t.get_name()
			add_child(t)
		else:
			get_node("Strip"+str(id)).refresh(knownDevices.Strips[id])
		addToLog(data_received)

#Godot Signals
func set_signals():
	BTHandler.connect("connected", self, "_on_connected")
	BTHandler.connect("disconnected", self, "_on_disconnected")
	BTHandler.connect("data_received", self, "_on_data_received")
	btn_connect.connect("pressed", self, "_on_connect_pressed")
	
func addToLog(data):
	terminalLog.push_back(data)
func readLog():
	return terminalLog

func poll():
	BTHandler.bluetooth.sendData("POLL")



func _on_Strip_pressed():
	pass