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
#ifndef NMGCONNECTION_H
#define NMGCONNECTION_H

#include "nmginterpret.h"
#include "nmgrandomgenerator.h"
#include "nmgmessagecenter.h"
#include "nmgpreferencesmanager.h"
#include <nmgestablishingconnectionwidget.h>

#include <QObject>
#include <QTcpSocket>

#define DEFAULT_PORT_SERVER 6122
#define DEFAULT_ADDR_SERVER "127.0.0.1"

#define APP_VERSION "0.1"
#define APP_NAME "netmetergui"

#define BUFFER_SIZE 1024

#define MAX_DISCONNECT_ATTEMPTS 3

#define MAX_ERROR_END 10


enum NMGStatus {Disconnected,Connected,AttachSent,ReadyToRead,WaitingDeattachAck,WaitingExitAck};

class NMGConnection : public QObject
{
		Q_OBJECT
	private:
		NMGConnection ( QObject *parent = 0, const char *name = 0 );

	public:
		~NMGConnection();
		static NMGConnection * getConnection();
		NMGStatus getConnectionStatus();

	signals:
		void newTask ( QString id, QString moduleName );
		void finishedTask ( QString id, QString moduleName, QString data );
		void errorReceived ( QString id, int errorType, QString errorMessage );
		void statusReceived ( QString id, QString data );
		void connectionClosed();
		void connectionStatusChanged ( NMGStatus status );

	public slots:
		void start();
		void end();
		void exitServer();
		void deattachServer();
		void readyToRead();
		bool send ( QString message );
		bool sendCommand ( NMGCommandType commandType, QString moduleName, QString testId, QString data );
		void socketError ( QAbstractSocket::SocketError );
		void socketConnected();
		void connectionClosedByServer();

	private:
		QString newValidMessageIdentifier();
		void processResult ( NMGIncomingDataInfo * di );
		void handleDeattachReply ( QString message );
		void handleExitReply ( QString message );
		void closeConnection();
		void setConnectionStatus ( NMGStatus newStatus );
		bool messageRead ( QString & message );

	private:
		QTcpSocket * sock;
		NMGStatus status;
		int disconnectAttempts;
		int errorDisconnect;
		NMGRandomGenerator randomGen;
		NMGMessageCenter messageCenter;
		static NMGConnection * instance;
		char * readBuffer;
		int bytesToRead;
		int bytesRead;

	public:
		NMGInterpret interpret;
		NMGEstablishingConnectionWidget establishingConnectionWidget;

};

#endif
