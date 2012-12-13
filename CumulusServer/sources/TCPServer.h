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


#include "SocketManager.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Mutex.h"


class TCPServer : private Cumulus::SocketHandler {
public:
	TCPServer(Cumulus::SocketManager& manager);
	virtual ~TCPServer();

	bool			start(Poco::UInt16 port);
	bool			running();
	Poco::UInt16	port();
	void			stop();
	Poco::Mutex & mutex();

protected:
	Cumulus::SocketManager&	manager;

private:
	virtual void	clientHandler(Poco::Net::StreamSocket& socket)=0;


	void	onReadable(Poco::Net::Socket& socket);
	void	onError(const Poco::Net::Socket& socket,const std::string& error);

	Poco::Net::ServerSocket		_socket;
	Poco::UInt16				_port;

	Poco::Mutex _mutex;
};

inline bool	TCPServer::running() {
	return _port>0;
}

inline Poco::UInt16	TCPServer::port() {
	return _port;
}

inline Poco::Mutex & TCPServer::mutex() {
	return _mutex;
}
