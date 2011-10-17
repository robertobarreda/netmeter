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
#ifndef NMEXTERNALREMOTE_H
#define NMEXTERNALREMOTE_H

#include <nmexternal.h>
#include <nmsocket.h>
#include <nmexternallocal.h>
#include "nmhost.h"


/**
@brief Permit to run remote programs.
@author Pau Capella
*/
/**
See NMExternalLocal documentation for more information about NMExternalRemote.
*/
class NMExternalRemote : public NMExternal
{
	
public:
	NMExternalRemote(const char *name = 0, bool listener = true, NMParent *parent = 0);
	~NMExternalRemote();
	NMExternal::Error run();
	bool kill();
	bool killsignal();
	bool termsignal();
	void setHost(const NMHost &host, int connectionPort=22, bool noHostKeyCheck=false);
	void setUsername(const NMString &username);
	void setPassword(const NMString &password);

private: 
	NMExternal::Error converseSsh();

private:
	/* El external local que sera l'encarregat de excutar el ssh.*/
	NMExternalLocal *m_ssh;
	NMHost m_host;
	NMString m_username;
	NMString m_password;
	int m_port;
	bool m_noHostKeyCheck;

};

#endif
