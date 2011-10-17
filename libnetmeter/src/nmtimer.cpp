/*
 *   $Id: nmtimer.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#include "nmtimer.h"
#include "nmeventmanager.h"
#include<signal.h>
#include<unistd.h>
#include<time.h>

/*********************** NMTimerListener **********************/

NMTimerListener::NMTimerListener(const char *name, NMTimer *parent)
 : NMParent(name, parent)
{
}

NMTimerListener::~NMTimerListener()
{
}

NMTimer *NMTimerListener::timer()
{
	return (NMTimer *)this->parent();
}

void NMTimerListener::activate()
{
	NMTimer *t = timer();
	if( NMDateTime::currentDateTime() >= t->m_limittime)
		t->timeout.activate();
}

/*********************** NMTimer **********************/
/*! 
    \fn NMTimer::NMTimer(NMParent *parent)
    \brief Create a new empty timer
*/
NMTimer::NMTimer(NMParent *parent)
 : NMParent(parent),
   timeout("timeout",this),
   m_active(false),
   m_timerlistener(0)
{
}

NMTimer::~NMTimer()
{
}

/*! 
    \fn NMTimer::isActive () const
    \brief Return true if the timer is running, otherwise return false
*/
bool NMTimer::isActive () const
{
	return m_active;
}

/*! 
    \fn NMTimer::start ( int sec ) 
    \brief Starts the timer with a seconds timeout
    \param sec Seconds timeout
    \return true if time is running, otherwise return false
*/
bool NMTimer::start ( int sec ) 
{
	NMDateTime datetime;
	NMTime time = NMTime::currentTime();
	time.addSecs( sec);
		
	datetime.setDate( NMDate::currentDate());
	datetime.setTime( time);

	if( datetime.isNull()) return false;

	return start( datetime);
}

/*! 
    \fn NMTimer::start ( NMDateTime datetime) 
    \brief Starts the timer with a datetime timeout
    \param datetime NMDateTime timeout
    \return true if time is running, otherwise return false
*/
bool NMTimer::start( NMDateTime datetime) 
{
	if (m_active) return false;

	if( !datetime.isValid() || datetime.isNull()) return false;
	
	m_active = true;
	m_limittime = datetime;

	if (!m_timerlistener) {
		m_timerlistener = new NMTimerListener("timerlistener",this);
		NMEventManager::self()->registerTimerListener( m_timerlistener);
	}
	return true;
}

/*! 
    \fn NMTimer::stop ()
    \brief Stop the timer 
*/
void NMTimer::stop ()
{
	if (!m_active) return;

	if (timeout.isConnected()) {
		NMEventManager::self()->unregisterTimerListener( m_timerlistener);
		m_timerlistener = 0;
		timeout.disconnect();
		m_active = false;
	}
}
