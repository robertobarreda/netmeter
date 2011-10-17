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
#include <nminterpret.h>
#include <nmmodulemanager.h>
#include <nmerror.h>


#include <nmtaskquit.h>
#include <nmtaskdeattach.h>
#include <nmtaskstart.h>
#include <nmtaskstop.h>
#include <nmtaskstat.h>
#include <nmtaskconfigure.h>

NMString NMInterpret::client = "Unknow";
NMString NMInterpret::version = "0.0";

list<NMString> *NMInterpret::pendingqueue = 0;

NMInterpret::NMInterpret()
{
	if(!pendingqueue) pendingqueue = new list<NMString>();	
}


NMInterpret::~NMInterpret()
{
}

/*! 
    \fn NMInterpret(char *xmlData)
    \brief Understand client request and generates an answer.
    \param xmlData Request of the client in XML format.
    \return XML answer for the client.
*/  
void NMInterpret::translate( char *xmldata)
{
	NMXML nmxml_buffer;
	NMString root;
	NMString id;
	
	//Carguem el xml desde memoria
	if( nmxml_buffer.loadTree(xmldata) < 0) {
		cerr << "NMInterpret failed translate: I can\'t load XML tree." << endl;
		generateNACK( 0);
		nmxml_buffer.freeTree();
		return;
	}

	//Quin identificador té
	id = nmxml_buffer.getValue( "/*/id");
	if(id.IsEmpty()) {  
		cerr << "NMInterpret failed translate: Not a valid id." << endl;
		generateNACK( 0);
		nmxml_buffer.freeTree();
		return;
	}

	//De quin tipus és el missatge enviat!
	root = nmxml_buffer.getElement( "/*");
	if( root.IsEmpty()) {  
		cerr << "NMInterpret failed translate: Not a valid petition." << endl;
		generateNACK( id.toInt() );
		nmxml_buffer.freeTree();
		return;
	}
	

	if(root == "quit") {
		//Enviem el ack per el usleep que hi ha el NMInterface!. Quan millorem aquella part això es pot borrar.
		nmxml_buffer.freeTree();
		generateACK( id.toInt() );
		NMTaskQuit *quit = new NMTaskQuit( "quit", NMTaskManager::self());
		NMTaskManager::self()->addNewTask( quit);
		delete pendingqueue;
		return;
	}

	else if(root == "deattach") {
		//Enviem el ack per el usleep que hi ha el NMInterface!. Quan millorem aquella part això es pot borrar.
		nmxml_buffer.freeTree();
		generateACK( id.toInt() );
		NMTaskDeattach *deattach = new NMTaskDeattach( "deattach", NMTaskManager::self());
		NMTaskManager::self()->addNewTask( deattach);
		return;	
	}
	
	else {

		NMString plugin;
		int testid;
		NMString test;
		NMString xmldata;

		plugin = nmxml_buffer.getValue( "/*/plugin");
		if(plugin.IsEmpty()) {
			cerr << "NMInterpret failed attach(): Malformed xml data." << endl;
			generateNACK( id.toInt());
			nmxml_buffer.freeTree();
			return;
		}

		//Exsisteix el plugin? no se, preguntem-ho al ModuleManager
		if(!NMModuleManager::self()->exsistModule( plugin.c_str())) {
			cerr << "NMInterpret failed configure: Plugin don't exsist." << endl;
			generateNACK( id.toInt());
			nmxml_buffer.freeTree();
			return;		
		}

		test = nmxml_buffer.getValue( "/*/test");
		if(test.IsEmpty()) {
			cerr << "NMInterpret failed attach(): test malformed xml data." << endl;
			generateNACK( id.toInt());
			nmxml_buffer.freeTree();
			return;		
		
		}	
		testid = test.toInt();

	
		if(nmxml_buffer.nodeExists( "/*/data")) 
			xmldata = nmxml_buffer.getTree( "/*/data", plugin.c_str());	
		
		if(root == "configure") {
			NMTaskConfigure *configure = new NMTaskConfigure( testid, plugin, xmldata, "configure", NMTaskManager::self());
			NMTaskManager::self()->addNewTask( configure);
			
		}

		else if(root == "start") {
			NMDateTime dt;
			if(nmxml_buffer.nodeExists( "/*/data/starttime")) {
				NMString time = nmxml_buffer.getValue( "/*/data/starttime");
				dt = NMDateTime::fromString( time);
				if ( dt.isNull() || !dt.isValid()) {
					//TODO Missatge de error
					cerr << "NMInterpret Error: Time not valid" << endl;
					generateNACK( id.toInt());
					nmxml_buffer.freeTree();
                                        return;
                                }
			}
			NMTaskStart *start = new NMTaskStart( testid, plugin, xmldata, dt, "start", NMTaskManager::self());
			NMTaskManager::self()->addNewTask( start);
		}

		else if(root == "stop") {
			NMTaskStop *stop = new NMTaskStop( testid, plugin, xmldata, "stop", NMTaskManager::self());
			NMTaskManager::self()->addNewTask( stop);
			
		}

		else if(root == "status") {
			NMTaskStat *stat = new NMTaskStat( testid, plugin, xmldata, "stat", NMTaskManager::self());
			NMTaskManager::self()->addNewTask( stat);
		}

		else {	
			cerr << "NMInterpret failed translate: I don't know the key " << root.c_str() << endl;
			generateNACK( id.toInt());
			nmxml_buffer.freeTree();
			return;
		}
	}
	nmxml_buffer.freeTree();
	generateACK( id.toInt() );
}

