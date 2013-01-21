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

#include "RTMFPSending.h"
#include "RTMFP.h"
#include "Logs.h"
#include "RTMFPServer.h"
#include "StatManager.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Cumulus {

RTMFPSending::RTMFPSending(RTMFPServer & server): _server(server), packet(_buffer,sizeof(_buffer)),id(0),farId(0),pSocket(NULL) {
	packet.clear(6);
	packet.limit(RTMFP_MAX_PACKET_LENGTH); // set normal limit
}

RTMFPSending::~RTMFPSending() {
	if(pSocket)
		delete pSocket;
}

void RTMFPSending::run() {
    StatManager::global.stat_data._sendPackets++;
    _ts.update();
	if(!pSocket) {
		ERROR("Socket sending impossible on session %u impossible with a null socket",id);
		return;
	}
	RTMFP::Encode(encoder,packet);
	RTMFP::Pack(packet,farId);
	try {
		if(pSocket->sendTo(packet.begin(),(int)packet.length(),address)!=packet.length())
			ERROR("Socket sending error on session %u : all data were not sent",id);
	} catch(Exception& ex) {
		 WARN("Socket sending error on session %u : %s",id,ex.displayText().c_str());
	} catch(exception& ex) {
		 WARN("Socket sending error on session %u : %s",id,ex.what());
	} catch(...) {
		 WARN("Socket sending unknown error on session %u",id);
	}
    Poco::Int64 cost = Poco::Int64(_ts.elapsed());
    StatManager::global.stat_data._sendAccDuration += cost;
    StatManager::global.stat_data._sendPeakCost.CompBigAndSwap(cost);
}




} // namespace Cumulus
