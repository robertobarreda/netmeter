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

NMString nms_id;

struct function_info info_nmping = 
{
    "Make a conectivity test with two hosts.\nUsage >nmping [start|stop|config|stat]",
    nmpingMain,
    nmpingPrint
};

NMString nmpingConfigure(NMString args)
{
	int ipversion=0;
	char host_src[100];
	char host_dest[100];
	char login[100];
	char password[100];
	bool sourceislocal=true;
	int random = rand() %1000;
//	NMString nms_id;

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
	
	/*Generem la petició.*/
	
	NMXML xml_nmping;
	NMString nms_nmping;
	
	xml_nmping.newTree("configure");
	xml_nmping.addData("plugin", "nmping");
	nms_id = nms_id.number(random,10);
	//nms_id = "5001";

	xml_nmping.addData("id", nms_id);

	xml_nmping.addData("data");

	if(ipversion == 4) 
		xml_nmping.addData("protocol", "IPv4");
	else 
		xml_nmping.addData("protocol", "IPv6");

	xml_nmping.addData("source");
	xml_nmping.addData("name", host_src);

	if(!sourceislocal) {
		xml_nmping.addData("login", login);
		xml_nmping.addData("password", password);
	}
	xml_nmping.goUp();

	xml_nmping.addData("destination");
	xml_nmping.addData("name", host_dest);

	nms_nmping = xml_nmping.toString();
	xml_nmping.freeTree();
	
	return nms_nmping;
}

NMString nmpingStart()
{

	NMXML xml_nmping;
	NMString nms_nmping;

//	int random = rand() %1000;
//	NMString nms_id;
	//nms_id = nms_id.number(random,10);
//	nms_id = "5001";

	xml_nmping.newTree("start");
	xml_nmping.addData("plugin", "nmping");
	xml_nmping.addData("id", nms_id);
	
	nms_nmping = xml_nmping.toString();
	xml_nmping.freeTree();
	
	return nms_nmping;

}

NMString nmpingStatus()
{
	NMXML xml_nmping;
	NMString nms_nmping;

//	int random = rand() %1000;
	NMString nms_id;
	nms_id = "5001";

	xml_nmping.newTree("status");
	xml_nmping.addData("plugin", "nmping");
	xml_nmping.addData("id", nms_id);
	
	nms_nmping = xml_nmping.toString();
	xml_nmping.freeTree();
	
	return nms_nmping;

}
NMString nmpingStop()
{
	NMXML xml_nmping;
	NMString nms_nmping;

//	int random = rand() %1000;
	NMString nms_id;
	nms_id = "5001";

	xml_nmping.newTree("stop");
	xml_nmping.addData("plugin", "nmping");
	xml_nmping.addData("id", nms_id);
	
	nms_nmping = xml_nmping.toString();
	xml_nmping.freeTree();
	
	return nms_nmping;

}


void nmpingPrint(NMString args)
{
	NMXML xmlbuffer;
	xmlbuffer.loadTree(args.c_str());

	if(xmlbuffer.nodeExists("/result")) {
		int i_stat = xmlbuffer.getValue("//status").toInt();
		NMString nms_ipsource = xmlbuffer.getValue("//source");
		NMString nms_ipdest = xmlbuffer.getValue("//destination");
		cout << "Conectivity between " << nms_ipsource << " to " << nms_ipdest << " ";
		switch(i_stat) {
				case 0: 
					cout << "is up!. :)" << '\n';
					break;
				
				case 1: 
					cout << "is down. :(" << '\n';
					break;		

				default:
					cout << "has an error. :(" << '\n';
					
		}

		xmlbuffer.freeTree();		
		return;
	}
}

NMString nmpingMain(NMString args)
{
	
	cout << "args";
	NMString nms_retorn="";
	
	if(args == "start")
		nms_retorn = nmpingStart();
	else if(args == "config")
		nms_retorn = nmpingConfigure(args);
	else if(args == "stat")
		nms_retorn = nmpingStatus();
	else if(args == "stop")
		nms_retorn = nmpingStop();
	
	return nms_retorn;
}







