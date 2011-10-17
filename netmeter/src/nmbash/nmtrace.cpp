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
#include "nmtrace.h"


struct function_info info_nmtrace = 
{
    "Make a conectivity test with two hosts.\nUsage >nmtrace [start|stop|config|stat]",
    nmtraceMain,
    nmtracePrint
};


NMString nmtraceConfigure(NMString args)
{
#if 0
	int offprot;
	int offsrc;
	int offdest;

	NMString destination;
	NMString source;
	NMString protocol;
	int random = rand() %1000;
	NMString nms_id;
	
	nms_id = nms_id.number(random,10);

	offprot = args.search("-p");
	offsrc = args.search("-s");
	offdest = args.search("-d");

	if(offprot > 0) {
		protocol = args.subString(offprot+2,offprot+7);
		protocol = protocol.trim();
		if(protocol != "IPv4" && protocol != "IPv6") {
			cerr << "Error protocol no exsistent...." << '\n';
		}
	}
	else {
		offprot = 0;
		protocol = "IPv4";
	}
	if(offsrc > 0) {
		source = args.subString(offsrc+3,(offdest - offsrc));
		source = source.trim();
	}
	else source = "local";
	
	destination = args.subString( offdest+2,args.Length());
	destination=destination.trim();
	
	/*Generem la resposta...*/
	NMXML xml_nmtrace;
	NMString nms_nmtrace;

	xml_nmtrace.newTree("configure");
	xml_nmtrace.addData("plugin","nmtrace");
	xml_nmtrace.addData("id","43474");
	xml_nmtrace.addData("data");
	
	xml_nmtrace.addData("protocol",protocol);
	
	list<NMXMLKey> host;
	NMXMLKey key;
	key.name = "name";
	key.value = source;
	host.push_back(key);
	
	key.name = "login";
	key.value = "pcapella";
	host.push_back(key);

	key.name = "password";
	key.value = "";
	host.push_back(key);


	xml_nmtrace.addKey("source",host);

	host.clear();
	key.name = "name";
	key.value = destination;
	host.push_back(key);

	
	xml_nmtrace.addKey("destination",host);
	host.clear();
	nms_nmtrace = xml_nmtrace.toString();
	xml_nmtrace.freeTree();
	cout << nms_nmtrace.c_str();

	return nms_nmtrace;
#endif


	int ipversion=0;
	char host_src[100];
	char host_dest[100];
	char login[100];
	char password[100];
	bool sourceislocal=true;
	int random = rand() %1000;
	NMString nms_id;

	memset(host_src,'\0',100);
	memset(host_dest,'\0',100);
	memset(login,'\0',100);
	memset(password,'\0',100);

	cout << "IP version? (4/6) ";
	cin >> ipversion;

	cout << "Source: (type 'local' if you want that source is locahost) ";
	cin >> host_src;
	if(strcmp(host_src,"local")) {
		sourceislocal = false;
		cout << "Login: ";
		cin >> login;
		cout << "Password: ";
		cin >> password;
	}
	
	cout << "Destination: ";
	cin >> host_dest;
	
	/*Generem la petici�..*/
	
	NMString nms_nmtrace;
	NMXML xml_nmtrace("configure");
	
	xml_nmtrace.addData("plugin","nmtrace");
	nms_id = nms_id.number(random,10);
	xml_nmtrace.addData("id",nms_id);

	xml_nmtrace.addData("data");

	if(ipversion == 4) 
		xml_nmtrace.addData("protocol","IPv4");
	else 
		xml_nmtrace.addData("protocol","IPv6");

	xml_nmtrace.addData("source");
	xml_nmtrace.addData("name", host_src);

	if(!sourceislocal) {
		xml_nmtrace.addData("login", login);
		xml_nmtrace.addData("password", password);
	}
	xml_nmtrace.goUp();

	xml_nmtrace.addData("destination");
	xml_nmtrace.addData("name", host_dest);

	nms_nmtrace = xml_nmtrace.toString();
	xml_nmtrace.freeTree();
	
	return nms_nmtrace;
}

NMString nmtraceStart()
{

	NMXML xml_nmtrace;
	NMString nms_nmtrace;

	int random = rand() %1000;
	NMString nms_id;
	nms_id = nms_id.number(random,10);

	xml_nmtrace.newTree("start");
	xml_nmtrace.addData("plugin", "nmtrace");
	xml_nmtrace.addData("id", nms_id);
	
	nms_nmtrace = xml_nmtrace.toString();
	xml_nmtrace.freeTree();
	
	return nms_nmtrace;

}

NMString nmtraceStatus()
{
	NMXML xml_nmtrace;
	NMString nms_nmtrace;

	int random = rand() %1000;
	NMString nms_id;
	nms_id = nms_id.number(random,10);

	xml_nmtrace.newTree("status");
	xml_nmtrace.addData("plugin","nmtrace");
	xml_nmtrace.addData("id",nms_id);
	
	nms_nmtrace = xml_nmtrace.toString();
	xml_nmtrace.freeTree();
	
	return nms_nmtrace;

}
NMString nmtraceStop()
{
	NMXML xml_nmtrace;
	NMString nms_nmtrace;

	int random = rand() %1000;
	NMString nms_id;
	nms_id = nms_id.number(random,10);

	xml_nmtrace.newTree("stop");
	xml_nmtrace.addData("plugin","nmtrace");
	xml_nmtrace.addData("id",nms_id);
	
	nms_nmtrace = xml_nmtrace.toString();
	xml_nmtrace.freeTree();
	
	return nms_nmtrace;

}



NMString nmtraceMain(NMString args)
{
	NMString nms_retorn="";
	
	if(args == "start")
		nms_retorn = nmtraceStart();
	else if(args == "config")
		nms_retorn = nmtraceConfigure(args);
	else if(args == "stat")
		nms_retorn = nmtraceStatus();
	else if(args == "stop")
		nms_retorn = nmtraceStop();
	
	return nms_retorn;
}

void nmtracePrint(NMString args)
{

	NMXML xmlbuffer;
	NMString host; 

	xmlbuffer.loadTree(args.c_str());
	if(xmlbuffer.nodeExists("//status")) {
		int i_stat = xmlbuffer.getValue("//status").toInt();
		switch(i_stat) {
				case -2: 
					cout << "NMTrace is not configured." << '\n';
					break;
				
				case -1: 
					cout << "NMTrace is configured, but is not started" << '\n';
					break;		

				default:
					cout << "NMTrace has found " << i_stat << " hosts" << '\n';
				}

		xmlbuffer.freeTree();		
		return;
	}

/*	if(!xmlbuffer.searchKey("hosts")) {
		xmlbuffer.freeTree();
		return;
	}
	cout << args.c_str() << endl;
	cout << "****Resultat del NMTrace****" << '\n';
	NMXMLNode hostnode;
	xmlbuffer.child(); //Primer host
	hostnode = xmlbuffer.getCurrentNode();
	int contador=0;
	while(hostnode != NULL) {
		NMXMLNode node;

		node = hostnode->xmlChildrenNode;
		cout << "Host " << contador << " --> " << (char *)xmlNodeGetContent(node) << '\n';
		contador ++;
		
		hostnode = hostnode->next;
	}
	cout << "****************************" << '\n';
	
	xmlbuffer.freeTree();*/
}

