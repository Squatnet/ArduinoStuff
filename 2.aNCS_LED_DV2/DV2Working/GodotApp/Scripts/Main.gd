extends Control
var TheDict = {}
var TypeNums = []
func _ready():
	set_signals()
	
func set_signals():
	GS.connect("connected", self, "_on_connected")
	GS.connect("disconnected", self, "_on_disconnected")
	GS.connect("data_received", self, "_on_data_received")
	GS.connect("reg_finished", self, "_on_reg_finished")

func _on_connected():
	print("MAIN - Connected")
	$Connect.set_text("Disconnect")
	GS.BT.sendData("App,Init")

func _on_disconnected():
	$Connect.set_text("Connect")
	for i in get_children():
		queue_free()
	
func _on_reg_finished():
	print("Main : Regfin")
	var ss = load("res://Scenes/DeviceList.tscn")
	var cock = ss.instance()
	cock.rect_position = Vector2(0,80)
	add_child(cock)


func _on_data_received(data_received):
	var s = "{"+data_received+"}"
	print("MAIN: Data :"+s)
	var test = parse_json(s)
	print("MAIN: Parsed json with type"+str(typeof(test)))
	if typeof(test) == 18:
		if test.has("Nums"):
			GS.devNums = test.Nums
			for i in range(GS.devNums[0]):
				GS.regStepsArray.push_back("App,Dev,Str,"+str(i))
				#print(str(GS.regStepsArray))
			for i in range(GS.devNums[1]):
				GS.regStepsArray.push_back("App,Dev,Mat,"+str(i))
				#print(str(GS.regStepsArray))
			for i in range(GS.devNums[2]):
				GS.regStepsArray.push_back("App,Dev,Ter,"+str(i))
				#print(str(GS.regStepsArray))
			for i in range(GS.devNums[3]):
				GS.regStepsArray.push_back("App,Dev,Rtr,"+str(i))
				#print(str(GS.regStepsArray))
			GS.registerStep += 1
			print("MAIN: regStepsArray created: "+str(GS.regStepsArray))
			GS.doRegStep()
		elif test.has("Dev"):
			var sglDev = test.Dev
			print(str(sglDev))
			var type = sglDev.pop_front()
			GS.addDvc(type,sglDev)
			print(str(GS.knownDevs))
			GS.doRegStep()

func _on_Connect_pressed():
	if GS.BT:
		GS.BT.getPairedDevices(true)
	else:
		print("Module not initialized!")