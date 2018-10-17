extends Control

onready var btn_connect = get_node("ConnBtn")
var StripScene = load("res://Scenes/LEDString.tscn")
var bt
var matrixes = []
var strips = []
func _ready():
	set_signals()
	if BTHandler.bluetooth:
		bt = true
	
#GodotBluetooth Methods
func _on_connect_pressed():
	if bt:
		BTHandler.bluetooth.getPairedDevices(true)
		OS.delay_msec(100)
	else:
		OS.alert("Module not initialized!")
	pass

#GodotBluetooth Callbacks
func _on_connected():
	btn_connect.set_text("Disconnect")
	
func _on_disconnected():
	btn_connect.set_text("Connect")
	pass

func set_signals():
	BTHandler.connect("connected", self, "_on_connected")
	BTHandler.connect("disconnected", self, "_on_disconnected")
	BTHandler.connect("data_received", self, "_on_data_received")
	btn_connect.connect("pressed", self, "_on_connect_pressed")
	

func _on_LEDA_toggled(button_pressed):
	if button_pressed:
		$LEDB.pressed = false
		$LEDX.pressed = false
		$MATA.pressed = false
		$MATB.pressed = false
		$MATC.pressed = false
		$MATX.pressed = false
		
		_on_LEDB_toggled(false)
		_on_LEDX_toggled(false)
		_on_MATA_toggled(false)
		_on_MATB_toggled(false)
		_on_MATC_toggled(false)
		_on_MATX_toggled(false)
		for child in $LEDA.get_children():
			child.show()
	else:
		for child in $LEDA.get_children():
			child.hide()

func _on_LEDB_toggled(button_pressed):
	if button_pressed:
		$LEDA.pressed = false
		$LEDX.pressed = false
		$MATA.pressed = false
		$MATB.pressed = false
		$MATC.pressed = false
		$MATX.pressed = false
		_on_LEDA_toggled(false)
		_on_LEDX_toggled(false)
		_on_MATA_toggled(false)
		_on_MATB_toggled(false)
		_on_MATC_toggled(false)
		_on_MATX_toggled(false)
		for child in $LEDB.get_children():
			child.show()
	else:
		for child in $LEDB.get_children():
			child.hide()

func _on_LEDX_toggled(button_pressed):
	if button_pressed:
		$LEDB.pressed = false
		$LEDA.pressed = false
		$MATA.pressed = false
		$MATB.pressed = false
		$MATC.pressed = false
		$MATX.pressed = false
		
		_on_LEDB_toggled(false)
		_on_LEDA_toggled(false)
		_on_MATA_toggled(false)
		_on_MATB_toggled(false)
		_on_MATC_toggled(false)
		_on_MATX_toggled(false)
		for child in $LEDX.get_children():
			child.show()
	else:
		for child in $LEDX.get_children():
			child.hide()


func _on_MATA_toggled(button_pressed):
	if button_pressed:
		$LEDB.pressed = false
		$LEDX.pressed = false
		$LEDA.pressed = false
		$MATB.pressed = false
		$MATC.pressed = false
		$MATX.pressed = false
		
		_on_LEDB_toggled(false)
		_on_LEDX_toggled(false)
		_on_LEDA_toggled(false)
		_on_MATB_toggled(false)
		_on_MATC_toggled(false)
		_on_MATX_toggled(false)
		for child in $MATA.get_children():
			child.show()
	else:
		for child in $MATA.get_children():
			child.hide()


func _on_MATB_toggled(button_pressed):
	if button_pressed:
		$LEDB.pressed = false
		$LEDA.pressed = false
		$MATA.pressed = false
		$LEDX.pressed = false
		$MATC.pressed = false
		$MATX.pressed = false
		
		_on_LEDB_toggled(false)
		_on_LEDA_toggled(false)
		_on_MATA_toggled(false)
		_on_LEDX_toggled(false)
		_on_MATC_toggled(false)
		_on_MATX_toggled(false)
		for child in $MATB.get_children():
			child.show()
	else:
		for child in $MATB.get_children():
			child.hide()

func _on_MATC_toggled(button_pressed):
	if button_pressed:
		$LEDB.pressed = false
		$LEDA.pressed = false
		$MATA.pressed = false
		$MATB.pressed = false
		$LEDX.pressed = false
		$MATX.pressed = false
		
		_on_LEDB_toggled(false)
		_on_LEDA_toggled(false)
		_on_MATA_toggled(false)
		_on_MATB_toggled(false)
		_on_LEDX_toggled(false)
		_on_MATX_toggled(false)
		for child in $MATC.get_children():
			child.show()
	else:
		for child in $MATC.get_children():
			child.hide()


func _on_MATX_toggled(button_pressed):
	if button_pressed:
		$LEDB.pressed = false
		$LEDA.pressed = false
		$MATA.pressed = false
		$MATB.pressed = false
		$MATC.pressed = false
		$LEDX.pressed = false
		
		_on_LEDB_toggled(false)
		_on_LEDA_toggled(false)
		_on_MATA_toggled(false)
		_on_MATB_toggled(false)
		_on_MATC_toggled(false)
		_on_LEDX_toggled(false)
		for child in $MATX.get_children():
			child.show()
	else:
		for child in $MATX.get_children():
			child.hide()
