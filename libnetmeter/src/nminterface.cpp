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
#include "nminterface.h"
#include "nmtaskdeattach.h"
#include "nmsettings.h"

NMInterface *NMInterface::interface = 0;

/**
  \fn NMInterface::self() 
  \brief Return an instance of NMInterface
*/
NMInterface *NMInterface::self() 
{
	if(interface)
		return interface;
	return interface = new NMInterface("interfaceGui",0); 
}


NMInterface::NMInterface( const char *name, NMParent *parent)
: NMServerSocket(NMSettings::self()->nmport(), 1, name, parent),
m_attach(false), m_client(0)
{
}


NMInterface::~NMInterface()
{
	closeClient();
}



void NMInterface::newConnection( NMSocket *socket )
{
	/*TODO NMInterface sols pot gestionar un client (m_client) al mateix temps, falta afegir funcionalitats sobre multiples clients*/
	m_client = socket;

	if (attachClient() < 0) return;
		
	m_attach = true;

	m_client->readyRead.connect(this, &NMInterface::petitionClient);
}



/*! 
    \fn NMInterface::attachClient()
    \brief Initial communication with the GUI. 
    \return On succes, 0 is returned. On error -1 is returned.
*/   
int NMInterface::attachClient()
{
	char buffer[BUFFER];
	NMString nms_answer;
	NMInterpret interpret;

	//Esperem que el client ens doni les dades...
	memset(buffer,'\0', BUFFER);
	if(m_client->recv( buffer, BUFFER) <= 0) {
		cerr << NMError( NMError::fatal,"NMInterface failed attachClient: No data recived");
		closeClient();
		return -1;
	}

	nms_answer = interpret.attach( buffer);
	if(!nms_answer.Length()) {
		cerr << NMError( NMError::fatal,"NMInterface failed attachClient: Imposible attach this client");
		closeClient();
		return -1;
	}

	int length = nms_answer.Length();
	if(m_client->send( &length, sizeof(length)) <= 0) {
		cerr << NMError( NMError::fatal,"NMInterface failed attachClient: No data send"); 
		closeClient();
		return -1;
	}
	if(m_client->send( nms_answer.c_str(), nms_answer.Length()) <= 0) {
		cerr << NMError( NMError::fatal,"NMInterface failed attachClient: No data send"); 
		closeClient();
		return -1;
	}

	//Mirem si ha mesures pendent si hi ha, les enviem.
	if(interpret.isPendent()) {
		//Sino ens esperem un poc, ens envia tot dins el mateix paquet.
		sleep(1);
		nms_answer = interpret.pendent();
		int length = nms_answer.Length();
		if(m_client->send(&length,sizeof(length)) <= 0 or
		   m_client->send( nms_answer.c_str(), nms_answer.Length()) <= 0) {
		cerr << NMError( NMError::fatal,"NMInterface failed attachClient: No data send"); 
		closeClient();
		return -1;
		}	
	}
	return 0;
}

/*! 
    \fn NMInterface::close()
    \brief Close server interface socket.
*/   
// void NMInterface::close() {
// 	
// 	closeClient();
// 	nmserver.close();
// }

/*! 
    \fn NMInterface::petitionClient(NMSocket *sock, void *data)
    \brief Static member that wait for petition client
    \param sock Socket to listen
    \param data Data
*/ 
void NMInterface::petitionClient()
{
	char buffer[BUFFER];
	NMString nms_answer;
	memset( buffer,'\0', BUFFER);
	NMInterpret interpret;
	int nbytes;

	nbytes = m_client->recv(buffer, BUFFER);
// 	cout << "***************REBUT**************************" << endl;
// 	cout << buffer;
// 	cout << "*****************************************" << endl;
	if(nbytes <= 0) {
		cerr << NMError(NMError::fatal, "NMInterface failed sendCommand: No data recv. Disconnect the gui");
		NMTaskDeattach *deattach = new NMTaskDeattach( "deattach", NMTaskManager::self());
		NMTaskManager::self()->addNewTask( deattach);
		return;
	}
	interpret.translate(buffer);
	return;
}

/**
\brief Transmit a message to the GUI.
\param command Message to transmit
\return On success, return the number of characters sent. On error, -1 is returned,
*/
 int NMInterface::sendCommand(NMString command)
 {
	m_mutex.lock();

	if (!m_attach) {
		cerr << NMError(NMError::user, "NMInterface failed sendCommand: You try send data without client attached; why?");
		m_mutex.unlock();
		return -1;
	}

	int length = command.Length();
	int rc = m_client->send(&length,sizeof(length));
	if(rc>=0) rc = m_client->send( command.c_str(), command.Length());
	if( rc < 0) {
		cerr << NMError(NMError::user, "NMInterface failed sendCommand: No data send.");
		NMTaskDeattach *deattach = new NMTaskDeattach("deattach", NMTaskManager::self());
		NMTaskManager::self()->addNewTask(deattach);
		m_attach = false;
		m_mutex.unlock();
		return -1;
	}
	usleep(400); //Evitem que els missatges es solapin. Es molt cutre!.
	m_mutex.unlock();
 	return rc;
}


/**
\brief Close socket client..
*/
void NMInterface::closeClient()
{
 	if(m_client != 0) {
		if (m_client->readyRead.isConnected())
			m_client->readyRead.disconnect();
 		m_client->close();
 		delete m_client;
 		m_client = 0;
		m_attach = false;
 	}
}

/**
\fn NMInterface::isAttach() const;
\brief Return true if there are a client connected. Otherwise return false
*/
bool NMInterface::isAttach() const
{
	return m_attach;
}
