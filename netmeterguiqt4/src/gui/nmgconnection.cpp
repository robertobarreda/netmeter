/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#include "nmgconnection.h"
#include "nmgresulthandler.h"
#include "messagebar.h"

#include <nmgmodulemanager.h>

#include <QDomNode>
#include <QMessageBox>
#include <QString>

#include <iostream>
using namespace std;

NMGConnection * NMGConnection::instance = 0;

/**
 * Constructor of the class NMGConnection
 * @param parent QObject parent of this object
 * @param name Name of the object
 * @return New instance of NMGConnection
 */
NMGConnection::NMGConnection ( QObject *parent, const char *name )
		: QObject ( parent ), establishingConnectionWidget ( 0, "establishingConnectionWidget" )
{
	setObjectName ( name );
	setConnectionStatus ( Disconnected );
	errorDisconnect = 0;
	readBuffer = 0;
	bytesToRead = 0;
	bytesRead = 0;
	sock = new QTcpSocket ( this );
	connect ( sock, SIGNAL ( disconnected() ), this, SLOT ( end() ) );
	connect ( sock, SIGNAL ( readyRead() ), this, SLOT ( readyToRead() ) );
	connect ( sock, SIGNAL ( error ( QAbstractSocket::SocketError ) ), SLOT ( socketError ( QAbstractSocket::SocketError ) ) );
	connect ( sock, SIGNAL ( connected() ), SLOT ( socketConnected() ) );
	connect ( this, SIGNAL ( errorReceived ( QString, int, QString ) ),
	          NMGModuleManager::self(), SLOT ( errorReceived ( QString, int, QString ) ) );
	connect ( this, SIGNAL ( statusReceived ( QString, QString ) ),
	          NMGModuleManager::self(), SLOT ( statusReceived ( QString, QString ) ) );
	connect ( sock, SIGNAL ( disconnected() ), this, SLOT ( connectionClosedByServer() ) );
}

/**
 * Destructor of the class NMGConnection
 */
NMGConnection::~NMGConnection()
{
	delete sock;
}

/**
 * Returns the unique instance of the class
 * @return Instance of the class
 */
NMGConnection * NMGConnection::getConnection()
{
	if ( !instance ) instance = new NMGConnection();
	return instance;
}

/**
 * Return the status of the connection to the server
 * @return Status of the connection to the server
 */
NMGStatus NMGConnection::getConnectionStatus()
{
	return status;
}

/**
 * Initializes the connection with the server
 */
void NMGConnection::start()
{
#ifdef DEBUG
	cout << "NMGConnection starts" << endl;
#endif

	QString address = NMGPreferencesManager::self()->getNetMeterAddress();
	if ( address.isEmpty() )
	{
		address = DEFAULT_ADDR_SERVER;
		NMGPreferencesManager::self()->setNetMeterAddress ( address );
	}

	int port = NMGPreferencesManager::self()->getNetMeterPort();
	if ( port < 0 )
	{
		port = DEFAULT_PORT_SERVER;
		NMGPreferencesManager::self()->setNetMeterPort ( port );
	}

	//setConnectionStatus ( Connected );
	sock->connectToHost ( address, port );

	establishingConnectionWidget.show();
}

/**
 * Ends the connection with the server
 */
void NMGConnection::end()
{
	if ( status != Disconnected )
	{
		if ( errorDisconnect++ > MAX_ERROR_END )
		{
			setConnectionStatus ( Disconnected );
			sock->close();
		}

#ifdef DEBUG
		cout << "NMGConnection ends" << endl;
#endif
		if ( status == WaitingExitAck ) exitServer();
		else deattachServer();
	}
}

/**
 * Returs ture when a message xml has been read
 * @param message
 * @return If true message cotains the message, if false nobody knows
 */
bool NMGConnection::messageRead ( QString & message )
{
	if ( bytesToRead == 0 )
	{
		int n = sock->read ( ( char * ) & bytesToRead, sizeof ( bytesToRead ) );
		if ( n <= 0 )
		{
			bytesToRead = 0;
			cerr << "Error reading size of the message" << endl;
			return false;
		}
		readBuffer = new char[bytesToRead+1];
		readBuffer[bytesToRead] = 0x00;
		bytesRead = 0;
	}
	int read = sock->read ( & ( readBuffer[bytesRead] ), bytesToRead - bytesRead );
	if ( read == bytesToRead - bytesRead )
	{
		QString str ( readBuffer );
		message = str;
		cout << bytesToRead << " " << qPrintable ( message ) << endl;
		delete []readBuffer;
		readBuffer = 0;
		bytesToRead = 0;
		bytesRead = 0;
		return true;
	}
	else bytesRead += read;
	return false;
}

/**
 * Executed when there is data ready to read from the connection's socket
 */
