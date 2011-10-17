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
#include "nmgnetperfxmlinterpret.h"
#include "nmgnetperfwidget.h"
#include "nmgpreferencesmanager.h"
#include <nmghostinformationcombowidget.h>
#include <nmgmetadataparser.h>

#include <QFile>
#include <QMessageBox>

#include <iostream>
using namespace std;

#define TYPE_TPUT	"throughput"

/**
 * Parses an xml reply form a start
 * @param xmlMessage XML message with tags data as root
 * @return Path of the decode file, or "" if there have been an error while parsing
 */
QString NMGNetperfXMLInterpret::getResultFilePathFromResult ( const QString & xmlMessage )
{
	QDomDocument doc;
	if ( !doc.setContent ( xmlMessage ) )
	{
		cerr << "Error: the document is not well formed." << endl;
		return "";
	}
	QDomElement root = doc.documentElement();
	QString resultPath = "";

	if ( root.tagName() != TAG_DATA )
		cerr << "Error root element at NMGNetperfXMLInterpret different from data (" << qPrintable ( root.tagName() ) << ")." << endl;
	else
	{
		for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
		{
			if ( node.isElement() )
			{
				QDomElement elem = node.toElement();
				if ( elem.tagName() == TAG_DECODE_FILE )
				{
					QDomText id = elem.firstChild().toText();
					if ( id.isNull() )
						cerr << "Result file path node has no value!" << endl;
				}
				else if ( elem.tagName() == TAG_TEST_FILE )
				{
					QDomText id = elem.firstChild().toText();
					if ( !id.isNull() ) resultPath = id.nodeValue();
					else cerr << "Result file path node has no value!" << endl;
				}
				else cerr << "Incorrect Tag in Netperf XML reply (" << qPrintable ( elem.tagName() ) << ")." << endl;
			}
		}
	}
	return resultPath;
}

/**
 * Parses the status message and returns the integer value of the status
 * @param xmlMessage
 * @return Status value or -1 if error
 */
int NMGNetperfXMLInterpret::parseStatusMessage ( const QString & xmlMessage )
{
	QDomDocument doc;
	if ( !doc.setContent ( xmlMessage ) )
	{
		cerr << "Error: the document is not well formed." << endl;
		return -1;
	}
	QDomElement root = doc.documentElement();

	if ( root.tagName() != TAG_DATA )
		cerr << "Error root element at NMGNetperfXMLInterpret different from data (" << qPrintable ( root.tagName() ) << ")." << endl;
	else
	{
		for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
		{
			if ( node.isElement() )
			{
				QDomElement elem = node.toElement();
				if ( elem.tagName() == TAG_STATUS )
				{
					QDomText id = elem.firstChild().toText();
					if ( !id.isNull() )
					{
						bool ok;
						int num = id.nodeValue().toInt ( &ok );
						if ( ok ) return num;
					}
					else cerr << "The status node has no value!" << endl;
				}
#ifdef DEBUG
				else cerr << "Tag not managed in Netperf Status XML (" << elem.tagName() << ")." << endl;
#endif
			}
		}
	}
	return -1;
}

void NMGNetperfXMLInterpret::generateXMLTestDocument ( const QString & fileName, bool putUserAndPass )
{
	QFile file ( fileName );
	if ( file.open ( QFile::WriteOnly | QFile::Text ) )
		file.write ( generateXMLTestDocument ( putUserAndPass ).toUtf8() );
	else
		QMessageBox::critical ( widget, "Error while creating the test document",
		                        "Error while creating the export test document at " + fileName );
	file.close();
}

/**
 * Generates the xml document of the test with the information of the widget
 * @param putUserAndPass Indicates if the user and password of the management hosts must be in the XML
 * @return Returns the xml document
 */
