extends Control
var menuBtn = load("res://Assets/Objects/Menu_A_Btn.tscn")
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
	print("Main - Connected")
	$Connect.set_text("Disconnect")
	if GS.BT:
		$ModeLbl.set_text("BlueTooth")
		GS.BT.sendData("App,Init")
	else:
		$ModeLbl.set_text("FakeData")
		GS.fakeReg()

func _on_disconnected():
	print("Main: Disconnected")
	$Connect.set_text("Connect")
	$ModeLbl.set_text("Disconnected")
	$Control.hide()
	for i in $Control/MenuBarA.get_children():
		i.queue_free()
	for i in $Control/ContentArea.get_children():
		i.queue_free()
	
func _on_reg_finished():
	$Control.show()
	print("Main : Register Devices finished")
	addMenuButton("List All")
	if GS.knownGroups.empty():
		pass
	else:
		addMenuButton("Groups")
	addMenuButton("Types")

func addMenuButton(name):
	var btn = menuBtn.instance()
	btn.set_text(name)
	btn.rect_scale = Vector2(2,2)
	btn.connect("pressed",self,"_on_Menu_A_Btn_Pressed",[btn.text])
	$Control/MenuBarA.add_child(btn)

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
func _on_Menu_A_Btn_Pressed(btn):
	print("Main: Menu_A_Btn_Pressed("+btn+")")
	for i in $Control/ContentArea.get_children():
		i.queue_free()
	var newSc = load("res://Scenes/"+btn+".tscn")
	var newInsScn = newSc.instance()
	newInsScn.setup(btn)
	$Control/ContentArea.add_child(newInsScn)
	
func _on_Connect_pressed():
	if GS.getSetting("fakeData") == false:
		if !GS.BT:
			print("Main: Setup BT true")
			GS.setupBT(true)
	else:
		print("Main: Setup BT false")
		GS.setupBT(false)
	
	if GS.BT: #got bluetooth
		GS.BT.getPairedDevices(true) ## show bluetooth device list
	else: ## no bluetooth module
		if $Connect.text == "Connect":
			GS.emit_signal("connected")
			OS.alert("BT Module not initialised, Using FakeData","Alert!")
		else:
			GS.emit_signal("disconnected")
			GS._on_disconnected()

func _on_SettingsBtn_pressed():
	$Settings.setOpen()
	print("Main: "+str($Settings.getOpen()))
	if $Settings.getOpen():
		$Settings.show()
	else:
		$Settings.hide()