void NMGConnection::readyToRead()
{
	QString message = "";
	if ( !messageRead ( message ) ) return;

	QDomDocument reply;
	switch ( status )
	{
                /* Disconnected and Connected don't wait anything */
		case Disconnected:
		case Connected:
			cerr << "There is incoming data from the server that I don't need." << endl;
			break;

		case AttachSent:
		{
			/* We have sent the attach message to the server, we can read the server's attach reply */
			QMapPluginInformation * map = interpret.parseAttachReply ( message );
			if ( !map ) cerr << " The information of the plugins of the server didn't arrive correctly." << endl;
			else setConnectionStatus ( ReadyToRead );
			break;
		}

		case WaitingDeattachAck:
		{
			/* Deattach message sent to the server, waiting for the ack or nack */
			handleDeattachReply ( message );
			break;
		}

		case WaitingExitAck:
		{
			/* Exit message sent to the server, waiting for the ack or nack */
			handleExitReply ( message );
			break;
		}

		case ReadyToRead:
		{
			NMGIncomingDataInfo * di = interpret.parseIncomingData ( message );
			if ( di )
			{
				if ( di->module.isEmpty() ) di->module = messageCenter.getModuleName ( di->id );

				/* acknowleged message */
				if ( di->replyType == NAck )
				{
					QString message = messageCenter.getMessage ( di->id );
					////TODO: nacks properly check
					cout << "NACK recieved: id " << qPrintable ( di->id ) << ", with the message: " << endl;
					cout << qPrintable ( message ) << endl;
				}
				else if ( di->replyType == Ack ) messageCenter.deleteMessage ( di->id );

				if ( di->replyType == Result or di->replyType == Pendent
				        or di->replyType == StatusServer or di->replyType == Error )
					processResult ( di );
				//NMGModuleManager::->self()->
				/* TODO: look other fields ...*/
				delete di;
			}
			break;
		}
	}
}

/**
 * The connection ends and a exit message is sent to the server
 */
void NMGConnection::exitServer()
{
	if ( getConnectionStatus() == Disconnected )
	{
		emit connectionClosed();
		return;
	}
	if ( status != WaitingExitAck )
	{
		disconnectAttempts = 0;
		QString id = newValidMessageIdentifier();
		QString data = interpret.quit ( id );
		messageCenter.addMessage ( id, data );
		if ( sock->write ( data.toAscii().constData(), data.length() ) < 0 )
			cerr << "ERROR: It's not possible to send the quit message to the server." << endl;
		setConnectionStatus ( WaitingExitAck );
	}
}

/**
 * The connection ends and a deattach message is sent to the server
 */
void NMGConnection::deattachServer()
{
	if ( status != WaitingExitAck and status != WaitingDeattachAck )
	{
		disconnectAttempts = 0;
		QString id = newValidMessageIdentifier();
		QString data = interpret.deattach ( id );
		messageCenter.addMessage ( id, data );
		if ( sock->write ( data.toAscii().constData(), data.length() ) < 0 )
			cerr << "ERROR: It's not possible to send the deattach message to the server." << endl;
		sock->flush();
		setConnectionStatus ( WaitingDeattachAck );
	}
}

/**
 * Method executed when data arrives after having sent the deattach message
 */
void NMGConnection::handleDeattachReply ( QString message )
{
	NMGIncomingDataInfo * di = interpret.parseIncomingData ( message );
	if ( !di ) cerr << "Error while reading deattach reply." << endl;
	else
	{
		QString message = messageCenter.getMessage ( di->id );
		if ( di->replyType == Ack )
		{
			/* check if it's the ack of the deattach message */
			if ( message.indexOf ( "<" + QString ( TAG_DEATTACH ) + ">" ) < 0 )
				messageCenter.deleteMessage ( di->id ); //Ack of a message which wasn't a deattach
			else closeConnection();
		}
		else if ( di->replyType == NAck )
		{
			/* resent the message (3 attempts, if not we close the connection) */

			if ( message.indexOf ( "<" + QString ( TAG_DEATTACH ) + ">" ) < 0 )
				if ( disconnectAttempts > MAX_DISCONNECT_ATTEMPTS )
					closeConnection(); // There is a limit for everything
				else
				{
					send ( message );
					disconnectAttempts++;
				}
			else cerr << "Error of the message with id " << qPrintable ( di->id ) << ":" << qPrintable ( message ) << endl;
		}
		else if ( di->replyType == Result or di->replyType == Pendent or
		          di->replyType == StatusServer or di->replyType == Error )
			processResult ( di );
	}
}

/**
 * Method executed when data arrives after having sent the exit message
 */
