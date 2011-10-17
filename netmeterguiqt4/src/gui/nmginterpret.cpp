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
#include "nmginterpret.h"

#include <QString>
#include <QDomNode>
#include <QDomDocument>
#include <QDomElement>

/**
 * Constructor of the class NMGInterpret
 */
NMGInterpret::NMGInterpret()
{}

/**
 * Destructor of the class NMGInterpret
 */
NMGInterpret::~NMGInterpret()
{}

/**
 * Create the XML attach message for the server
 * @return QString with the XML message of attach
 */
QString NMGInterpret::attach()
{
	QDomDocument attach;
	QDomElement root = attach.createElement ( TAG_ATTACH );
	QDomElement client = attach.createElement ( TAG_CLIENT );
	QDomText name = attach.createTextNode ( NMGApplicationInformation::getApplicationInformation()->getAppName() );
	client.appendChild ( name );
	QDomElement version = attach.createElement ( TAG_VERSION );
	QDomText vers = attach.createTextNode ( NMGApplicationInformation::getApplicationInformation()->getAppVersion() );
	version.appendChild ( vers );
	root.appendChild ( client );
	root.appendChild ( version );
	attach.appendChild ( root );
	return attach.toString();
}

/**
 * Parsese the XML attach reply of the server
 * @param io QIODevice where is the XML message
 * @return Map with all the information of the plugin (0 if error)
 */
QMapPluginInformation * NMGInterpret::parseAttachReply ( QString message )
{
	QDomDocument reply;
	QString errorMessage, nameServer, versionServer;

	QMapPluginInformation * map = new QMapPluginInformation;
	QDomElement element = reply.documentElement();
	QDomNode node = element.firstChild();

	if ( !reply.setContent ( message, &errorMessage ) )
	{
		cerr << "The attach's reply of the server is not correct (" << qPrintable ( errorMessage ) << ")" << endl;
		return 0;
	}

	while ( !node.isNull() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_SERVER ) nameServer = node.firstChild().toText().nodeValue();
			else if ( name == TAG_VERSION )
				versionServer = node.firstChild().toText().nodeValue();
			else if ( name == TAG_PLUGINS )
			{
				map = parseAtachReplyPlugins ( node );
			}
			else cerr << "Incorrect XML tag: " << qPrintable ( name ) << endl;
		}
		node = node.nextSibling();
	}

#ifdef DEBUG
	cout << "Connected with " << qPrintable ( nameServer ) << "(version " << qPrintable ( versionServer ) << ")." << endl;
#endif
	NMGApplicationInformation::getApplicationInformation()->setServerName ( nameServer );
	NMGApplicationInformation::getApplicationInformation()->setServerVersion ( versionServer );
	NMGApplicationInformation::getApplicationInformation()->setPluginInformation ( map );

	QMapPluginInformation::iterator it;
	for ( it = map->begin(); it != map->end(); ++it )
	{
		cout << qPrintable ( it.value().name ) << " - " << qPrintable ( it.value().capabilities );
		cout << " - " << qPrintable ( it.value().version ) << endl;
	}
	return map;
}

/**
 * Returns a Map with the information of the plugins of th server
 * @param parent Node which contains the tag "plugins"
 * @return QMap with all the plugins information of the server
 */
QMapPluginInformation * NMGInterpret::parseAtachReplyPlugins ( QDomNode &parent )
{
	QMapPluginInformation * map;
	map = new QMapPluginInformation();
	QDomNode node = parent.firstChild();
	while ( !node.isNull() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_PLUGIN )
			{
				NMGPluginInformation * pi = parsePluginInformation ( node );
				if ( pi ) map->insert ( pi->name, *pi );
			}
			else
			{
				cerr << "Incorrect XML tag: " << qPrintable ( name ) << endl;
				delete map;
				return 0;
			}
		}
		node = node.nextSibling();
	}

	return map;
}

/**
 * Return the struct PluginInformation related to he node
 * @param parent Node which contains the tag "plugin"
 * @return PluginInformation of the parameter node
 */
