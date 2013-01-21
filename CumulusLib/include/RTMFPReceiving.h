/* 
	Copyright 2010 OpenRTMFP
 
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License received along this program for more
	details (or else see http://www.gnu.org/licenses/).

	This file is a part of Cumulus.
*/

#pragma once

#include "Cumulus.h"
#include "AESEngine.h"
#include "PacketReader.h"
#include "Task.h"
#include "WorkThread.h"
#include "Poco/Net/DatagramSocket.h"


namespace Cumulus {

class RTMFPServer;
class RTMFPReceiving : public WorkThread, /* private */ public Task {
public:
	RTMFPReceiving(RTMFPServer& server,Poco::Net::DatagramSocket& socket);
	~RTMFPReceiving();

	Poco::UInt32				id;
	AESEngine					decoder;
	Poco::Net::SocketAddress	address;
	Poco::Net::DatagramSocket	socket;
	PacketReader*				pPacket;

private:
	void						handle();
	void						run();

	RTMFPServer&				_server;
	Poco::UInt8					_buff[PACKETRECV_SIZE];
    Poco::Timestamp             _ts; 
};

} // namespace Cumulus
