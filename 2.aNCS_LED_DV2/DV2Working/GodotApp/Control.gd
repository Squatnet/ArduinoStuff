extends Control
func _ready():
	if OS.get_name() != "Android":
		print("Loader : NOT ANDROID")
		OS.set_window_size(1280,720)
		changeScene("res://Scenes/Main.tscn")
func changeScene(scn):
	get_tree().change_scene(scn)
