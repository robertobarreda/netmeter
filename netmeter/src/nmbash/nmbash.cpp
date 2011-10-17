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
#include "nmbash.h"
#include "help.h"
#include "list.h"
#include "nmtrace.h"
#include "nmping.h"
//#include "oreneta.h"


struct function_entry functions[] =
{
	{"help",&info_help},
	{"list",&info_list},
	{"nmtrace",&info_nmtrace},
	{"nmping",&info_nmping}
//	{"oreneta",&info_oreneta}
};

NMBash::NMBash(NMString nms_addr, unsigned int ui_port)
:NMThread(),
nmsr_client("nmbash_socket", NMSocketRemote::stream, 0),nmh_server(nms_addr),port(ui_port)
{
	for(int i=0; i<MAXTASK; i++) 
		tasks[i].isbusy = false;	
}


NMBash::~NMBash()
{
}

void NMBash::init()
{
	if(openConnection() < 0) {
		cerr << "Connection error" << endl;
		exit(-1);
	}

	welcomeBash();
	//Excuta el thread que va escoltant
	this->start();

	initBash();
	
}

void NMBash::welcomeBash()
{
	cout << "\tWelcome to NetMeter Console." << endl;
	cout << "Conected with " << info.name.c_str() << " " << info.version << " in " << nmh_server.getAddress() << ":" << port << endl;
	cout << "\nList of avaliable Plugins:" << endl;

	cout << "\tName\t\tVersion\t\tCapabilities" << endl;
	cout << "******************************************************\n";
	list<NMPlugin>::iterator iter;
	for ( iter = pluginslist.begin(); iter != pluginslist.end(); ++iter ) {
		cout << "\t" << iter->name.c_str() <<"\t\t" << iter->version.c_str() << "\t\t" << iter->capability.c_str() << endl;
	 }

	cout << "\n\nWrite \'help <command>\' for NetMeter command help\n or \'list\' for avaliable commands." << endl;
}

int NMBash::openConnection()
{
	if (nmsr_client.connect(nmh_server, port) < 0) {
		cerr << "Error en el connect" << endl;
		return -1;
	}
	return talkWithServer();
}
	
	
int NMBash::talkWithServer()
{
	NMXML attach;
	NMString nms_buffer;
	char buffer[2000];
	

	attach.newTree("attach");
	attach.addData("client", NMBashName.c_str());
	attach.addData("version", NMBashVersion.c_str());
	nms_buffer = attach.toString();
	if(nmsr_client.send(nms_buffer.c_str(), nms_buffer.Length()) <=0) {
		cerr << "ERROR: I can not send XML attach" << endl;
		attach.freeTree();
		return -1;
	}
	cerr << "Sent attach:\n" << nms_buffer.c_str() << endl;
	attach.freeTree();
	
	memset(buffer,'\0',2000);
	//Waiting for response...
	nmsr_client.recv(buffer, 2000);
	
	NMString value;
	cout << buffer << endl;
	attach.loadTree(buffer);
	if(!attach.nodeExists("/attach")) {
		cerr << "Malformed XML packet. Root node should be attach." << endl;
		attach.freeTree();
		return -1;
	}
	info.name = attach.getValue("/attach/server");
	if(info.name == "") {
		cerr << "Malformed XML packet. No <server> tag." << endl;
		attach.freeTree();
		return -1;
	}
	info.version = attach.getValue("/attach/version");
	if(info.version == "") {
		cerr << "Malformed XML packet. No <version> tag" << endl;
		attach.freeTree();
		return -1;
	}
	
	if(!attach.nodeExists("/attach/plugins")) {
		cerr << "Malformed XML packet. I was waiting plugins" << endl;
		attach.freeTree();
		return -1;
	}

	int i_numofplug = attach.getValue("/attach/plugins/plugin[last()]/@id").toInt();
	for (int i=0; i < i_numofplug; i++) {
		NMPlugin plugin;
		plugin.name = attach.getValue("/attach/plugins/plugin/name", i);
		plugin.capability = attach.getValue("/attach/plugins/plugin/capabilities", i);
		plugin.version = attach.getValue("/attach/plugins/plugin/version", i);
		pluginslist.push_back(plugin);
	}
	attach.freeTree();
	return 0;
}

void NMBash::initBash()
{
	char buffer[100];
	NMString command;
	
	memset(buffer,'\0',100);
	while(command != "quit" && command != "exit" && command != "deattach") {
		cout << "NetMeter> ";
		cin.getline(buffer,100);
		command = buffer;
		memset(buffer,'\0',100);
		executeCommand(command);
	}
	//Avisem a netmeterd que he sortit....
	NMString nms_quit;
	NMXML xml_quit;
	
	if(command == "deattach") {
		xml_quit.newTree("deattach");
		xml_quit.addData("id","343282");
		nms_quit = xml_quit.toString();
		xml_quit.freeTree();
	}
	else {
		xml_quit.newTree("quit");
		xml_quit.addData("id","3432");
		nms_quit = xml_quit.toString();
		xml_quit.freeTree();
	}

	if(nmsr_client.send(nms_quit.c_str(), nms_quit.Length()) <=0) {
		cerr << "ERROR: I can not send XML attach" << endl;
		xml_quit.freeTree();
		pluginslist.clear();
		nmsr_client.close();
		exit(1);
	}
	pluginslist.clear();
	nmsr_client.close();
	//thread.killThread();
}

