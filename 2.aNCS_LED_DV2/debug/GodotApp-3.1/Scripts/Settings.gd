extends Panel
#warnings-disable
var isOpen = false
var pallet = 0
var udpSettings
var oscSettings
var validIp = false
var validPort = false
var validRPort = false
var fkeDta
var dbgMod
var admMod
func setup():
	$FakeData.pressed = GS.getSetting("fakeData")
	fkeDta = $FakeData.pressed
	$DebugMode.pressed = GS.getSetting("debugMode")
	dbgMod = $DebugMode.pressed
	$AdminMode.pressed = GS.getSetting("adminMode")
	admMod = $AdminMode.pressed
	udpSettings = GS.getSetting("udp")
	oscSettings = GS.getSetting("osc")
	$TabContainer/UDPSettings/LocPort.text = str(udpSettings["locPort"])
	$TabContainer/UDPSettings/RemIp.text = udpSettings["ip"]
	$TabContainer/UDPSettings/RemPort.text = str(udpSettings["remPort"])
	$TabContainer/OSCSettings/LocPort.text = str(oscSettings["locPort"])
	$TabContainer/OSCSettings/RemPort.text = str(oscSettings["remPort"])
	$TabContainer/OSCSettings/RemIp.text = str(oscSettings["ip"])
func _ready():
	setup()
func getOpen():
	return isOpen
func setOpen():
	print(isOpen)
	if isOpen:
		isOpen = false
	else:
		isOpen = true
		setup()

func _on_Close_pressed():
	setOpen()
	hide()

func _on_SaveButton_pressed():
	GS.setSetting("fakeData",$FakeData.pressed)
	GS.setSetting("adminMode",$AdminMode.pressed)
	GS.setSetting("debugMode",$DebugMode.pressed)
	GS.setSetting("udp",udpSettings)
	GS.setSetting("osc",oscSettings)
	if GS.getSetting("debugMode") == true:
		OS.alert("Settings Saved: "+str(GS.settings),"Success!")
	setOpen()
	hide()

func _on_ParsePallettes_pressed():
	print("Settings: Parse pallettes")
	var pallParser = load("res://Scripts/PallParser.gd").new()
	pallParser.parse()
	setup()



func _on_DeletePallettes_pressed():
	print("Settings: Delete All Palletes")
	GS.remAllPallettes()
	setup()
		

func _on_LockoutButton_toggled(button_pressed):
	GS.setSetting("lockout",button_pressed)
	get_parent().updateLock()
	


func _on_SetUdpBtn_pressed():
	if validIp:
		GS.setRemoteAddr(udpSettings["ip"])
	if validPort:
		GS.setLocalUdpPort(udpSettings["locPort"])
	if validRPort:
		GS.setRemoteUdpPort(udpSettings["remPort"])

func _on_RemIp_text_changed(new_text):
	if new_text.is_valid_ip_address():
		validIp = true
		udpSettings["ip"] = new_text
	else:
		validIp = false


func _on_RemPort_text_changed(new_text):
	if int(new_text) != 0:
		validRPort = true
		udpSettings["remPort"] = int(new_text)
	else:
		validRPort = false

func _on_LocPort_text_changed(new_text):
	if int(new_text) != 0:
		validPort = true
		udpSettings["locPort"] = int(new_text)
	else:
		validPort = false


func _on_UdpBtn_pressed():
	$Terminal.hide()
	$UDPSettings.show()


func _on_TermBtn_pressed():
	$UDPSettings.hide()
	$Terminal.show()


func _on_BtUdpSw_toggled(button_pressed):
	GS.udp = button_pressed
	GS.setupBT(GS.getSetting("fakeData"))


func _on_FakeData_toggled(button_pressed):
	fkeDta = button_pressed
func _on_DebugMode_toggled(button_pressed):
	dbgMod = button_pressed
func _on_AdminMode_toggled(button_pressed):
	admMod = button_pressed
