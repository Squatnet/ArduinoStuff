extends Node
var UdpSocket = PacketPeerUDP.new()
func _process(delta):
	if UdpSocket.get_available_packet_count() > 0:
		var array_bytes = UdpSocket.get_packet()
		print("Server says "+array_bytes.get_string_from_ascii())
		GS._on_data_received(array_bytes.get_string_from_ascii())
func init(instanceID,required):
	print(instanceID)
	print(required)
func setupUDP():
	if (UdpSocket.listen(GS.getLocalUdpPort()) != OK):
		print("Error listening on port: " + str(GS.getLocalUdpPort()))
	else:
		print("Listening on port: " + str(GS.getLocalUdpPort()))
func getPairedDevices(val):
	if val == true:
		redoSetup()
		connected()
	else:
		disconnected()
func connected():
	OS.delay_msec(100)
	GS._on_connected("UDP","0.0.0.0")
func disconnected():
	OS.delay_msec(100)
	GS._on_disconnected()
func redoSetup():
	UdpSocket.close()
	setupUDP()
func sendData(data):
	if UdpSocket.is_listening():
		UdpSocket.set_dest_address(GS.getRemoteAddr(), GS.getRemoteUdpPort())
		var pac = data.to_ascii()
		print(pac)
		UdpSocket.put_packet(pac)
		print("send data via UDP! "+data)
