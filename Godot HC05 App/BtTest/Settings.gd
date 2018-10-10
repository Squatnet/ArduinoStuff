extends Panel

# class member variables go here, for example:
# var a = 2
# var b = "textvar"
var option
func _ready():
	$AddDevice/AddDevPnl/OptionButton/PopupMenu2.add_item("Strip")
	$AddDevice/AddDevPnl/OptionButton/PopupMenu2.add_item("Matrix")
	pass


func _on_OptionButton_pressed():
	$AddDevice/AddDevPnl/OptionButton/PopupMenu2.show()


func _on_PopupMenu2_index_pressed(index):
	option = index