NMGPluginInformation * NMGInterpret::parsePluginInformation ( QDomNode &parent )
{
	NMGPluginInformation * pi = new NMGPluginInformation();
	QDomNode node = parent.firstChild();
	while ( !node.isNull() )
	{
		QString name = node.nodeName();
		if ( name == TAG_NAME ) pi->name = node.firstChild().toText().nodeValue();
		else if ( name == TAG_CAPABILITIES ) pi->capabilities = node.firstChild().toText().nodeValue();
		else if ( name == TAG_VERSION ) pi->version = node.firstChild().toText().nodeValue();
		else
		{
			cerr << "Incorrect XML tag: " << qPrintable ( name ) << endl;
			delete pi;
			return 0;
		}
		node = node.nextSibling();
	}
	return pi;
}

/**
 * Returns the XML message of quit to the server
 * @return QString with the XML message of quit
 */
QString NMGInterpret::quit ( QString id )
{
	QDomDocument quit;
	QDomElement root = quit.createElement ( TAG_EXIT );
	quit.appendChild ( root );

	QDomElement idElem = quit.createElement ( TAG_ID );
	QDomText idText = quit.createTextNode ( id );
	idElem.appendChild ( idText );
	root.appendChild ( idElem );

	return  quit.toString();
}

/**
 * Returns the XML message of deattach to the server
 * @return QString with the XML message of quit
 */
QString NMGInterpret::deattach ( QString id )
{
	QDomDocument deattach;
	QDomElement root = deattach.createElement ( TAG_DEATTACH );
	deattach.appendChild ( root );

	QDomElement idElem = deattach.createElement ( TAG_ID );
	QDomText idText = deattach.createTextNode ( id );
	idElem.appendChild ( idText );
	root.appendChild ( idElem );

	return  deattach.toString();
}

/**
 * Creates a XML command message
 * @param commandType CommandType wanted to be executed
 * @param pluginName Name of the plugin to receive the command on the server
 * @param id Identifier of the message
 * @param data Data which will directly passed to the plugin on the server
 * @return String with the XML command message
 */
QString NMGInterpret::command ( NMGCommandType commandType, QString pluginName, QString id, QString testId, QString data )
{
	QDomDocument command;
	QDomElement root = command.createElement ( getStringCommandType ( commandType ) );
	if ( !id.isEmpty() )
	{
		QDomElement idElem = command.createElement ( TAG_ID );
		QDomText idText = command.createTextNode ( id );
		idElem.appendChild ( idText );
		root.appendChild ( idElem );
	}

	if ( !testId.isEmpty() )
	{
		QDomElement testIdElem = command.createElement ( TAG_TEST );
		QDomText testIdText = command.createTextNode ( testId );
		testIdElem.appendChild ( testIdText );
		root.appendChild ( testIdElem );
	}

	QDomElement plugin = command.createElement ( TAG_PLUGIN );
	QDomText whichPlugin = command.createTextNode ( pluginName );
	plugin.appendChild ( whichPlugin );
	root.appendChild ( plugin );

	if ( !data.isEmpty() )
	{
		QDomDocument test;
		QString testXML = data; //"<"+QString(TAG_DATA)+">"+data+"</"+TAG_DATA+">";
		if ( test.setContent ( testXML ) )
		{
			/* if it's a XML document */
			QDomElement rootData = test.documentElement();
			if ( rootData.tagName() != TAG_DATA )
			{
				QDomElement dataElem = command.createElement ( TAG_DATA );
				dataElem.appendChild ( rootData );
				root.appendChild ( dataElem );
			}
			else root.appendChild ( rootData );
		}
		else
		{ 	/* if it's plain text
			   trying to create the doc with the tag data in case it doesn'h have a root */
			testXML = "<" + QString ( TAG_DATA ) + ">" + data + "</" + QString ( TAG_DATA ) + ">";
			if ( test.setContent ( testXML ) )
			{
				QDomElement rootData = test.documentElement();
				root.appendChild ( rootData );

			}
			else
			{
				QDomElement dataElem = command.createElement ( TAG_DATA );
				QDomText dataText = command.createTextNode ( data );
				dataElem.appendChild ( dataText );
				root.appendChild ( dataElem );
			}
		}
	}
	command.appendChild ( root );
	return command.toString();
}


