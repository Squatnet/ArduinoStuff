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
var chkStr = false
var chkMat = false
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
func _ready():
	call_deferred("showChkBox")
	print(rect_scale)
	rect_scale = Vector2(0.3,0.3)
	#get_node("/root/Main").connect("ack",self,"onAck")
func showChkBox():
	var arg = shwChks
	print("Page: showChkBox: "+str(arg))
	for i in get_tree().get_nodes_in_group("chkBox"):
		if arg == true:
			i.show()
		else:
			i.hide()
func setup(args):
	knownPatterns = GS.knownPatterns
	numPatterns = knownPatterns.size()
	numMirror = mirrorOpts.size()
	$MirrorModeName.set_text(mirrorOpts[mirrorSel])
	$PatternName.set_text(knownPatterns[1])
	$AutoMode/Label.set_text(autoOpts[automode])
	pageType = args[0]
	if pageType == 0:
		$PAGE.set_text("Single Device: "+str(args[1]))
		var x = str(args[1]).split(":")
		print("Page: "+str(x))
		if x[0] != "Mat":
			$MirrorModeName.hide()
			$MirrorModeLbl.hide()
			$Messge.hide()
		if x[0] == "Str":
			attchStr = int(x[3])
		if x[0] != "Str":
			$InStrMode.hide()
		commandDevClassOrID += str(x[2])+","
	elif pageType == 1:
		$PAGE.set_text("Device Type :"+str(args[1]))
		if args[1] != "Mat":
			$Messge.hide()
			$MirrorModeLbl.hide()
			$MirrorModeName.hide()
		if args[1] == "Str":
			attchStr = GS.mostKnownStrips
		if args[1] != "Str":
			$InStrMode.hide()
		commandDevClassOrID += str(args[1])+",All,"
	elif pageType == 2:
		$PAGE.set_text("Group :"+str(args[1]))
		attchStr = GS.mostKnownStrips
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
		GS.BT.sendData(commandToSend)
		var msec = 0
		while !gotAck and msec < 300:
			print("wait for ack "+str(msec))
			OS.delay_msec(10)
			msec += 10
		if !gotAck:
			if msec >= 300:
				print("Didn't get ack")
	if GS.getSetting("debugMode") == true:
		OS.alert("Page: commendToSend: "+commandToSend,"Cmd")
	else:
		pass
	commandToRelay = ""
	updated.clear()
func _on_CloseBtn_pressed():
	queue_free()

func _on_AutoMode_toggled(button_pressed):
	if button_pressed == true:
		automode = 1
	else:
		automode = 0
	if !updated.has("automode"):
		updated.push_back("automode")


func _on_LineEdit_text_changed(new_text):
	if int(new_text) != 0:
		autoSecs = int(new_text)
		if !updated.has("autoSecs"):
			updated.push_back("autoSecs")


func _on_PalleteMode_toggled(button_pressed):
	if button_pressed == true:
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
			if chkStr == true && chkMat == true && chkRtr == true && chkTerm == true:
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
			sendMessage()
	else:
		if message.length() > 0:
			commandToRelay += "Msg,"+message+","
			message = ""
			sendMessage()
		elif GS.getSetting("debugMode") == true:
			OS.alert("Nothing was updated or failed to generate command","Fail!")
		else:
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

func _on_InStrMode_toggled(button_pressed):
	indStrAddr = int(button_pressed)
	for i in $InStrMode.get_children():
		if indStrAddr == 1:
			i.show()
		else:
			i.hide()
	if !updated.has("inSt"):
		updated.push_back("inSt")

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

func _on_MirPrevBtn_pressed():
	mirrorSel -= 1
	if mirrorSel < 0:
		mirrorSel = numMirror-1
	$MirrorModeName.set_text(mirrorOpts[mirrorSel])
	if !updated.has("mirror"):
		updated.push_back("mirror")


func _on_MirNextBtn_pressed():
	mirrorSel += 1
	if mirrorSel > numMirror-1:
		mirrorSel = 0
	$MirrorModeName.set_text(mirrorOpts[mirrorSel])
	if !updated.has("mirror"):
		updated.push_back("mirror")


func _on_Prev_pressed():
	automode -= 1
	if automode < 0:
		automode = autoOpts.size()-1
	$AutoMode/Label.set_text(autoOpts[automode])
	if !updated.has("automode"):
		updated.push_back("automode")


func _on_Next_pressed():
	automode += 1
	if automode > autoOpts.size()-1:
		automode = 0
	$AutoMode/Label.set_text(autoOpts[automode])
	if !updated.has("automode"):
		updated.push_back("automode")
