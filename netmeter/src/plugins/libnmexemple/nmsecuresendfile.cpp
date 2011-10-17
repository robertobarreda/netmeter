/*
 *   $Id: nmsecuresendfile.cpp,v 1.1 2006-04-02 11:22:43 enieto Exp $
 *
 *   Copyright (c) 2006 Pau Capella
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
#include "nmsecuresendfile.h"

#include <iostream>
using namespace std;

NMSecureSendFile::NMSecureSendFile()
{
	externalScp = 0;
}


NMSecureSendFile::~NMSecureSendFile()
{
}

void NMSecureSendFile::initModule()
{

}

void NMSecureSendFile::removeModule()
{

}

void NMSecureSendFile::start(void *data)
{
	NMString *xml = (NMString *) data;
	NMXML xmlData;
// 	NMString hostAddr, hostUser, hostPass, localFile, remotePath;
	
	xmlData.loadTree(xml->c_str());
	

	hostAddr = xmlData.getValue("/*/host/address");
	hostUser = xmlData.getValue("/*/host/username");
	hostPass = xmlData.getValue("/*/host/password");
	localFile = xmlData.getValue("/*/file");
	remotePath = xmlData.getValue("/*/path");

	cout << hostAddr << ",.. " << remotePath << endl;	
	
	externalScp = new NMExternalLocal("localScp",true,this);

	//ejecutado localmente:
	//scp nm_dscript enieto@xardonay.ccaba.upc.edu:/home/enieto
	//Password:
	*externalScp << "scp";
	NMString file = " " + localFile;
	NMString path = NMString(" ") + hostUser + "@" + hostAddr + ":"+remotePath;
	*externalScp << file;
	*externalScp << path;
	cout << "Command: scp"+file + path << endl;
	
	externalScp->readyReadOutput.connect(this, &NMSecureSendFile::processReceived);
	externalScp->finished.connect(this, &NMSecureSendFile::processExit);
	
	if(!(externalScp->run()))
	{
		cout << "Error running" << endl;
		exit();
	}
	cout << "Scp running..."<<endl;
	
}

void NMSecureSendFile::stop(void *data)
{

}

void NMSecureSendFile::status(void *data)
{

}

void NMSecureSendFile::configure(void *data)
{

}

void NMSecureSendFile::processReceived()
{
	externalScp->stdInOut.writeLine(hostPass,true);
	return;

	cout << "NMSecureSendFile::processReceived " << endl;
	NMString line = externalScp->stdInOut.readWholeLine();
	if(not line.IsEmpty() and line.Length()>0) line = line.subString(0,line.Length()-1);
	else line = "";
	cout << "Rcvd scp (" << line.c_str() <<")"<< endl;
	
}

void NMSecureSendFile::processExit()
{
	cout << "NMSecureSendFile::processExit" << endl;
	
	exit();
}




