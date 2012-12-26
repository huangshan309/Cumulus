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
#include "Session.h"
#include "Gateway.h"
#include "Entities.h"
#include "Poco/Mutex.h"
#include <cstddef>

namespace Cumulus {

class Sessions
{
public:
	typedef std::map<Poco::UInt32,Session*>::const_iterator Iterator;

	Sessions(Gateway& gateway);
	virtual ~Sessions();

	Poco::UInt32	count();
	Poco::UInt32	nextId() const;

	Session* find(Poco::UInt32 id);
	Session* find(const Poco::UInt8* peerId);
	Session* find(const Poco::Net::SocketAddress& address);
	
	void	 changeAddress(const Poco::Net::SocketAddress& oldAddress,Session& session);
	Session* add(Session* pSession);

	Iterator begin() const;
	Iterator end() const;
	
	void		manage();
	void		clear();

public:
	Poco::Mutex	mutex;
	Poco::UInt32 peakCount;

private:
	void    remove(std::map<Poco::UInt32,Session*>::iterator it);

	struct Compare {
	   bool operator()(const Poco::Net::SocketAddress& a,const Poco::Net::SocketAddress& b) const {
		   return (a.host()==b.host() && a.port()==b.port()) ? 0 : (a.port()<b.port());
	   }
	};

	Poco::UInt32					_nextId;
	std::map<Poco::UInt32,Session*>						_sessions;
	Entities<Session>::Map								_sessionsByPeerId;
	std::map<Poco::Net::SocketAddress,Session*,Compare>	_sessionsByAddress;
	Gateway&						_gateway;
	Poco::UInt32					_oldCount;
	std::map<Poco::UInt32,Session*>::iterator  _sss_scan_pos;
};


inline Poco::UInt32	Sessions::nextId() const {
	return _nextId;
}

inline Sessions::Iterator Sessions::begin() const {
	return _sessions.begin();
}

#if 0
inline Poco::UInt32 Sessions::count() const {
	return _sessions.size();
}
#endif

inline Sessions::Iterator Sessions::end() const {
	return _sessions.end();
}


} // namespace Cumulus