QString NMGNetperfXMLInterpret::generateXMLTestDocument ( bool putUserAndPass )
{
	QString result;
	QXmlStreamWriter xmlw ( &result );
	xmlw.setAutoFormatting ( true );

	NMGStation * sourceStation = widget->hostSourceCombo->getStationFromCombo();
	NMGStation * destinationStation = widget->hostDestinationCombo->getStationFromCombo();

	xmlw.writeStartElement ( TAG_TEST );

	xmlw.writeStartElement ( TAG_METADATA );

	xmlw.writeTextElement ( TAG_NAME, widget->txtTestName->text() );
	xmlw.writeTextElement ( TAG_IDENTIFIER, widget->txtTestId->text() );

	/* Result Directory Path */
	xmlw.writeTextElement ( TAG_RESULT_PATH, NMGPreferencesManager::self()->getResultDirectory() );

	/* Source station parameters */
	xmlw.writeStartElement ( TAG_SOURCE );

	xmlw.writeStartElement ( TAG_ADDRESS );
	QString sourceAddresStr;
	if ( sourceStation )
		sourceAddresStr = sourceStation->getTestHost().resolveAddress();
	if ( sourceAddresStr.isEmpty() )
		sourceAddresStr = sourceStation->getTestHost().getAddress();
	xmlw.writeCharacters ( sourceAddresStr );
	xmlw.writeEndElement(); /*</address>*/

	xmlw.writeTextElement ( TAG_NETPERF_PATH,
	                        sourceStation->getStationFieldsManager().getField (
	                            widget->getStationManager()->getModuleStationKey(),
	                            ID_NETPERF_PATH ) );

	xmlw.writeStartElement ( TAG_MANAGEMENT );
	xmlw.writeTextElement ( TAG_ADDRESS, sourceStation->getManagementHost().getAddress() );

	xmlw.writeTextElement ( TAG_PORT, QString::number ( sourceStation->getManagementAddressPort() ) );

	if ( putUserAndPass )
	{
		xmlw.writeTextElement ( TAG_USERNAME, sourceStation->getManagementUsername() );
		xmlw.writeTextElement ( TAG_PASSWORD, sourceStation->getManagementPassword() );
	}
	xmlw.writeEndElement(); /*</management>*/
	xmlw.writeEndElement(); /*</source>*/

	/* Destination station parameters */
	xmlw.writeStartElement ( TAG_DESTINATION );

	xmlw.writeStartElement ( TAG_ADDRESS );
	QString destAddressStr;
	if ( destinationStation )
		destAddressStr = destinationStation->getTestHost().resolveAddress();
	if ( destAddressStr.isEmpty() )
		destAddressStr = destinationStation->getTestHost().getAddress();
	xmlw.writeCharacters ( destAddressStr );
	xmlw.writeEndElement(); /*</address>*/

	xmlw.writeTextElement ( TAG_NETSERVER_PATH,
	                        destinationStation->getStationFieldsManager().getField (
	                            widget->getStationManager()->getModuleStationKey(),
	                            ID_NETSERVER_PATH ) );

	xmlw.writeStartElement ( TAG_MANAGEMENT );
	xmlw.writeTextElement ( TAG_ADDRESS, destinationStation->getManagementHost().getAddress() );

	xmlw.writeTextElement ( TAG_PORT, QString::number ( destinationStation->getManagementAddressPort() ) );

	if ( putUserAndPass )
	{
		xmlw.writeTextElement ( TAG_USERNAME, destinationStation->getManagementUsername() );
		xmlw.writeTextElement ( TAG_PASSWORD, destinationStation->getManagementPassword() );
	}
	xmlw.writeEndElement(); /*</management>*/
	xmlw.writeEndElement(); /*</source>*/

	createXMLConfNetperf ( xmlw );
	createXMLConfNetserver ( xmlw );

	xmlw.writeEndElement(); /*</metadata>*/
// 	xmlw.writeEmptyElement ( TAG_RESULT );
	xmlw.writeEndElement(); /*</tesdt>*/

	return result;
}

/**
 * Creates the conf netperf part of the xml
 * @param doc
 * @param root
 */
