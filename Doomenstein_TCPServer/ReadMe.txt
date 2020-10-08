Jonathan Sarasua
SD3-A2
Due: 10/8/2020

Console Commands:
~: Toggle Console

TCP
Server Commands
	StartTCPServer: starts server
	StopTCPServer: Stops listening server
	SendMessageToClient msg="MESSAGE"

Client Commands
	TCPClientConnect host=HOSTNAME port=PORT | host defaults to "", port defaults to 48000
	DisconnectClient
	SendMessageToServer msg="MESSAGE"

UDP
OpenUDPPort bindPort=<PortNumber> sendToPort=<PortNumber>
SendUDPMessage msg="<Message>"
CloseUDPPort bindPort=<PortNumber>