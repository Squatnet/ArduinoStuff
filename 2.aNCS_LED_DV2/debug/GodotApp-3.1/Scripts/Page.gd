extends Control
#warnings-disable
var gotAck = false
var updated = []
var knownPatterns = []
var parserOrder = ["automode","autoSecs","inSt","strNo","pattern","color","palMode","pall","mirror"]
var numPatterns = 0
var numMirror = 0
var automode = 0
var autoSecs = 10
var patternSel = 1
var mirrorSel = 0
var paletteMode = 0
var currCol = [0,0,0]
var currPallette = 0
var indStrAddr = 0
var AddrStrNo = 1
var attchStr = 0
var message = ""
var chkStr = true
var chkMat = true
var chkRtr = false
var chkTerm = false
var shwChks = false
var pageType = 0
var pageTypeText = ["Single Device","Type","Group"]
var mirrorOpts = ["Off","Quadrant","Quad. Rotate","Vertical"]
var autoOpts = ["Off", "All", "Pattern", "Pallette"]
var deviceClass = 0
var deviceClassText = ["Str","Mat","Ter","Rou"]
var commandPrefix = "Ctl,"
var commandDevClassOrID = ""
var commandGrpName = ""
var commandToRelay = ""
var commandSuffix = ","
var autoBtn = load("res://Assets/Objects/AutoBtn.tscn")
func _ready():
	call_deferred("showChkBox")
	print(rect_scale)
	rect_scale = Vector2(0.4,0.4)
	rect_global_position = Vector2(0,0)
	GS.connect("clock",self,"_on_SendButton_pressed")
	get_node("/root/InitialLoader/Main").connect("ack",self,"onAck")
	for i in autoOpts:
		var aTb = autoBtn.instance()
		aTb.setup(i,3,"auto")
		$AutoMode/HBoxContainer.add_child(aTb)
	for i in mirrorOpts:
		var mTb = autoBtn.instance()
		mTb.setup(i,3,"mirr")
		$MirrMode/HBoxContainer.add_child(mTb)
	for i in knownPatterns:
		var pTb = autoBtn.instance()
		pTb.setup(i,3,"patt")
		$Pattern/HBoxContainer.add_child(pTb)
	var clrBt = autoBtn.instance()
	clrBt.setup("Color",3,"clr")
	$ColorMode/HBoxContainer.add_child(clrBt)
	var palBt = autoBtn.instance()
	palBt.setup("Palette",3,"clr")
	$ColorMode/HBoxContainer.add_child(palBt)
	var ismOff = autoBtn.instance()
	ismOff.setup("Off",3,"ism")
	$ISMode/HBoxContainer.add_child(ismOff)
	var ismOn = autoBtn.instance()
	ismOn.setup("On",3,"ism")
	$ISMode/HBoxContainer.add_child(ismOn)
func showChkBox():
	var arg = shwChks
	print("Page: showChkBox: "+str(arg))
	if arg == true:
		$Control.show()
func setup(args):
	knownPatterns = GS.knownPatterns
	numPatterns = knownPatterns.size()
	numMirror = mirrorOpts.size()
	#$PatternName.set_text(knownPatterns[1])
	#$AutoMode/Label.set_text(autoOpts[automode])
	pageType = args[0]
	if pageType == 0:
		$PAGE.set_text("Single Device: "+str(args[1]))
		var x = str(args[1]).split(":")
		print("Page: "+str(x))
		if x[0] != "Mat":
			$MirrMode.hide()
			$Messge.hide()
			$MatrixLbl.hide()
		if x[0] == "Str":
			attchStr = 4
		if x[0] != "Str":
			$ISMode.hide()
			$InStrMode.hide()
			$StripLbl.hide()
		commandDevClassOrID += str(x[2])+","
	elif pageType == 1:
		$PAGE.set_text("Device Type :"+str(args[1]))
		if args[1] != "Mat":
			$Messge.hide()
			$MirrMode.hide()
			$MatrixLbl.hide()
		if args[1] == "Str":
			attchStr = 4
		if args[1] != "Str":
			$ISMode.hide()
			$InStrMode.hide()
			$StripLbl.hide()
		commandDevClassOrID += str(args[1])+",All,"
	elif pageType == 2:
		$PAGE.set_text("Group :"+str(args[1]))
		attchStr = 4
		shwChks = true
		commandGrpName += str(args[1])+","
