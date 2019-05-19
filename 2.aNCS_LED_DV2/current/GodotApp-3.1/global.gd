extends Node

signal connected
signal disconnected
signal data_received
signal reg_finished
signal locked
#warnings-disable
var version = 0.3
var appDebug = true
var appAdminMode = false
var BTMast
var BT
var UDPMast
var udp = false
var udpRemoteIp = "192.168.1.1"
var udpLocalPort = 1
var udpSendPort = 1
var settings = {"fakeData":false,"adminMode":false,"debugMode":false,"lockout":true,"udp":{"ip":"127.0.0.1","locPort":9000,"remPort":9001}}
var devNums = [0,0,0,0]
var knownDevs = {"Str":{},
				"Mat":{},
				"Ter":{},
				"Rtr":{}
				}
var knownGroups = []
var knownPatterns = ["Turn Off","Turn On","The Lights","Rainbow","Rainbow w/ Glitter","Confetti","Sinelon","BPM","Juggle","ScrollText / Strobe"]
var registerStep = 0
var regStepsArray = []
var pallettes = {}
var pallettesList = []
var mostKnownStrips = 0
func saveSettings(): 
	# STORES THE NUMBER OF THE LAST USED FILE
	var f = File.new()
	f.open("user://settings.xws", File.WRITE)
	settings["udp"] = {"ip":udpRemoteIp,"locPort":udpLocalPort,"remPort":udpSendPort}
	f.store_var(settings)
	f.close()
	setupBT(settings.fakeData)
	print("GS: Saved Settings")
func LoadSettings(): # Loads settings (volume, etc)
	var f = File.new() # Open a new file container
	if f.file_exists("user://settings.xws"): # check the file exists, 
		# it exists, open it
		f.open("user://settings.xws", File.READ)
		if f.is_open():
			# if its open, get the var
			settings = f.get_var() # get the variable
			patcher()
			print("GS: settings: "+str(settings))
			udpRemoteIp = settings.udp.ip
			udpLocalPort = settings.udp.locPort
			udpSendPort = settings.udp.remPort
			if settings.debugMode == true:
				appDebug = true
			else:
				appDebug = false
			if settings.adminMode == true:
				appAdminMode = true
			else:
				appAdminMode = false
			f.close()
		else:
			#file exists but not open
			print("GS: unable to read Settings file!")
			saveSettings()
	else:
		#file doesnt exist, create one
		print("GS: Settings file does not exist")
		saveSettings()
		print("GS: Settings file created")
func savePallettes(): 
	# STORES THE NUMBER OF THE LAST USED FILE
	var f = File.new()
	f.open("user://pallettes.xws", File.WRITE)
	f.store_var(pallettes)
	f.store_var(pallettesList)
	f.close()
	print("GS: Saved Pallettes")
func LoadPallettes(): # Loads settings (volume, etc)
	var f = File.new() # Open a new file container
	if f.file_exists("user://pallettes.xws"): # check the file exists, 
		# it exists, open it
		f.open("user://pallettes.xws", File.READ)
		if f.is_open():
			# if its open, get the var
			pallettes = f.get_var() # get the variable
			#print(pallettes)
			OS.delay_msec(100)
			pallettesList = f.get_var()
			print(pallettesList)
			for key in pallettes:
				if !pallettesList.has(key):
					pallettesList.push_back(key)
			#print(pallettes)
			f.close()
			print("GS: Loaded Pallettes")
		else:
			#file exists but not open
			print("GS: unable to read pallettes file!")
			print("GS: loadPalletes: Possible corruption, Attempting save!")
			savePallettes()
	else:
		#file doesnt exist, create one
		print("GS: Pallettes file does not exist")
		savePallettes()
func patcher():
	if !settings.has("udp"):
		settings["udp"] = {"ip":"127.0.0.1","locPort":9000,"remPort":9001}
		saveSettings()
	
