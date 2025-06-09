all: server.exe client.exe

server.exe: server.cpp
	cl /EHsc /std:c++17 server.cpp /link ws2_32.lib

client.exe: client.cpp
	cl /EHsc /std:c++17 client.cpp /link ws2_32.lib
