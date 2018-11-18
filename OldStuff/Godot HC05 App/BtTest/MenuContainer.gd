extends ScrollContainer

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

func _ready():
	pass
func addBtn(text):
	var sBtn = Button.new()
	sBtn.set_text(text)
	sBtn.connect("pressed", self, "on_button_pressed")
	$HBoxContainer.add_child(sBtn)

func on_button_pressed(btn):
	get_node("../PageWrapper").BringToFront(btn)