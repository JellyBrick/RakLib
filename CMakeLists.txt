cmake_minimum_required (VERSION 3.1)
project (RakLib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS
	source/Common.h
	source/RakLib.h
	source/Session.h
	source/SessionManager.h
	source/network/ByteBuffer.h
	source/network/Packet.h
	source/network/Swap.h
	source/network/UDPSocket.h
	source/packets/Acknowledge.h
	source/packets/CustomPacket.h
	source/packets/DataPacket.h
	source/packets/Info.h
	source/packets/InternalPacket.h
	source/packets/LoginPacket.h
	source/packets/Reply1.h
	source/packets/Reply2.h
	source/packets/Request1.h
	source/packets/Request2.h
	source/packets/UnConnectedPing.h
	source/packets/UnConnectedPong.h)

set(SOURCES
	source/RakLib.cpp
	source/Session.cpp
	source/network/ByteBuffer.cpp
	source/network/Packet.cpp
	source/network/UDPSocket.cpp
	source/packets/Acknowledge.cpp
	source/packets/CustomPacket.cpp
	source/packets/InternalPacket.cpp
	source/packets/LoginPacket.cpp
	source/packets/Reply1.cpp
	source/packets/Reply2.cpp
	source/packets/Request1.cpp
	source/packets/Request2.cpp
	source/packets/UnConnectedPing.cpp
	source/packets/UnConnectedPong.cpp)

include_directories(source/)

add_library(RakLib STATIC ${SOURCES})