/*! 
    \fn NMInterpret::attach()
    \brief Process the attach petition.
    \param xmlbuffer XML client petition.
    \return Answer for the client.
*/  
NMString NMInterpret::attach(char *xmldata)
{
	NMString answer;
	NMXML nmxml_buffer;

	if (nmxml_buffer.loadTree( xmldata) < 0) {
		cerr << "NMInterpret failed translate: I can\'t load XML tree." << endl;
		return "";
	}

	NMInterpret::client = nmxml_buffer.getValue( "/attach/client");
	if(NMInterpret::client == "") {
		cerr << "NMInterpret failed attach(): Malformed xml data." << endl;
		return "";
	}

	NMInterpret::version = nmxml_buffer.getValue( "/attach/version");
	if(NMInterpret::version == "") {
		cerr << "NMInterpret failed attach(): Malformed xml data." << endl;
		return "";
	}

	nmxml_buffer.freeTree();

	#ifdef DEBUG
	cout << "NMInterpret attach debug: Client " << client.c_str() << " v." << version.c_str() << " conected!" << endl;
	#endif
	
	//Generem la resposta per el client
	answer = generateAttach();
	return answer;
}

/**
\fn NMInterpret::generateAttach()
\brief Generate the answer for the attach petition.
\return Answer for the client.
*/
NMString NMInterpret::generateAttach()
{
	NMString nms_answer;
	NMXML nmxml_answer( "attach");
	
	nmxml_answer.addData( "server", "netmeter");
	nmxml_answer.addData( "version", "0.1");
	nmxml_answer.addData( "plugins");
	
	NMModuleManager *modmanager = NMModuleManager::self();
	int i_numofplug = modmanager->modulelist.size();

	for (int i=0; i < i_numofplug; i++) {
		list<NMXMLAttr> *attlist = new list<NMXMLAttr>;
		NMXMLAttr attribute;
		attribute.name = "id";
		attribute.value = i+1;
		attlist->push_back( attribute);

		nmxml_answer.addData( "plugin", "", attlist, true);
		delete attlist;

		NMModule *modul = modmanager->getModule( i);
		
		nmxml_answer.addData( "name", modul->getPluginName());
		nmxml_answer.addData( "capabilities", modul->getCapabilities());
		nmxml_answer.addData( "version", modul->getVersion());

		nmxml_answer.goUp();
	}
	
	nms_answer = nmxml_answer.toString();
	nmxml_answer.freeTree();
	return nms_answer;	
}

