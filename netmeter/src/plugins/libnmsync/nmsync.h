/***************************************************************************
 *   Copyright (C) 2005 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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

#ifndef NMSYNC_H
#define NMSYNC_H

#include <nmmoduleinterface.h>

#include <nmfile.h>
#include <nmdns.h>
#include <nmexternallocal.h>
#include <nmexternalremote.h>
#include <nmxml.h>
#include <nmftp.h>

#define OK 		0
#define ERROR 		1
#define NOTFOUND	2
#define UNDEF		3

/**
@author Joan Sala
@brief Sync plugin
*/

// Class used to encapsulate a mesure from ntptime:

class SyncItem
{
        public:
	        NMString timestamp;
		NMString offset;
		NMString maximum_error;
		NMString estimated_error;
		NMString ntp_status;
		NMString pps_status;
};

class NMSync : public NMModuleInterface
{	
	NMExternal *nme_sync;
	
	// list used to gather results from polling
	list<SyncItem> resultList;

	bool poll, active;
	int result_ntp,result_pps, sleepnumber;
	NMString maximum_error, offset, estimated_error, sleeptime, time, address;
	NMString scriptfile, resultfile;
	NMXML nmxml_data;

public:
	NMSync();
	~NMSync();

	void initModule();
	void removeModule();
	void start(void *data);
	void stop(void *data);
	void status(void *data);
	void configure(void *data);

	void stopAndGetResults(void *data);
	void sendLastData();

private:
	void parseResult();
	void addElement();
	void processExit();
	void syncScriptSent();

	NMFtp sftp;
};

NMMODULE_CLASS(NMSync)
NMMODULE_NAME((char *)"nmsync")
NMMODULE_CAPABILITIES((char *)"test")
NMMODULE_VERSION((char *)"0.1")

#endif
