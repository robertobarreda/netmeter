/***************************************************************************
 *   Copyright (C) 2005 by Joan Sala                                       *
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

#include "nmgsyncwidget.h"

#include <QMenu>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QMessageBox>
#include <QWidget>
#include <QLayout>

#include <nmxml.h>
#include <nmghostinformationmanager.h>

#include <iostream>
using namespace std;

NMGSyncWidget::NMGSyncWidget(NMGISender * s, QWidget *parent, const char *name)
 : QWidget(parent)
{
	setupUi(this);
	setObjectName(name);

	sender = s;
	alredystarted = false;

	connect(startButton,SIGNAL(clicked()),this,SLOT(startSync()));
	connect(stopButton, SIGNAL(clicked()),this,SLOT(stopSync()));

	startButton->setEnabled(true);
	stopButton->setEnabled(false);
}

NMGSyncWidget::~NMGSyncWidget()
{
}

void NMGSyncWidget::startSync()
{
	NMGStation * source = sourceBox->getStationFromCombo();
	
	if(!source) 
	{
		QMessageBox::warning( 0, "Sync",
		"Sorry, Source station is not in the database\n Please insert it in order to continue the test.");
		NMGHostInformationManager::self()->showHostWidget( sourceBox->getStationAddressFromCombo() );
		return;
	}

	if(alredystarted)
	{
		QMessageBox::warning( 0, "Sync",
		"Sorry, Polling in progress\n Press <Stop> to finish it before starting it again.");
		return;
	}
	
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
	data = petition.createTextNode(sleepbox->text());
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

	// Send the command to the nm-core:

	if(sender->sendCommand(Start, petition.toString()))
	{
		alredystarted = true;
		startButton->setEnabled(false);
		if(sleepbox->text().toInt() != 0)	stopButton->setEnabled(true);
	}
	else	QMessageBox::warning( 0, "Sync","Command not started\nThere is no connection with the server!");
}

void NMGSyncWidget::stopSync()
{
	if(alredystarted)	sender->sendCommand(Stop, "");
	else			alredystarted = false;

	startButton->setEnabled(true);
	stopButton->setEnabled(false);
}

void NMGSyncWidget::resultReceived(QString data)
{
	alredystarted = false;
	startButton->setEnabled(true);
	stopButton->setEnabled(false);

	if (data.isEmpty())
	{
		std::cout << "Sorry, we've not recieved data from nmsync";
		return;
	}

	// Parse the result:	
	QDomDocument doc;
	doc.setContent(data);
	
	QDomElement element = doc.documentElement();
	QDomNode parent;
	
	QString pps_status, ntp_status, info;
/*
	**************************************
	NMSync receive start reply:
 	<data>
   		<ntpdata>
      			<status>0</status>
         		<pps>0</pps>
	    		<maxerr>4017 us</maxerr>
	       		<estimatederr>3 us</estimatederr>
	          	<offset>-56.000 us</offset>
		     	<time>13:59:24.363819</time>
			<time_file>/tmp/ntptime1212.time</time_file>
		</ntpdata>
	</data>
	**************************************
*/
	for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
    	{
		if(n.isElement()) 
		{
			if(n.nodeName() == "status") 
			{
				ntp_status = n.toElement().text();
				
				if (ntp_status == "0")		ntp_status = " ntptime :: OK";
				else if (ntp_status == "1")	ntp_status = " ntptime :: ERROR";
				else if (ntp_status == "2")	ntp_status = " ntptime :: COMMAND NOT FOUND";
				else				ntp_status = " ntptime :: UNKNOWN RESPONSE";
			}

			else if(n.nodeName() == "pps")
			{
				pps_status = n.toElement().text();
		
				if(pps_status == "0")	pps_status = " ppsignal :: PPS FOUND";
				else			pps_status = " ppsignal :: PPS NOT FOUND";
			}

			else if(n.nodeName() == "maxerr")
			{
				info = n.toElement().text();
				resultat->setText("MAXIMUM ERROR:  " + info);
			}

			else if(n.nodeName() == "estimatederr")
			{
				info = n.toElement().text();
				resultat4->setText("ESTIMATED ERROR:  " + info);
			}

			else if(n.nodeName() == "offset")
			{
				info = n.toElement().text();
				resultat5->setText("OFFSET:  " + info);
			}

			else if(n.nodeName() == "time")
			{
				info = n.toElement().text();
				resultat2->setText("TIME:  "+ info);
			}

			else if(n.nodeName() == "time_file")
			{
				info = n.toElement().text();
				resultat7->setText("RESULT FILE:  "+ info);
			}

			else
			{
				n = n.firstChild();
			}
		}
    	}

	resultat3->setText("NTPD/PPS STATUS:  " + ntp_status +  " , " + pps_status);
}

void NMGSyncWidget::notifyError(int errorType, QString errorMessage)
{
	if(errorType>=1 and errorType<=5)
	{
		cout << "NMGSyncWidget::notifyError" << endl;
	}
}

QMenu * NMGSyncWidget::getPopupMenu()
{
 	return menu;
}

void NMGSyncWidget::testCancelled( ProgressItem *)
{
	stopSync();
}

#include "nmgsyncwidget.moc"
