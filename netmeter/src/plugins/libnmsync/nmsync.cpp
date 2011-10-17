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

#include "nmsync.h"
#include <nmxmlstreamwriter.h>

#define NTPTIME_SCRIPT_CONTENT	"#!/bin/bash\n while [ 1 ]; do echo `ntpq -c as` `ntptime`; sleep $1; done\n"

NMSync::NMSync()
 : NMModuleInterface(), sftp("sync_sftp",this)
{
}


NMSync::~NMSync()
{
}

void NMSync::initModule()
{
	poll = false;	
	result_ntp = -1;
	result_pps = -1;
	maximum_error = "not available";
	nme_sync = 0;

	resultList.clear();
}

void NMSync::removeModule()
{
}

// Start function: starts and configures the externalremote(ntptime) 
// (two possible modes of query... Poll(sleeptime > 0) or Single Mesure (0)

void NMSync::start(void *data)
{
	NMString *nms_xml = (NMString *)data;

	#ifdef DEBUG
		cout << "###################################################" << endl;
        	cout << "   NMSync::start " << endl;
		cout << "###################################################" << endl;
        #endif
	
	// configure data
	
	if(nms_xml->Length()) configure(data);
}

void NMSync::configure(void *data)
{	
	NMString *nms_xml = (NMString *)data;
	NMString protocol;
	NMHost source;

	nmxml_data.loadTree(nms_xml->c_str());

	#ifdef DEBUG
		cout << "###################################################" << endl;	
        	cout << "   NMSync::configure() " << endl;
		cout << "---------------------------------------------------" << endl;
		cout <<  nmxml_data.toString() << endl;
		cout << "###################################################" << endl;	
	#endif	

	protocol = nmxml_data.getValue("/*/protocol");
	if(!protocol.Length()) 
	{
		*this << NMError(NMError::user, "No protocol key");
		nmxml_data.freeTree();
		return;
	}
	
	address = nmxml_data.getValue("/*/source/name");
	if(!address.Length()) 
	{
		*this << NMError(NMError::user, "No valid source key");
		nmxml_data.freeTree();
		return;
	}

        sleeptime = nmxml_data.getValue("/*/sleeptime");
        if(!sleeptime.Length())
        {
                *this << NMError(NMError::user, "No valid sleeptime");
                nmxml_data.freeTree();
                return;
        }
	else	
	{
		sleepnumber = sleeptime.toInt();

		if(sleepnumber <=0)	poll = false;
		else			poll = true;
	}
	
	// External remote

	if(address != "127.0.0.1") 
	{
		NMString nms_login;
		NMString nms_password;
		NMString nms_source;

		if(protocol == "IPv4")		source = NMDns::resolv(address, NMDns::IPv4);
		else 				source = NMDns::resolv(address, NMDns::IPv6);
		
		// Password and login (external remote):

		nms_source = nmxml_data.getValue("/*/source/management/name");
		nms_login = nmxml_data.getValue("/*/source/management/login");
		nms_password = nmxml_data.getValue("/*/source/management/password");

		nme_sync = new NMExternalRemote("remote_sync", true, this ); 
		NMExternalRemote *nmer_sync = dynamic_cast<NMExternalRemote *> ( nme_sync);

		nmer_sync->setHost(nms_source);
		nmer_sync->setPassword(nms_password);
		nmer_sync->setUsername(nms_login);

		// We've got to create the script to send to the remote host:

		scriptfile = "/tmp/ntptime" + (NMString)testId() + ".sh"; 

		NMFile tempfile = NMFile(scriptfile, this);
		int fd = tempfile.create(scriptfile);
		
		if(fd > 0)
		{
			tempfile.write(NTPTIME_SCRIPT_CONTENT, sizeof(NTPTIME_SCRIPT_CONTENT));	
			tempfile.commit();
		}
		else 	cout << "### NMSync::Error -> " << scriptfile << " wasn't succesfully created" << endl;

		// We've got to send the script via sftp in order to execute it:

		NMFtp::Error error;

		if((error = sftp.connectToHost(nms_source, nms_login, nms_password, 22, true)) == 0)
                {
			#ifdef DEBUG
			cout << "### NMSync::Sending script file " << scriptfile << " to remote host ..." << endl;
			#endif

			sftp.commandFinished.connect(this,&NMSync::syncScriptSent);
			sftp.cd("/tmp");
			sftp.put(scriptfile);
			sftp.close();
                }
		
		else
		{
			#ifdef DEBUG
			cerr << "### NMSync::Error -> connecting via sftp to host :: " << nms_source << endl;
                        #endif

			if(error==NMFtp::WrongPassword)		cerr << "Wrong password" << endl;
                        else if(error==NMFtp::Timeout)		cerr << "Connection Timeout" << endl; 
                        else if (error==NMFtp::IsConnected)	cerr << "Alredy connected" << endl;
                        else if (error==NMFtp::Unknow)		cerr << "Unknown sftp error" << endl;
			else					cerr << "Error type Unknown" << endl;
		}
	}

	// Extern local:

	else 
	{ 
		if(protocol == "IPv4")		source.setAddress("127.0.0.1");
		else 				source.setAddress("::1");

		nme_sync = new NMExternalLocal("local_sync", true, this);	
	}

	// Distinction between Single Mesure (sleeptime = 0) and Polling (sleeptime > 0):
	
	if(!poll)	*nme_sync << "echo `ntpq -c as` `ntptime`";
	else		*nme_sync << "/bin/bash " + scriptfile + " " + sleeptime;

	nmxml_data.freeTree();

	#ifdef DEBUG
		cout << "### COMMAND: " << nme_sync->shellCommand() << endl;
	#endif
}

// function called when the script has been sent to the remote host

