extends Panel

# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var profFilter = ["FUCK","SHIT","ARSE","CUNT"]
var profReplace = ["F**K","S**T","A**E","C**T"]
var keyOrders = {
				"qwe":["1","2","3","4","5","6","7","8","9","0",
						"Q","W","E","R","T","Y","U","I","O","P",
						"A","S","D","F","G","H","J","K","L","#",
						"Z","X","C","V","B","N","M",".","!","?"],
				"abc":["1","2","3","4","5","6","7","8","9","0",
						"A","B","C","D","E","F","G","H","I","J",
						"K","L","M","N","O","P","Q","R","S","T",
						"U","V","W","X","Y","Z","#",".","!","?"],
				}
						
var keyOrder = "qwe"
var theText = ""
func setupKeys():
	var keyArr = []
	keyArr += keyOrders[keyOrder]
	for i in $GridContainer.get_children():
		i.text = keyArr.pop_front()
# Called when the node enters the scene tree for the first time.
func _ready():
	setupKeys()
	for button in $GridContainer.get_children():
		button.connect("pressed",self,"keyPressed",[button])
func keyPressed(button):
	theText += button.text
	print(button.text)
	profFilt()
func profFilt():
	var trk = 0
	for i in profFilter:
		if theText.find(i) != -1:
			print(theText)
			print("found "+i)
			print("replacement "+profReplace[trk])
			theText = theText.replace(i,profReplace[trk])
			print(theText)
		trk += 1
	$Label.text = theText

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass


func _on_SpaceBar_pressed():
	theText += " "
	$Label.text = theText


func _on_Layou_pressed():
	if $Layou.text == "Qwerty":
		$Layou.text = "ABC"
		keyOrder = "abc"
		setupKeys()
	else:
		$Layou.text = "Qwerty"
		keyOrder = "qwe"
		setupKeys()


func _on_Enter_pressed():
	get_parent().text = theText
	get_parent().get_parent()._on_Messge_text_changed(theText)
	theText = ""
	$Label.text = theText
	hide()


func _on_Button_pressed():
	theText = ""
	$Label.text = theText
	hide()


func _on_Bksp_pressed():
	theText = theText.substr(0,theText.length()-1)
	$Label.text = theText
