extends Control

# class member variables go here, for example:
# var a = 2
# var b = "textvar"
var palletteList = [ 
						[ 
							[244, 118, 98],[244, 118, 98],[101, 157, 190],[101, 157, 190],[142, 213, 133],[142, 213, 133],
							[177, 154, 192],[177, 154, 192],[252, 178, 87],[252, 178, 87],[255, 255, 145],[255, 255, 145],
							[192, 176, 120],[192, 176, 120],[249, 180, 210],[249, 180, 210]
						],
						[
							[83, 159, 190],[83, 159, 190],[1, 48, 106],[1, 48, 106],[100, 189, 54],[100, 189, 54],[3, 91, 3],
							[3, 91, 3],[244, 84, 71],[244, 84, 71],[188,  1,  1],[188,  1,  1],[249, 135, 31],[249, 135, 31],
  							[255, 55,  0],[255, 55, 0]
						]
					]

func _ready():
	setGradient(1)
	pass
func setGradient(num):
	var grad = Gradient.new()
	var pallList = palletteList[num]
	var points = pallList.size()
	var pointsVal = float(1.0/points)
	var currPoint = 0.0
	for i in pallList:
		var newCol = Color(0,0,0)
		newCol.r8 = i[0]
		newCol.g8 = i[1]
		newCol.b8 = i[2]
		print(newCol)
		grad.add_point(currPoint,newCol)
		currPoint+=pointsVal
	var Tex = GradientTexture.new()
	Tex.gradient = grad
	Tex.width = grad.get_point_count()
	$Sprite.set_texture(Tex)
	$Sprite.scale = Vector2(20,140)