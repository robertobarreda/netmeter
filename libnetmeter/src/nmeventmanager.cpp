/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "nmeventmanager.h"
#include "nmmodulemanager.h"
#include<time.h>

NMEventManager *NMEventManager::nmeventmanager = 0;


/**
  \fn NMEventManager::self() 
  \brief Return an object of EventManager.
*/
NMEventManager *NMEventManager::self() 
{
	if(nmeventmanager)
		return nmeventmanager;
	return nmeventmanager = new NMEventManager("eventmanager",0); 
}


NMEventManager::NMEventManager(const char *name, NMParent *nmparent)
 : NMParent(name, nmparent),
   m_exit(false),
   m_socketlist(0),
   m_externallist(0), m_timerlist(0)
{
}

/**
  \fn NMEventManager::~NMEventManager()
  \brief Destroy NMEventManager object
*/
NMEventManager::~NMEventManager()
{
	if(m_socketlist)
		delete m_socketlist;
	if(m_externallist)
		delete m_externallist;
}


/**
  \fn NMEventManager::eventLoop()
  \brief Begin the main event loop.
*/
void NMEventManager::runLoop()
{
	if( m_exit) return;
	cout << "EventLoop runnning" << '\n';
	while( !m_exit) {
		activateSockets();
		activateExternals();
		activateTimers();
		dispatchEvents();
		usleep(500);
	}
}

/**
  \fn NMEventManager::eventDispatcher()
  \brief Dispath events.
*/
void NMEventManager::dispatchEvents()
{	
	list<NMParent *> queue;
	NMEvent *event;

	queue = events();

	while(queue.size()) {
		event = (NMEvent *)queue.front();
		queue.pop_front();
		processEvent(event);
	}
}

/**
  \fn NMEventManager::processEvent(NMEvent *event)
  \brief Handle one event.
*/
void NMEventManager::processEvent(NMEvent *event)
{	
	if( event->isConnected() && event->isActived() && !event->callBack()->isRunning())
		event->callBack()->start();
	event->desActived();
}


/**
  \fn NMEventManager::exitLoop()
  \brief Stop the event loop.
*/
void NMEventManager::exitLoop()
{
	m_exit = true;
}


void NMEventManager::run()
{
	runLoop();
}

/**
\fn NMEventManager::registerSocketListener( NMSocketListener *listener);
\brief Register a new socket to EventManager
*/
void NMEventManager::registerSocketListener( NMSocketListener *listener)
{
	if( !(listener->socket())->isOpen()) {
		cerr << "EventManager Warning: Socket is not open" << endl;
		return;
	}

	m_sockmutex.lock();
	
	if( !m_socketlist)
		m_socketlist = new list<NMSocketListener *>;
	else {
		list<NMSocketListener *>::iterator iter;
		for ( iter = m_socketlist->begin(); iter != m_socketlist->end(); iter++ ) {
			if( *iter == listener) {
				cerr << "EventManager Warning: SocketListner exsist" << endl;
				m_sockmutex.unlock();
	 			return;
			}
		}
	}
	m_socketlist->push_front( listener);
	m_sockmutex.unlock();
}

/**
\fn NMEventManager::unregisterSocketListener( NMSocketListener *listener)
\brief Unregister socket from EventManager
*/
void NMEventManager::unregisterSocketListener( NMSocketListener *listener)
{
	if( !m_socketlist) return;

	list<NMSocketListener *>::iterator iter;

	m_sockmutex.lock();
	
	for ( iter = m_socketlist->begin(); iter != m_socketlist->end(); iter++ ) {
		if( *iter == listener) {
			m_socketlist->remove( listener);
			break;
		}
	}
	
	if( !m_socketlist->size()) {
		delete m_socketlist;
		m_socketlist = 0;
	}

	m_sockmutex.unlock();	
}

