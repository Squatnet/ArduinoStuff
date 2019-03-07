extends Panel

var isOpen = false
var pallet = 0
func setup():
	if OS.get_name() == "Android":
		$FakeData.disabled = false
	else:
		$FakeData.disabled = true
	$FakeData.pressed = GS.getSetting("fakeData")
	$DebugMode.pressed = GS.getSetting("debugMode")
	$AdminMode.pressed = GS.getSetting("adminMode")
	$PallettesAvail.text = str(GS.pallettes.size())
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

func _on_Close_pressed():
	setOpen()
	hide()

func _on_SaveButton_pressed():
	GS.setSetting("fakeData",$FakeData.pressed)
	GS.setSetting("adminMode",$AdminMode.pressed)
	GS.setSetting("debugMode",$DebugMode.pressed)
	if pallet == 0:
		pass
	else:
		GS.setSetting("pallettes",pallet)
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
		
