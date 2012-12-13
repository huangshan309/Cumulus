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

#include "Servers.h"
#include "Logs.h"
#include "Poco/StringTokenizer.h"

using namespace std;
using namespace Cumulus;
using namespace Poco;
using namespace Poco::Net;


Servers::Servers(UInt16 port,ServerHandler& handler,SocketManager& manager,const string& targets) : TCPServer(manager),_port(port),_handler(handler),_manageTimes(1) {
	if(port>0)
		NOTE("Servers incoming connection enabled on port %hu",port)
	else if(!_targets.empty())
		NOTE("Servers incoming connection disabled (servers.port==0)")

	StringTokenizer tokens(targets,";",StringTokenizer::TOK_IGNORE_EMPTY | StringTokenizer::TOK_TRIM);
	StringTokenizer::Iterator it;
	for(it=tokens.begin();it!=tokens.end();++it)
		_targets.insert(new ServerConnection(*it,manager,_handler,*this));
}

Servers::~Servers() {
	stop();

	ScopedLock<Mutex> lock(mutex());
	Iterator it;
	for(it=_targets.begin();it!=_targets.end();++it)
		delete (*it);
}

void Servers::manage() {
	ScopedLock<Mutex> lock(mutex());
	if(_targets.empty() || (--_manageTimes)!=0)
		return;
	_manageTimes = 5; // every 10 sec
	Iterator it;
	for(it=_targets.begin();it!=_targets.end();++it)
		(*it)->connect();
}

void Servers::start() {
	if(_port>0)
		TCPServer::start(_port);
}


void Servers::stop() {
	TCPServer::stop();

	ScopedLock<Mutex> lock(mutex());
	_connections.clear();
	targets._connections.clear();
	initiators._connections.clear();
	set<ServerConnection*>::iterator it;
	for(it=_clients.begin();it!=_clients.end();++it)
		delete (*it);
	_clients.clear();
}

void Servers::connection(ServerConnection& server) {
	ScopedLock<Mutex> lock(mutex());
	_connections.insert(&server);
	if(server.isTarget)
		targets._connections.insert(&server);
	else
		initiators._connections.insert(&server);
	NOTE("Connection etablished with %s server ",server.publicAddress.c_str())
}

bool Servers::disconnection(ServerConnection& server) {
	ScopedLock<Mutex> lock(mutex());
	_connections.erase(&server);
	_clients.erase(&server);
	if(server.isTarget)
		targets._connections.erase(&server);
	else
		initiators._connections.erase(&server);
	NOTE("Disconnection from %s server ",server.publicAddress.c_str())
	if(_targets.find(&server)==_targets.end())
		return true;
	return false;
}