func setCol(col):
	currCol = col
	print("PAGE: Color Updated: "+str(currCol))
	if !updated.has("color"):
		updated.push_back("color")
func setPall(pall):
	currPallette = pall
	print("PAGE: Pallette Updated: "+str(pall))
	if !updated.has("pall"):
		updated.push_back("pall")
#func _process(delta):
#	# Called every frame. Delta is time since last frame.
#	# Update game logic here.
#	pass

func sendMessage():
	var commandToSend = commandPrefix+commandDevClassOrID+commandGrpName+commandPrefix+commandToRelay
	if GS.BT:
		gotAck = false
		var st = OS.get_unix_time()
		GS.BT.sendData(commandToSend)
		var msec = 0
		var iter = 0
		while !gotAck:
			if GS.getSetting("fakeData"):
				gotAck = true
			if msec < 2000:
				print("wait for ack "+str(msec))
				yield(get_tree(),"idle_frame")
				msec += 10
			if iter >= 4:
				break
			if msec >= 2000:
				iter += 1
				print("Didn't get ack")
				GS.BT.sendData(commandToSend)
				msec = 0
		var et = OS.get_unix_time()
		print("elapsed time = "+str(et-st))
	if GS.getSetting("debugMode") == true:
		OS.alert("Page: commendToSend: "+commandToSend,"Cmd")
	else:
		pass
	commandToRelay = ""
	updated.clear()
func _on_CloseBtn_pressed():
	queue_free()
func _on_LineEdit_text_changed(new_text):
	if int(new_text) != 0:
		autoSecs = int(new_text)
		print("autoSecs")
		if !updated.has("autoSecs"):
			updated.push_back("autoSecs")


func _on_PalleteMode_toggled(button_pressed):
	if button_pressed == true:
		pass
	else:
		pass

func _on_SendButton_pressed():
	if updated.size() > 0:
		var newArr = []
		for i in parserOrder:
			if updated.has(i):
				newArr.push_back(i)
		print("Page: updated: "+str(newArr))
		for i in newArr:
			if i == "automode":
				commandToRelay += "Atm,"+str(automode)+","
			elif i == "autoSecs":
				commandToRelay += "Ats,"+str(autoSecs)+","
			elif i == "color":
				var colStr = str(currCol)
				colStr.erase(0,1)
				colStr.erase(colStr.length()-1,1)
				commandToRelay += "Col,"+str(colStr)+","
			elif i == "pattern":
				commandToRelay += "Ptn,"+str(patternSel)+","
			elif i == "pall":
				commandToRelay += "PNo,"+str(currPallette)+","
			elif i == "palMode":
				commandToRelay += "Pal,"+str(paletteMode)+","
			elif i == "mirror":
				commandToRelay += "Mir,"+str(mirrorSel)+","
			elif i == "inSt":
				commandToRelay += "ISt,"+str(indStrAddr)+","
			elif i == "strNo":
				commandToRelay += "SNo,"+str(AddrStrNo)+","
				
		if pageType == 2:
			if chkStr == true && chkMat == true:
				commandDevClassOrID = "All,"
				sendMessage()
			elif chkStr == false && chkMat == false && chkRtr == false && chkTerm == false:
				if GS.getSetting("debugMode") == true:
					OS.alert("All chkBox Values are false. There is to type to send to","Whoops")
				else:
					OS.alert("No device type selected","Whoops")
				commandToRelay = ""
			else:
				var savedCommand = commandToRelay
				if chkStr:
					commandDevClassOrID = "Str,"
					commandToRelay = savedCommand
					sendMessage()
				if chkMat:
					commandDevClassOrID = "Mat,"
					commandToRelay = savedCommand
					sendMessage()
				if chkRtr:
					commandDevClassOrID = "Rou,"
					commandToRelay = savedCommand
					sendMessage()
				if chkTerm:
					commandDevClassOrID = "Ter,"
					commandToRelay = savedCommand
					sendMessage()
		else:
			sendMessage()
		if message.length() > 0:
			if pageType == 2:
				commandDevClassOrID = "Mat,"
			commandToRelay += "Msg,"+message+","
			message = ""
			$Messge.text =""
			sendMessage()
	else:
		if message.length() > 0:
			commandToRelay += "Msg,"+message+","
			message = ""
			$Messge.text = ""
			sendMessage()
		elif GS.getSetting("debugMode") == true:
			OS.alert("Nothing was updated or failed to generate command","Fail!")
		else:
			if !GS.locked == false && GS.getSetting("adminMode") == true:
					OS.alert("NOTHING TO SEND","Warning!")

