extends Control
onready var str_btn_send = get_node("SendBtn")
onready var str_btn_color = get_node("ColorBtn")
onready var str_btn_option = get_node("OptionButton")
onready var str_popup_men = get_node("OptionButton/PopupMEN")
var ThisStrip
var ThisId
var color = "ffffff"
var colorBtn
var option = 0
var mode
var auto = 1
var secs = 10
var msg = ""
var bt
func _ready():
	if BTHandler.bluetooth:
		bt = true
	$OptionButton/PopupMEN.add_item("OFF")
	$OptionButton/PopupMEN.add_item("ALL ON")
	$OptionButton/PopupMEN.add_item("theLights")
	$OptionButton/PopupMEN.add_item("rainbow")
	$OptionButton/PopupMEN.add_item("rainbowGlitter")
	$OptionButton/PopupMEN.add_item("confetti")
	$OptionButton/PopupMEN.add_item("sinelon")
	$OptionButton/PopupMEN.add_item("bpm")
	$OptionButton/PopupMEN.add_item("juggle")
	$OptionButton/PopupMEN.add_item("scrollText")
	$AutoBtn.set_text("AUTO ON")
	$LineEdit.set_text("10")
func setup(id,data):
	ThisId = id
	ThisStrip = data
	refresh()
func refresh():
	_on_PopupMEN_index_pressed(ThisStrip.pattern)
	_on_ColorPicker_color_changed(ThisStrip.color)
	setAuto(ThisStrip.auto)
	$LineEdit.text = str(ThisStrip.timeout)
func _on_PopupMEN_index_pressed(index):
	print(index)
	$OptionButton.set_text($OptionButton/PopupMEN.get_item_text(index))
	option = index
func _on_ColorPicker_color_changed(val):
	$ColorBtn/ColorRect.color = val
	print(val)
	color = val.to_html(false)
func setAuto(val):
	auto = val
func ChangeAuto():
	if auto == 1:
			auto = 0
			$AutoBtn.set_text("AUTO OFF")
	else:
		auto=1
		$AutoBtn.set_text("AUTO ON")
func getAuto():
	if auto == 1:
			$AutoBtn.set_text("AUTO ON")
	else:
		$AutoBtn.set_text("AUTO OFF")
func _on_AutoBtn_pressed():
	if bt:
		ChangeAuto()
		BTHandler.bluetooth.sendData(get_parent().get_name()+"X"+str(auto))
	else:
		print(get_name()+"X"+str(auto))
		OS.alert("Module not initialised")


func _on_SecsBtn_pressed():
	if bt:
		BTHandler.bluetooth.sendData(get_name()+"S"+str(secs))
	else:
		print(get_name()+"S"+str(secs))
		OS.alert("Module not initialized!")
	pass



func _on_LineEdit_text_changed(new_text):
	var tmp = $LineEdit.text
	secs = tmp.to_int()


func _on_ToolButton_pressed():
	var col = getImPix()
	color = col.to_html(false)
	$ColorBtn/ColorRect.color =color
	print(color)
func getImPix():
	var cfg = ConfigFile.new()
	cfg.load("res://1024px-RGB_color_wheel_360.svg.png.import")
	print(cfg.get_value("remap", "path"))

	var srcpath = cfg.get_value("remap", "path")

	var fin = File.new()
	fin.open(srcpath, File.READ)

	var header = fin.get_32();
	var width = fin.get_32();
	var height = fin.get_32();
	var flags = fin.get_32();
	var data_format = fin.get_32();

	print("w: ", width, " h:", height)

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
	print("COL: "+str(col))
	return col
func _on_send_pressed():
	if bt:
		BTHandler.bluetooth.sendData(get_name()+str(option)+color)
	else:
		print(get_name()+str(option)+color)
		OS.alert("Module not initialized!")
func _on_color_pressed():
	if !colorBtn:
		colorBtn = true
		$Sprite.show()
		$Label.show()
	else:
		colorBtn = false
		$Sprite.hide()
		$Label.hide()
func _on_OptionButton_pressed():
	$OptionButton/PopupMEN.popup()


func _on_LineEdit2_text_changed(new_text):
	msg = $LineEdit2.text


func _on_MsgBtn_pressed():
	if bt:
		BTHandler.bluetooth.sendData(get_name()+"M"+msg)
	else:
		print(get_name()+"M"+msg)
		OS.alert("Module not initialized!")
	pass
