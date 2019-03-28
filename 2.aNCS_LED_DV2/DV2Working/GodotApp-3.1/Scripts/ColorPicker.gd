extends Control

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

func _ready():
	# Called when the node is added to the scene for the first time.
	# Initialization here
	pass

func getImPix():
	var cfg = ConfigFile.new()
	cfg.load("res://1024px-RGB_color_wheel_360.svg.png.import")
	#print(cfg.get_value("remap", "path"))
	
	var srcpath = cfg.get_value("remap", "path")

	var fin = File.new()
	fin.open(srcpath, File.READ)

	var header = fin.get_32();
	var width = fin.get_32();
	var height = fin.get_32();
	var flags = fin.get_32();
	var data_format = fin.get_32();

	#print("w: ", width, " h:", height)

	# assume it's RGBA8 format:
	var buffsize = width * height * 4;
	var imgdata = fin.get_buffer(buffsize);

	fin.close()

	# copy the data to Image object
	var img = Image.new()
	img.create_from_data(width, height, false, Image.FORMAT_RGBA8, imgdata)
	#var im = img.get_texture().get_data()
	#print(im)
	var mp = get_node("Sprite").get_local_mouse_position()
	img.lock()
	var col = img.get_pixel(mp.x,mp.y)
	img.unlock()
	print("CLRPCK: Picked: "+str(col))
	return col

func _on_ToolButton_pressed():
	var c = getImPix()
	print("CLRPCK: ClrAsInt: R:"+str(c.r8)+" G:"+str(c.g8)+" B:"+str(c.b8)+" A:"+str(c.a8))
	var sendBack = [c.r8,c.g8,c.b8]
	if get_parent().has_method("setCol"):
		get_parent().setCol(sendBack)
