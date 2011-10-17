/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella                                     *
 *   pcapella@ac.upc.edu                                                   *
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

#define ID_HOSTSVIEW_COL 	0
#define ADDRESS_HOSTSVIEW_COL 	1
#define ALIAS_HOSTSVIEW_COL 	2
#define TIME_HOSTVIEW_COL 	3

#include <iostream>
using namespace std;

#include <QMessageBox>
#include <QDomElement>
#include <QDomDocument>
#include <QDomNode>
#include <QComboBox>
#include <QPushButton>

#include "nmgtracewidget.h"
#include "hostlistviewitem.h"

#include <messagebar.h>
#include <popupmessage.h>
#include <nmghostinformationmanager.h>
#include <nmgutil.h>


NMGTraceWidget::NMGTraceWidget(NMGISender *s, QWidget *parent,  const char *name)
 : QWidget(parent), m_progress(0)
{
	setupUi(this);
	setObjectName(name);
	sender = s;
	stopButton->setEnabled(false);
}

NMGTraceWidget::~NMGTraceWidget()
{
}

void NMGTraceWidget::infoHostSource()
{
	NMGHostInformationManager::self()->showHostWidget();
}


void NMGTraceWidget::infoHostDestination()
{
	NMGHostInformationManager::self()->showHostWidget();
}

/**
	Start Trace, create a valid XML to send to NetMeter
*/
void NMGTraceWidget::startTrace()
{
	NMGStation *source = sourceBox->getStationFromCombo();
	
	if (!source) 
	{
		QMessageBox::warning( 0, "Trace",
		"Sorry, Source station is not in the database\n Please insert it for continuing the test.");
		NMGHostInformationManager::self()->showHostWidget( sourceBox->getStationAddressFromCombo() );
		return;
	}

	NMGStation *destination = destinationBox->getStationFromCombo();
	
	if (!destination) 
	{
		QMessageBox::warning( 0, "Trace",
		"Sorry, Destination station is not in the database\n Please insert it for continuing the test.");
		NMGHostInformationManager::self()->showHostWidget( destinationBox->getStationAddressFromCombo() );
		return;
	}
	
	// Generem el xml
	QDomDocument petition("tracexml");
	QDomElement root = petition.createElement("data");
 	petition.appendChild(root);

	QDomElement tag;
	QDomText data;

	// Quin protocol tenim:
	tag = petition.createElement("protocol");
	if (source->getTestHost().isHostIPv4() )
		data = petition.createTextNode("IPv4");
	else
		data = petition.createTextNode("IPv6");
	tag.appendChild(data);
	root.appendChild(tag);

	// Creem les dades:
	QDomElement xmlsource = petition.createElement("source");
	
	tag = petition.createElement("name");
	data = petition.createTextNode(source->getTestHost().getAddress());
	tag.appendChild(data);
	xmlsource.appendChild(tag);

	// TODO Falta comprovar el soure si es IPv6
	if ( source->getTestHost().getAddress() != "127.0.0.1") {
		QDomElement xmlmanage = petition.createElement("management");
		
		// Name
		QDomElement tagmanage = petition.createElement("name");
		QDomText datamanage = petition.createTextNode( source->getManagementHost().getAddress() );
		tagmanage.appendChild(datamanage);
		xmlmanage.appendChild(tagmanage); 
		
		// Login
		tagmanage = petition.createElement("username");
		datamanage = petition.createTextNode( source->getManagementUsername() );
		tagmanage.appendChild(datamanage);
		xmlmanage.appendChild(tagmanage); 

		tagmanage = petition.createElement("password");
		datamanage = petition.createTextNode( source->getManagementPassword() );
		tagmanage.appendChild(datamanage);
		xmlmanage.appendChild(tagmanage); 

		xmlsource.appendChild (xmlmanage);
	}
	root.appendChild(xmlsource);

	QDomElement xmldestination = petition.createElement("destination");
	
	tag = petition.createElement("name");
	data = petition.createTextNode( destination->getTestHost().getAddress() );
	tag.appendChild(data);
	xmldestination.appendChild(tag);
	root.appendChild(xmldestination);

	sender->sendCommand(Start, petition.toString());

	// Make progress bar
	
	QString title = "Trace: " + source->getTestHost().getAddress() + " - " + destination->getTestHost().getAddress();
	m_progress = ProgressManager::createProgressItem(title);
	m_progress->setTotalItems(8);
	connect(m_progress, SIGNAL(progressItemCanceled(ProgressItem*)), this, SLOT(slotProgressItemCanceled(ProgressItem *)));

	stopButton->setEnabled(TRUE);
	startButton->setEnabled(FALSE);
}

