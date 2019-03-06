extends Control

var updated = []
var automode = 0
var autoSecs = 10
var patternSel = 1
var paletteMode = 0
var currCol = [0,0,0]
var pageType = 0
var pageTypeText = ["Single Device","Type","Group"]
var deviceClass = 0
var deviceClassText = ["Str","Mat","Ter","Rou"]
var commandPrefix = "Ctl,"
var commandDevClassOrID = ""
var commandGrpName = ""
var commandToRelay = ""
var commandSuffix = ","
func _ready():
	# Called when the node is added to the scene for the first time.
	# Initialization here
	pass
func setup(args):
	pageType = args[0]
	if pageType == 0:
		$PAGE.set_text("Single Device"+str(args[1]))
		var x = str(args[1]).split(":")
		print(x)
		commandDevClassOrID += str(x[2])+","
	elif pageType == 1:
		$PAGE.set_text("Device Type :"+str(args[1]))
		commandDevClassOrID += str(args[1])+",All,"
	elif pageType == 2:
		$PAGE.set_text("Group :"+str(args[1]))
		commandGrpName += str(args[1])+","
func setCol(col):
	currCol = col
	print("PAGE: Color Updated: "+str(currCol))
	if !updated.has("color"):
		updated.push_back("color")
#func _process(delta):
#	# Called every frame. Delta is time since last frame.
#	# Update game logic here.
#	pass

func sendMessage():
	var commandToSend = commandPrefix+commandDevClassOrID+commandGrpName+commandToRelay
	if GS.BT:
		GS.BT.sendData(commandToSend)
	if GS.getSetting("debugMode") == true:
		OS.alert("Sending Message:\n commandToSend : "+commandToSend,"Sending")
	else:
		OS.alert("Sent message to "+commandDevClassOrID+commandGrpName,"Sending")
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
	else:
		paletteMode = 0
		$ColorPicker.show()


func _on_SendButton_pressed():
	if updated.size() > 0:
		print(updated)
		for i in updated:
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
		print(commandToRelay)
		sendMessage()
		
	else:
		if GS.getSetting("debugMode") == true:
			OS.alert("Nothing was updated or failed to generate command","Fail!")
		else:
			OS.alert("NOTHING TO SEND","Warning!")