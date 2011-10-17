/*
 *   $Id: nmtimer.h,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
 *   Copyright (c) 2005 Pau Capella
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef NMTIMER_H
#define NMTIMER_H

#include "nmevent.h"
#include "nmdatetime.h"

class NMTimer;

class NMTimerListener: public NMParent {

friend class NMEventManager;

public:
	NMTimerListener(const char *name, NMTimer *parent);
	~NMTimerListener();

	NMTimer *timer();

private:
	void activate();
};

/**
@author Pau Capella
@brief Provides single-shot timers
*/
/**
NMTimer is very easy to use: create a NMTimer, call start() to start it and connect its timeout() to the appropriate callback. When the time is up it will emit the timeout() signal.\n
<b> Example:</b>\n
\code
    NMTimer timer("timer", myobject);
    timer.timeout.connect(myobject, &myobject::callback)
    timer.start(2); // 2 sec single-shot timer

 \endcode
*/
class NMTimer: public NMParent
{
	friend class NMTimerListener;
public:
	NMTimer(NMParent *parent = 0);
	~NMTimer();

	bool isActive () const;
	bool start ( int sec );
	bool start ( NMDateTime datetime);
	void stop ();

	/**
	\brief This event is throw when the timer is activated.
	*/
	NMEvent timeout;

private:
	bool m_active;
	NMDateTime m_limittime;
	NMTimerListener *m_timerlistener;
};

#endif