void NMGNetperfXMLInterpret::createXMLConfNetperf ( QXmlStreamWriter & xmlw )
{
	xmlw.writeStartElement ( TAG_CONF_NETPERF );

	/*	TIP: never used but btw is kept

		if(widget->checkStartTime->isChecked())
		{

		QString startTime = widget->timeStartTime->time().toString("hh:mm:ss");

		QString gmt = "";
		if(widget->comboGMTOrLocalTime->currentText()=="GMT") gmt = " GMT";

		QDomElement start = doc.createElement(TAG_START);
		confNetperf.appendChild(start);
		QDomText startText = doc.createTextNode(startTime+gmt);
		start.appendChild(startText);
	}
	*/

	NMGStation * destinationStation = widget->hostDestinationCombo->getStationFromCombo();
	xmlw.writeStartElement ( TAG_DESTINATION );
	QString destAddressStr;
	if ( destinationStation )
		destAddressStr = destinationStation->getTestHost().resolveAddress();
	if ( destAddressStr.isEmpty() )
		destAddressStr = destinationStation->getTestHost().getAddress();
	xmlw.writeCharacters ( destAddressStr );
	xmlw.writeEndElement(); /*</destination>*/

	//<port>...</port>
	xmlw.writeTextElement ( TAG_PORT, QString::number ( widget->txtSourcePort->value() ) );

	//<duration>...</duration>
	xmlw.writeStartElement ( TAG_DURATION );
	xmlw.writeAttribute ( ATTR_TYPE, widget->radSeconds->isChecked() ? "seconds" : "bytes" );
	xmlw.writeCharacters ( QString::number ( widget->txtTestDuration->value() ) );
	xmlw.writeEndElement();  /*</duration>*/

	//<proto>...</proto>
	xmlw.writeStartElement ( TAG_PROTO );
	xmlw.writeAttribute ( ATTR_NODELAY, widget->chkNoDelay->isChecked() ? "yes" : "no" );
	xmlw.writeCharacters ( widget->radTcp->isChecked() ? "tcp" : "udp" );
	xmlw.writeEndElement();  /*</proto>*/

	//<sms>..</sms>
	if ( widget->chkSms->isChecked() )
		xmlw.writeTextElement ( TAG_SMS, widget->txtSms->text() );

	//<sss>...</sss>
	if ( widget->chkSss->isChecked() )
		xmlw.writeTextElement ( TAG_SSS, widget->txtSss->text() );

	//<rms>..</rms>
	if ( widget->chkRms->isChecked() )
		xmlw.writeTextElement ( TAG_RMS, widget->txtRms->text() );

	//<rss>...</rss>
	if ( widget->chkRss->isChecked() )
		xmlw.writeTextElement ( TAG_RSS, widget->txtRss->text() );

	xmlw.writeEndElement(); /*</conf_netperf>*/
}

/**
 * Creates the conf netserver part of the xml
 * @param doc
 * @param root
 */
void NMGNetperfXMLInterpret::createXMLConfNetserver ( QXmlStreamWriter & xmlw )
{
	xmlw.writeStartElement ( TAG_CONF_NETSERVER );

	//<port>...</port>
	xmlw.writeTextElement ( TAG_PORT, QString::number ( widget->txtSourcePort->value() ) );

	//<ipv>...</ipv>
	xmlw.writeTextElement ( TAG_IPVERSION, widget->isProtocolIPv4() ? "4" : "6" );

	xmlw.writeEndElement(); /*</conf_netserver>*/
}


/**
 * Updates the widget from a xml document (of a test)
 * @param root
 */
