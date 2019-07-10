extends Control

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

func getImPix():
	var theimage = $Sprite.get_texture().get_data()
	theimage.lock()
	var gMoPos = get_global_mouse_position()
	var lMoPos = $Sprite.get_local_mouse_position()
	print(str(gMoPos)+" "+str(lMoPos))
	var col = theimage.get_pixel(lMoPos.x,lMoPos.y)
	col.a = 1
	$ColorRect.modulate = col
	
	var thePix = theimage.get_pixel(lMoPos.x,lMoPos.y)
	print(thePix)
	theimage.unlock()
	return thePix
func _on_ToolButton_pressed():
	var c = getImPix()
	print("CLRPCK: ClrAsInt: R:"+str(c.r8)+" G:"+str(c.g8)+" B:"+str(c.b8)+" A:"+str(c.a8))
	var sendBack = [c.r8,c.g8,c.b8]
	if get_parent().has_method("setCol"):
		get_parent().setCol(sendBack)