void NMGTraceWidget::statusReceived(QString)
{
	m_progress->incCompletedItems();
	m_progress->updateProgress();
}

void NMGTraceWidget::errorRecieved(int err, QString Message)
{
	m_progress->setStatus("Error: " + Message);
	QMessageBox::critical(0,"Error", Message);
	m_progress->setComplete();
	stopButton->setEnabled(FALSE);
	startButton->setEnabled(TRUE);
}

/**
	Received result from NetMeter, create a list of hosts
*/
void NMGTraceWidget::resultReceived(QString data)
{
	m_progress->setStatus(tr("Received results...."));
	m_progress->setComplete();
	cout << GREEN << "[OK] Traceroute complete" << ENDCOLOR << endl;
	

	if(data.isEmpty())
		return;

	// Parsejat de les dades rebudes:
	
	QDomDocument doc;
	doc.setContent(data);
	
	QDomElement element = doc.documentElement();
	QDomNode parent;
	
	QString address;
	QString time;
	int pos = 0;

	hostsView->clear();

	for( QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling() )
    	{
		if(n.isElement()) {
			if( n.nodeName() == "data") 
				n = n.firstChild();
			if( n.nodeName() == "hosts")
				n = n.firstChild();
			if( n.nodeName() == "host") 
			{
				parent = n;
				n = n.firstChild();
			}
			if( n.nodeName() == "address")
			{
				address = n.toElement().text();
				if(address.isEmpty())	address = "*";
			}
			if( n.nodeName() == "time") 
			{
				time = n.toElement().text();
				if(time.isEmpty())	time = "*";
				
				n = parent;
				showHost(pos, address, time);
				pos++;
			}
		}
    	}

	stopButton->setEnabled(FALSE);
	startButton->setEnabled(TRUE);
}

/**
	Show host data on the listView
*/
void NMGTraceWidget::showHost(const int &pos, const QString &address, const QString &time)
{
	HostListViewItem *item = new HostListViewItem(hostsView);
		
	item->setText(ID_HOSTSVIEW_COL, QString::number(pos));
	item->setText(ADDRESS_HOSTSVIEW_COL, address);

	if(address == "Unknow") 
	{
		item->setText(ALIAS_HOSTSVIEW_COL, "Unknown");
		item->setBackgroundColor(0, Qt::gray);
		item->setBackgroundColor(1, Qt::gray);
		item->setBackgroundColor(2, Qt::gray);
		item->setBackgroundColor(3, Qt::gray);
		item->setToolTip(1, "This station doesn't belong to the netmeter list of known stations");
		item->setToolTip(2, "This station doesn't belong to the netmeter list of known stations");
	}
	else if (!NMGHostInformationManager::self()->existsStation(address)) 
	{
		item->setText(ALIAS_HOSTSVIEW_COL, address);
		item->setBackgroundColor(0, Qt::green);
		item->setBackgroundColor(1, Qt::green);
		item->setBackgroundColor(2, Qt::green);
		item->setBackgroundColor(3, Qt::green);
		item->setToolTip(1, "This station belongs to the netmeter list of known stations");
		item->setToolTip(2, "This station doesn't belong to the netmeter list of known stations");
	}
	else 
	{ 	//Aqui el la ip existeix a les BB.DD
		QString alias = ((NMGHostInformationManager::self()->getStation(address))->getTestHost()).getAlias();
		item->setText(ALIAS_HOSTSVIEW_COL, alias);
		item->setBackgroundColor(0, Qt::green);
		item->setBackgroundColor(1, Qt::green);
		item->setBackgroundColor(2, Qt::green);
		item->setBackgroundColor(3, Qt::green);
                item->setToolTip(1, "This station belongs to the netmeter list of known stations");
                item->setToolTip(2, "This station doesn't belong to the netmeter list of known stations");
	}
	item->setText(TIME_HOSTVIEW_COL, time.simplified());
	hostsView->addTopLevelItem(item);
}

/**
	stop button clicked
*/
void NMGTraceWidget::stopTrace()
{
	m_progress->setStatus(tr("Sending Stop..."));
	m_progress->setComplete();
	QMessageBox::critical(0,"Error", "The query couldn't be done due to errors\n");
	cout << RED << "[ERROR] The query couldn't be done due to errors" << ENDCOLOR << endl;
	sender->sendCommand(Stop, "");	
	stopButton->setEnabled(FALSE);
	startButton->setEnabled(FALSE);
}

/**
	cancel buton in progress bar clicked
*/
void NMGTraceWidget::slotProgressItemCanceled( ProgressItem *)
{
	stopTrace();
}


#include "nmgtracewidget.moc"
