/*
 *   $Id: nmmgenexternaldrec.h,v 1.12 2007-05-27 11:19:34 rserral Exp $
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
#ifndef NMMGENEXTERNALDREC_H
#define NMMGENEXTERNALDREC_H

#include <nmexternalremote.h>
#include <nmevent.h>
#include <nmscp.h>

#include "nmmgenerrormanager.h"
#include "nmmgenstatusmanager.h"
#include "nmmgeneventdrecrunning.h"
#include "nmmgeneventreadytorundrec.h"
#include "nmmgeneventfinisehddrec.h"


class NMMGenExternalDrec : public NMParent
{
	public:
		NMMGenExternalDrec ( NMParent * parent, NMMgenErrorManager * errorMan, NMMgenStatusManager * statusMan,
		                     NMMgenEventDrecRunning * drecRunning, NMMgenEventReadyToRunDrec * readyToRunDrec,
		                     NMMgenEventFinishedDrec * finishedDrec );

		~NMMGenExternalDrec();

		bool configureDrec ( const NMString & pathDrec, const NMString & pathDecode, const NMString & localPathScript,
		                     const NMString & drecOutputFilePath, const NMString & address, int addresPort, const NMString & username,
		                     const NMString & password, const NMString & protocol, int mgenVersion );
		bool runDrec();
		void drecProcessReceived();
		void drecProcessExit();
		void drecFileSent();
		void killDrec();

	private:
		bool isRemote;
		NMHost destHost;
		NMMgenErrorManager * errorManager;
		NMMgenStatusManager * statusManager;
		NMExternal * drecExternal;
		NMMgenEventDrecRunning * drecRunning;
		NMMgenEventReadyToRunDrec * readyToRunDrec;
		NMMgenEventFinishedDrec * finishedDrec;
		NMScp scp;

		NMString drecPath;
		NMString decodePath;
		NMString managementAddress;
		int managementAddressPort;
		NMString managementUsername;
		NMString managementPassword;
		NMString managementProtocol;
		NMString drecOutputFile;
};

#endif
