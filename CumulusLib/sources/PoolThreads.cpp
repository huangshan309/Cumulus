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

#include "PoolThreads.h"
#include "Poco/Environment.h"

using namespace std;
using namespace Poco;

namespace Cumulus {

PoolThreads::PoolThreads(UInt32 threadsAvailable):_threads(threadsAvailable==0 ? Environment::processorCount() : threadsAvailable)  {
	vector<PoolThread*>::iterator it;
	for(UInt16 i=0;i<_threads.size();++i)
		_threads[i] = new PoolThread();
}

PoolThreads::~PoolThreads() {
	vector<PoolThread*>::iterator it;
	for(it=_threads.begin();it!=_threads.end();++it)
		delete *it;
}

void PoolThreads::clear() {
	vector<PoolThread*>::iterator it;
	for(it=_threads.begin();it!=_threads.end();++it)
		(*it)->clear();
}

void PoolThreads::launch() {
	vector<PoolThread*>::iterator it;
	for(it=_threads.begin();it!=_threads.end();++it) 
		(*it)->start();
}

PoolThread* PoolThreads::enqueue(AutoPtr<WorkThread> pWork,PoolThread* pThread) {

	UInt32 queue=0;
	if(!pThread) {
		vector<PoolThread*>::const_iterator it;
		for(it=_threads.begin();it!=_threads.end();++it) {
			UInt32 newQueue = (*it)->queue();
			if(!pThread || newQueue<=queue) {
				pThread = *it;
				if((queue=newQueue)==0)
					break;
			}
		}
	}

	if (queue >= 10000)
		throw Exception("PoolThreads 10000 limit runnable entries for every thread reached");

	pThread->push(pWork);
	return pThread;
}


} // namespace Cumulus
