extends Panel

var thePin = "856963"
var theLine = ""

func _on_Button1_pressed():
	theLine += "1"
	$LineEdit.text = theLine


func _on_Button2_pressed():
	theLine += "2"
	$LineEdit.text = theLine


func _on_Button3_pressed():
	theLine += "3"
	$LineEdit.text = theLine

func _on_Button4_pressed():
	theLine += "4"
	$LineEdit.text = theLine

func _on_Button5_pressed():
	theLine += "5"
	$LineEdit.text = theLine # Replace with function body.


func _on_Button6_pressed():
	theLine += "6"
	$LineEdit.text = theLine # Replace with function body.


func _on_Button7_pressed():
	theLine += "7"
	$LineEdit.text = theLine # Replace with function body.


func _on_Button8_pressed():
	theLine += "8"
	$LineEdit.text = theLine # Replace with function body.


func _on_Button9_pressed():
	theLine += "9"
	$LineEdit.text = theLine # Replace with function body.


func _on_Cancel_pressed():
	theLine = ""
	$LineEdit.text = theLine # Replace with function body.


func _on_Button10_pressed():
	theLine += "0"
	$LineEdit.text = theLine # Replace with function body.


func _on_Accept_pressed():
	if theLine == thePin:
		print("Success")
		GS.setSetting("lockout",false)
		GS.emit_signal("locked")
		theLine = ""
		$LineEdit.text = theLine
		$Label2.text = "Enter Pin"
		$Label2.modulate = Color(1,1,1)
		self.hide()
	else:
		$Label2.text = "Wrong Pin"
		$Label2.modulate = Color(1,0,0)

func _on_CloseButton_pressed():
	theLine = ""
	$LineEdit.text = theLine
	$Label2.text = "Enter Pin"
	$Label2.modulate = Color(1,1,1)
	self.hide()
