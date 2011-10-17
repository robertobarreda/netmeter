/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
 *   Copyright (C) 2007 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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

#include <qlineedit.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include "nmgmgenxmlinterpret.h"
#include "nmgmgenwidget.h"
#include "nmgmgenxmltags.h"
#include "nmgpreferencesmanager.h"

#include <iostream>
using namespace std;

#include <nmghostinformationcombowidget.h>

/**
 * Constructor of the class
 * @param w 
 * @return 
 */
NMGMGenXMLInterpret::NMGMGenXMLInterpret(NMGMGenWidget * w)
{
	widget = w;
	lastFlowId = 0;
}

/**
 * Destroyer of the class
 * @return 
 */
NMGMGenXMLInterpret::~NMGMGenXMLInterpret()
{

}

/**
 * Parses an xml reply form a start
 * @param xmlMessage XML message with tags data as root
 * @return Path of the decode file, or "" if there have been an error while parsing
 */
QString NMGMGenXMLInterpret::getResultFilePathFromResult(const QString & xmlMessage)
{
	QDomDocument doc;
	if(!doc.setContent(xmlMessage))
	{
		cerr << RED << "[ERROR] the document is not well formed." << ENDCOLOR << endl;
		return "";
	}
	NMGQDomSortElement root = doc.documentElement();
	QString resultPath = "";
	
	if(root.tagName()!=TAG_DATA) 
		cerr << RED << "[ERROR] root element at NMGMGenXMLInterpret different from data ("<< qPrintable(root.tagName()) <<")."<< ENDCOLOR <<endl;
	else
	{
		for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling())
		{
			if(node.isElement())
			{
				NMGQDomSortElement elem = node.toElement();
				if(elem.tagName()==TAG_DECODE_FILE)
				{
					QString text = "";
					QDomText id = elem.firstChild().toText();
					if(!id.isNull()) text = id.nodeValue();
					//if(!id.isNull()) resultPath = id.nodeValue();
					//else cerr << "The decode result path node has no value!" << endl;
				}
				else if(elem.tagName()==TAG_RESULT_FILE)
				{
					QString text = "";
					QDomText id = elem.firstChild().toText();
					if(!id.isNull()) text = id.nodeValue();
					
				}
				else if(elem.tagName()==TAG_TEST_FILE)
				{
					//QString text = "";
					QDomText id = elem.firstChild().toText();
					if(!id.isNull()) resultPath = id.nodeValue();
					else cerr << "The decode result path node has no value!" << endl;
				}
				else if(elem.tagName()==TAG_RESULT_GRAPH_OWDD)
				{
					QString text = "";
					QDomText id = elem.firstChild().toText();
					if(!id.isNull()) text = id.nodeValue();
					
				}
				else if(elem.tagName()==TAG_RESULT_GRAPH_IPDV)
				{
					QString text = "";
					QDomText id = elem.firstChild().toText();
					if(!id.isNull()) text = id.nodeValue();
					
				}
				else if(elem.tagName()==TAG_RESULT_GRAPH_IPDVD)
				{
					QString text = "";
					QDomText id = elem.firstChild().toText();
					if(!id.isNull()) text = id.nodeValue();
					
				}
				else if(elem.tagName()==TAG_TIME_FILE)
				{
					
				}
				else cerr << "Tag not correct in Mgen XML reply (" << qPrintable(elem.tagName()) <<")." << endl;
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
int NMGMGenXMLInterpret::parseStatusMessage(const QString & xmlMessage)
{
	QDomDocument doc;
	if(!doc.setContent(xmlMessage))
	{
		cerr << "Error: the document is not well formed." << endl;
		return -1;
	}
	NMGQDomSortElement root = doc.documentElement();
	
	if(root.tagName()!=TAG_DATA) 
		cerr << "Error root element at NMGMGenXMLInterpret different from data ("<< qPrintable(root.tagName())<<")."<< endl;
	else
	{
		for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling())
		{
			if(node.isElement())
			{
				NMGQDomSortElement elem = node.toElement();
				if(elem.tagName()==TAG_STATUS)
				{
					QDomText id = elem.firstChild().toText();
					if(!id.isNull())
					{
						bool ok;
						int num = id.nodeValue().toInt(&ok);
						if(ok) return num;
					}
					else cerr << "The status node has no value!" << endl;
				}
//				else cerr << "Tag not correct in Mgen Status XML (" << elem.tagName()<<")." << endl;
			}
		}
	}
	return -1;
}

/**
 * Generates the xml document of the sync test
 * @return Returns the xml document
 */
QString NMGMGenXMLInterpret::generateXMLSync()
{
	NMGStation * source = widget->mainWidget->hostSourceCombo->getStationFromCombo();
	NMGStation * dest = widget->mainWidget->hostDestinationCombo->getStationFromCombo();

	if(source->getTestHost().getAddress().isEmpty())	return "";
	if(dest->getTestHost().getAddress().isEmpty())		return "";

	QDomDocument petition("syncxml");
	QDomElement root = petition.createElement("data");
 	petition.appendChild(root);
	QDomElement tag;
	QDomText data;

	// Protocol
	tag = petition.createElement("protocol");
	if (source->getTestHost().isHostIPv4() )
		data = petition.createTextNode("IPv4");
	else
		data = petition.createTextNode("IPv6");
	tag.appendChild(data);
	root.appendChild(tag);

	// Sleeptime 
	tag = petition.createElement("sleeptime");
	data = petition.createTextNode(widget->mainWidget->sleepbox->text());
	tag.appendChild(data);
	root.appendChild(tag);

	// Source
	QDomElement xmlsource = petition.createElement("source");	
	tag = petition.createElement("name");
	data = petition.createTextNode( source->getTestHost().getAddress() );
	tag.appendChild(data);
	xmlsource.appendChild(tag);

	if (source->getTestHost().getAddress() != "127.0.0.1") 
	{
		QDomElement xmlmanage = petition.createElement("management");
	
		// Name
		QDomElement tagmanage = petition.createElement("name");
		QDomText datamanage = petition.createTextNode(source->getManagementHost().getAddress());
		tagmanage.appendChild(datamanage);
		xmlmanage.appendChild(tagmanage); 
	
		//login
		tagmanage = petition.createElement("login");
		datamanage = petition.createTextNode(source->getManagementUsername());
		tagmanage.appendChild(datamanage);
		xmlmanage.appendChild(tagmanage); 

		// Passwd
		tagmanage = petition.createElement("password");
		datamanage = petition.createTextNode(source->getManagementPassword());
		tagmanage.appendChild(datamanage);
		xmlmanage.appendChild(tagmanage); 

		xmlsource.appendChild (xmlmanage);
	}
	root.appendChild(xmlsource);

	return petition.toString();
}

/**
 * Generates the xml document of the test with the information of the widget
 * @param putUserAndPass Indicates if the user and password of the management hosts must be in the XML
 * @return Returns the xml document
 */
QString NMGMGenXMLInterpret::generateXMLTestDocument(bool putUserAndPass)
{
	NMGStation * sourceStation = widget->mainWidget->hostSourceCombo->getStationFromCombo();
	NMGStation * destinationStation = widget->mainWidget->hostDestinationCombo->getStationFromCombo();
	int mgenVersion;

	QDomDocument doc(TAG_TEST);
	NMGQDomSortElement root = doc.createElement(TAG_TEST);
	doc.appendChild(root);

	/* Mgen tags */
	NMGQDomSortElement testMgen = doc.createElement(TAG_METADATA);
	root.appendChild(testMgen);
	NMGQDomSortElement resultMgen = doc.createElement(TAG_RESULT);
	root.appendChild(resultMgen);
	
	/* Mgen test name */
	NMGQDomSortElement testName = doc.createElement(TAG_NAME);
	testMgen.appendChild(testName);
	QDomText testNameText = doc.createTextNode(widget->mainWidget->txtTestName->text());
	testName.appendChild(testNameText);
	
	/* Mgen test identifier */
	NMGQDomSortElement testId = doc.createElement(TAG_IDENTIFIER);
	testMgen.appendChild(testId);
	QDomText testIdText = doc.createTextNode(widget->mainWidget->txtTestIdentifier->text());
	testId.appendChild(testIdText);

	/* Mgen Version */
	NMGQDomSortElement testMgenVersion = doc.createElement(TAG_MGEN_VERSION);
	testMgen.appendChild(testMgenVersion);
	QDomText testMgenVersionText = doc.createTextNode(widget->mainWidget->comboMgenVersion->currentText());
	testMgenVersion.appendChild(testMgenVersionText);
	mgenVersion = (widget->mainWidget->comboMgenVersion->currentText()).toInt();

	/* Mgen Sync (optional) */
	NMGQDomSortElement testMgenSync = doc.createElement(TAG_MGEN_SYNC);
	testMgen.appendChild(testMgenSync);
	QDomText testMgenSyncText;

	if(widget->mainWidget->checkSync->isChecked())
	{
		widget->syncActivated = true;
		testMgenSyncText = doc.createTextNode("1");	// sync data will be collected
	}
	else
	{	
		widget->syncActivated = false;
		testMgenSyncText = doc.createTextNode("0");	// no sync data
	}

	testMgenSync.appendChild(testMgenSyncText);
	mgenVersion = (widget->mainWidget->comboMgenVersion->currentText()).toInt();

	/* Result Directory Path */
	NMGQDomSortElement testResultPath = doc.createElement(TAG_RESULT_PATH);
	testMgen.appendChild(testResultPath);
	QDomText testResultPathText = doc.createTextNode(NMGPreferencesManager::self()->getResultDirectory());
	testResultPath.appendChild(testResultPathText);
	
	/* Source station parameters */
	NMGQDomSortElement sourceNode = doc.createElement(TAG_SOURCE);
	testMgen.appendChild(sourceNode);
	NMGQDomSortElement sourceAddress = doc.createElement(TAG_ADDRESS);
	sourceNode.appendChild(sourceAddress);
	QString sourceAddresStr;
	if (sourceStation)
		sourceAddresStr = sourceStation->getTestHost().resolveAddress();
	if(sourceAddresStr=="") sourceAddresStr = sourceStation->getTestHost().getAddress();
	QDomText sourceAddressText = doc.createTextNode(sourceAddresStr);
		
	sourceAddress.appendChild(sourceAddressText);
	NMGQDomSortElement sourceMgenPath = doc.createElement(TAG_MGEN_PATH);
	sourceNode.appendChild(sourceMgenPath);
	QDomText sourceMgenPathText;
	if (mgenVersion == 4)
		sourceMgenPathText = doc.createTextNode(sourceStation->getStationFieldsManager().		
			getField(widget->getStationManager()->getModuleStationKey(),ID_MGEN4_PATH));
	else 
		sourceMgenPathText = doc.createTextNode(sourceStation->getStationFieldsManager().		
			getField(widget->getStationManager()->getModuleStationKey(),ID_MGEN_PATH));
	sourceMgenPath.appendChild(sourceMgenPathText);
	
	
	NMGQDomSortElement sourceManagement = doc.createElement(TAG_MANAGEMENT);
	sourceNode.appendChild(sourceManagement);
	NMGQDomSortElement sourceManagementAddress = doc.createElement(TAG_ADDRESS);
	sourceManagement.appendChild(sourceManagementAddress);
	QDomText sourceManagementAddressText =  doc.createTextNode(sourceStation->getManagementHost().getAddress());
	
	sourceManagementAddress.appendChild(sourceManagementAddressText);
	NMGQDomSortElement sourceManagementAddressPort = doc.createElement(TAG_PORT);
	sourceManagement.appendChild(sourceManagementAddressPort);
	QDomText sourceManagementAddressPortText =  doc.createTextNode(QString("%1").arg(sourceStation->getManagementAddressPort()));
	sourceManagementAddressPort.appendChild(sourceManagementAddressPortText);
	
	if(putUserAndPass)
	{
		NMGQDomSortElement sourceManagementUsername = doc.createElement(TAG_USERNAME);
		sourceManagement.appendChild(sourceManagementUsername);
		QDomText sourceManagementUsernameText = doc.createTextNode(sourceStation->getManagementUsername());
		sourceManagementUsername.appendChild(sourceManagementUsernameText);
		NMGQDomSortElement sourceManagementPassword = doc.createElement(TAG_PASSWORD);
		sourceManagement.appendChild(sourceManagementPassword);
		QDomText sourceManagementPasswordText = doc.createTextNode(sourceStation->getManagementPassword());
		sourceManagementPassword.appendChild(sourceManagementPasswordText);
	}
	
	
	/* Destination station parameters */
	
	NMGQDomSortElement destinationNode = doc.createElement(TAG_DESTINATION);
	testMgen.appendChild(destinationNode);
	
	NMGQDomSortElement destAddress = doc.createElement(TAG_ADDRESS);
	destinationNode.appendChild(destAddress);
	QString destAddressStr = destinationStation->getTestHost().resolveAddress();
	if(destAddressStr=="") destAddressStr = destinationStation->getTestHost().getAddress();
	QDomText destAddressText = doc.createTextNode(destAddressStr);
	destAddress.appendChild(destAddressText);
	NMGQDomSortElement sourceDrecPath = doc.createElement(TAG_DREC_PATH);
	destinationNode.appendChild(sourceDrecPath);
	QDomText sourceDrecPathText;
	if (mgenVersion == 4)
		sourceDrecPathText = doc.createTextNode(destinationStation->getStationFieldsManager().
			getField(widget->getStationManager()->getModuleStationKey(),ID_MGEN4_PATH));
	else
		sourceDrecPathText = doc.createTextNode(destinationStation->getStationFieldsManager().
			getField(widget->getStationManager()->getModuleStationKey(),ID_DREC_PATH));
	sourceDrecPath.appendChild(sourceDrecPathText);
	NMGQDomSortElement sourceDecodePath = doc.createElement(TAG_DECODE_PATH);
	destinationNode.appendChild(sourceDecodePath);
	QDomText sourceDecodePathText;
	if (mgenVersion == 4)
		sourceDecodePathText = doc.createTextNode(destinationStation->getStationFieldsManager().
			getField(widget->getStationManager()->getModuleStationKey(),ID_MGEN4_PATH));
	else
		sourceDecodePathText = doc.createTextNode(destinationStation->getStationFieldsManager().
			getField(widget->getStationManager()->getModuleStationKey(),ID_DECODE_PATH));
	sourceDecodePath.appendChild(sourceDecodePathText);
	
	NMGQDomSortElement destManagement = doc.createElement(TAG_MANAGEMENT);
	destinationNode.appendChild(destManagement);
	NMGQDomSortElement destManagementAddress = doc.createElement(TAG_ADDRESS);
	destManagement.appendChild(destManagementAddress);
	QDomText destManagementAddressText =  doc.createTextNode(destinationStation->getManagementHost().getAddress());
	destManagementAddress.appendChild(destManagementAddressText);
	NMGQDomSortElement destManagementAddressPort = doc.createElement(TAG_PORT);
	destManagement.appendChild(destManagementAddressPort);
	QDomText destManagementAddressPortText =  doc.createTextNode(QString("%1").arg(destinationStation->getManagementAddressPort()));
	destManagementAddressPort.appendChild(destManagementAddressPortText);
	
	if(putUserAndPass)
	{
		NMGQDomSortElement destManagementUsername = doc.createElement(TAG_USERNAME);
		destManagement.appendChild(destManagementUsername);
		QDomText destManagementUsernameText = doc.createTextNode(destinationStation->getManagementUsername());
		destManagementUsername.appendChild(destManagementUsernameText);
		NMGQDomSortElement destManagementPassword = doc.createElement(TAG_PASSWORD);
		destManagement.appendChild(destManagementPassword);
		QDomText destManagementPasswordText = doc.createTextNode(destinationStation->getManagementPassword());
		destManagementPassword.appendChild(destManagementPasswordText);
	}
/*	
	if(putUserAndPass and widget->mainWidget->txtDistrLocalPath->text()!="")
	{
		NMGQDomSortElement localNode = doc.createElement(TAG_LOCAL);
		testMgen.appendChild(localNode);
		NMGQDomSortElement distrPath = doc.createElement(TAG_DISTR_PATH);
		localNode.appendChild(distrPath);
		QDomText distrPathText = doc.createTextNode(widget->mainWidget->txtDistrLocalPath->text());
		distrPath.appendChild(distrPathText);
	}
*/	
	createXMLConfMGen(doc,testMgen);
	createXMLConfDrec(doc,testMgen);
	
	return doc.toString();
}

/**
 * Creates the conf mgen part of the xml
 * @param doc 
 * @param root 
 */
void NMGMGenXMLInterpret::createXMLConfMGen(QDomDocument & doc, NMGQDomSortElement & root)
{
	NMGQDomSortElement confMgen = doc.createElement(TAG_CONF_MGEN);
	root.appendChild(confMgen);

	if(widget->mainWidget->checkStartTime->isChecked())
	{
		QString startTime = widget->mainWidget->timeStartTime->time().toString("hh:mm:ss");
		QString gmt = "";
		if(widget->mainWidget->comboGMTOrLocalTime->currentText()=="GMT") gmt = " GMT";
		
		NMGQDomSortElement start = doc.createElement(TAG_START);
		confMgen.appendChild(start);
		QDomText startText = doc.createTextNode(startTime+gmt);
		start.appendChild(startText);
	}
	
	NMGQDomSortElement port = doc.createElement(TAG_PORT);
	confMgen.appendChild(port);
	QDomText portText = doc.createTextNode(QString("%1").arg(widget->mainWidget->txtSourcePort->value()));
	port.appendChild(portText);
	
	NMGXMLFlowSortedList sortedList;
	QString testAddress = widget->mainWidget->hostDestinationCombo->getStationFromCombo()->getTestHost().resolveAddress();
	if(testAddress=="") 
	testAddress = widget->mainWidget->hostDestinationCombo->getStationFromCombo()->getTestHost().getAddress();;

	int n = widget->mainWidget->flowList->topLevelItemCount();
        int i = 0;
	
	while(i < n)
        {
                NMGMGenFlow *flow = (NMGMGenFlow *)widget->mainWidget->flowList->topLevelItem(i);
		
		if(widget->mainWidget->comboMgenVersion->currentText() != "4") 
		{
			flow->setBurstInterval(DEFAULT_BURST_INTERVAL,0);
			flow->setBurstDuration(DEFAULT_BURST_DURATION,0);
			flow->setBurst(false);
		}
	
		flow->addFlowToXMLNode(sortedList,doc,confMgen,testAddress,flow->getId());
	i++;
        }

	for(NMGXMLFlowSortedList::iterator it = sortedList.begin(); it != sortedList.end(); ++it)
		confMgen.appendChild(**it);
}

/**
 * Creates the conf drec part of the xml
 * @param doc 
 * @param root 
 */
void NMGMGenXMLInterpret::createXMLConfDrec(QDomDocument & doc, NMGQDomSortElement & root)
{
	NMGQDomSortElement confDrec = doc.createElement(TAG_CONF_DREC);
	root.appendChild(confDrec);
	
	NMGQDomSortElement drecPorts = doc.createElement(TAG_PORTS);
	confDrec.appendChild(drecPorts);
	QString ports = "";

	int n = widget->mainWidget->flowList->topLevelItemCount();
	int i = 0;

	while(i < n)
        {
		NMGMGenFlow *flow = (NMGMGenFlow *)widget->mainWidget->flowList->topLevelItem(i);
		ports += QString("%1").arg(flow->getPort());		
		if(i+1 < n)	ports += ",";
		i++;
        }

	QDomText drecPortsText = doc.createTextNode(ports);
	drecPorts.appendChild(drecPortsText);
	
	NMGXMLFlowSortedList sortedList;
	QString testAddress = widget->mainWidget->hostDestinationCombo->getStationFromCombo()->getTestHost().resolveAddress();
	if(testAddress=="") 
		testAddress = widget->mainWidget->hostDestinationCombo->getStationFromCombo()->getTestHost().getAddress();
	
	i = 0;
	
	while(i < n)
        {
                NMGMGenFlow *flow = (NMGMGenFlow *)widget->mainWidget->flowList->topLevelItem(i);
		flow->addFlowRSVPDestinationToXMLNode(sortedList,doc,confDrec,testAddress);
		i++;
        }

	//sortedList.sort();

	for(NMGXMLFlowSortedList::iterator it = sortedList.begin(); it != sortedList.end(); ++it)
		confDrec.appendChild(**it);
}


/**
 * Updates the widget from a xml document (of a test)
 * @param filename 
 * @return error -> boolean (ok=true , false=errors)
 */
bool NMGMGenXMLInterpret::updateWidgetFromXMLDocument(QString fileName)
{
	widget->mainWidget->txtTestName->setText("");
	widget->mainWidget->txtTestIdentifier->setText("");

	QRegExp exp ( "<.*>" );
	exp.setMinimal ( true );

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) 	return false;

	NMGMetaData metadata;
	NMGMetaDataParser metaParser (metadata);

	if (!metaParser.read(&file)) 
	{
		cerr << "Error while reading xml file" << qPrintable(fileName) << endl;
		return false;
	}

	/** if everything is fine... start parsing the xml test file **/
	
	// Version:
	QString version = metadata.getUncommonElement(TAG_MGEN_VERSION).remove(exp);
	if (!version.isEmpty())
	{
		int index = widget->mainWidget->comboMgenVersion->findText(version);
		widget->mainWidget->comboMgenVersion->setCurrentIndex(index);
	}
	// Identifier:
	if (!metadata.getTestIdentifier().isEmpty())
		widget->mainWidget->txtTestIdentifier->setText(metadata.getTestIdentifier());

	// Name:
	if (!metadata.getTestName().isEmpty())
		widget->mainWidget->txtTestName->setText(metadata.getTestName());

	// Source:
	if (!metadata.getSourceAddress().isEmpty())
	{
		NMGHostInformationComboWidget *combo = widget->mainWidget->hostSourceCombo;
		combo->setAddressOfStation(metadata.getSourceAddress());
	}
	// Destination:
	if (!metadata.getDestinationAddress().isEmpty())
	{
		NMGHostInformationComboWidget *combo = widget->mainWidget->hostDestinationCombo;
		combo->setAddressOfStation(metadata.getDestinationAddress());
	}
	// Mgen Configuration:
	widget->mainWidget->checkStartTime->setChecked(false);
	widget->mainWidget->txtSourcePort->setValue(5000);
	QXmlStreamReader xmlmgen(metadata.getUncommonElement(TAG_CONF_MGEN));
	
	NMGMGenFlow * flow = 0;
	NMGMGenFlowModification * mod = 0;
	bool isOnTag = false, isModTag = false;

	while (!xmlmgen.atEnd())
	{
		xmlmgen.readNext();
		if (xmlmgen.isStartElement())
		{
			if (xmlmgen.name() == TAG_CONF_MGEN) { continue; }
			else if (xmlmgen.name() == TAG_PORT)	
			{
				QString port = xmlmgen.readElementText();
                                widget->mainWidget->txtSourcePort->setValue(port.toInt());
			}
			else if(xmlmgen.name() == TAG_START)
			{
				widget->mainWidget->checkStartTime->setChecked(true);
				QString startTime = xmlmgen.readElementText();
			
				if(startTime.right(3) == "GMT")
					widget->mainWidget->comboGMTOrLocalTime->setItemText(0,"GMT");
				else
					widget->mainWidget->comboGMTOrLocalTime->setItemText(0,"Local");

				QString hour = startTime.left(2);
				QString min = startTime.mid(3,2);
				QString sec = startTime.mid(6,2);
				widget->mainWidget->timeStartTime->setTime(QTime(hour.toInt(),min.toInt(),sec.toInt()));
			}
			else if (xmlmgen.name() == TAG_OFF)
			{
				QString flowId = xmlmgen.attributes().value(ATTR_FLOW_ID).toString();
				QString time = xmlmgen.attributes().value(ATTR_TIME).toString();
				NMGMGenFlow * flow3 = widget->getFlowManager()->getFlowById(flowId.toInt());
				
				if (flow3)
					flow3->setDuration(time.toInt() - flow3->getStartTime());
				else 
					cerr << "panic: no flow for id in TAG_OFF " << qPrintable(flowId) << endl;
			}
			else if (xmlmgen.name() == TAG_ON)
			{
				flow = new NMGMGenFlow(widget->getFlowManager(), widget->mainWidget->flowList);
				
				flow->setRate(xmlmgen.attributes().value(ATTR_RATE).toString().toInt());
				flow->setPacketSize(xmlmgen.attributes().value(ATTR_SIZE).toString().toInt());
				flow->setId(xmlmgen.attributes().value(ATTR_FLOW_ID).toString().toInt());
				flow->setName(xmlmgen.attributes().value(ATTR_NAME).toString());
				flow->setStartTime(xmlmgen.attributes().value(ATTR_TIME).toString().toInt());
				flow->setPattern(xmlmgen.attributes().value(ATTR_PATTERN).toString());

				QString dest = xmlmgen.attributes().value(ATTR_DEST ).toString();
				int posPort = dest.lastIndexOf("/");
				
				flow->setPort(dest.mid(posPort+1).toInt());
				flow->getRSVP().setActive(false);
				flow->setBurstInterval(DEFAULT_BURST_INTERVAL,0);
				flow->setBurstDuration(DEFAULT_BURST_DURATION,0);
				flow->setBurst(false);
				flow->setTypeOfService(-1);
				flow->setTrafficClass(-1);
				
				isOnTag = true;
			}
			else if (isOnTag)
			{
				flow->setParametersFromOnNode(xmlmgen);
				isOnTag = false;
				flow = 0;
			}
			else if(xmlmgen.name() == TAG_MOD)
			{
				QString dest = xmlmgen.attributes().value(ATTR_DEST).toString();
				int posPort=-1, port=-1;
				
				if(dest != 0)
				{
					posPort = dest.lastIndexOf("/");
					port = dest.mid(posPort+1).toInt();
				}
				
				NMGMGenFlow * flow2 = widget->getFlowManager()->getFlowByPort(port);
				if(flow2)
				{
					mod = new NMGMGenFlowModification(flow2);
					flow2->getListViewItem()->setExpanded(true);
					
					mod->setRate(xmlmgen.attributes().value(ATTR_RATE).toString().toInt());
					mod->setPacketSize(xmlmgen.attributes().value(ATTR_SIZE).toString().toInt());
					mod->setPattern(xmlmgen.attributes().value(ATTR_PATTERN).toString());
					mod->setStartTime(xmlmgen.attributes().value(ATTR_TIME).toString().toInt() - flow2->getStartTime());

					mod->getRSVP().setActive(false);
					mod->setBurstInterval(DEFAULT_BURST_INTERVAL,0);
					mod->setBurstDuration(DEFAULT_BURST_DURATION,0);
					mod->setBurst(false);
					mod->setTypeOfService(-1);
					mod->setTrafficClass(-1);

					isModTag = true;
				}
				else 	cerr << "panic: no flow for mod with port " << port << endl;
			}
			else if (isModTag) 
			{
				mod->setParametersFromModNode(xmlmgen);
				isModTag = false;
				mod = 0;
			}
			else cerr << "unknow tag at conf_mgen document " << qPrintable(xmlmgen.name().toString()) << endl;
		}
		else if (xmlmgen.isEndElement())
		{
			if (xmlmgen.name() == TAG_ON) isOnTag = false;
			else if (xmlmgen.name() == TAG_MOD) isModTag = false;
		}
	}

	// Drec Configuration:
	QXmlStreamReader xmldrec(metadata.getUncommonElement(TAG_CONF_DREC));
	while (!xmldrec.atEnd())
	{
		xmldrec.readNext();
		if (xmldrec.isStartElement())
		{
			if (xmldrec.name() == TAG_RESV)	
			{
				QString dest = xmldrec.attributes().value(ATTR_DEST).toString();
				int posPort = dest.lastIndexOf("/");
				int port = dest.mid(posPort+1).toInt();
				NMGMGenFlow * flowd = widget->getFlowManager()->getFlowByPort(port);
				
				if(!flowd)
				{
					cerr << "panic: no flow for resv with port "<< port << endl;
				}

				QString style = xmldrec.attributes().value(ATTR_STYLE).toString();
				QString spec = xmldrec.attributes().value(ATTR_SPEC).toString();
				int time = xmldrec.attributes().value(ATTR_TIME).toString().toInt();
				
				if(flowd->getRSVP().isActive() and flowd->getStartTime() == time and flowd->getRSVP().getDestParam() == "")
				{
					flowd->getRSVP().setDestParam(spec);
					flowd->getRSVP().setResvStyleAsShortString(style);
				}
				else
				{
					// RESV d'alguna modificacio del flow
					int n = flowd->childCount();
					int i = 0;

					while(i < n)
   					{
						NMGMGenFlowListItem *item = (NMGMGenFlowListItem *)flowd->child(i);
						if(item->getFlowItemType()==FLOW_LIST_ITEM_MODIFICATION)
						{
							NMGMGenFlowModification * mod = (NMGMGenFlowModification *)item;
							if(mod->getRSVP().isActive() and mod->getRSVP().getDestParam()=="" and time==(flowd->getStartTime() + mod->getStartTime()))
							{
								mod->getRSVP().setDestParam(spec);
								mod->getRSVP().setResvStyleAsShortString(style);
							}
						}
					i++;
        				}
				}
			}
			else if(xmldrec.name() == TAG_UNRESV)		{ continue; }
			else if(xmldrec.name() == TAG_PORTS)		{ continue; }
			else if(xmldrec.name() == TAG_CONF_DREC)	{ continue; }
			else cerr << "unknow tag at conf_drec document " << qPrintable(xmldrec.name().toString()) << endl;
		}
	}
	
	/** End of xml parsing **/
	
	//TODO: delete &metaParser;
	
	return true;
}
