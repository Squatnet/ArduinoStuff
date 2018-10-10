extends Control
var settingsPg = load("res://Scenes/Settings.tscn")
var stripsPg = load("res://Scenes/LEDString.gd")
var matrixPg = load("res://Scenes/Matrix.gd")
var currentPages = []

func _ready():
	pass
func addPage(pgnm):
	var nwPg
	if pgnm == "Settings":
		nwPg = settingsPg.new()
	elif pgnm.begins_with("LED"):
		nwPg = stripsPg.new()
	elif pgnm.begins_with("MAT"):
		nwPg = matrixPg.new()
	currentPages.push_back(pgnm)
	nwPg.name = pgnm
	add_child(nwPg.name)
func BringToFront(pgnm):
	get_node(pgnm).raise()
func remPage(pgnm):
	get_node(pgnm).queue_free()