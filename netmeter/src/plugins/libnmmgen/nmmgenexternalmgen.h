/*
 *   $Id: nmmgenexternalmgen.h,v 1.10 2007-05-27 11:19:34 rserral Exp $
 *
 *   Copyright (c) 2006 Ernest Nieto
 *   Copyright (c) 2008 Roberto Barreda
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
#ifndef NMMGENEXTERNALMGEN_H
#define NMMGENEXTERNALMGEN_H

#include <nmexternalremote.h>
#include <nmevent.h>
#include <nmscp.h>

#include "nmmgenerrormanager.h"
#include "nmmgenstatusmanager.h"
#include "nmmgeneventfinishedmgen.h"
#include "nmmgenreadytorunmgen.h"

class NMMGenExternalMgen : public NMParent
{
	public:
		NMMGenExternalMgen ( NMParent * parent, NMMgenErrorManager * errorMan, NMMgenStatusManager * statusMan,
		                     NMMgenEventFinishedMgen * endMgen, NMMgenEventReadyToRunMgen * runMgen );

		~NMMGenExternalMgen();

		bool configureMgen ( const NMString & pathMgen, const NMString & localPathScript,
		                     const NMString & address, int addressPort,
		                     const NMString & username, const NMString & password,
		                     const NMString & protocol, int mgenVersion );
		void mgenScriptSent();
		bool runMgen();
		void processReceived();
		void processExit();
		void killMgen();

	private:
		bool isRemote;
		NMMgenEventFinishedMgen * finisehdMgen;
		NMMgenEventReadyToRunMgen * readyToRunMgen;
		NMMgenErrorManager * errorManager;
		NMMgenStatusManager * statusManager;
		NMExternal * external;
		NMString mgenPath;
		NMString managementAddress;
		int managementAddressPort;
		NMString managementUsername;
		NMString managementPassword;
		NMString managementProtocol;
		NMScp scp;

};

#endif

