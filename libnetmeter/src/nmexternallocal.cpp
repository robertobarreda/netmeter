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
#include "nmexternallocal.h"
#include <pty.h>
#include <termios.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

/**
\fn NMExternalLocal::NMExternalLocal(const char *name = 0, bool listener = true, NMParent *parent = 0)
\brief Construc a new NMExternalLocal object
\param listener True if you want to use events, otherwise false
*/
NMExternalLocal::NMExternalLocal( const char *name, bool listener, NMParent *parent)
 : NMExternal(name, listener, parent )
{
	
}


NMExternalLocal::~NMExternalLocal()
{
	if ( m_pid > 0)
		kill();
}

static int open_pty_pair(int fd[2])
{
        int master, slave;
        char *name;
        struct ::termios ti;
        memset( &ti, 0, sizeof(ti));

        ti.c_cflag = CLOCAL|CREAD|CS8;
        ti.c_cc[VMIN] = 1;

        master = getpt();
        if ( master < 0) return 0;

        if (grantpt( master) < 0 || unlockpt( master) < 0) goto close_master;

        name = ptsname( master);
        if (name == NULL) goto close_master;

        slave = open( name, O_RDWR);
        if (slave == -1) goto close_slave;

        tcsetattr( slave, TCSANOW, &ti);
        fd[0] = master;
        fd[1] = slave;
        return 0;

close_slave:
        close(slave);

close_master:
        close(master);
        return -1;
        return openpty( fd, fd+1, NULL, &ti, NULL);
}


/*!
    \fn NMExternalLocal::run()
    \brief Run the process.
    \return On succes, 0 is returned. On  failure, error code is returned
*/
NMExternal::Error NMExternalLocal::run()
{
	if( m_pid > 0) {
		cerr << "The process is already runnnig" << '\n';
		return NMExternal::IsRunning;
	}

	int fd[2] = {0,0};
	int rc,flags;

	rc = open_pty_pair( fd );
	if (rc < 0) {
		cerr << "NMExternalLocal open_pty_pair() failed: " << strerror(errno) << '\n';
		return NMExternal::Unknow;
        }
        m_pid = fork();
	if( m_pid == -1) {
		cerr << "NMExternalLocal fork failed, error: " << strerror(errno) << '\n';   
		close( fd[0]);
		close( fd[1]);
		return NMExternal::ForkFailed;
	}
	
	if ( m_pid == 0) {
		// Ignore all signals recieved:
		for (int sig = 10; sig < NSIG; sig++) signal(sig,SIG_IGN);

		struct rlimit rlp;
		getrlimit(RLIMIT_NOFILE, &rlp);
		for (int i = 0; i < (int)rlp.rlim_cur; i++)
			if ( i != fd[1]) close(i);

		dup2(fd[1],0);
		dup2(fd[1],1);
		dup2(fd[1],2);
		if (fd[1] > 2) close(fd[1]);

		setsid();
		int pgrp = pid();
		tcsetpgrp(0, pgrp);
	
		const char *dev = ttyname(0);
		setpgid(0,0);
		if (dev) close(open(dev, O_WRONLY, 0));
		setpgid(0,0);
		//cout << "\n\nExecute " << shellCommand() << '\n';
                execl("/bin/sh", "sh", "-c", shellCommand().c_str(), NULL);
		//execl(shellCommand().c_str(), NULL);
                cerr << "NMExternal execl failed: could not exec" << '\n';
		exit(-1);
	} 
	else  { //Pare
		close(fd[1]);
		//Volem el fd no bloquejant, ens asegurem
        	rc = fcntl( fd[0],F_GETFL,&flags);
		flags &= ~O_NONBLOCK;
		rc = fcntl( fd[0],F_SETFL,flags);

		stdInOut.open( fd[0], NMSocket::ReadWrite);
		m_isrun = true;
		
		if(m_listener) {
			m_externallistener = new NMExternalListener( "locallistener", this);
			NMEventManager::self()->registerExternalListener( m_externallistener);
		}
	}
	return NMExternal::NoError;
}