void NMGConnection::handleExitReply ( QString message )
{
	NMGIncomingDataInfo * di = interpret.parseIncomingData ( message );
	if ( !di ) cerr << "Error while reading exit reply." << endl;
	else
	{
		QString message = messageCenter.getMessage ( di->id );
		if ( di->replyType == Ack )
		{
			/* check if it's the ack of the deattach message */
			if ( message.indexOf ( "<" + QString ( TAG_EXIT ) + ">" ) < 0 )
				messageCenter.deleteMessage ( di->id ); //Ack of a message with wasn't a exit
			else closeConnection();
		}
		else if ( di->replyType == NAck )
		{
			/* resent the message (3 attempts, if not we close the connection) */

			if ( message.indexOf ( "<" + QString ( TAG_EXIT ) + ">" ) < 0 )
				if ( disconnectAttempts > MAX_DISCONNECT_ATTEMPTS )
					closeConnection(); /* There is a limit for everything */
				else
				{
					send ( message );
					disconnectAttempts++;
				}
			else cerr << "Error of the message with id " << qPrintable ( di->id ) << ":" << qPrintable ( message ) << endl;
		}
		else if ( di->replyType == Result or di->replyType == Pendent or
		          di->replyType == StatusServer or di->replyType == Error )
			processResult ( di );
	}
}

/**
 * Closes the connection
 */
void NMGConnection::closeConnection()
{
	setConnectionStatus ( Disconnected );
	sock->flush();
	sock->close();
	emit connectionClosed();
}

/**
 * Processes an incoming message of Result or Pendet
 * @param di
 */
void NMGConnection::processResult ( NMGIncomingDataInfo * di )
{
	QString data = di->data;

	int iniPos = data.indexOf ( "<" + QString ( TAG_DATA ) + ">" ), lastPos = data.lastIndexOf ( "</" + QString ( TAG_DATA ) + ">" );
	if ( iniPos < 0 or lastPos < 0 ) data = "</" + QString ( TAG_DATA ) + ">";
	else data = data.left ( lastPos + QString ( "</" + QString ( TAG_DATA ) + ">" ).length() ).mid ( iniPos - 1 );

	if ( di->replyType == Error ) emit errorReceived ( di->id, di->errorType, di->errorCode );
	else if ( di->replyType == StatusServer ) emit statusReceived ( di->id, data );
	else emit finishedTask ( di->id, di->module, data );
}

/**
 * Sends the message to the server
 * @param message Message to send to the server
 */
bool NMGConnection::send ( QString message )
{
	long nb;
	if ( ( nb = sock->write ( message.toAscii().constData(), message.length() ) ) < 0 )
	{
		cerr << "ERROR: It's not possible to send the message." << endl;
		return false;
	}
	sock->flush();
	return true;
}

/**
 * Sends a command message to the server
 * @param commandType Command type of the message
 * @param moduleName Name of the module to receive the command message on the server
 * @param data Data of the message
 */
bool NMGConnection::sendCommand ( NMGCommandType commandType, QString moduleName, QString testId, QString data )
{
	if ( status == Disconnected ) return false;
	QString id = newValidMessageIdentifier();
	QString message = interpret.command ( commandType, moduleName, id, testId, data );

	long nb;
	if ( ( nb = sock->write ( message.toUtf8().constData(), message.length() ) ) < 0 )
	{
		cerr << "ERROR: It's not possible to send the command message." << endl;
		return false;
	}
	else
	{
		messageCenter.addMessage ( id, message );
		if ( commandType == Start ) emit newTask ( testId, moduleName );
		sock->flush();
		return true;
	}
}

/**
 * Returns a valid message identifier which isn't being used in any of the acknowleged messages
 * @return The new identifier
 */
QString NMGConnection::newValidMessageIdentifier()
{
	QString id;
	do id.setNum ( randomGen.random() ); while ( messageCenter.existsMessageWithId ( id ) );
	return id;
}

/**
 * Slot executed when there is an error associated to the socket
 * @param e
 */
void NMGConnection::socketError ( QAbstractSocket::SocketError )
{
	establishingConnectionWidget.hide();
	setConnectionStatus ( Disconnected );
	QMessageBox::warning ( 0, "Connection error",
	                       "Unable to connect to NetMeter.\n"
	                       "Please check the NetMeter server IP or port." );
	NMGPreferencesManager::self()->showConnectionPreferencesWidget();
}

/**
 * Slot executed when the socket gets the connection
 */
void NMGConnection::socketConnected()
{
        setConnectionStatus ( Connected );

	long nb;
	QString data = interpret.attach();

	if ( ( nb = sock->write ( data.toAscii().constData(), data.length() ) ) < 0 )
	{
		cerr << "ERROR: It's not possible to send the attach message to the server." << endl;
		setConnectionStatus ( Disconnected );
		return;
	}
#ifdef DEBUG
	cout << "Bytes sent: " << nb << "\n(\n" << qPrintable ( data ) << ")" << endl;
#endif
	setConnectionStatus ( AttachSent );
	disconnectAttempts = 0;
}

/**
 * Slot executed when the connection is closed by the server
 */
void NMGConnection::connectionClosedByServer()
{
	emit connectionClosed();
}

/**
 * Sets the connection status
 * @param newStatus
 */
void NMGConnection::setConnectionStatus ( NMGStatus newStatus )
{
	status = newStatus;
	if ( status == Disconnected )
		MessageBar::shortMessage ( "Disconnected" );
	else
		MessageBar::shortMessage ( "Connected" );
	emit connectionStatusChanged ( status );
}

#include "nmgconnection.moc"