/**
 * Parses the incoming data from io and returns a struct NMGIncomingDataInfo with the info readed
 * @param io IODevice to read from
 * @return NMGIncomingDataInfo with info of the data or 0 if error (The result must be deleted after its use)
 */
NMGIncomingDataInfo * NMGInterpret::parseIncomingData ( QString message )
{
	QDomDocument doc;
	QString errorMessage;
	if ( !doc.setContent ( message, &errorMessage ) )
	{
		cerr << "The incoming data form the server it's not an XML file! (" << qPrintable ( errorMessage ) << ")." << endl;
		return 0;
	}
	QString dades = doc.toString();
#ifdef DEBUG
	cout << "Document received: " << qPrintable ( dades ) << endl;
#endif
	NMGIncomingDataInfo * di = new NMGIncomingDataInfo();
	di->id = di->status = di->data = di->errorCode = di->module = "";
	QDomElement root = doc.documentElement();
	QString tag = root.tagName();

	if ( tag == TAG_ACK ) parseAck ( root, di );
	else if ( tag == TAG_RESULT ) parseResult ( root, di );
	else if ( tag == TAG_NACK ) parseNAck ( root, di );
	else if ( tag == TAG_ERROR ) parseError ( root, di );
	else if ( tag == TAG_STATUS ) parseStatus ( root, di );
	else cerr << "Unknown tag " << qPrintable ( tag ) << " in server's reply!" << endl;

	di->data = dades;
	return di;
}

/**
 * Parses the ack XML document and fills the information in di
 * @param root Root of the XML tree with tag ack
 * @param di Struct to fill with the information of the XML document
 */
void NMGInterpret::parseAck ( QDomElement & root, NMGIncomingDataInfo * di )
{
	di->replyType = Ack;
	for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_ID )
			{
				QDomText id = node.firstChild().toText();
				if ( !id.isNull() ) di->id = id.nodeValue();
				else cerr << "There is no id value in the ack!" << endl;
			}
			else if ( name == TAG_DATA )
			{
				QDomText data = node.firstChild().toText();
				if ( !data.isNull() ) di->data = data.nodeValue();
			}
			else cerr << "Unknown tag " << qPrintable ( name ) << " in server's reply!" << endl;
		}
		else if ( node.isText() )
		{
			QDomText text = node.toText();
			if ( !text.isNull() )
			{
				cout << "[warning] Data in the main clause : " << qPrintable ( text.nodeValue() ) << endl;
				di->data = text.nodeValue();
			}
		}
	}
}

/**
 * Parses the ack XML document and fills the information in di
 * @param root Root of the XML tree with tag ack
 * @param di Struct to fill with the information of the XML document
 */
void NMGInterpret::parseNAck ( QDomElement & root, NMGIncomingDataInfo * di )
{
	di->replyType = NAck;
	for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_ID )
			{
				QDomText id = node.firstChild().toText();
				if ( !id.isNull() ) di->id = id.nodeValue();
				else cerr << "There is no id value in the nack!" << endl;
			}
			else if ( name == TAG_DATA )
			{
				QDomText data = node.firstChild().toText();
				if ( !data.isNull() ) di->data = data.nodeValue();
			}
			else cerr << "Unknown tag " << qPrintable ( name ) << " in server's reply!" << endl;
		}
		else if ( node.isText() )
		{
			QDomText text = node.toText();
			if ( !text.isNull() )
			{
				cout << "[warning] Data in the main clause : " << qPrintable ( text.nodeValue() ) << endl;
				di->data = text.nodeValue();
			}
		}
	}
}

/**
 * Parses the status XML document and fills the information in di
 * @param root Root of the XML tree with tag retult
 * @param di Struct to fill with the information of the XML document
 */
