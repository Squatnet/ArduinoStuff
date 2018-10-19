extends Control

# class member variables go here, for example:
# var a = 2
# var b = "textvar"
var bt
func _ready():
	if BTHandler.bluetooth:
		bt = true



func _on_Button_pressed():
	var msg = $LineEdit.text
	
	BTHandler.bluetooth.sendData(msg)
	
	
func received(dat):
	$TextEdit.add_line(dat)

func _on_Button2_pressed():
	self.hide()
