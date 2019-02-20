extends Control

var currCol = [0,0,0]
var pageType = 0
var pageTypeText = ["Single Device","Type","Group"]
var deviceClass = 0
var deviceClassText = ["Str","Mat","Ter","Rou"]
var commandPrefix = "Ctl,"
var commandDevClassOrID = ""
var commandGrpName = ""
var commandToRelay = ""
var commandSuffix = ","
func _ready():
	# Called when the node is added to the scene for the first time.
	# Initialization here
	pass
func setup(args):
	pageType = args[0]
	if pageType == 0:
		$PAGE.set_text("Single Device"+str(args[1]))
		var x = str(args[1]).split(":")
		print(x)
		commandDevClassOrID += str(x[2])+","
	elif pageType == 1:
		$PAGE.set_text("Device Type :"+str(args[1]))
		commandDevClassOrID += str(args[1])+","
	elif pageType == 2:
		$PAGE.set_text("Group :"+str(args[1]))
		commandGrpName += str(args[1])+","
	var commandToSend = commandPrefix+commandDevClassOrID+commandGrpName+commandToRelay+commandSuffix
	print("PAGE: commandToSend: "+commandToSend)
func setCol(col):
	currCol = col
	print("PAGE: Color Updated: "+str(currCol))
#func _process(delta):
#	# Called every frame. Delta is time since last frame.
#	# Update game logic here.
#	pass


func _on_CloseBtn_pressed():
	queue_free()