//funcio principal, espera les comanades introduides per l'usuari i avisa a les corresponets classes.
void NMBash::executeCommand( NMString command )
{
	int index;
	NMString args("");
	//Separem lo que es la commnada dels arguments...
	command.trim();
	NMString nms_peticio;

	if(command == "") return; 

	if((index = command.search(" ")) > 0) {
		args = command.subString(index+1,command.Length() - (index+1));
		command = command.subString(0, index);
	}
	int numcom=0;
	if(command == "exit" || command == "quit" || command == "deattach") return;
	
	//Si la comnada que ha introduit es l'ajuda
	if(command == "help") {
		for(numcom=0;numcom<MAXCOMMANDS;numcom++) {
			if(args == functions[numcom].command) {
				args = functions[numcom].info->help;
				break;
			}	
		}
		if(numcom == MAXCOMMANDS) {
			cout << args.c_str() << ":help not available." << endl;
			return;
		}
	}

	
	//*************************************/
	numcom = 0;
	/*Quina comanda hem introduit?*/
	for(numcom=0; numcom<MAXCOMMANDS; numcom++) {
		if(command == functions[numcom].command) {
			nms_peticio=functions[numcom].info->fmain(args);
			break;
		}	
	}
	if(numcom == MAXCOMMANDS) {
			cout << command.c_str() << ":Command not found" << endl;
			return;
	}
	else if(nms_peticio.Length() > 0) {
		generaTask(nms_peticio);
		if(nmsr_client.send(nms_peticio.c_str(), nms_peticio.Length()) <=0) {
			cerr << "Your petition is not sent. Try again...." << endl;
			pluginslist.clear();
			nmsr_client.close();
			exit(1);
		}
		/*else {
			char buffer[100];
			memset(buffer,'\0',100);
			if(nmsr_client.recv(buffer, 100) <=0) {
				cerr << "Your petition is not send. Try again...." << endl;
				pluginslist.clear();
				nmsr_client.close();
				exit(1);
			}
		}*/

			
			
	}
}

//Aquesta funcio extreu la informacion de la tasca i la guarda a l'estructura NMTaskManager
void NMBash::generaTask(NMString peticio)
{
	NMXML nmxml_buffer;
	NMString action;
	NMString plugin;
	NMString id;

	//cout << "\ngeneraTask called with buffer:\n" << peticio.c_str() << endl;

	if (nmxml_buffer.loadTree(peticio.c_str()) < 0) {
		cerr << "NMBash failed translate: I can\'t load XML tree." << endl;
		return;
	}

	action = nmxml_buffer.getElement("/*");
	if(action == "") {
		cerr << "NMBash failed attach(): Malformed xml data." << endl;
		return;
	}

	plugin = nmxml_buffer.getValue("/*/plugin");
	if(plugin == "") {
		cerr << "NMBash failed attach(): Malformed xml data." << endl;
		return;
	}

	id = nmxml_buffer.getValue("/*/id");
	if(id == "") {
		cerr << "NMBash failed attach(): Malformed xml data." << endl;
		return;
	}
	nmxml_buffer.freeTree();
	
	for(int i=0;i<MAXTASK;i++) {
		if(tasks[i].isbusy == false) {
			tasks[i].action = action;
			tasks[i].id = id;
			tasks[i].pluginname = plugin;
			tasks[i].isbusy = true;
			break;
		}
	}
}

//Sols espera peticions del core
void NMBash::run()
{
	while(1) {
		if(nmsr_client.waitMS(0) > 0) {
			char buffer[1024];
			memset(buffer,'\0',1024);
			if(nmsr_client.recv(buffer,1024) <= 0) {
				cerr << "Client closed! :)" << endl;
				nmsr_client.close();
				return;
			}
			tractaXML(buffer);
		}
		if(!nmsr_client.isOpen()) break;
	}

	
}


void NMBash::tractaXML(char *buffer)
{

	NMXML xmlbuffer;
	xmlbuffer.loadTree(buffer);
	NMString nms_pluginname;
	NMString nms_action;

	cout << "Paquet XML rebut del core:" << endl;
	cout << buffer << '\n';

	if(xmlbuffer.nodeExists("/pendent")) {
		cout << "Pendent Test: " << endl;
		cout << buffer << endl;
		xmlbuffer.freeTree();
		return;
	}
	
	

	NMString nms_id = xmlbuffer.getValue("/*/id");
	if(nms_id == "") {
		cerr << "Malformed XML!" << endl;
		xmlbuffer.freeTree();
		return;
	}
	
	for(int i=0;i<MAXTASK;i++) {
		if(tasks[i].isbusy && tasks[i].id == nms_id ) {
			nms_pluginname = tasks[i].pluginname;
			nms_action = tasks[i].action;				
			
			if(xmlbuffer.nodeExists("/ack")) {
				cout << tasks[i].pluginname << " [ " << tasks[i].action <<  " ] ... ok." << endl;
				/*Quan borrem el config elimina aixo*/
				if(nms_action == "configure") tasks[i].isbusy = false;
				xmlbuffer.freeTree();
				return;
			}

			else if(xmlbuffer.nodeExists("/result")) {
				int numcom;
				for(numcom=0; numcom<MAXCOMMANDS; numcom++) {
		//Pasem de majuscules a miniscules.... aix�pot anar a la classe NMString!
					int enter;
					char prova[50];
					memset(prova,'\0',50);
					strncpy(prova, nms_pluginname.c_str(), nms_pluginname.Length());
					for(int j=0; prova[j] != '\0';j++) {
						enter = (int)prova[j];
						if(enter <= 90 && enter >= 65)
						enter = enter+32;
						prova[j] = (char) enter;
					}
					nms_pluginname = prova;
				
					if(nms_pluginname == functions[numcom].command) {
						tasks[i].isbusy = false;
						functions[numcom].info->print(buffer);
						break;
					}	
				}			
			}
		}
	}
	xmlbuffer.freeTree();
}