void NMGNetperfXMLInterpret::updateWidgetFromXMLDocument ( const QString & fileName )
{
	widget->txtTestName->setText ( "" );
	widget->txtTestId->setText ( "" );

	QString testType = "";
	NMGMetaData * metadata = new NMGMetaData();

	QFile file ( fileName );
	if ( !file.open ( QFile::ReadOnly | QFile::Text ) )
		cerr << "File " << qPrintable( fileName ) << " couldn't be read";
	else
	{
		NMGMetaDataParser metaParser ( *metadata );
		if ( !metaParser.read ( &file ) )
			cerr << "Parse error reading metadata in file " << qPrintable ( file.fileName() ) << ":\n" << qPrintable( metaParser.errorString() );
		else
		{
			testType = metadata->getTypeOfTest().toLower();

			//Backward compatibility with older test files that doesn't have <type> tag
			if ( testType.isEmpty() )
			{
				if ( metadata->isSetUncommonElement ( TAG_CONF_NETPERF ) ) metadata->setTypeOfTest ( TYPE_TPUT );
				testType = metadata->getTypeOfTest().toLower();
			}
		}
	}

	if ( testType == TYPE_TPUT )
	{
		widget->txtTestName->setText ( metadata->getTestName() );
		widget->txtTestId->setText ( metadata->getTestIdentifier() );
		//TODO result Path?
		//updateWidgetFromXMLNodeSource ( elem );
		NMGHostInformationComboWidget * combo = widget->hostSourceCombo;
		combo->setAddressOfStation ( metadata->getSourceAddress() );
	
		//updateWidgetFromXMLNodeDestination ( elem );
		combo = widget->hostDestinationCombo;
		combo->setAddressOfStation ( metadata->getDestinationAddress() );
	
		updateWidgetFromXMLNodeConfNetperf ( metadata->getUncommonElement ( TAG_CONF_NETPERF ) );
		delete metadata;
	}
}

/**
 * Updates the widget from the conf netperf node
 * @param elem
 */
void NMGNetperfXMLInterpret::updateWidgetFromXMLNodeConfNetperf ( const QString & elem )
{
// 	widget->chkStartTime->setCheckded ( false );
	widget->txtSourcePort->setValue ( 12865 );

	widget->chkSms->setChecked ( false );
	widget->txtSms->setText ( "" );
	widget->chkSss->setChecked ( false );
	widget->txtSss->setText ( "" );
	widget->chkRms->setChecked ( false );
	widget->txtRms->setText ( "" );
	widget->chkRss->setChecked ( false );
	widget->txtRss->setText ( "" );
	
	QXmlStreamReader xml ( elem );

	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() )
		{
			if ( xml.name() == TAG_PORT )
				widget->txtSourcePort->setValue ( xml.readElementText().toInt() );
			/*else if ( xml.name() == TAG_START )
			{
				widget->chkStartTime->setChecked ( true );
				QString startTime = xml.readElementText();
				if ( startTime.right ( 3 ) == "GMT" )
					widget->comboGMTOrLocalTime->setItemText ( 1, "GMT" );
				else widget->comboGMTOrLocalTime->setItemText ( 0, "Local" );
				QString hour = startTime.left ( 2 );
				QString minutes = startTime.mid ( 3, 2 );
				QString seconds = startTime.mid ( 6, 2 );
				widget->timeStartTime->setTime ( QTime ( hour.toInt(), minutes.toInt(), seconds.toInt() ) );
			}*/
			else if ( xml.name() == TAG_DURATION )
			{
				bool secs = ( xml.attributes().value ( ATTR_TYPE ).toString() == "seconds" );
				widget->txtTestDuration->setValue ( xml.readElementText().toInt() );
				widget->radSeconds->setChecked ( secs );
				widget->radBytes->setChecked ( !secs );
			}
			else if ( xml.name() == TAG_PROTO )
			{
				bool nodly = ( xml.attributes().value ( ATTR_NODELAY ).toString() == "yes" );
				widget->chkNoDelay->setChecked ( nodly );
				
				bool tcp = (xml.readElementText() == "tcp");
				widget->radTcp->setChecked ( tcp );
				widget->radUdp->setChecked ( !tcp );
			}
			else if ( xml.name() == TAG_SMS )
			{
				widget->chkSms->setChecked ( true );
				widget->txtSms->setText ( xml.readElementText() );
			}
			else if ( xml.name() == TAG_SSS )
			{
				widget->chkSss->setChecked ( true );
				widget->txtSss->setText ( xml.readElementText() );
			}
			else if ( xml.name() == TAG_RMS )
			{
				widget->chkRms->setChecked ( true );
				widget->txtRms->setText ( xml.readElementText() );
			}
			else if ( xml.name() == TAG_RSS )
			{
				widget->chkRss->setChecked ( true );
				widget->txtRss->setText ( xml.readElementText() );
			}
		}
	}
	if ( xml.hasError() )
		cerr << "Error in MGEN configuration:\n" << qPrintable ( xml.errorString() );
}
