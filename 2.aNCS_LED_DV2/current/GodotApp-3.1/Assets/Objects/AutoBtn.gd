extends Button

# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var parent
var toParent
var type
# Called when the node enters the scene tree for the first time.
func _ready():
	pressed = false
	parent = self
	for i in range(toParent):
		parent = parent.get_parent()
func setup(name, levelsToParent, butType):
	text = name
	toParent = levelsToParent
	type = butType
func _on_AutoBtn_toggled(button_pressed):
	for i in get_parent().get_children():
		i.pressed = false
	parent.autoButtonPressed(text,type)
	
