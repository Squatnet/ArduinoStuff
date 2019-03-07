extends Control
var numPallettes = 0
var currPallette = 0
func _ready():
	setGradient(0)
	numPallettes = GS.pallettes.size()
func setGradient(num):
	$NumLbl.set_text(str(currPallette))
	var grad = Gradient.new()
	var pallNm = GS.getPalletteById(num)
	$NameLbl.set_text(pallNm)
	var pall = GS.getPallette(pallNm)
	var points = pall.size()
	var pointsVal = float(1.0/points)
	var currPoint = 0.0
	for i in pall:
		var newCol = Color(0,0,0)
		newCol.r8 = i[0]
		newCol.g8 = i[1]
		newCol.b8 = i[2]
		#print(newCol)
		grad.add_point(currPoint,newCol)
		currPoint+=pointsVal
	var Tex = GradientTexture.new()
	Tex.gradient = grad
	Tex.width = 18
	$Sprite.set_texture(Tex)
	$Sprite.scale = Vector2(28,140)

func _on_PrevBtn_pressed():
	currPallette -= 1
	if currPallette < 0:
		currPallette = numPallettes-1
	setGradient(currPallette)
	get_parent().setPall(currPallette)

func _on_NextBtn_pressed():
	currPallette += 1
	if currPallette >= numPallettes:
		currPallette = 0
	setGradient(currPallette)
	get_parent().setPall(currPallette)
