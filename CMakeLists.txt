cmake_minimum_required (VERSION 3.1)
project (RakLib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
	source/RakLib.cpp
	source/Session.cpp
	source/network/ByteBuffer.cpp
	source/network/Packet.cpp
	source/network/UDPSocket.cpp
	source/protocol/Acknowledge.cpp
	source/protocol/CustomPacket.cpp
	source/protocol/InternalPacket.cpp
	source/protocol/LoginPacket.cpp
	source/protocol/Reply1.cpp
	source/protocol/Reply2.cpp
	source/protocol/Request1.cpp
	source/protocol/Request2.cpp
	source/protocol/UnConnectedPing.cpp
	source/protocol/UnConnectedPong.cpp)

include_directories(include/)

add_library(RakLib STATIC ${SOURCES})