void NMSync::syncScriptSent()
{
        if (!active)
	{
		cout << "### NMSync::syncScriptSent() -> shellscript sent to remote machine" << endl;

		// We're ready to run the sync command:
	
		if(!nme_sync) 
		{
			*this << NMError(NMError::user, "First send configure data!");
			exit();
			return;
		}
	
		// execute sync command
	
		if(nme_sync->run() != NMExternal::NoError)
		{
			*this << NMError(NMError::user, "Problem to execute command sync");
			exit();
			return;
		}
	

		active = true;
		nme_sync->finished.connect(this, &NMSync::processExit);
		nme_sync->readyReadOutput.connect(this, &NMSync::parseResult);
	}
}

void NMSync::parseResult()
{
	if(active)
	{
		cout << "### NMSync :: parseResult (ntptime response::recieved) ###" << endl; 
		
		NMString line;
		int initial, final;
		line = nme_sync->stdInOut.readWholeLine();
		// cout << line << endl << endl << endl;
	
		// parse pps result:
			
		if(line.search("pps.peer") >= 0)
			result_pps = OK;
			
		// parse ntp result:
			
		if(line.search("OK") >= 0) 
			result_ntp = OK;
			
		else if (line.search("ERROR") >= 0) 
			result_ntp = ERROR;
			
		else if (line.search("command not found") >= 0)
			result_ntp = NOTFOUND;
	
		// other useful data:
		
		final = line.search("maximum");
		time = line.subString(final - 25, 8) + line.subString(final - 10, 7);
	
	        initial = line.search("maximum");
	        final = line.search("estimated");
		maximum_error = line.subString(initial+14, final-initial-16);
	
		initial = line.search("estimated");
		final = line.search("ntp_adjtime");
		estimated_error = line.subString(initial+16, final-initial-17);
	
		initial = line.search("offset");
		final = line.search("frequency");
		offset = line.subString(initial+7, final-initial-9);

		// add new data to the list (with polling) 
		addElement();
	}
}

void NMSync::addElement()
{
	// Add new ntp mesure to the list:
	
	SyncItem item;
	NMString nms_resultstr;

	item.offset = offset;
	item.maximum_error = maximum_error;
	item.estimated_error =  estimated_error;
	item.timestamp = time;

	nms_resultstr = NMString::number(result_ntp);
	item.ntp_status = nms_resultstr;

	nms_resultstr = NMString::number(result_pps);
	item.pps_status = nms_resultstr;

	resultList.push_back(item);
}

void NMSync::processExit()
{
	#ifdef DEBUG
		cout << "### NMSync::processExit()" << endl;
        #endif

	exit();
}

// Exiting in Polling mode (Asynchronous):

void NMSync::stop(void *data)
{
	// 1) Only one stop is permitted
	
	if(active)	active = false;
	else 		return;

	#ifdef DEBUG
        	cout << "###################################################" << endl;
		cout << "   NMSync::stop() " << endl;
        	cout << "###################################################" << endl;
	#endif	
	
	// 2) generate xml with all the measures (includes a kill to nmexternal):
	
	stopAndGetResults(data);
}

void NMSync::stopAndGetResults(void *data)
{
	#ifdef DEBUG
		cout << "### NMSync::stopAndGetResults -> gathering results..." << endl;
	#endif
	
	// Select a file:
	list<SyncItem>::iterator iter;
	resultfile = "/tmp/ntptime" + (NMString)testId() + ".time";

	// Write to a file:
	NMXMLStreamWriter xmlw( resultfile );
	xmlw.setAutoFormatting( true );
	xmlw.writeStartDocument();			// Start of DOCUMENT

	xmlw.writeStartElement("test");			// Start of data
	xmlw.writeStartElement("station");		// Start of station
	xmlw.writeTextElement("address", address);
	xmlw.writeEndElement();				// End of station
	

	int i = 0;
	xmlw.writeStartElement("data");

        for (iter = resultList.begin(); iter != resultList.end(); iter++) 
	{
		xmlw.writeStartElement("ntpdata");

		xmlw.writeAttribute("id", NMString::number(i));
		xmlw.writeAttribute("time", iter->timestamp);
		xmlw.writeAttribute("pps", iter->pps_status);
		xmlw.writeAttribute("status", iter->ntp_status);
		xmlw.writeAttribute("maxerr", iter->maximum_error);
		xmlw.writeAttribute("estimatederr", iter->estimated_error);
		xmlw.writeAttribute("offset", iter->offset);
		
		i++;
		xmlw.writeEndElement();
        }
	xmlw.writeEndDocument();
	resultList.clear();

	// Kill the external remote.
	sendLastData();

	if(!nme_sync->killsignal())	
		cout << "### ERROR: failed to terminate nme_sync" << endl;
	else
		cout << "### OK: nme_sync terminated" << endl;

	// Exit
	exit();
}

void NMSync::sendLastData()
{
	NMXML xmlSync;
	NMString nms_syncdata;
	NMString nms_resultstr, nms_resultstr2;

	nms_resultstr = NMString::number(result_ntp);
	nms_resultstr2 = NMString::number(result_pps);
		
	xmlSync.newTree("ntpdata");
	xmlSync.addData("ip", address);
	xmlSync.addData("pps", nms_resultstr2); 
	xmlSync.addData("status", nms_resultstr);
	xmlSync.addData("maxerr", maximum_error);
	xmlSync.addData("estimatederr", estimated_error); 
	xmlSync.addData("offset", offset);
	xmlSync.addData("time", time);
	xmlSync.addData("time_file", resultfile);

	nms_syncdata = xmlSync.toString();
	xmlSync.freeTree();
	*this << nms_syncdata;
}

void NMSync::status(void *data)
{
}

