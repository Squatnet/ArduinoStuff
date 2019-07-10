extends Panel

var pincode = "8569"
var pinEntered = ""
var locked = false
func checkLen():
	if pinEntered.length() == pincode.length():
		for i in $GridContainer.get_children():
			if i.get_name() == "PinBtn12":
				pass
			elif i.get_name() == "PinBtn10":
				pass
			else:
				i.disabled = true
	else:
		for i in $GridContainer.get_children():
			i.disabled = false
func _on_PinBtn_pressed():
	pinEntered+="1"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn2_pressed():
	pinEntered+="2"
	$PinLabel.text += "*"
	checkLen()


func _on_PinBtn3_pressed():
	pinEntered+="3"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn4_pressed():
	pinEntered+="4"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn5_pressed():
	pinEntered+="5"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn6_pressed():
	pinEntered+="6"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn7_pressed():
	pinEntered+="7"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn8_pressed():
	pinEntered+="8"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn9_pressed():
	pinEntered+="9"
	$PinLabel.text += "*"
	checkLen()

func _on_PinBtn11_pressed():
	pinEntered+="0"
	$PinLabel.text += "*"
	checkLen()
func _on_PinBtn10_pressed():
	$PinLabel.text = ""
	pinEntered = ""
	checkLen()

func _on_PinBtn12_pressed():
	if pinEntered == pincode:
		GS.setLock(false)
		pinEntered = ""
		$PinLabel.text = ""
		get_parent().get_parent().get_node("SendButton").show()
		get_parent().get_parent().get_node("CloseBtn").show()
		get_parent().hide()
		OS.window_fullscreen = false
	else:
		$AnimationPlayer.play("pinShake")


func _on_Close_pressed():
	pinEntered = ""
	checkLen()
	get_parent().get_parent().get_node("ToolButton").pressed = true
	get_parent().hide()
