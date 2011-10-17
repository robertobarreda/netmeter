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
#include "nmgstation.h"

#include <QDomNode>
#include <QDomDocument>
#include <QDomElement>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @return
 */
NMGStation::NMGStation()
{
	managementPassword = managementUsername = QString();
	managementAddressPort = 22;
	stationFieldsManager = NMGStationFieldsManager();
}

/**
 * Destructor of the class
 * @return
 */
NMGStation::~NMGStation()
{}

/**
 * Returns the management password
 * @return
 */
QString NMGStation::getManagementPassword()
{
        return managementPassword;
}

/**
 * Sets the management password
 * @param pass
 */
void NMGStation::setManagementPassword( QString pass )
{
	managementPassword = pass;
}

/**
 * Returns the management username
 * @return
 */
QString NMGStation::getManagementUsername()
{
	return managementUsername;
}

/**
 * Sets the management username
 * @param username
 */
void NMGStation::setManagementUsername( QString username )
{
	managementUsername = username;
}

/**
 * Returns the management address port
 * @return
 */
int NMGStation::getManagementAddressPort()
{
	return managementAddressPort;
}

/**
 * Sets the management address port
 * @param port
 */
void NMGStation::setManagementAddressPort( int port )
{
	managementAddressPort = port;
}

/**
 * Returns the management host
 * @return
 */
NMGHost & NMGStation::getManagementHost()
{
	return managementHost;
}

/**
 * Returns the test host
 * @return
 */
NMGHost & NMGStation::getTestHost()
{
	return testHost;
}

/**
 * Returns the station fields manager
 * @return
 */
NMGStationFieldsManager & NMGStation::getStationFieldsManager()
{
	return stationFieldsManager;
}

/**
 * Adds a QDomElement for the station to the element stationsElements (it will have the tags station)
 * @param doc XML Document
 * @param stationsElement
 */
void NMGStation::addXMLStationNode( QDomDocument& doc, QDomElement& stationsElement )
{
	QDomElement element = doc.createElement( TAG_STATION );
	stationsElement.appendChild( element );

	/* test host info */
	QDomElement test = doc.createElement( TAG_STATION_TEST );
	element.appendChild( test );

	QDomElement testAddr = doc.createElement( TAG_STATION_ADDRESS );
	test.appendChild( testAddr );
	QDomText testAddrText = doc.createTextNode( getTestHost().getAddress() );
	testAddr.appendChild( testAddrText );

	if ( !getTestHost().getAlias().isEmpty() )
	{
		QDomElement testAlias = doc.createElement( TAG_STATION_ALIAS );
		test.appendChild( testAlias );
		QDomText testAliasText = doc.createTextNode( getTestHost().getAlias() );
		testAlias.appendChild( testAliasText );
	}

	QDomElement testProtocol = doc.createElement( TAG_STATION_PROTOCOL );
	test.appendChild( testProtocol );
	QDomText testProtocolText = doc.createTextNode( getTestHost().isHostIPv4() ? STATION_PROTOCOL_IPV4 : STATION_PROTOCOL_IPV6 );
	testProtocol.appendChild( testProtocolText );

	/* management host info */
	QDomElement management = doc.createElement( TAG_STATION_MANAGEMENT );
	element.appendChild( management );

	QDomElement managementAddr = doc.createElement( TAG_STATION_ADDRESS );
	management.appendChild( managementAddr );
	QDomText managementAddrText = doc.createTextNode( getManagementHost().getAddress() );
	managementAddr.appendChild( managementAddrText );

	QDomElement managementPort = doc.createElement( TAG_STATION_PORT );
	management.appendChild( managementPort );
	QDomText managementPortText = doc.createTextNode( QString::number(getManagementAddressPort()) );
	managementPort.appendChild( managementPortText );


	if ( !getManagementHost().getAlias().isEmpty() )
	{
		QDomElement managementAlias = doc.createElement( TAG_STATION_ALIAS );
		management.appendChild( managementAlias );
		QDomText managementAliasText = doc.createTextNode( getManagementHost().getAlias() );
		managementAlias.appendChild( managementAliasText );
	}

	QDomElement managementProtocol = doc.createElement( TAG_STATION_PROTOCOL );
	management.appendChild( managementProtocol );
	QDomText managementProtocolText = doc.createTextNode( getManagementHost().isHostIPv4() ? STATION_PROTOCOL_IPV4 : STATION_PROTOCOL_IPV6 );
	managementProtocol.appendChild( managementProtocolText );

	QDomElement username = doc.createElement( TAG_STATION_USERNAME );
	management.appendChild( username );
	QDomText usernameText = doc.createTextNode( getManagementUsername() );
	username.appendChild( usernameText );

	////B64 encrypted password!!!
	QDomElement password = doc.createElement( TAG_STATION_PASSWORD );
	management.appendChild( password );
	//QDomText passwordText = doc.createTextNode ( getManagementPassword() );
        QByteArray b64pass (managementPassword.toUtf8());
        QDomText passwordText = doc.createTextNode ( QString(b64pass.toBase64()) );
	password.appendChild( passwordText );

	/* Save fields to XML */
	stationFieldsManager.addFieldNodetoXML( doc, element );
}

