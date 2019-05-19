extends Panel
#warnings-disable
var isOpen = false
var pallet = 0
var udpSettings
var validIp = false
var validPort = false
var validRPort = false
func setup():
	$FakeData.pressed = GS.getSetting("fakeData")
	$DebugMode.pressed = GS.getSetting("debugMode")
	$AdminMode.pressed = GS.getSetting("adminMode")
	$PallettesAvail.text = str(GS.pallettes.size())
	udpSettings = GS.getSetting("udp")
	$BtUdpSw.pressed = GS.udp
	$UDPSettings/LocPort.text = str(udpSettings["locPort"])
	$UDPSettings/RemIp.text = udpSettings["ip"]
	$UDPSettings/RemPort.text = str(udpSettings["remPort"])
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
	print(str(int($FakeData.pressed)))
	if GS.getSetting("debugMode") == true:
		OS.alert("Settings Saved: "+str(GS.settings),"Success!")
	else:
		OS.alert("Settings Saved Succesfully","Saved")
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