/**
\fn NMInterpret::generateAttach()
\brief Generate the answer for the pendent tests.
\return Answer for the client.
*/
NMString NMInterpret::pendent()
{
	NMString nms_pendent;
	NMXML nmxml_pendent("pendent");
	
	while ( pendingqueue->size()) {

		NMString nms_test; 
		NMXML nmxml_test;
		
		nms_test = pendingqueue->front();
		pendingqueue->pop_front();

		nmxml_test.loadTree(nms_test.c_str());
		nmxml_pendent.addData("", nmxml_test);
		nmxml_test.freeTree();	
	}
	
	nms_pendent = nmxml_pendent.toString();
	nmxml_pendent.freeTree();
	return nms_pendent;	
}


/**
\fn NMInterpret::generateACK(const int& id)
\brief Send to GUI a ACK with indetification id.
\param id Identification 
\return On success, return the number of bytes sent. On error, -1 is returned,
*/
int NMInterpret::generateACK ( const int& id)
{
	NMXML xml_ack( "ack");
	NMString nms_ack;
	NMString nms_id;

	nms_id=nms_id.number(id,10);

	xml_ack.addData( "id", nms_id.c_str());
	nms_ack = xml_ack.toString();
	xml_ack.freeTree();
	
	return NMInterface::self()->sendCommand( nms_ack);
}


/**
\fn NMInterpret::generatenACK(const int& id)
\brief Send to GUI a NACK with indetification id.
\param id Identification 
\return On success, return the number of bytes sent. On error, -1 is returned,
*/
int NMInterpret::generateNACK ( const int& id)
{
	NMXML xml_nack( "nack");
	NMString nms_nack;
	NMString nms_id;

	nms_id=nms_id.number( id, 10);

	xml_nack.addData( "id", nms_id.c_str());
	nms_nack = xml_nack.toString();
	xml_nack.freeTree();

	return NMInterface::self()->sendCommand( nms_nack);
}

/**
\fn NMInterpret::generateResult(const int &id, const NMString &pluginame, const NMString &data)
\brief Send to GUI a ACK with indetification id.
\param id Identification 
\param pluginame Name of plugin
\param data Data to send to the GUI.
\return On success, return the number of bytes sent. On error, -1 is returned,
*/
int NMInterpret::generateResult (const int& id, const NMString &pluginname, const NMString& data)
{
	if ( !NMInterface::self()->isAttach()) 
		return generatePendent( pluginname, data);
	else {
		int rc;
		if ( ( rc = generateResult(id, data)) <= 0 )
			return generatePendent( pluginname, data);
		return rc;
	}
}

/**
\fn NMInterpret::generateResult(const int& id, const NMString& data)
\brief Send to GUI a ACK with indetification id.
\param id Identification 
\param data Data to send to the GUI.
\return On success, return the number of bytes sent. On error, -1 is returned,
*/
int NMInterpret::generateResult (const int& id, const NMString& data)
{
	NMXML nmxml_result( "result");
	NMXML nmxml_data;

	NMString nms_result;
	NMString nms_id;

	nms_id = nms_id.number( id, 10);

	nmxml_result.addData( "id", nms_id.c_str());

	nmxml_data.loadTree( data.c_str());
	if(nmxml_data.nodeExists( "/data")) 
		nmxml_result.addData( "",nmxml_data);
	else
		nmxml_result.addData( "data",nmxml_data);
	
	nms_result = nmxml_result.toString();
	nmxml_data.freeTree();
	nmxml_result.freeTree();

	return NMInterface::self()->sendCommand( nms_result);
}