/**
 * Creates the station from the QDomElement
 * @param element QDomElement with the tags <station>
 * @return New Station created
 */
NMGStation * NMGStation::makeStationFromXMLElement( QDomElement & element )
{
	if ( element.tagName() != TAG_STATION ) return 0;

	NMGStation * result = new NMGStation();
	for ( QDomNode node = element.firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			if ( node.nodeName() == TAG_STATION_TEST )
			{
				for ( QDomNode stationNode = node.toElement().firstChild(); !stationNode.isNull();
				        stationNode = stationNode.nextSibling() )
				{
					if ( stationNode.isElement() )
					{
						if ( stationNode.nodeName() == TAG_STATION_ADDRESS )
							result->getTestHost().setAddress( stationNode.toElement().text() );
						else if ( stationNode.nodeName() == TAG_STATION_ALIAS )
							result->getTestHost().setAlias( stationNode.toElement().text() );
						else if ( stationNode.nodeName() == TAG_STATION_PROTOCOL )
							result->getTestHost().setHostIPV4(
							    stationNode.toElement().text() == STATION_PROTOCOL_IPV4 ? true : false );
						else cerr << "Error while parsing test host " << qPrintable( stationNode.nodeName() ) << endl;
					}
				}
			}
			else if ( node.nodeName() == TAG_STATION_MANAGEMENT )
			{
				for ( QDomNode stationNode = node.toElement().firstChild(); !stationNode.isNull();
				        stationNode = stationNode.nextSibling() )
				{
					if ( stationNode.isElement() )
					{
						if ( stationNode.nodeName() == TAG_STATION_ADDRESS )
							result->getManagementHost().setAddress( stationNode.toElement().text() );
						else if ( stationNode.nodeName() == TAG_STATION_PORT )
							result->setManagementAddressPort( stationNode.toElement().text().toInt() );
						else if ( stationNode.nodeName() == TAG_STATION_ALIAS )
							result->getManagementHost().setAlias( stationNode.toElement().text() );
						else if ( stationNode.nodeName() == TAG_STATION_PROTOCOL )
							result->getManagementHost().setHostIPV4(
							    stationNode.toElement().text() == STATION_PROTOCOL_IPV4 ? true : false );
						else if ( stationNode.nodeName() == TAG_STATION_USERNAME )
							result->setManagementUsername( stationNode.toElement().text() );
						else if ( stationNode.nodeName() == TAG_STATION_PASSWORD )
							//result->setManagementPassword ( stationNode.toElement().text() );
							result->setManagementPassword (
                                                             QString(QByteArray::fromBase64(stationNode.toElement().text().toUtf8())) );
						else cerr << "Error while parsing test host " << qPrintable( stationNode.nodeName() ) << endl;
					}
				}
			}
			else if ( node.nodeName() == TAG_STATION_FIELDS )
				result->getStationFieldsManager().fromXML( node.toElement() );
			else cerr << "Unknown tag " << qPrintable( node.nodeName() ) << endl;
		}
	}
	return result;
}
