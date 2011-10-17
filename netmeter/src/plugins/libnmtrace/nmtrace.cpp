/*
 *   $Id: nmtrace.cpp,v 1.14 2006-06-08 09:50:55 pcapella Exp $
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
#include "nmtrace.h"
#include <nmdns.h>
#include <nmxml.h>

NMTrace::NMTrace()
 : NMModuleInterface()
{
}


NMTrace::~NMTrace()
{
}

void NMTrace::initModule()
{
	nme_traceroute = 0;
}

void NMTrace::removeModule()
{
}

void NMTrace::start(void *data)
{
	listhosts.clear();

	NMString *nms_xml = (NMString *) data;
	if(nms_xml->Length()) configure(data);

	if(!nme_traceroute) {
		exit();
		return;
	}

	if( nme_traceroute->run() != NMExternal::NoError) {
		*this << NMError(NMError::user, "Problem to execute traceroute:");
		exit();
		return;
	}
	nme_traceroute->readyReadOutput.connect(this, &NMTrace::processRecived);
	nme_traceroute->finished.connect(this, &NMTrace::processExit);
}

void NMTrace::processRecived()
{
	NMString line;
	TraceItem item;

	line = nme_traceroute->stdInOut.readWholeLine();

	if(line.search("traceroute") >= 0) return;

	else if(line.search("* * *") >= 0)  {
		item.address = "Unknow";
		item.time = "";
		sendStatus( item );
 		listhosts.push_back( item);
	}
	else {
		int initial;
		int final;

		initial = line.search("(");
		final = line.search(")");
		item.address = line.subString( initial + 1, final - initial - 1 );
		item.time = line.subString( final + 3, line.Length() - final - 2 );

		sendStatus( item );
		listhosts.push_back( item);
	}
}

void NMTrace::processExit()
{
	list<TraceItem>::iterator iter;
        NMXML xmlTrace;
        NMString nms_traceresult;

        xmlTrace.newTree("hosts");
        for ( iter = listhosts.begin(); iter != listhosts.end(); iter++ ) {
		xmlTrace.addData( "host" );
                xmlTrace.addData( "address", iter->address.c_str() );
		xmlTrace.addData( "time", iter->time.c_str() );
		xmlTrace.goUp();
	}

        nms_traceresult = xmlTrace.toString();
        xmlTrace.freeTree();
        listhosts.clear();
        *this << nms_traceresult;
	exit();
}


void NMTrace::stop(void *data)
{
	if(nme_traceroute->isRunning()) 
		nme_traceroute->kill();
}	

void NMTrace::status(void *data)
{
	
}

void NMTrace::configure(void *data)
{
	NMString *nms_xml = (NMString *) data;
	NMXML nmxml_data;
	NMString protocol;
	NMString src;
	NMString dst;

	NMHost source;
	NMHost destination;
	
	nmxml_data.loadTree(nms_xml->c_str());
	
	protocol = nmxml_data.getValue("/*/protocol");
	if(!protocol.Length()) {
		*this << NMError(NMError::user, "No protocol key");
		nmxml_data.freeTree();
		return;
	}
	
	src = nmxml_data.getValue("/*/source/name");
	if(!src.Length()) {
		*this << NMError(NMError::user, "No valid source key");
		nmxml_data.freeTree();
		return;
	}

	dst = nmxml_data.getValue("/*/destination/name");
	if(!dst.Length()) {
		*this << NMError(NMError::user, "No valid destination key");
		nmxml_data.freeTree();
		return;
	}

	//Es una external remote
	if(src != "127.0.0.1") {
		
		NMString nms_login;
		NMString nms_password;
		NMString nms_managementhost;

		if(protocol == "IPv4") {
			source = NMDns::resolv(src, NMDns::IPv4);
			destination = NMDns::resolv(dst, NMDns::IPv4);
		}
		else {
			source = NMDns::resolv(src, NMDns::IPv6);
			destination = NMDns::resolv(dst, NMDns::IPv6);
		}
		
		//Password i login
		nms_managementhost = nmxml_data.getValue("/*/source/management/name");
		nms_login = nmxml_data.getValue("/*/source/management/username");
		nms_password = nmxml_data.getValue("/*/source/management/password");

		nme_traceroute = new NMExternalRemote("remote_traceroute", true, this ); 
		NMExternalRemote *nmer_trace = dynamic_cast<NMExternalRemote *> ( nme_traceroute);

		nmer_trace->setHost(nms_managementhost);
		nmer_trace->setPassword(nms_password);
		nmer_trace->setUsername(nms_login);
	}
	else { //Extern local....

		if(protocol == "IPv4") {
			source.setAddress("127.0.0.1");
			destination = NMDns::resolv(dst, NMDns::IPv4);
		}
		else {
			source.setAddress("::1");
			destination = NMDns::resolv(dst, NMDns::IPv6);
		}

		if(destination.isNull()) {
			NMString msg = "I can not resolv the address: " + dst;
			*this << NMError(NMError::user, msg);
			return;
		}

		nme_traceroute = new NMExternalLocal("local_traceroute", true, this);		
	}

	*nme_traceroute << "traceroute";
	*nme_traceroute << "-m 8";
	*nme_traceroute << destination.getAddress();

	//Insert the source host ;)
	TraceItem item;
	item.address = source.getAddress();
	item.time = "0 ms";
	listhosts.push_back( item);

	nmxml_data.freeTree();
}


/**
	Send and status message with the last host found
*/
void NMTrace::sendStatus( TraceItem item)
{
	NMXML xmltrace;
	NMStatus status;

	xmltrace.newTree("host");
	xmltrace.addData("address", item.address.c_str());
	xmltrace.addData("time", item.time.c_str());
	status.setMessage(xmltrace.toString());
	xmltrace.freeTree();

	//Send status msg...
	*this << status;
}

