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

#include "TCPServer.h"
#include "Logs.h"

using namespace std;
using namespace Cumulus;
using namespace Poco;
using namespace Poco::Net;


TCPServer::TCPServer(SocketManager& manager) : _port(0),manager(manager),_pSocket(NULL) {
}


TCPServer::~TCPServer() {
	stop();
}

bool TCPServer::start(UInt16 port) {
	if(port==0) {
		ERROR("TCPServer port have to be superior to 0");
		return false;
	}
	stop();
	try {
		_pSocket = new ServerSocket();
		_pSocket->bind(port, true);
		_pSocket->setLinger(false,0);
		_pSocket->setBlocking(false);
		_pSocket->listen();
	} catch(Exception& ex) {
		ERROR("TCPServer starting error: %s",ex.displayText().c_str())
		return false;
	}
	_port=port;
	manager.add(*_pSocket,*this);
	return true;
}

void TCPServer::stop() {
	if(_port==0 || !_pSocket)
		return;
	manager.remove(*_pSocket);
	delete _pSocket;
	_pSocket = NULL;
}

void TCPServer::onReadable(Socket& socket) {
	try {
		StreamSocket ss = _pSocket->acceptConnection();
		// enabe nodelay per default: OSX really needs that
		ss.setNoDelay(true);
		clientHandler(ss);
	} catch(Exception& ex) {
		WARN("TCPServer socket acceptation: %s",ex.displayText().c_str());
	}
}

void TCPServer::onError(const Socket& socket,const string& error) {
	ERROR("TCPServer socket: %s",error.c_str())
}
