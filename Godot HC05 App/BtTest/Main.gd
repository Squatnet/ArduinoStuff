extends Control

onready var btn_connect = get_node("ConnBtn")
onready var str_btn_send = get_node("LEDString/SendBtn")
onready var str_btn_color = get_node("LEDString/ColorBtn")
onready var str_color_picker = get_node("LEDString/ColorPicker")
onready var str_btn_option = get_node("LEDString/OptionButton")
onready var str_popup_men = get_node("LEDString/OptionButton/PopupMEN")
var bt
var color = "ffffff"
var colorBtn
var option = 0
var mode
var auto = 1
var secs = 10
func _ready():	
	set_signals()
	if BTHandler.bluetooth:
		bt = true
	$LEDString/OptionButton/PopupMEN.add_item("OFF")
	$LEDString/OptionButton/PopupMEN.add_item("ALL ON")
	$LEDString/OptionButton/PopupMEN.add_item("theLights")
	$LEDString/OptionButton/PopupMEN.add_item("rainbow")
	$LEDString/OptionButton/PopupMEN.add_item("rainbowGlitter")
	$LEDString/OptionButton/PopupMEN.add_item("confetti")
	$LEDString/OptionButton/PopupMEN.add_item("sinelon")
	$LEDString/OptionButton/PopupMEN.add_item("bpm")
	$LEDString/OptionButton/PopupMEN.add_item("juggle")
	$LEDString/AutoBtn.set_text("AUTO ON")
	$LEDString/LineEdit.set_text(str(secs))
#GodotBluetooth Methods
func _on_connect_pressed():
	if bt:
		BTHandler.bluetooth.getPairedDevices(true)
	else:
		OS.alert("Module not initialized!")
	pass
func _on_send_pressed():
	if bt:
		if !mode:
			BTHandler.bluetooth.sendData("POLL")
			OS.delay_msec(100)
		BTHandler.bluetooth.sendData("LED"+str(option)+color)
	else:
		print("LED"+str(option)+color)
		OS.alert("Module not initialized!")
func _on_color_pressed():
	if !colorBtn:
		colorBtn = true
		$LEDString/Sprite.show()
		$LEDString/Label.show()
	else:
		colorBtn = false
		$LEDString/Sprite.hide()
		$LEDString/Label.hide()
func _on_opt_pressed():
	$LEDString/OptionButton/PopupMEN.popup()
#GodotBluetooth Callbacks
func _on_connected():
	btn_connect.set_text("Disconnect")
	$LEDString.show()
	pass

func _on_disconnected():
	btn_connect.set_text("Connect")
	$LEDString.hide()
	pass

func _on_data_received(data_received):
	print(data_received)
	mode = data_received
	$Title.set_text($Title.get_text()+" Mode:"+mode)

#Godot Signals
func set_signals():
	BTHandler.connect("connected", self, "_on_connected")
	BTHandler.connect("disconnected", self, "_on_disconnected")
	BTHandler.connect("data_received", self, "_on_data_received")

	btn_connect.connect("pressed", self, "_on_connect_pressed")
	str_btn_send.connect("pressed", self, "_on_send_pressed")
	str_btn_color.connect("pressed", self, "_on_color_pressed")
	str_btn_option.connect("pressed", self, "_on_opt_pressed")
	

func _on_PopupMEN_index_pressed(index):
	print(index)
	$LEDString/OptionButton.set_text($LEDString/OptionButton/PopupMEN.get_item_text(index))
	option = index
func _on_ColorPicker_color_changed(val):
	$LEDString/ColorBtn/ColorRect.color = val
	print(val)
	color = val.to_html(false)



func _on_AutoBtn_pressed():
	if bt:
		if auto == 1:
			auto = 0
			$LEDString/AutoBtn.set_text("AUTO OFF")
		else:
			auto=1
			$LEDString/AutoBtn.set_text("AUTO ON")
		BTHandler.bluetooth.sendData("LEDX"+str(auto))
	else:
		OS.alert("Module not initialised")


func _on_SecsBtn_pressed():
	if bt:
		BTHandler.bluetooth.sendData("LEDS"+str(secs))
	else:
		OS.alert("Module not initialized!")
	pass



func _on_LineEdit_text_changed(new_text):
	var tmp = $LEDString/LineEdit.text
	secs = tmp.to_int()


func _on_ToolButton_pressed():
	var col = getImPix()
	color = col.to_html(false)
	$LEDString/ColorBtn/ColorRect.color =color
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
	var mp = get_node("LEDString/Sprite").get_local_mouse_position()
	img.lock()
	var col = img.get_pixel(mp.x,mp.y)
	img.unlock()
	print("COL: "+str(col))
	return col