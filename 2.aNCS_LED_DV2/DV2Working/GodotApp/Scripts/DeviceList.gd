extends ScrollContainer
var listType = -1
var page = load("res://Scenes/Page.tscn")
func setup(args):
	print("DevList: List Type :"+str(args))
	if args == "List All":
		listType=0
	elif args == "Types":
		listType=1
	elif args == "Groups":
		listType=2
	else:
		print("DevList: "+str(args)+" is not a valid list type")
	var buttonsMade = 0
	if listType == 0: #List ALL
		for key in GS.knownDevs:
			var tmp = GS.knownDevs[key]
			for item in tmp:
				var but = Button.new()
				var thisItm = tmp[item]
				#print(thisItm)
				but.set_text(key+":"+thisItm[0]+":"+str(thisItm[1]))
				but.rect_scale = Vector2(2,2)
				but.connect("pressed",self,"_list_type_0_btn_pressed",[but.text])
				$VBoxContainer.add_child(but)
				buttonsMade += 1
	if listType == 1: # list types
		for key in GS.knownDevs:
			var but = Button.new()
			but.set_text(key)
			but.rect_scale = Vector2(2,2)
			but.connect("pressed",self,"_list_type_1_btn_pressed",[but.text])
			$VBoxContainer.add_child(but)
			buttonsMade += 1
	if listType == 2:
		for i in GS.knownGroups:
			var but = Button.new()
			but.set_text(i)
			but.rect_scale = Vector2(2,2)
			but.connect("pressed",self,"_list_type_2_btn_pressed",[but.text])
			$VBoxContainer.add_child(but)
			buttonsMade += 1
	print("DevList: Made "+str(buttonsMade)+" buttons")

func _list_type_0_btn_pressed(arg):
	print("DevList: BtnPress: "+arg)
	var newPg = page.instance()
	newPg.setup([0,arg])
	get_parent().add_child(newPg)
	pass
func _list_type_1_btn_pressed(arg):
	print("DevList: BtnPress: "+arg)
	var newPg = page.instance()
	newPg.setup([1,arg])
	get_parent().add_child(newPg)
	pass
func _list_type_2_btn_pressed(arg):
	# Show a list for groups here
	print("DevList: BtnPress: "+arg)
	var newPg = page.instance()
	newPg.setup([2,arg])
	get_parent().add_child(newPg)
	pass
func _ready():
	pass
#func _process(delta):
#	# Called every frame. Delta is time since last frame.
#	# Update game logic here.
#	pass
