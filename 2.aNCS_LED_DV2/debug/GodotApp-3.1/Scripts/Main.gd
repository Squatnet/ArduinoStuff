extends Control
#warnings-disable
signal ack 
var regFin = false
var menuBtn = load("res://Assets/Objects/Menu_A_Btn.tscn")
var loadSc
var oscrcv
var currOSCMess = []
var TheDict = {}
var TypeNums = []
func _ready():
	set_signals()
	loadSc = get_parent()
	if GS.pallettes.empty():
		var pallParser = load("res://Scripts/PallParser.gd").new()
		pallParser.parse()
	if GS.osc:
		setupOSC()
func set_signals():
	GS.connect("connected", self, "_on_connected")
	GS.connect("disconnected", self, "_on_disconnected")
	GS.connect("data_received", self, "_on_data_received")
	GS.connect("reg_finished", self, "_on_reg_finished")

func _on_connected():
	print("Main - Connected")
	$Connect.set_text("Disconnect")
	if GS.getSetting("fakeData"):
		$ModeLbl.set_text("FakeData")
		loadSc.loadScreenAdd(3)
		GS.fakeReg()
	else:
		if GS.BT:
			if GS.udp:
				$ModeLbl.set_text("UDP")
				GS.BT.redoSetup()
				GS.BT.sendData("Udp,Init")
			else:
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
	loadSc.loadScreenStep()
	regFin = true
	print("Main : Register Devices finished")
	addMenuButton("List All")
	if GS.knownGroups.empty():
		pass
	else:
		addMenuButton("Groups")
	addMenuButton("Types")
	
	loadSc.loadScreenRemove()

func addMenuButton(name):
	var btn = menuBtn.instance()
	btn.set_text(name)
	btn.rect_scale = Vector2(2,2)
	btn.connect("pressed",self,"_on_Menu_A_Btn_Pressed",[btn.text])
	$Control/MenuBarA.add_child(btn)
func _process(delta):
	if !regFin:
		if !GS.lastRegStepConfirmed:
			$Timer.start()
	if GS.osc:
		while( oscrcv.has_message() ):
			var msg = oscrcv.get_next()
			print(str(msg))
			currOSCMess.push_back(msg["address"])
			for i in range( 0, msg["arg_num"] ):
				currOSCMess.push_back(msg["args"][i])
			if currOSCMess[0] == "/Clk":
				GS.emit_signal("clock")
func _on_data_received(data_received):
	$DebugLabel.add_text(data_received+"\n")
	$Settings/Terminal/RichTextLabel.add_text(data_received+"\n")
	if data_received.begins_with("ack"):
		emit_signal("ack")
	var s
	if !data_received.begins_with("{"):
		 s = "{"+data_received+"}"
	else:
		s = data_received
	print("MAIN: Data :"+s)
	var test = parse_json(s)
	print("MAIN: Parsed json with type"+str(typeof(test)))
	if typeof(test) == 18:
		if test.has("Nums"):
			var prefix = "Udp"
			if !GS.udp:
				prefix = "App"
				
			GS.devNums = test.Nums
			var totalDevs = test.Nums[0] + test.Nums[1] + test.Nums[2] + test.Nums[3]
			loadSc.loadScreenAdd(totalDevs+3)
			for i in range(GS.devNums[0]):
				GS.regStepsArray.push_back(prefix+",Dev,Str,"+str(i))
				#print(str(GS.regStepsArray))
			for i in range(GS.devNums[1]):
				GS.regStepsArray.push_back(prefix+",Dev,Mat,"+str(i))
				#print(str(GS.regStepsArray))
			for i in range(GS.devNums[2]):
				GS.regStepsArray.push_back(prefix+",Dev,Ter,"+str(i))
				#print(str(GS.regStepsArray))
			for i in range(GS.devNums[3]):
				GS.regStepsArray.push_back(prefix+",Dev,Rtr,"+str(i))
				#print(str(GS.regStepsArray))
			
			GS.registerStep += 1
			loadSc.loadScreenStep()
			print("MAIN: regStepsArray created: "+str(GS.regStepsArray))
			GS.doRegStep()
		elif test.has("Dev"):
			GS.lastRegStepConfirmed = true
			$Timer.stop()
			var sglDev = test.Dev
			print("Main: hasDev"+str(sglDev))
			var type = sglDev.pop_front()
			GS.addDvc(type,sglDev)
			print(str(GS.knownDevs))
			GS.doRegStep()
			loadSc.loadScreenStep()
func _on_Menu_A_Btn_Pressed(btn):
	print("Main: Menu_A_Btn_Pressed("+btn+")")
	loadSc.loadScreenAdd(20)
	OS.delay_msec(50)
	for i in $Control/ContentArea.get_children():
		i.queue_free()
		loadSc.loadScreenStep()
	var newSc = load("res://Scenes/"+btn+".tscn")
	var newInsScn = newSc.instance()
	loadSc.loadScreenStep()
	$Control/ContentArea.add_child(newInsScn)
	newInsScn.setup(btn)
	loadSc.loadScreenRemove()
	
func _on_Connect_pressed():
	if GS.getSetting("fakeData") == false:
		if GS.BT: #got bluetooth
			GS.BT.getPairedDevices(true) ## show bluetooth device list
			if $Connect.text == "Connect":
				GS.emit_signal("connected")
				OS.alert("Using FakeData due to No Module","Alert!")
			else:
				GS.emit_signal("disconnected")
				GS._on_disconnected()
	else: ## no bluetooth module
		if $Connect.text == "Connect":
			GS.emit_signal("connected")
			if GS.getSetting("adminMode") == true:
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

func updateLock():
	var lock = GS.getSetting("lockout")
	var LoadIcon
	if lock == true:
		LoadIcon = load("res://Assets/Locked.png")
	else:
		LoadIcon = load("res://Assets/Unlocked.png")
	$LockIcon.set_texture(LoadIcon)
	if GS.BT:
		GS.BT.sendData("Lck,"+str(int(lock)))
	else:
		OS.alert("LOCKOUT: "+str(lock),str(int(lock)))

func setupOSC():
	var oscSet = GS.getSetting("osc")
	oscrcv = load("res://addons/gdosc/bin/gdoscreceiver.gdns").new()
	oscrcv.max_queue( 15 )
	oscrcv.setup(oscSet.locPort)
	oscrcv.start()
func _exit_tree():
	oscrcv.stop()
func _on_Timer_timeout():
	GS.doRegStep()
