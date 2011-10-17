/*
 *   $Id: nmmgenexternaldecode.h,v 1.7 2007-05-27 11:19:34 rserral Exp $
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
#ifndef NMMGENEXTERNALDECODE_H
#define NMMGENEXTERNALDECODE_H

#include <nmexternalremote.h>
#include <nmevent.h>
#include <nmscp.h>

#include "nmmgenstatusmanager.h"
#include "nmmgenerrormanager.h"
#include "nmmgeneventdecodefinished.h"

class NMMGenExternalDecode : public NMParent
{
	public:
		NMMGenExternalDecode ( NMParent * parent, NMMgenErrorManager * errorMan,
		                       NMMgenStatusManager * statusMan, NMMgenEventDecodeFinished * decodeEnd );

		~NMMGenExternalDecode();

		bool configureDecode ( const NMString & pathDecode, const NMString & drecOutputFilePath, const NMString & decodeOutputFilePath,
		                       const NMString & address, int addressPort, const NMString & username, const NMString & password,
		                       const NMString & protocol, int mgenVersion );
		bool startDecode();
		void killDecode();

	private:
		void decodeFileSent();
		void decodeProcessReceived();
		void decodeProcessExit();

	private:
		bool isRemote;
		NMMgenEventDecodeFinished * decodeFinished;
		NMHost destHost;
		NMMgenErrorManager * errorManager;
		NMMgenStatusManager * statusManager;
		NMExternal * decodeExternal;
		NMString decodePath;
		NMString managementAddress;
		int managementAddressPort;
		NMString managementUsername;
		NMString managementPassword;
		NMString managementProtocol;
		NMString drecOutputFile;
		NMString decodeOutputFile;
		NMScp scp;
};

#endif
