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
#include "PacketReader.h"
#include "PacketWriter.h"
#include "AESEngine.h"
#include "RTMFP.h"
#include "Peer.h"
#include "Invoker.h"
#include "RTMFPReceiving.h"
#include "RTMFPSending.h"
#include "Poco/Net/DatagramSocket.h"

namespace Cumulus {

class RTMFPServer;
class Session {
public:

	Session(RTMFPServer & server, Poco::UInt32 id,
			Poco::UInt32 farId,
			const Peer& peer,
			const Poco::UInt8* decryptKey,
			const Poco::UInt8* encryptKey,
			Invoker& invoker);

	virtual ~Session();

	const Poco::UInt32	id;
	const Poco::UInt32	farId;
	Peer				peer;
	const bool			checked;
	const bool			died;

	bool				nextDumpAreMiddle;

	virtual void		manage(){}

	bool				setEndPoint(Poco::Net::DatagramSocket& socket,const Poco::Net::SocketAddress& address);
	void				decode(Poco::AutoPtr<RTMFPReceiving>& pRTMFPSending);
	void				decode(Poco::AutoPtr<RTMFPReceiving>& pRTMFPSending,AESEngine::Type type);
	void				receive(PacketReader& packet);
	void				send();
	void				send(AESEngine::Type type);
	PacketWriter&		writer();
	virtual void		kill();
	AESEngine::Type		prevAESType();
protected:
	void				send(Poco::UInt32 farId,Poco::Net::DatagramSocket& socket,const Poco::Net::SocketAddress& receiver,AESEngine::Type type=AESEngine::DEFAULT);

	AESEngine			aesDecrypt;
	AESEngine			aesEncrypt;
	Invoker&			invoker;

private:
	virtual void	packetHandler(PacketReader& packet)=0;
	
	PoolThread*						_pReceivingThread;
	Poco::AutoPtr<RTMFPSending>	    _pRTMFPSending;
	PoolThread*						_pSendingThread;

	Poco::FastMutex					_mutex;
	AESEngine::Type					_prevAESType;


	Poco::Net::DatagramSocket	_socket;

	RTMFPServer & _server;
};

inline AESEngine::Type Session::prevAESType() {
	Poco::ScopedLock<Poco::FastMutex> lock(_mutex);
	return _prevAESType;
}

inline void Session::decode(Poco::AutoPtr<RTMFPReceiving>& pRTMFPReceiving) {
	decode(pRTMFPReceiving,farId==0 ? AESEngine::SYMMETRIC : AESEngine::DEFAULT);
}

inline void Session::send() {
	send(farId,_socket,peer.address,prevAESType());
}

inline void Session::send(AESEngine::Type type) {
	send(farId,_socket,peer.address,type);
}


inline PacketWriter& Session::writer() {
	return _pRTMFPSending->packet;
}


} // namespace Cumulus
