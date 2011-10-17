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
#ifndef NMEVENTMANAGER_H
#define NMEVENTMANAGER_H

#include "nmparent.h"
#include "nmstring.h"
#include "nmevent.h"
#include "nmthread.h"
#include "nminterface.h"
#include "nmmutex.h"
#include "nmexternal.h"
#include "nmtimer.h"
#include <list>


class NMExternalListener;
class NMSocketListener;

/**
@author Pau Capella
@brief This class permit handle events.
*/
class NMEventManager : public NMParent, public NMThread
{

public:	
	~NMEventManager();
	static NMEventManager *self();	
	 
	//eventId removeEvent(eventId id);
	void runLoop();
	void exitLoop();

	void registerSocketListener( NMSocketListener *listener);	
	void unregisterSocketListener( NMSocketListener *listener);

	void registerExternalListener( NMExternalListener *listener);	
	void unregisterExternalListener( NMExternalListener *listener);

	void registerTimerListener( NMTimerListener *listener);	
	void unregisterTimerListener( NMTimerListener *listener);

private:

	void run();

	void activateSockets();
	void activateExternals();
	void activateTimers();

	NMEventManager(const char *name, NMParent *nmparent);
	
	void dispatchEvents();
	void processEvent(NMEvent *event);

	static NMEventManager* nmeventmanager;
	bool m_exit;

	list<NMSocketListener *> *m_socketlist;
	NMMutex m_sockmutex;

	list<NMExternalListener *> *m_externallist;
	NMMutex m_externalmutex;

	list<NMTimerListener *> *m_timerlist;
	NMMutex m_timermutex;
	
};

#endif
	

