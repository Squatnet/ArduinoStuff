extends Node2D
# Declare member variables here. Examples:
var screen_size = OS.get_screen_size()
var window_size = OS.get_window_size()
var loadSc = load("res://Assets/Objects/LoadWindow.tscn")
var loadScInst
var loadScreenNode
var admin = load("res://Scenes/Admin.tscn")
var client = load("res://Scenes/Admin.tscn")
var isFS = false
# Called when the node enters the scene tree for the first time.
func _ready():
	screen_metrics()
	if OS.get_name() == "Android":
		OS.set_screen_orientation(0)
	loadScene()
func loaderTest():
	loadScreenAdd(80)
	for i in 80:
		print("Doing a step")
		get_node(loadScreenNode).loadScreenStep()
		OS.delay_msec(100)
	OS.delay_msec(100)
	loadScreenRemove()

func loadScene():
	var newScene
	if OS.get_name() == "Android":
		GS.appAdminMode = true
	if GS.appAdminMode:
		print("LOADER : ADMIN MODE")
		newScene = admin
		setFS(false)
	else:
		print("LOADER : CLIENT MODE")
		newScene = client
		if GS.appDebug:
			print("LOADER : DEBUG ON")
			setFS(false)
		else:
			print("LOADER : DEBUG OFF")
			setFS(false)
	add_child(newScene.instance())
func loadScreenAdd(steps):
	loadScInst = loadSc.instance()
	loadScInst.loadScreenSetVars(steps,0)
	add_child(loadScInst)
	loadScreenNode = loadScInst.get_name()
	print(loadScreenNode)
	yield($Timer,"timeout")
func loadScreenRemove():
	get_node(loadScreenNode).queue_free()
func loadScreenStep():
	get_node(loadScreenNode).loadScreenStep()
func loadIsFinished():
	loadScreenRemove()
func setFS(val):
	if val == true:
		print("LOADER : Set Fullscreen")
		isFS = true
		OS.set_window_fullscreen(true)
	else:
		print("LOADER : Exit Fullscren")
		isFS = false
		OS.set_window_fullscreen(false)
		OS.set_window_maximized(true)
		OS.set_window_position(screen_size*0.5 - window_size*0.5)
func swFS():
	print("shitCunt")
	isFS = !isFS
	setFS(isFS)

func screen_metrics():
	print("                 [Screen Metrics]")
	print("            Display size: ", OS.get_screen_size())
	print("   Decorated Window size: ", OS.get_real_window_size())
	print("             Window size: ", OS.get_window_size())
	print("        Project Settings: Width=", ProjectSettings.get_setting("display/window/size/width"), " Height=", ProjectSettings.get_setting("display/window/size/height"))
	print(OS.get_window_size().x)
	print(OS.get_window_size().y)