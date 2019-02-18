extends ScrollContainer

func _ready():
	for key in GS.knownDevs:
		var tmp = GS.knownDevs[key]
		for item in tmp:
			var but = Button.new()
			var thisItm = tmp[item]
			print(thisItm)
			but.set_text(key+":"+thisItm[0]+":"+str(thisItm[1]))
			but.rect_scale = Vector2(2,2)
			$VBoxContainer.add_child(but)
#func _process(delta):
#	# Called every frame. Delta is time since last frame.
#	# Update game logic here.
#	pass
