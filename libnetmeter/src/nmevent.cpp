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
#include "nmevent.h"

NMEvent::NMEvent(const char *name, NMParent *parent)
:NMParent(name,parent),
m_active(false),
m_connected(false),
m_remove(false),
m_callbackrun(false),
m_value(0)
{
	isevent = true;
}


NMEvent::~NMEvent()
{
	disconnect();
}

/**
\fn NMEvent::disconnect ( const NMString name)
\brief Disconnect the event
\param name Name of the event
\return On succes, true is returned. On error false is returned.
*/	
bool NMEvent::disconnect ()
{
	if(!m_connected) return true;
	if(m_active) {
		cout << NMError(NMError::warning,"You try to delete a callback and it's running; try to force...");
//  		if(!callback->kill()) return false;
//  		else cout << "...forced ok" << endl;
// 		
// 		connected = false;
		return false;
	}
	delete m_callback;
	m_connected = false;
	
	NMString msg = "Disconnected event ";
	msg += this->name();
	msg += " from ";
	msg += this->parent()->name(); 
	cout << NMError(NMError::debug, msg);

	return true;
}

/**
\fn NMEvent::activate()
\brief Activate the event
*/
void NMEvent::activate()
{
	m_active = true;
}
/**
\fn NMEvent::isActivate()
\return True if the event is active, otherwise return false 
*/
bool NMEvent::isActived()
{
	return m_active;
}

/**
\fn NMEvent::isConnected()
\return True if the event is connected, otherwise return false 
*/
bool NMEvent::isConnected()
{
	return m_connected;
}

void NMEvent::remove()
{
	m_remove = true;
}


bool NMEvent::isRemove()
{ 
	return m_remove;
}

void NMEvent::setValue( void *value)
{
	m_value = value;
}

void *NMEvent::value()
{
	return m_value;
}

void NMEvent::desActived() 
{ 
	m_active = false; 
}

NMEventCallBackBase *NMEvent::callBack()
{
	return m_callback;
}



