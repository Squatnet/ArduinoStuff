extends Node2D
# Declare member variables here. Examples:
var screen_size = OS.get_screen_size() 
var window_size = OS.get_window_size()
var main = load("res://Scenes/Main.tscn")

# Called when the node enters the scene tree for the first time.
func _ready():
	screen_metrics()
	if OS.get_name() == "Android":
		OS.set_screen_orientation(0)
	else:
		OS.set_window_position(screen_size*0.5 - window_size*0.5)
	OS.set_window_fullscreen(true)
	var loadedScene = main.instance()
	add_child(loadedScene)
func screen_metrics():
    print("                 [Screen Metrics]")
    print("            Display size: ", OS.get_screen_size())
    print("   Decorated Window size: ", OS.get_real_window_size())
    print("             Window size: ", OS.get_window_size())
    print("        Project Settings: Width=", ProjectSettings.get_setting("display/window/size/width"), " Height=", ProjectSettings.get_setting("display/window/size/height")) 
    print(OS.get_window_size().x)
    print(OS.get_window_size().y)