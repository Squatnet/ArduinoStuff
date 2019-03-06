extends Node

signal connected
signal disconnected
signal data_received
signal reg_finished

var BT
var settings = {"fakeData":false,"adminMode":false,"debugMode":true,"pallettes":19}
var devNums = [0,0,0,0]
var knownDevs = {"Str":{},
				"Mat":{},
				"Ter":{},
				"Rtr":{}
				}
var knownGroups = []
var registerStep = 0
var regStepsArray = []
func saveSettings(): 
	# STORES THE NUMBER OF THE LAST USED FILE
	var f = File.new()
	f.open("user://settings.xws", File.WRITE)
	f.store_var(settings)
	f.close()
	print("saved Settings")
func LoadSettings(): # Loads settings (volume, etc)
	var f = File.new() # Open a new file container
	if f.file_exists("user://settings.xws"): # check the file exists, 
		# it exists, open it
		f.open("user://settings.xws", File.READ)
		if f.is_open():
			# if its open, get the var
			settings = f.get_var() # get the variable
			print(settings)
		else:
			#file exists but not open
			print("GLOBALS: unable to read Settings file!")
			saveSettings()
	else:
		#file doesnt exist, create one
		print("GLOBALS: Settings file does not exist")
		saveSettings()
		print("GLOBALS: Settings file created")
func fakeReg(): # Populates knownDevice with fake data, Fired only when GS.BT is null and connect is pressed
	knownDevs = {
		"Str":{
			"0":["Debug",1],
			"1":["Debug",2],
			"2":["Debug",3],
			"3":["Long",4],
		},
		"Mat":{
			"0":["Debug",21],
			"1":["Long",13],
			"2":["Left",11],
			"3":["Left",9],
		},
		"Ter":{
			"0":["MasterTerm",30],
			"1":["Debug",32],
		},
		"Rtr":{
			"0":["Tree",67],
			"1":["Debug",45]
		}
	}
	registerStep = 2
	doRegStep()
	
func addDvc(typ,dev): # Adds a new device from a bluetooth message, expects a string as typ and an array as dev [String Name, int PjonID]
	var sPos = dev[0]
	var sName = dev[1]
	var sId = dev[2]
	print(knownDevs[typ])
	knownDevs[typ][str(sPos)] = [sName,sId]
func doRegStep():
	if BT && regStepsArray.size() == 0:
		print("GS: doRegStep: regStepsArray Empty!!")
		registerStep += 1
	if registerStep == 1:
		var t = regStepsArray.pop_front()
		print("GS: doRegStep: Sending "+str(t))
		BT.sendData(str(t))
	if registerStep == 2:
		print("GS: doRegStep: Register Queue Finished")
		print("GS: knownDevices: "+str(knownDevs))
		for key in knownDevs:
			var knownDevsSglType = knownDevs[key]
			for item in knownDevsSglType:
				var singleDevice = knownDevsSglType[item]
				print("GS : "+key+" : "+item+" : "+str(singleDevice))
				if !knownGroups.has(singleDevice[0]):
					knownGroups.push_back(singleDevice[0])
		print("GS : Known groups "+str(knownGroups))
		registerStep += 1
	if registerStep == 3:
		emit_signal("reg_finished")
func _ready():
	LoadSettings()
	setupBT(settings.fakeData)
func setupBT(arg):
	if arg == true:
		if(Engine.has_singleton("GodotBluetooth")):
			BT = Engine.get_singleton("GodotBluetooth")
			BT.init(get_instance_id(), true)
	else:
		BT = null
#GodotBluetooth Callbacks
func _on_connected(device_name, device_adress):
	print("GS - Connected")
	emit_signal("connected")
	pass

func _on_disconnected():
	print("GS - Disconnected")
	emit_signal("disconnected")
	knownDevs.clear()
	knownDevs = {"Str":{},"Mat":{},"Ter":{},"Rtr":{}}
	regStepsArray.clear()
	knownGroups.clear()
	registerStep = 0
	pass

func _on_data_received(data_received):
	print("GS - Got data "+str(data_received))
	emit_signal("data_received", data_received)
	pass

func getSetting(arg):
	if settings.has(arg):
		return settings[arg]
	else:
		OS.alert("Setting doesn't exist"+arg,"Whoops!")
func setSetting(arg,val):
	settings[arg] = val
	saveSettings()