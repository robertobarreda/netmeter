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
#ifndef NMEVENT_H
#define NMEVENT_H

#include<nmparent.h>
#include<nmthread.h>
#include<nmerror.h>

class NMEventCallBackBase: public NMThread {

	public:
		virtual void run() = 0;
};

/**
@author Pau Capella
@brief The NMEvent class can be used to send events for classes.
*/

template <typename NMClientCb>
class NMEventCallBack: public NMEventCallBackBase {
	
	public:
		
		typedef void (NMClientCb::*NMMemberCb)();

		NMEventCallBack(NMClientCb *obj, NMMemberCb mem) {
			client = obj;
			member = mem;
		}

		~NMEventCallBack() {}

		void run() {
			(client->*member)();
		}
	
	private:
		NMClientCb* client;
    		NMMemberCb member;
};
		
		

/**
@author Pau Capella
@brief Provide information about events. Status, callback,...
*/
class NMEvent: public NMParent {

	friend class NMEventManager;
	
	NMEventCallBackBase *m_callback;
	bool m_active;
	bool m_connected;
	bool m_remove;
	bool m_callbackrun;

	void *m_value;

public:

	NMEvent(const char *name, NMParent *parent);
	~NMEvent();

	/**
  	\fn connect (NMClientCb *callobject, void (NMClientCb::*member)())
  	\brief Add a new event
  	\param callobject Object to raise when the event occurs.
	\param member Function to raise.when the event occurs.
  	\return On succes, true is returned. On error false is returned.
	*/	
	template <typename NMClientCb>
	bool connect (NMClientCb *callobject, void (NMClientCb::*member)()) {

		typedef void (NMClientCb::*PtrMember)();

		if(m_connected) return false;

		m_callback = new NMEventCallBack<NMClientCb>(callobject,member);
		m_connected = true;
	
		NMString msg = "Connected event ";
		msg += this->name();
		msg += " from ";
		msg += this->parent()->name(); 
		cout << NMError(NMError::debug, msg);

		return true;
	}
	
	bool disconnect();
	void activate();
	void remove();
	bool isConnected();
	
	void setValue( void *value);
	void *value();
	
private:
	bool isActived();
	bool isRemove();
	void desActived();
	NMEventCallBackBase *callBack();
};

#endif