/**
\fn NMEventManager::registerExternalListener( NMExternalListener *listener)
\brief Register a new external to EventManager
*/
void NMEventManager::registerExternalListener( NMExternalListener *listener)
{
// 	if( !( listener->external())->isRunning()) {
// 		cerr << "EventManager Warning: External is not running" << endl;
// 		return;
// 	}

	m_externalmutex.lock();
	
	if( !m_externallist)
		m_externallist = new list<NMExternalListener *>;
	else {
		list<NMExternalListener *>::iterator iter;
		for ( iter = m_externallist->begin(); iter != m_externallist->end(); iter++ ) {
			if( *iter == listener) {
				cerr << "EventManager Warning: ExternalListner exsist" << endl;
				m_externalmutex.unlock();
	 			return;
			}
		}
	}
	m_externallist->push_front( listener);
	m_externalmutex.unlock();
}

/**
\fn NMEventManager::unregisterExternalListener( NMExternalListener *listener)
\brief Unregister external from EventManager
*/
void NMEventManager::unregisterExternalListener( NMExternalListener *listener)
{
	if( !m_externallist) return;

	list<NMExternalListener *>::iterator iter;

	m_externalmutex.lock();
	
	for ( iter = m_externallist->begin(); iter != m_externallist->end(); iter++ ) {
		if( *iter == listener) {
			m_externallist->remove( listener);
			break;
		}
	}
	if( !m_externallist->size()) {
		delete m_externallist;
		m_externallist = 0;
	}
	m_externalmutex.unlock();	
}

/**
\fn NMEventManager::registerTimerListener( NMTimerListener *listener)
\brief Register a new timer to EventManager
*/
void NMEventManager::registerTimerListener( NMTimerListener *listener)
{
	if( !( listener->timer())->isActive()) {
		cerr << "EventManager Warning: Timer is not active" << endl;
		return;
	}

	m_timermutex.lock();
	
	if( !m_timerlist)
		m_timerlist = new list<NMTimerListener *>;
	else {
		list<NMTimerListener *>::iterator iter;
		for ( iter = m_timerlist->begin(); iter != m_timerlist->end(); iter++ ) {
			if( *iter == listener) {
				cerr << "EventManager Warning: Timer exsist" << endl;
				m_timermutex.unlock();
	 			return;
			}
		}
	}
	m_timerlist->push_front( listener);
	m_timermutex.unlock();
}

/**
\fn NMEventManager::unregisterTimerListener( NMTimerListener *listener)
\brief Unregister timer from EventManager
*/
void NMEventManager::unregisterTimerListener( NMTimerListener *listener)
{
	if( !m_timerlist) return;

	list<NMTimerListener *>::iterator iter;

	m_timermutex.lock();
	
	for ( iter = m_timerlist->begin(); iter != m_timerlist->end(); iter++ ) {
		if( *iter == listener) {
			m_timerlist->remove( listener);
			break;
		}
	}
	
	if( !m_timerlist->size()) {
		delete m_timerlist;
		m_timerlist = 0;
	}

	m_timermutex.unlock();	
}


void NMEventManager::activateSockets()
{	
	if( !m_socketlist) return;

	list<NMSocketListener *>::iterator iter;
	
	m_sockmutex.lock();

	for ( iter = m_socketlist->begin(); iter != m_socketlist->end(); iter++ ) {
		NMSocketListener *listener = (NMSocketListener *) *iter;
		listener->activate();
	}
	
	m_sockmutex.unlock();	
}

void NMEventManager::activateExternals()
{
	if( !m_externallist) return;

	list<NMExternalListener *>::iterator iter;
	
	m_externalmutex.lock();

	for ( iter = m_externallist->begin(); iter != m_externallist->end(); iter++ ) {
		NMExternalListener *listener = (NMExternalListener *) *iter;
		listener->activate();
		if ( listener->external()->finished.isActived()) {
			m_externallist->remove( listener);
			iter--;
		}
	}
	m_externalmutex.unlock();	
}


void NMEventManager::activateTimers()
{
	if( !m_timerlist) return;

	list<NMTimerListener *>::iterator iter;
	
	m_timermutex.lock();

	for ( iter = m_timerlist->begin(); iter != m_timerlist->end(); iter++ ) {
		NMTimerListener *listener = (NMTimerListener *) *iter;
		listener->activate();
		if ( listener->timer()->timeout.isActived()) {
			m_timerlist->remove( listener);
			iter--;
		}
	}
	m_timermutex.unlock();	
}