func _on_Messge_text_changed(new_text):
	message = new_text
	
func _on_StrChk_toggled(button_pressed):
	chkStr = button_pressed

func _on_MatChk_toggled(button_pressed):
	chkMat = button_pressed


func _on_RtrChk_toggled(button_pressed):
	chkRtr = button_pressed

func _on_TerChk_toggled(button_pressed):
	chkTerm = button_pressed


func _on_PrevBtn_pressed():
	AddrStrNo -= 1
	if AddrStrNo < 1:
		AddrStrNo = attchStr
	$InStrMode/AddStrNum.set_text(str(AddrStrNo))
	if !updated.has("strNo"):
		updated.push_back("strNo")

func _on_NextBtn_pressed():
	AddrStrNo += 1
	if AddrStrNo > attchStr:
		AddrStrNo = 1
	$InStrMode/AddStrNum.set_text(str(AddrStrNo))
	if !updated.has("strNo"):
		updated.push_back("strNo")



func _on_PatPrevBtn_pressed():
	patternSel -= 1
	if patternSel < 0:
		patternSel = numPatterns-1
	$PatternName.set_text(knownPatterns[patternSel])
	if !updated.has("pattern"):
		updated.push_back("pattern")

func _on_PatNextBtn_pressed():
	patternSel += 1
	if patternSel > numPatterns-1:
		patternSel = 0
	$PatternName.set_text(knownPatterns[patternSel])
	if !updated.has("pattern"):
		updated.push_back("pattern")
func onAck():
	gotAck = true

func autoButtonPressed(btnName,btnType):
	print(btnName)
	if btnType == "auto":
		if autoOpts.has(btnName):
			automode = autoOpts.find(btnName)
			if !updated.has("automode"):
				updated.push_back("automode")
		else:
			OS.alert("Undefined Behaviour")
	elif btnType == "mirr":
		if mirrorOpts.has(btnName):
			mirrorSel = mirrorOpts.find(btnName)
			if !updated.has("mirror"):
				updated.push_back("mirror")
		else:
			OS.alert("Undefined Behaviour")
	elif btnType == "patt":
		if knownPatterns.has(btnName):
			patternSel = knownPatterns.find(btnName)
			if !updated.has("pattern"):
				updated.push_back("pattern")
	elif btnType == "clr":
		if btnName == "Palette":
			paletteMode = 1
			$ColorPicker.hide()
			$PallettePicker.show()
			if updated.has("color"):
				updated.erase("color")
			if !updated.has("pall"):
				updated.push_back("pall")
		else:
			paletteMode = 0
			$ColorPicker.show()
			$PallettePicker.hide()
			if updated.has("pall"):
				updated.erase("pall")
			if !updated.has("color"):
				updated.push_back("color")
		if !updated.has("palMode"):
			updated.push_back("palMode")
	elif btnType == "ism":
		if btnName == "On":
			indStrAddr = 1
			$InStrMode.show()
		else:
			indStrAddr = 0
			$InStrMode.hide()
		if !updated.has("inSt"):
			updated.push_back("inSt")

func _on_HSlider_value_changed(value):
	$AutoS.text = str(value+3)
	_on_LineEdit_text_changed($AutoS.text)


func _on_StripsBtn_toggled(button_pressed):
	chkStr = button_pressed


func _on_MatrixBtn_toggled(button_pressed):
	chkMat = button_pressed


func _on_ToolButton_toggled(button_pressed):
	if button_pressed == true:
		GS.setLock(true)
		$CloseBtn.hide()
		$SendButton.hide()
		OS.window_fullscreen = true
	else:
		$Panel.show()


func _on_PinBtn11_pressed():
	pass # Replace with function body.


func _on_ToolButton_pressed():
	if message != "":
		$Messge/Osk.theText = message
		$Messge/Osk/Label.text = message
	$Messge/Osk.show()
func _process(delta):
	if Input.is_action_just_pressed("set_fullsc"):
		get_tree().get_root().get_node("InitialLoader").swFS()
