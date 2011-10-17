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
#include "nmexternalremote.h"
#include <stdlib.h>

/**
\fn NMExternalRemote::NMExternalRemote(const char *name = 0, bool listener = true, NMParent *parent = 0)
\brief Create an instance of NExternalRemote
\param listener True if you want to use events, otherwise false
*/
NMExternalRemote::NMExternalRemote(const char *name, bool listener, NMParent *parent)
 : NMExternal(name, listener, parent),
   m_ssh(0),
   m_host(),
   m_username(""),
   m_password("")
{
}


NMExternalRemote::~NMExternalRemote()
{
	if(m_ssh) 
	{
		m_ssh->stdInOut.close();
		delete m_ssh;
		m_ssh = 0;
	}
}

/**
    \fn NMExternalRemote::run()
    \brief Run the current remote program.
    \return On succes, 0 is returned. On  failure, error code is returned
*/
NMExternal::Error NMExternalRemote::run()
{
	// Ignore all signals recieved:
	for (int sig = 10; sig < NSIG; sig++) signal(sig,SIG_IGN);

	if( m_pid > 0) 
	{
		cerr << "NMExternalRemote run failed: The process is already runnnig" << '\n';
		return NMExternal::IsRunning; 
	}

	if( m_host.isNull()) 
	{
		cerr << "NMExternalRemote run failed: Host is not defined" << '\n';
		return NMExternal::NoHost;
	}
	
	NMExternal::Error rc;

	if( !m_ssh)
		m_ssh = new NMExternalLocal("nmel_ssh", false, this);
	//*m_ssh << "exec ssh -q -l";
	*m_ssh << "ssh -q -l";

	//Si no hem ficat usuari agafem la variable de entorn
	
	if( !m_username.Length()) 
		m_username = getenv("USER");
	
	*m_ssh << m_username;
	*m_ssh << m_host.getAddress();
	
	if(m_port != 22)
	{
		*m_ssh << "-p";
		*m_ssh << m_port;
	}
	
	if (m_noHostKeyCheck) {
		*m_ssh <<  "-oStrictHostKeyChecking=no";
	}
		
	*m_ssh << "\'echo SSH_NETMETER:;";
	*m_ssh << "/bin/sh";
	*m_ssh << "\'";
	
	#ifdef DEBUG
		cout << "NMExternalRemote::run with shellCommand " << m_ssh->shellCommand() << endl;
	#endif
	
	rc = m_ssh->run(); 
	
	if( rc != NMExternal::NoError) 
	{
		m_ssh->stdInOut.close();
		m_ssh->killsignal();
		delete m_ssh;
		m_ssh = 0;
		return rc;
	}

	rc = converseSsh();
	
	if( rc != NMExternal::NoError) 
	{
		m_ssh->stdInOut.close();
		m_ssh->killsignal();
		delete m_ssh;
		m_ssh = 0;
		return rc;
	}

	// Copiem la estructura de dades perque l'usuari pugi usar stdInOut directament amb el stdInOut:
	
	m_isrun = true;
	m_pid = m_ssh->pid();
	stdInOut.open(m_ssh->stdInOut.fd(), NMBuffer::ReadWrite);
	//stdInOut = ssh.stdInOut;
	
	// Sortir del ssh despres de la comanda:
	
	stdInOut.writeLine(shellCommand() + ";exit", true);

	if(m_listener) 
	{
		m_externallistener = new NMExternalListener("locallistener", this);
		NMEventManager::self()->registerExternalListener( m_externallistener);
	}

	return NMExternal::NoError;
}


/**
    \fn NMExternalRemote::run()
    \brief Try to login to the ssh host server.
    \return TRUE if we can log us; otherwise return FALSE
*/
NMExternal::Error NMExternalRemote::converseSsh()
{
	NMString line;
	bool isPasswordSend = false;
	bool isLoggedIn = false;
	
	while( !isLoggedIn) 
	{
		if( m_ssh->stdInOut.waitMS(30000) < 0) 
		{
			cerr << "NMExternalRemote establishConnection failed: Timeout in ssh connexion" << '\n';
			return NMExternal::Timeout;
		}

		line = m_ssh->stdInOut.readLine();

		if( line.search("(yes/no)?") >= 0) 
		{
			m_ssh->stdInOut.writeLine("yes",true);
		}

		else if ( line.search("SSH_NETMETER:") >= 0) 
		{
			#ifdef DEBUG
				cout << name() << " ";
				cout << m_host.getAddress();
				cout << ": Logged In!! :)" << '\n';
			#endif
			isLoggedIn = true;
		}

		else if( line.search("Password:") >= 0 || line.search("password:") >= 0 ) 
		{
			if( isPasswordSend ) 	return NMExternal::WrongPassword;
			isPasswordSend = true;
			m_ssh->stdInOut.writeLine( m_password, true);
		}

                // (rbarreda) For debain ssh bug compatibility. More info: DSA-1571-1 openssl -- predictable random number generator
                else if ( line.search ( "blacklisted" ) >= 0 ) continue;
                else if (line.search("\n") == 0 || line.search(".") >= 0 || line.search(" ") >= 0 ) continue;
		
		else return NMExternal::Unknow;
	}
	return NMExternal::NoError;
}

