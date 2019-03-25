extends Panel

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

func _ready():
	# Called when the node is added to the scene for the first time.
	# Initialization here
	moveLbls()
func moveLbls():
	for i in get_tree().get_nodes_in_group("labels"):
		if i.has_method("set_text"):
			i.set_text("Fuck")
			