/**
\fn NMInterpret::generatePendent(const NMString &pluginame, const NMString& data)
\brief Queue the test result
\param pluginame Name of plugin
\param data Data to send to the GUI.
\return On success, return 0. On error, -1 is returned,
*/
int NMInterpret::generatePendent( const NMString &pluginname, const NMString& data)
{	
	int cont = pendingqueue->size() + 1;
		
	NMXML nmxml_data;
	NMXML nmxml_test;
	NMString nms_test; 
		
	nmxml_data.loadTree( data.c_str()); 

	list<NMXMLAttr> attlist;
		
	NMXMLAttr attribute;
	// Identificador del test
	attribute.name = "id";
	attribute.value = cont;
	attlist.push_back( attribute);
	// Nom del plugin que ha fet el test
	attribute.name = "plugin";
	attribute.value = pluginname;
	attlist.push_back(attribute);
	// Data en que ha finalitzat el test
	attribute.name = "finished";
	attribute.value = NMDateTime::currentDateTime().toString();
	attlist.push_back( attribute);

	nmxml_test.newTree( "test",&attlist);

	if(nmxml_data.nodeExists( "/data")) 
		nmxml_test.addData( "",nmxml_data);
	else
		nmxml_test.addData( "data",nmxml_data);

	nms_test = nmxml_test.toString();

	pendingqueue->push_back( nms_test);

	nmxml_test.freeTree();
	nmxml_data.freeTree();
	return 0;
}

#if 0
/**
\fn NMInterpret::generateStatus(const int& id, const int& status)
\brief Send to GUI a ACK with status.
\param id Identification 
\param status Plugin status to send
\return On success, return the number of bytes sent. On error, -1 is returned,
*/
int NMInterpret::generateStatus (const int& id, const int& status)
{
	NMXML xml_ack("ack");

	NMString nms_id;
	NMString nms_status;
	NMString nms_ack;

	nms_id = nms_id.number(id, 10);
	nms_status = nms_status.number(status, 10);

	xml_ack.addData("id", nms_id.c_str());
	xml_ack.addData("data");
	xml_ack.addData("status", nms_status);

	nms_ack = xml_ack.toString();
	xml_ack.freeTree();
	
	return NMInterface::self()->sendCommand(nms_ack);

}
#endif

/**
\fn NMInterpret::generateError(const int& testid, const NMString& type, const NMString& message)
\brief Send to GUI a ACK with status.
\param testid Identification
\param type Error class
\param message Plugin status to send
\return On success, return the number of bytes sent. On error, -1 is returned,
*/
int NMInterpret::generateError (const int& testid, NMError error)
{
	NMXML xml_err( "error");

	NMString nms_id;
	NMString nms_err;
	NMString nms_errtype;

	nms_id = nms_id.number( testid, 10);
	nms_errtype = nms_errtype.number( error.getType(), 10);

	xml_err.addData( "test", nms_id.c_str());
	xml_err.addData( "type", nms_errtype.c_str());
	xml_err.addData( "message", error.getMessage().c_str());

	nms_err = xml_err.toString();
	xml_err.freeTree();
	
	return NMInterface::self()->sendCommand(nms_err);
}

/**
\fn NMInterpret::generateStatus(const int& id, NMStatus status)
\brief Send to GUI a ACK with status.
\param tesid Identification 
\param status Message to send
*/
int NMInterpret::generateStatus( const int &testid, NMStatus status)
{
	NMXML xml_status( "status");
	NMXML xml_data;

	xml_data.loadTree( status.message().c_str()); 

	NMString nms_id;
	NMString nms_message;

	nms_id = nms_id.number(testid, 10);

	xml_status.addData( "test", nms_id.c_str());
	
	if(xml_data.nodeExists( "/data")) 
		xml_status.addData( "",xml_data);
	else 
		xml_status.addData( "data",xml_data);


	nms_message = xml_status.toString();
	xml_status.freeTree();
	
	return NMInterface::self()->sendCommand( nms_message);
}

/**
\fn NMInterpret::isPendent()
\brief Return true if there are a pendent tests, otherwise return false
*/
bool NMInterpret::isPendent()
{
	if (pendingqueue->size()) return true;
	return false;
}