/**
    \fn NMExternalRemote::setHost(const NMHost &nmh_host)
    \brief Set the host to connect.
    \param nmh_host Host to connect
*/
void NMExternalRemote::setHost(const NMHost &nmh_host, int connectionPort, bool noHostKeyCheck)
{
	m_host = nmh_host;
	m_port = connectionPort;
	m_noHostKeyCheck = noHostKeyCheck;
}

/**
    \fn NMExternalRemote::setUsername(const NMString &user)
    \brief Set username for ssh host
    \param username Username
*/
void NMExternalRemote::setUsername(const NMString &username)
{
	m_username = username;
}

/**
    \fn NMExternalRemote::setPassword(const NMString &pass)
    \brief Set password for ssh host
    \param password Password
*/
void NMExternalRemote::setPassword(const NMString &password)
{
	m_password = password;
}

/**
    \fn NMExternalRemote::kill()
    \brief Kill the current remote program.
*/
bool NMExternalRemote::kill()
{
	NMExternalRemote *kill = new NMExternalRemote( "killRemote",false,this);
	kill->setHost(m_host, m_port, m_noHostKeyCheck);
	kill->setUsername(m_username);
	kill->setPassword(m_password);

	*kill << "pkill -INT -f \""+shellCommand()+"\"";

	if (kill->run() != NMExternal::NoError) 
		return false;
	
	sleep(100);
	delete kill;
	return true;
}

bool NMExternalRemote::killsignal()
{
	NMExternalRemote *kill = new NMExternalRemote("killRemotesignal",false,this);
	kill->setHost(m_host, m_port, m_noHostKeyCheck);
	kill->setUsername(m_username);
	kill->setPassword(m_password);

	*kill << "pkill -KILL -f \""+shellCommand()+"\"";

	if (kill->run() != NMExternal::NoError) 
		return false;
	
	sleep(30);
	delete kill;
	return true;
}

bool NMExternalRemote::termsignal()
{
	NMExternalRemote *kill = new NMExternalRemote("termRemotesignal",false,this);
	kill->setHost(m_host, m_port, m_noHostKeyCheck);
	kill->setUsername(m_username);
	kill->setPassword(m_password);

	*kill << "pkill -TERM -f \""+shellCommand()+"\"";

	if (kill->run() != NMExternal::NoError) 
		return false;
	
	sleep(100);
	delete kill;
	return true;
}

/*
    \fn NMExternalRemote::kill()
    \brief Kill the current remote program.

bool NMExternalRemote::kill()
{
	NMString nms_name = getClassName();
	nms_name += "_nmfkill";
	NMFish nmf_kill (nms_name.c_str(), this);
	if(nmf_kill.openConnection(host) < 0) {
		cerr << "NMExternalRemote Connection failed!" << '\n';
		return;
	}
	#ifdef DEBUG
	cout << getClassName() << " Matant proces:  " << path.c_str() << '\n';
	#endif
	nmf_kill.exec("killall", path);
	sleep(1);
	nmf_kill.closeConnection();
	ssh->closeConnection();
	b_isrun = false;
	pid = -1;
	return true;

}*/



#if 0
/*!
    \fn NMExternalRemote::binExsist(NMString exec)
    \brief Verifies if application exsists.
    \param exec Name of executable.
    \return Path of the binary or empty if not exsist.
*/
NMString NMExternalRemote::binExsist()
{
	/*NMString nms_name = getClassName();
	nms_name += "_nmbExsist";
	NMFish nmf_kill (nms_name.c_str(), this);
	if(nmf_kill.openConnection(host) < 0) {
		cerr << "NMExternalRemote Connection failed!" << '\n';
		return;
	}
	#ifdef DEBUG
	cout << getClassName() << " Matant proces:  " << path.c_str() << '\n';
	#endif
	nmf_kill.exec("killall", path);
	sleep(1);
	nmf_kill.closeConnection();
	ssh->closeConnection();
	b_isrun = false;
	pid = -1;*/



        NMString ret = "";
	return ret;
}
#endif