void NMGInterpret::parseStatus ( QDomElement & root, NMGIncomingDataInfo * di )
{
	di->replyType = StatusServer;
	for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_ID or name == TAG_TEST )
			{
				QDomText id = node.firstChild().toText();
				if ( !id.isNull() ) di->id = id.nodeValue();
				else cerr << "There is no id value in the ack!" << endl;
			}
			else if ( name == TAG_DATA )
			{
				QDomText data = node.firstChild().toText();
				if ( !data.isNull() ) di->data = data.nodeValue();
				else
				{
					//rbarreda: Calling appendChild() on a null node does nothing.
					//QDomDocument doc;
					//doc.appendChild ( node );
					//di->data = doc.toString();
					di->data = "";
				}
			}
			else cerr << "Unknown tag " << qPrintable ( name ) << " in server's reply!" << endl;
		}
		else if ( node.isText() )
		{
			QDomText text = node.toText();
			if ( !text.isNull() )
			{
				cout << "[warning] Data in the main clause : " << qPrintable ( text.nodeValue() ) << endl;
				di->data = text.nodeValue();
			}
		}
	}
}

/**
 * Parses the result XML document and fills the information in di
 * @param root Root of the XML tree with tag retult
 * @param di Struct to fill with the information of the XML document
 */
void NMGInterpret::parseResult ( QDomElement & root, NMGIncomingDataInfo * di )
{
	di->replyType = Result;
	for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_ID or name == TAG_RESULT )
			{
				QDomText id = node.firstChild().toText();
				if ( !id.isNull() ) di->id = id.nodeValue();
				else cerr << "There is no id value in the ack!" << endl;
			}
			else if ( name == TAG_DATA )
			{
				QDomText data = node.firstChild().toText();
				if ( !data.isNull() ) di->data = data.nodeValue();
				else
				{
					//rbarreda: Calling appendChild() on a null node does nothing.
					//QDomDocument doc;
					//doc.appendChild ( node );
					//di->data = doc.toString();
					di->data = "";
				}
			}
			else cerr << "Unknown tag " << qPrintable ( name ) << " in server's reply!" << endl;
		}
		else if ( node.isText() )
		{
			QDomText text = node.toText();
			if ( !text.isNull() )
			{
				cout << "[warning] Data in the main clause : " << qPrintable ( text.nodeValue() ) << endl;
				di->data = text.nodeValue();
			}
		}
	}
}

/**
 * Parses the error XML document and fills the information in di
 * @param root Root of the XML tree with tag error
 * @param di Struct to fill with the information of the XML document
 */
void NMGInterpret::parseError ( QDomElement & root, NMGIncomingDataInfo * di )
{
	di->replyType = Error;
	for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_ID or name == TAG_TEST )
			{
				QDomText id = node.firstChild().toText();
				if ( !id.isNull() ) di->id = id.nodeValue();
				else cerr << "There is no id/test value in the error!" << endl;
			}
			else if ( name == TAG_TYPE )
			{
				QDomText type = node.firstChild().toText();
				if ( !type.isNull() ) di->errorType = type.nodeValue().toInt();
				else cerr << "There is no type value in the error!" << endl;
			}
			else if ( name == TAG_MESSAGE )
			{
				QDomText type = node.firstChild().toText();
				if ( !type.isNull() ) di->errorCode = type.nodeValue();
				else cerr << "There is no type value in the error!" << endl;
			}
			else cerr << "Unknown tag " << qPrintable ( name ) << " in server's reply!" << endl;
		}
	}
}

/**
 * Returns the string associated to the command type
 * @param command Command to convert to string
 * @return String of the command
 */
QString NMGInterpret::getStringCommandType ( NMGCommandType command )
{
	switch ( command )
	{
		case Configure: return TAG_CONFIGURE;
		case Start: return TAG_START;
		case Stop: return TAG_STOP;
		case Status: return TAG_STATUS;
		default: return TAG_UNKNOWN;
	}
}

/**
 * Returns the NMGCommandType associated to the String command
 * @param command Command to convert to NMGCommandType
 * @return NMGCommandType
 */
NMGCommandType NMGInterpret::getCommandType ( QString command )
{
	if ( command == TAG_CONFIGURE ) return Configure;
	else if ( command == TAG_START ) return Start;
	else if ( command == TAG_STOP ) return Stop;
	else if ( command == TAG_STATUS ) return Status;
	else return Unknown;
}
