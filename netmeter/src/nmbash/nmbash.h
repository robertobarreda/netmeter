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
#ifndef NMBASH_H
#define NMBASH_H

#define VERSION 0.1
#define NAME NMBash

#include <iostream>
#include <stdlib.h>
#include <nmsocketremote.h>
#include <nmhost.h>
#include <nmxml.h>
#include <nmthread.h>
//#include<nmthreadfunc.h>


#define MAXCOMMANDS 4
#define MAXTASK 15

struct NMBServerInfo {
	NMString name;
	NMString version;
};

/*Structs per les funcions disponibles en la consola*/

typedef NMString (*func) (NMString args);
typedef void (*funcvoid) (NMString args);

struct function_info {
	NMString help;
	func fmain;
	funcvoid print;
};

struct function_entry {
	NMString command;
	function_info *info;
};

class NMPlugin {
	
public:
	NMPlugin(){};
	~NMPlugin(){};
	NMString name;
	NMString capability;
	NMString version;
};	

const NMString NMBashName = "NMBash";
const NMString NMBashVersion = "0.1";

struct NMTaskManager
{
	NMString pluginname;
	NMString action;
	NMString id;
	bool isbusy;
};

//Llista de tasques pendents

/**
@author Pau Capella
*/
using namespace std;
class NMBash: public NMThread{

	NMSocketRemote nmsr_client;
	NMHost nmh_server;
	unsigned int port;

	NMBServerInfo info;
	
	NMTaskManager tasks[MAXTASK];
	//llista on guardem tots els plugins passats per Netmeter.
	list<NMPlugin> pluginslist;
	
public:

    NMBash(NMString nms_addr, unsigned int ui_port);
    ~NMBash();

    void init();

    private:

	int openConnection();
	int talkWithServer();
	void welcomeBash();
	void initBash();
	void executeCommand(NMString commnand);

	void generaTask(NMString peticio);
        void tractaXML(char *buffer);
	void run();

};


#endif
