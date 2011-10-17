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
#include "nmping.h"

NMPing::NMPing()
 : NMModuleInterface()
{
}


NMPing::~NMPing()
{
}

void NMPing::initModule()
{
	nme_ping = 0;	
}

void NMPing::removeModule()
{
}

void NMPing::start(void *data)
{
	NMString *nms_xml = (NMString *) data;
	if(nms_xml->Length()) configure(data);

	if(!nme_ping) {
		*this << NMError(NMError::user, "First send configure data!");
		exit();
		return;
	}

	if(!(nme_ping->run())) {
		*this << NMError(NMError::user, "Problem to execute command ping");
		exit();
		return;
	}

	nme_ping->readyReadOutput.connect(this, &NMPing::processRecived);
	nme_ping->finished.connect(this, &NMPing::processExit);
}

void NMPing::processRecived()
{
	NMString line;
	line = nme_ping->stdInOut.readLine();
	
	if(line.search("1 received") >= 0 || line.search("1 packets received") >= 0) result = HOST_UP;
	
	else if (line.search("unknown host") >= 0 || line.search("Network is unreachable") >=0) result = HOST_ERROR;
			
	else if (line.search("+1 errors") >= 0 || line.search("100% packet loss") >=0) result = HOST_DOWN;
}

void NMPing::processExit()
{
	NMXML xmlPing;
	NMString nms_pingresult;
	
	nms_pingresult = nms_pingresult.number(result);
	xmlPing.newTree("data");
	xmlPing.addData("source", source.getAddress());
	xmlPing.addData("destination", destination.getAddress());
	xmlPing.addData("status", nms_pingresult);
	
	nms_pingresult = xmlPing.toString();
	xmlPing.freeTree();
	*this << nms_pingresult;
	exit();
}
	
void NMPing::stop(void *data)
{
	
	//return 0;
}

void NMPing::status(void *data)
{
	//return stat;
}

void NMPing::configure(void *data)
{
	
	NMString *nms_xml = (NMString *) data;
	NMXML nmxml_data;
	NMString protocol;
	NMString src;
	NMString dst;

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
		exit();
		return;
	}

	dst = nmxml_data.getValue("/*/destination/name");
	if(!dst.Length()) {
		*this << NMError(NMError::user, "No valid destination key");
		nmxml_data.freeTree();
		exit();
		return;
	}

	//Es una external remote
	if(src != "local") {
		
		NMString nms_login;
		NMString nms_password;

		if(protocol == "IPv4") {
			source = NMDns::resolv(src, NMDns::IPv4);
			destination = NMDns::resolv(dst, NMDns::IPv4);
		}
		else {
			source = NMDns::resolv(src, NMDns::IPv6);
			destination = NMDns::resolv(dst, NMDns::IPv6);
		}
		
		//Password i login
		nms_login = nmxml_data.getValue("/*/source/login");
		nms_password = nmxml_data.getValue("/*/source/password");

		nme_ping = new NMExternalRemote("remote_ping", true, this ); 
		NMExternalRemote *nmer_ping = (NMExternalRemote *)nme_ping;
		nmer_ping->setHost(source);
		nmer_ping->setPassword(nms_password);
		nmer_ping->setUsername(nms_login);
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

		nme_ping = new NMExternalLocal("local_ping", true, this);		
	}

	*nme_ping << "ping";
	*nme_ping << "-c 1";
	*nme_ping << destination.getAddress();

	nmxml_data.freeTree();
}


