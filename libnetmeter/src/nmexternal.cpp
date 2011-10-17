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

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "nmexternal.h"

NMExternalListener::NMExternalListener(const char *name, NMExternal *parent)
 : NMParent(name, parent)
{
}

NMExternalListener::~NMExternalListener()
{
}

NMExternal *NMExternalListener::external()
{
	return (NMExternal *)this->parent();
}
	

void NMExternalListener::NMExternalListener::activate()
{
	NMExternal *proc = external();
	
	if( proc->stdInOut.waitMS(0) > 0) 
		proc->readyReadOutput.activate();

	else if ( !proc->isRunning())
		proc->finished.activate();
}


/**
\fn NMExternal::NMExternal(const char *name = 0, bool listener = true, NMParent *parent = 0)
\brief Construct a new external object
\param listener True if you want to use events, otherwise false
*/
NMExternal::NMExternal(const char *name , bool listener, NMParent *parent )
: NMParent(name,parent),
  stdInOut("nmsk_stdinout", false, this),
  readyReadOutput("readyReadOutput",this),
  finished("finished",this),
  m_isrun(false),
  m_pid(0),
  m_listener(listener),
  m_externallistener(0)
{
	//Aixo es per canvia el nom al socket... sino tots tindran el mateix nom cosa que pot liar al NMParent.
	NMString socketname(name);
	socketname = socketname + "_" + stdInOut.name();
	stdInOut.setName( socketname.c_str() );
}

/**

*/
NMExternal::~NMExternal()
{
}

/**
\fn NMExternal::operator<<(const NMString& arg)
\brief Sets the executable and the command line argument list for this process.\n
For example, doing an "ls -l /usr/local/bin" can be achieved by:\n
\code
	NMExternalLocal p;
	p << "ls" << "-l" << "/usr/local/bin";
\endcode
\param arg the argument to add
\return a reference to this NMExternal
*/
NMExternal &NMExternal::operator<<(const NMString& arg)
{
	m_arguments.push_back(arg);
	return *this;
}

/**
\fn NMExternal::operator<<(const char * arg)
\brief Similar to previous method, takes a char *.
*/
NMExternal &NMExternal::operator<<(const char * arg)
{
	NMString nms_arg(arg);
	m_arguments.push_back(nms_arg);
	return *this;
}

/**
\fn NMExternal::clearArguments()
\brief Clear a command line argument list that has been set by using operator <<.
*/
void NMExternal::clearArguments()
{
	m_arguments.clear();
}

/**
\fn NMExternal::kill() and killsignal()
\brief Stop the process( by sending it a signal).
\return true if the signal was delivered successfully.
*/
bool NMExternal::kill()
{
	if ( m_isrun && m_pid > 0 && !::kill( m_pid, SIGINT)) 
	{
		m_isrun = false;
		m_pid = 0;
		return true;
	}
	return false;
}

bool NMExternal::killsignal()
{
        if ( m_isrun && m_pid > 0 && !::kill( m_pid, SIGKILL)) 
	{
                m_isrun = false;
                m_pid = 0;
                return true;
        }
        return false;
}

bool NMExternal::termsignal()
{
        if ( m_isrun && m_pid > 0 && !::kill( m_pid, SIGTERM)) {
                m_isrun = false;
                m_pid = 0;
                return true;
        }
        return false;
}

/**
\fn NMExternal::isRunning()
\brief Checks if the process is running.
\return true if current proces is running.
*/
bool NMExternal::isRunning()
{
	int estat=0;
	int ctl=0;
 	
	if( !m_isrun || m_pid == 0) return false;

	//OJO amb aixo que estem malament
	if(stdInOut.waitMS(0) > 0) return true;
	
	if((ctl = waitpid( m_pid, &estat, WNOHANG))<0) 
	{
		if( ctl != ECHILD)
			cerr << "NMExternal waitpid failed: " << strerror(errno) << endl;
                m_isrun = false;
		m_pid = -1;
		return m_isrun;
	}
	
	if(ctl == 0) 
		m_isrun = true;
		
	else if (ctl > 0) 
	{
		m_isrun = false;
		m_pid = -1;
	}	
	return m_isrun;
}

/**
\fn NMExternal::pid()
\brief Returns the process id of the process.
\return The pid of the process or 0 if no process has been started.
*/
int NMExternal::pid()
{
	return m_pid;
}

/** 
\fn NMExternal::shellCommand()
\brief Extract the command from the arguments list.
\return Complete command.
*/
NMString NMExternal::shellCommand()
{
	NMString command;
	list<NMString>::iterator iter;
	for ( iter = m_arguments.begin(); iter != m_arguments.end(); iter++ ) {
		command += *iter;
		command += " ";		
	}
	return command.trimRight();
}