func fakeReg(): # Populates knownDevice with fake data, Fired only when GS.BT is null and connect is pressed
	print("GS: fakeReg: Adding devices")
	knownDevs = {
		"Str":{
			"0":["Debug",1,2],
			"1":["Debug",2,4],
			"2":["Debug",3,1],
			"3":["Long",4,8],
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
	registerStep = 1
	doRegStep()
	
func addDvc(typ,dev): # Adds a new device from a bluetooth message, expects a string as typ and an array as dev [String Name, int PjonID]
	var sPos = str(dev.pop_front())
	var sArr = []
	while !dev.empty():
		sArr.push_back(dev.pop_front())
	print("GS: addDvc: "+str(typ)+" : "+str(dev))
	
	knownDevs[typ][sPos] = sArr
func doRegStep():
	OS.delay_msec(500)
	if BT && regStepsArray.size() == 0:
		print("GS: doRegStep: regStepsArray Empty!!")
		registerStep += 1
	if registerStep == 1:
		var t = regStepsArray.pop_front()
		print("GS: doRegStep: Sending "+str(t))
		BT.sendData(str(t))
	if registerStep == 2:
		print("GS: doRegStep: Register Queue Finished")
		print("GS: knownDevices: "+str(knownDevs.size()))
		for key in knownDevs:
			var knownDevsSglType = knownDevs[key]
			for item in knownDevsSglType:
				var singleDevice = knownDevsSglType[item]
				print(singleDevice)
				if key == "Str":
					if mostKnownStrips < singleDevice[2]:
						mostKnownStrips = singleDevice[2]
				print("GS : "+key+" : "+item+" : "+str(singleDevice))
				if !knownGroups.has(singleDevice[0]):
					knownGroups.push_back(singleDevice[0])
		print("GS : Known groups "+str(knownGroups))
		registerStep += 1
	if registerStep == 3:
		emit_signal("reg_finished")
func _ready():
	LoadSettings()
	LoadPallettes()
	if(Engine.has_singleton("GodotBluetooth")):
		print("GS: Android Detected : Setting BTMast")
		BTMast = Engine.get_singleton("GodotBluetooth")
		BTMast.init(get_instance_id(), true)
		udp = false
	else:
		print("NO BT MODULE")
		BTMast = UDPHandle
		BTMast.init(get_instance_id(), true)
		udp = true
	setupBT(settings.fakeData)
func setupBT(arg):
	var x = int(arg)
	print("X : "+str(x))
	if(Engine.has_singleton("GodotBluetooth")):
		if udp:
			BT = UDPHandle
			print("BT set to UDP handle")
		else:
			print("BT set to BT handle")
			BT = BTMast
		if x == 0:
			print("GS: setupBT: Android: Networked")
			pass # keep current conf
		else:
			print("GS: setupBT: Android: FakeData")
			BT = null # Remove var
	else:
		print("GS: setupBT: No Module")
		BT = BTMast
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
		print("GS: getSetting: "+arg+": "+str(settings[arg]))
		return settings[arg]
	else:
		OS.alert("Setting doesn't exist"+arg,"Whoops!")
func setSetting(arg,val):
	print("GS: setSetting: "+arg+": "+str(val))
	settings[arg] = val
	saveSettings()
func addPallette(palName,palArr):
	if !pallettes.has(palName):
		print("GS: addPallette: "+palName)
		pallettes[palName] = palArr
	if !pallettesList.has(palName):
		pallettesList.push_back(palName)
	savePallettes()
func remPallette(palName):
	if pallettes.has(palName):
		print("GS: remPallette: "+palName)
		pallettes.erase(palName)
	if pallettesList.has(palName):
		pallettesList.erase(palName)
	savePallettes()
func remAllPallettes():
	print("GS: removing all pallettes")
	pallettes.clear()
	pallettesList.clear()
	savePallettes()
func getPallette(palName):
	if pallettes.has(palName):
		print("GS: getPallette: "+palName)
		return pallettes[palName]
func getPalletteById(num):
	print("GS: getPalletteById: "+str(num)+": "+pallettesList[num])
	return pallettesList[num]
func getRemoteAddr():
	return udpRemoteIp
func setRemoteAddr(addr):
	udpRemoteIp = addr
	saveSettings()
func setLocalUdpPort(port):
	udpLocalPort = port
	saveSettings()
func getLocalUdpPort():
	return udpLocalPort
func getRemoteUdpPort():
	return udpSendPort
func setRemoteUdpPort(port):
	udpSendPort = port
	saveSettings()