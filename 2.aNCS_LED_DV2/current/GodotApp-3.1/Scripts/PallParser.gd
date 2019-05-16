extends Control
var fileToParse
var parsedObject = {}
func parse():
	var f = File.new()
	if GS.pallettes.empty():
		if f.file_exists("res://def_pallettes.txt"): # check the file exists, 
#		# it exists, open it
			f.open("res://def_pallettes.txt", File.READ)
			print("PallParser: Read def pallettes")
		else:
			OS.alert("DEFAULT FILE MISSING","alert")
	else:
		if f.file_exists("user://custom_pallettes.txt"):
			f.open("user://custom_pallettes.txt", File.READ)
			print("PallParser: read user pallettes")
		else:
			OS.alert("USER FILE NOT FOUND","alert")
	if f.is_open():
		var incomingCode = f.get_as_text() # get the variable
		while incomingCode.length() > 1:
			print("PallParser: Input Length"+str(incomingCode.length()))
			var winkyBoyPos = incomingCode.find(";")
			if winkyBoyPos == -1:
				winkyBoyPos = incomingCode.find("}")
			var singlePallette = incomingCode.substr(0,winkyBoyPos+1)
			incomingCode.erase(0,winkyBoyPos+1)
			singlePallette.erase(0,singlePallette.find("(")+1)
			var pallName = singlePallette.substr(0,singlePallette.find(")"))
			pallName.strip_edges()
			print("PallParser: Name: "+pallName)
			singlePallette.erase(0,singlePallette.find(",")-2)
			var rawPallArr = singlePallette.substr(0,singlePallette.find("}"))
			var tmpPallArr = Array(rawPallArr.split_floats(",",false))
			var fnshdPallArr = []
			while tmpPallArr.size() > 0:
				var ignored = tmpPallArr.pop_front()
				var sglCol = [tmpPallArr.pop_front(),tmpPallArr.pop_front(),tmpPallArr.pop_front()]
				fnshdPallArr.push_back(sglCol)
			if !parsedObject.has(pallName):
				parsedObject[pallName] = fnshdPallArr
		for key in parsedObject:
			print("PallParser: Adding Pallette with name : "+key)
			print("PallParser: Pallette has "+str(parsedObject[key].size())+" points")
			GS.addPallette(key,parsedObject[key])
			OS.delay_msec(100)
	else:
		OS.alert("NO File Opened")