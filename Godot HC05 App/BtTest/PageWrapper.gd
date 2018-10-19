extends Control
var settingsPg = load("res://Scenes/Settings.tscn")
var stripsPg = load("res://Scenes/LEDString.tscn")
var matrixPg = load("res://Scenes/Matrix.tscn")
var currentPages = []

func _ready():
	pass
func addPage(pgnm):
	var nwPg
	if pgnm.begins_with("Set"):
		nwPg = settingsPg.instance()
	elif pgnm.begins_with("LED"):
		nwPg = stripsPg.instance()
	elif pgnm.begins_with("MAT"):
		nwPg = matrixPg.instance()
	currentPages.push_back(pgnm)
	nwPg.name = pgnm
	add_child(nwPg)
func BringToFront(pgnm):
	get_node(pgnm).raise()
func remPage(pgnm):
	get_node(pgnm).queue_free()