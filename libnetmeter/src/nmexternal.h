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
#ifndef NMEXTERNAL_H
#define NMEXTERNAL_H

#include "nmparent.h"
#include "nmsocket.h"
#include "nmeventmanager.h"

#include <list>


class NMExternal;

class NMExternalListener: public NMParent
{

friend class NMEventManager;

public:
	NMExternalListener(const char *name, NMExternal *parent);
	~NMExternalListener();

	NMExternal *external();

private:
	void activate();
};


/**
@author Pau Capella
@brief Is a abstract class where the generic execution environment is set up.
*/
class NMExternal : public NMParent
{

friend class NMExternalListener;

public:

	/**
	\enum Error
	\param NoError No error occurred
	\param TimeOut Timeout connection
	\param IsRunning Process is running
	\param ForkFailed Error in create fork for process
	\param WrongPassword Wrong password
	\param NoHost No objective host defined
	\param Unknow An error other than those specified above occurred
   	*/
	enum Error {
		NoError = 0,
		Timeout,
		IsRunning,
		ForkFailed,
		WrongPassword,
		NoHost,
		Unknow
	};

	/** 
		Socket for input/output standard.
	 */
	NMSocket stdInOut; 
		
public:
	NMExternal(const char *name = 0 , bool listener = true, NMParent *parent = 0);
	virtual ~NMExternal();

	NMExternal &operator<<(const NMString& arg);
	NMExternal &operator<<(const char* arg);

	void clearArguments();

	virtual bool kill();
	virtual bool killsignal();
	virtual bool termsignal();
	virtual Error run() = 0;
	
	bool isRunning(); 
	int pid();

//protected: // TODO ShellCommand ha de ser protected!
	NMString shellCommand();

public:
	/**
	The event is actived when the process has written data to standard output
	*/
	NMEvent readyReadOutput;
	/**
	This event is activated when the process has exited.
	*/
	NMEvent finished;

protected:
	bool m_isrun;
	int m_pid;
	bool m_listener;
	NMExternalListener *m_externallistener;

private:
	list<NMString> m_arguments;
};

#endif
