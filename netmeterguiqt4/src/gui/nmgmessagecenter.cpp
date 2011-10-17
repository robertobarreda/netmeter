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
#include "nmgmessagecenter.h"
#include "nmgconnection.h"
#include "nmgresulthandler.h"

#include <QDomDocument>
#include <QDomElement>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @param parent QT's parent parameter
 * @param name QT's name parameter
 * @return
 */
NMGMessageCenter::NMGMessageCenter ( QObject *parent, const char *name )
	: QObject ( parent )
{
	setObjectName ( name );
	connect ( &timer, SIGNAL ( timeout() ), this, SLOT ( checkMessages() ) );
	//timer.start(5000,true); //!!SINGLE SHOT
	//timer.start(5*1000); //TODO: Made it more customizable
}

/**
 * Destructor of the class
 * @return
 */
NMGMessageCenter::~NMGMessageCenter()
{}

/**
 * Adds a message to the message center
 * @param id Identifier of the message
 * @param message Message
 */
void NMGMessageCenter::addMessage ( QString id, QString message )
{
	messageMap.insert ( id, message );
	cout << "NMGMessageCenter::addMessage: " << qPrintable ( id ) << endl << qPrintable ( message ) << endl;
	/*	if(existsMessageWithId(id)) cout << "IT EXISTS!!" << endl;
		else cout<< "IT DOESN'T EXIST!" << endl;*/
}

/**
 * Deletes the message because it has been confirmed
 * @param id
 */
void NMGMessageCenter::deleteMessage ( QString id )
{
#ifdef DEBUG
	cout << "Message with id " << qPrintable ( id ) << " acknowleged." << endl;
#endif
	//messageMap.remove(id);
	//TODO: this must be deleted once the result with the pluginName has come
}

/**
 * Returns a message if exists with identifier id
 * @param id
 * @return
 */
QString NMGMessageCenter::getMessage ( QString id )
{
	QMessageMapIterator it = messageMap.find ( id );
	if ( it == messageMap.end() ) return "";
	else return it.value();
}

/**
 * Checks if exists a message with identifier id
 * @param id Identifier of the message
 * @return true if there is one message with identifier id
 */
bool NMGMessageCenter::existsMessageWithId ( QString id )
{
	QMessageMapIterator it = messageMap.find ( id );
	if ( it == messageMap.end() ) return false;
	else return true;
}

QString NMGMessageCenter::getModuleName ( QString id )
{
	QMessageMapIterator it = messageMap.find ( id );
	if ( it == messageMap.end() ) return "";
	else
	{
		QString data = it.value();
		int begin = data.indexOf ( "<plugin>" ), end = data.indexOf ( "</plugin" );
		data.remove ( end, data.length() );
		data.remove ( 0, begin + 8 );
		return data;
	}
}

/**
 *
 */
void NMGMessageCenter::checkMessages()
{
	cout << "Checking message ..." << endl;

	//NMGConnection::getConnection()->interpret->parseResult(

	QDomDocument doc;
	QDomElement elem = doc.createElement ( "result" );
	doc.appendChild ( elem );
	QDomElement id = doc.createElement ( "id" );
	elem.appendChild ( id );
	QDomText text = doc.createTextNode ( "123456" );
	id.appendChild ( text );

	QDomElement data = doc.createElement ( "data" );
	elem.appendChild ( data );
	QDomElement indata = doc.createElement ( "infoPlugin" );
	data.appendChild ( indata );
	QDomText text2 = doc.createTextNode ( "bla bla bla bla bla" );
	indata.appendChild ( text2 );

	//cout << qPrintable ( doc.toString() ) << endl;
	/*QDomDocument doc;
	QString errorMessage;
	if(!doc.setContent(io,&errorMessage))
	{
		cerr << "The incoming data form the server it's not an XML file! (" <<  qPrintable ( errorMessage ) << ")." << endl;
		return 0;
	}*/
#ifdef DEBUG
	cout << "Document received: " << qPrintable ( doc.toString() ) << endl;
#endif
	NMGIncomingDataInfo * di = new NMGIncomingDataInfo();
	di->id = di->status = di->data = di->errorCode = "";
	QDomElement root = doc.documentElement();
	QString tag = root.tagName();
	NMGConnection::getConnection()->interpret.parseResult ( root,di );

	cout << "ID: " << qPrintable ( di->id ) << endl;
	cout << "DATA: " << qPrintable ( di->data ) << endl;

//	NMGTaskManager::self()->endTask(di->id);
//	NMGResultHandler::self()->showResult("NMTrace",di->data);
	delete di;
}

#include "nmgmessagecenter.moc"
