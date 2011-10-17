/***************************************************************************
 *   Copyright (C) 2005 by Rus Andrei Bogdan                               *
 *   bogdanrus2004@yahoo.com                                               *
 *   Copyright (C) 2008 by Joan Sala Calero                                *
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

#include "nmgsimulatorwidget.h"

#include <nmdns.h>
#include <nmhost.h>
#include <nmstring.h>
#include <nmghostinformationmanager.h>
#include <nmgimagemanager.h>
#include <nmgmodtabwidgetmanager.h>
#include <nmgmoduleutils.h>

#include <iostream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QTextStream>
#include <QFileDialog>
#include <QValidator>
#include <QAction>

using namespace std;

//declarations

NMGStation * srcStationObj, * destStationObj, * nsStationObj;
NMGHost srcHostObj, destHostObj, nsHostObj;


NMGSimulatorWidget::NMGSimulatorWidget ( NMGISender * s, NMGModTabWidgetManager * tabWidgetMan, NMGModulePreferencesManager * prefMan, QWidget *parent, const char *name ): QWidget ( parent )
{
	setupUi ( this );
	labelLogoS->setPixmap( *NMGImageManager::loadPixmap( "host.png" ) );
	labelLogoR->setPixmap( *NMGImageManager::loadPixmap( "host_router.png" ) );
	labelLogoD->setPixmap( *NMGImageManager::loadPixmap( "host.png" ) );
	sender = s;
	toolBar = 0;
	tabWidgetManager = tabWidgetMan;
	preferencesManager = prefMan;
	flowListMenu = 0;
	flowList->setRootIsDecorated(true);
	flowList->setColumnCount(1);
	lastItem = 0;
	createFlowListMenu();
	
	//configuring the data types from the input fields

	//for delay tab
	delayDelay->setValidator ( new QDoubleValidator ( .0001, 99999, 4, delayDelay ) );
	delayJitter->setValidator ( new QDoubleValidator ( .0001, 99999, 4, delayJitter ) );

	//for rate tab
	rateRate->setValidator ( new QDoubleValidator ( .001, 99999, 4, rateRate ) );
	rateBurst->setValidator ( new QIntValidator ( 0, 9999, rateBurst ) );

	oldName = "Flow 0";

	// XML:
	root = doc.createElement ( "NetworkSimulatorSettings" );
	router = doc.createElement ( "router" );
	settings = doc.createElement ( "settings" );
	doc.appendChild ( root );
	root.appendChild ( router );
	root.appendChild ( settings );

	/* Connect's afegits que no estan al *.ui */
	connect(flowList,SIGNAL( itemSelectionChanged() ),this,SLOT( showItemSelected() ));
	connect( nsHost, SIGNAL( stationSelected(QString) ), this, SLOT( addNS() ) );
	connect( dRadio2, SIGNAL ( clicked () ), this, SLOT ( delayGroupChanged() ));
	connect( dRadio3, SIGNAL ( clicked () ), this, SLOT ( delayGroupChanged() ));
	connect( DDR1, SIGNAL ( clicked () ), this, SLOT ( dDGroupChanged() ));
	connect( DDR2, SIGNAL ( clicked () ), this, SLOT ( dDGroupChanged() ));
	connect( DDR3, SIGNAL ( clicked () ), this, SLOT ( dDGroupChanged() ));
}

NMGSimulatorWidget::~NMGSimulatorWidget()
{
}

QToolBar * NMGSimulatorWidget::getToolBar()
{
	QAction * action;

	if(!toolBar)
	{
		toolBar = new QToolBar(NMGModuleUtils::getMainWindow());

		action = new QAction(QIcon(*NMGModuleUtils::getPixmap("fileopen.png")), tr("Load settings from disk"),0);
 		connect(action,SIGNAL(triggered()),this,SLOT(loadSettingsSlot()));
		toolBar->addAction(action);

		action = new QAction(QIcon(*NMGModuleUtils::getPixmap("filesave.png")), tr("Save settings to disk"),0);
		connect(action,SIGNAL(triggered()),this,SLOT(saveSettingsSlot()));
		toolBar->addAction(action);	
		
		action = new QAction(QIcon(*NMGModuleUtils::getPixmap("add.png")), tr("Add new flow"),0);
		connect(action,SIGNAL(triggered()),this,SLOT(addFlowSlot()));
		toolBar->addAction(action);

		action = new QAction(QIcon(*NMGModuleUtils::getPixmap("remove-red.png")), tr("Delete flow"),0);
 		connect(action,SIGNAL(triggered()),this,SLOT(delFlowSlot()));
		toolBar->addAction(action);
	}
	return toolBar;
}

void NMGSimulatorWidget::createFlowListMenu()
{
	flowListMenu = new QMenu();
	flowListMenu->addAction("Add &new flow",this,SLOT(addFlowSlot()), Qt::CTRL + Qt::Key_N);
	flowListMenu->addAction("&Delete flow",this,SLOT(delFlowSlot()), Qt::CTRL + Qt::Key_D);
	flowListMenu->addSeparator();
	flowListMenu->addAction("&Load settings from disk",this,SLOT(loadSettingsSlot()), Qt::CTRL + Qt::Key_L);
	flowListMenu->addAction("&Save settings to disk",this,SLOT(saveSettingsSlot()), Qt::CTRL + Qt::Key_S);
	connect( flowList ,SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showMenu(const QPoint &)));
}

void NMGSimulatorWidget::showMenu(const QPoint & pos)
{
	flowListMenu->exec(QCursor::pos());
}

QMenu * NMGSimulatorWidget::getPopupMenu()
{
	return flowListMenu;
}

void NMGSimulatorWidget::addSource()
{
	srcAddress->setAlignment (Qt::AlignRight);
	srcStationObj = NMGHostInformationManager::self()->getStation ( srcHost->getStationAddressFromCombo() );
	
	if(srcStationObj)
	{
		srcHostObj = srcStationObj->getTestHost();
		QString hoststr = srcHostObj.getAddress();

		NMHost  host = NMDns::resolv( hoststr.toAscii().constData() , NMDns::IPv4);

		srcAddress->setText ( QString(host.getAddress().c_str()) );
		srcAlias->setText ( srcHostObj.getAlias() );
		srcAlias2->setAlignment ( Qt::AlignCenter );
		srcAlias2->setText ( srcHostObj.getAlias() );
	}
}

void NMGSimulatorWidget::addNS()
{
	nsAddress->setAlignment ( Qt::AlignRight );
	nsStationObj = NMGHostInformationManager::self()->getStation ( nsHost->getStationAddressFromCombo() );
	if(!nsStationObj)	return;

	nsHostObj = nsStationObj->getManagementHost();
	QString hoststr = nsHostObj.getAddress();
//	NMHost  host = NMDns::resolv( hoststr.toAscii().constData() , NMDns::IPv4);
//	nsAddress->setText ( QString(host.getAddress().c_str()) );
	nsAddress->setText ( hoststr );

	nsUser->setAlignment ( Qt::AlignRight );
	nsUser->setText ( nsStationObj->getManagementUsername() );
	nsPort->setAlignment ( Qt::AlignRight );
	port.sprintf ( "%d", nsStationObj->getManagementAddressPort() ); //convert the integer value to string
	nsPort->setText ( port );
	nsHostObj = nsStationObj->getTestHost();
	nsAlias2->setAlignment ( Qt::AlignCenter );
	nsAlias2->setText ( nsHostObj.getAlias() );
	connectButton->setEnabled ( 1 );
	srcInterface->setEnabled ( 0 );
	pixmapLabel4->setEnabled ( 0 );
	destInterface->setEnabled ( 0 );
	applyButton->setEnabled ( 0 );
	saveButton->setEnabled ( 1 );
	loadButton->setEnabled ( 1 );
	srcInterface->clear();
	srcInterface->insertItem ( srcInterface->count(), "Any" );	// append
	destInterface->clear();
	destInterface->insertItem ( destInterface->count(), "Any" );	// append
	flowList->setColumnCount(1);
	flowList->setRootIsDecorated(true);
}

void NMGSimulatorWidget::addDestination()
{
	destAddress->setAlignment ( Qt::AlignRight );
	destStationObj = NMGHostInformationManager::self()->getStation ( destHost->getStationAddressFromCombo() );
	
	if(destStationObj)
	{
		destHostObj = destStationObj->getTestHost();
		QString hoststr = destHostObj.getAddress();
		NMHost  host = NMDns::resolv( hoststr.toAscii().constData() , NMDns::IPv4);

		destAddress->setText ( QString(host.getAddress().c_str()) );
		destAlias->setText ( destHostObj.getAlias() );
		destAlias2->setAlignment ( Qt::AlignCenter );
		destAlias2->setText ( destHostObj.getAlias() );
	}
}

/**
 * Updates the flows identifiers of the treewidget
 */
void NMGSimulatorWidget::updateFlowListIdentifier()
{
	int n = flowList->topLevelItemCount();
	int i = 0;

	QString numstring;

	while (i < n)
	{
		QTreeWidgetItem *flow = (QTreeWidgetItem *)flowList->topLevelItem(i);
		flow->setText(0, "Flow " + QString::number(i));
		i++;
	}
}

/**
 * Updates the flows identifiers of the xml settings (more than 2 elements)
 */
void NMGSimulatorWidget::updateXMLIdentifiers( )
{
	QDomNode flow = settings.firstChild();
	int i = 0;

	while ( !flow.isNull() )
	{
		flow.toElement().setAttribute("name", "Flow " + QString::number(i));
		flow = flow.nextSibling();
		i++;
	}
}

/**
 * Adds a new flow to the XML & the treewidget
 */
void NMGSimulatorWidget::addFlowSlot()
{
	QString nr, name, radioDelay, radioDD; 

	// 1) give a new flowId
	name = "Flow " + QString::number(flowList->topLevelItemCount());
	newName = name;

	// 2) create a a flow item
	QTreeWidgetItem * flowItem = new QTreeWidgetItem ( flowList, 0 );
	flowItem->setText(0, name);

	// 3) constructing the element for the xml file
	QDomElement flow = doc.createElement ( "Flow" );
	QDomElement source = doc.createElement ( "Source" );
	QDomElement destination = doc.createElement ( "Destination" );
	QDomElement protocols = doc.createElement ( "Protocols" );
	QDomElement delayTab = doc.createElement ( "DelayTab" );
	QDomElement DDTab = doc.createElement ( "DDTab" );
	QDomElement rateTab = doc.createElement ( "RateTab" );

	settings.appendChild ( flow );
	flow.setAttribute ( "name", name );
	oldName = name;

	flow.appendChild ( source );
	source.setAttribute ( "srcAlias", srcAlias->text() );
	source.setAttribute ( "srcAddress", srcAddress->text() );
	source.setAttribute ( "srcMask", srcMask->text() );
	source.setAttribute ( "srcPort", srcPort->text() );
	source.setAttribute ( "srcInterface", srcInterface->currentText() );

	flow.appendChild ( destination );
	destination.setAttribute ( "destAlias", destAlias->text() );
	destination.setAttribute ( "destAddress", destAddress->text() );
	destination.setAttribute ( "destMask", destMask->text() );
	destination.setAttribute ( "destPort", destPort->text() );
	destination.setAttribute ( "destInterface", destInterface->currentText() );

	flow.appendChild ( protocols );
	protocols.setAttribute ( "l4Protocol", l4Protocol->currentText() );
	protocols.setAttribute ( "l3Protocol", l3Protocol->currentText() );

	flow.appendChild ( delayTab );
	delayTab.setAttribute ( "delayDelay", delayDelay->text() );
	delayTab.setAttribute ( "delayJitter", delayJitter->text() );
	radioDelay = returnDelayGroup();
	
	if ( radioDelay == "random" )
	{
		delayTab.setAttribute ( "radioDelay", "random" );
		delayTab.setAttribute ( "radioDelayValue", "random" );
	}
	else if ( radioDelay == "correlation" )
	{
		delayTab.setAttribute ( "radioDelayValue", delayCorrelation->text() );
		delayTab.setAttribute ( "radioDelay", "correlation" );
	}
	else
	{
		delayTab.setAttribute ( "radioDelayValue", delayDistribution->currentText() );
		delayTab.setAttribute ( "radioDelay", "distribution" );
	}

	flow.appendChild ( DDTab );
	radioDD = returnDDGroup();
	if ( radioDD == "none" )
	{
		DDTab.setAttribute ( "radioDD", "none" );
		DDTab.setAttribute ( "DD", "none" );
		DDTab.setAttribute ( "DDCorrelation", "none" );
	}
	else if ( radioDD == "drop" )
	{
		DDTab.setAttribute ( "radioDD", "drop" );
		DDTab.setAttribute ( "DD", DD->text() );
		DDTab.setAttribute ( "DDCorrelation", DDCorrelation->text() );
	}
	else
	{
		DDTab.setAttribute ( "radioDD", "duplicate" );
		DDTab.setAttribute ( "DD", DD->text() );
		DDTab.setAttribute ( "DDCorrelation", DDCorrelation->text() );
	}

	flow.appendChild ( rateTab );
	rateTab.setAttribute ( "rateRate", rateRate->text() );
	//rateTab.setAttribute("rateMaxBurst",rateMaxBurst->text());
	rateTab.setAttribute ( "rateBurst", rateBurst->text() );
/*
	DEBUG:

	cout << " ********** DATA ( NMGSimulatorWidget::addFlowSlot() ) *********** " << endl;
	cout << qPrintable(doc.toString(1)) << endl << endl;
	cout << " ***************************************************************** " << endl << endl;
*/
}

/**
 * Saves the last flow that could've suffered any kind of modification:
 */
void NMGSimulatorWidget::saveFlowSlot ( )
{
	if ( lastItem == 0 )	return; // we don't need to save it

	QString flowid = "Flow " + QString::number( flowList->indexOfTopLevelItem(lastItem) );
	QString radioDelay, radioDD;

	// 1) search item in the xml object
	QDomNode oldflow = settings.firstChild();
	int out = 0; 

	while (( !settings.isNull()) & (out != 1))
	{
		if ( oldflow.toElement().attribute ( "name" ) != flowid )
			oldflow = oldflow.nextSibling();
		else 
			out = 1;	
	}

	// 2) Replace the item (oldflow) with the new data (newflow):
	QDomElement newflow = doc.createElement ( "Flow" );
	QDomElement source1 = doc.createElement ( "Source" );
	QDomElement destination1 = doc.createElement ( "Destination" );
	QDomElement protocols1 = doc.createElement ( "Protocols" );
	QDomElement delayTab1 = doc.createElement ( "DelayTab" );
	QDomElement DDTab1 = doc.createElement ( "DDTab" );
	QDomElement rateTab1 = doc.createElement ( "RateTab" );

	settings.replaceChild ( newflow, oldflow );
	
	// 3) Save the attributes in the newflow:
	
	newflow.setAttribute ( "name", flowid );
	newflow.appendChild ( source1 );
	source1.setAttribute ( "srcAlias", srcAlias->text() );
	source1.setAttribute ( "srcAddress", srcAddress->text() );
	source1.setAttribute ( "srcMask", srcMask->text() );
	source1.setAttribute ( "srcPort", srcPort->text() );
	source1.setAttribute ( "srcInterface", srcInterface->currentText() );
	newflow.appendChild ( destination1 );
	destination1.setAttribute ( "destAlias", destAlias->text() );
	destination1.setAttribute ( "destAddress", destAddress->text() );
	destination1.setAttribute ( "destMask", destMask->text() );
	destination1.setAttribute ( "destPort", destPort->text() );
	destination1.setAttribute ( "destInterface", destInterface->currentText() );
	newflow.appendChild ( protocols1 );
	protocols1.setAttribute ( "l4Protocol", l4Protocol->currentText() );
	protocols1.setAttribute ( "l3Protocol", l3Protocol->currentText() );
	newflow.appendChild ( delayTab1 );
	delayTab1.setAttribute ( "delayDelay", delayDelay->text() );
	delayTab1.setAttribute ( "delayJitter", delayJitter->text() );
	radioDelay = returnDelayGroup();

	if ( radioDelay == "random" )
	{
		delayTab1.setAttribute ( "radioDelay", "random" );
		delayTab1.setAttribute ( "radioDelayValue", "random" );
	}
	else if ( radioDelay == "correlation" )
	{
		delayTab1.setAttribute ( "radioDelayValue", delayCorrelation->text() );
		delayTab1.setAttribute ( "radioDelay", "correlation" );
	}
	else
	{
		delayTab1.setAttribute ( "radioDelayValue", delayDistribution->currentText() );
		delayTab1.setAttribute ( "radioDelay", "distribution" );
	}

	newflow.appendChild ( DDTab1 );
	radioDD = returnDDGroup();
	if ( radioDD == "none" )
	{
		DDTab1.setAttribute ( "radioDD", "none" );
		DDTab1.setAttribute ( "DD", "none" );
		DDTab1.setAttribute ( "DDCorrelation", "none" );
	}
	else if ( radioDD == "drop" )
	{
		DDTab1.setAttribute ( "radioDD", "drop" );
		DDTab1.setAttribute ( "DD", DD->text() );
		DDTab1.setAttribute ( "DDCorrelation", DDCorrelation->text() );
	}
	else
	{
		DDTab1.setAttribute ( "radioDD", "duplicate" );
		DDTab1.setAttribute ( "DD", DD->text() );
		DDTab1.setAttribute ( "DDCorrelation", DDCorrelation->text() );
	}

	newflow.appendChild ( rateTab1 );
	rateTab1.setAttribute ( "rateRate", rateRate->text() );
	//rateTab1.setAttribute("rateMaxBurst",rateMaxBurst->text());
	rateTab1.setAttribute ( "rateBurst", rateBurst->text() );
}

/**
 * Shows the current item selected in the treewidget
 */
void NMGSimulatorWidget::showItemSelected ( )
{
	QTreeWidgetItem * item = flowList->currentItem();
	
	if (lastItem != item)
		saveFlowSlot();		// Check if we need to save any modifications
	
	showItem( item );		// Show the selected item
	lastItem = item;		// Update the last selected item
}

void NMGSimulatorWidget::showItem ( QTreeWidgetItem * itemShow )
{
	int out, val, index;
	bool ok;
	QString value, radioDelay, radioDD, flowId;

	if(itemShow == 0)
	{
		//cout << "NMGSimulatorWidget::showItem -> Trying to show null item" << endl;
		return;
	}
	
	index = flowList->indexOfTopLevelItem(flowList->currentItem());
	flowId = itemShow->text ( 0 );
	newName = flowId;
	
	// 1) show the values for the rest of the fields
	QDomNode node = settings.firstChild();
	out = 0;
	
	// searching the item
	while ( ( !settings.isNull() ) & ( out == 0 ) )
	{
		if ( node.toElement().attribute ( "name" ) != flowId )
		{
			node = node.nextSibling();
		}
		else { out = 1;}
	}

	QDomNode source = node.firstChild();
	QDomNode destination = source.nextSibling();
	QDomNode protocols = destination.nextSibling();
	QDomNode delayTab = protocols.nextSibling();
	QDomNode DDTab = delayTab.nextSibling();
	QDomNode rateTab = DDTab.nextSibling();

	// 3) input datas in source tab
	srcAlias->setText ( source.toElement().attribute ( "srcAlias" ) );
	srcAlias2->setText ( source.toElement().attribute ( "srcAlias" ) );
	srcAddress->setText ( source.toElement().attribute ( "srcAddress" ) );
	srcHost->clear();

	value = source.toElement().attribute ( "srcMask" );
	val = value.toInt ( &ok, 10 );
	srcMask->setValue ( val );
	value = source.toElement().attribute ( "srcPort" );
	val = value.toInt ( &ok, 10 );
	srcPort->setValue ( val );
	value = source.toElement().attribute ( "srcInterface" );
	val = value.toInt ( &ok, 10 );
	srcInterface->setCurrentIndex ( val );

	// 4) input datas in destination tab
	destAlias->setText ( destination.toElement().attribute ( "destAlias" ) );
	destAlias2->setText ( destination.toElement().attribute ( "destAlias" ) );
	destAddress->setText ( destination.toElement().attribute ( "destAddress" ) );
	destHost->clear();

	value = destination.toElement().attribute ( "destMask" );
	val = value.toInt ( &ok, 10 );
	destMask->setValue ( val );
	value = destination.toElement().attribute ( "destPort" );
	val = value.toInt ( &ok, 10 );
	destPort->setValue ( val );
	value = destination.toElement().attribute ( "destInterface" );
	val = value.toInt ( &ok, 10 );
	destInterface->setCurrentIndex ( val );

	// 5) input datas in protocols tab
	value = protocols.toElement().attribute ( "l4Protocol" );
	val = l4Protocol->findText( value );
	l4Protocol->setCurrentIndex ( val );
	l4ProtocolChanged ( l4Protocol->itemText ( val ) );
	value = protocols.toElement().attribute ( "l3Protocol" );
	val = l3Protocol->findText( value );
	l3Protocol->setCurrentIndex ( val );

	// 6) input datas in delay tab
	delayDelay->setText ( delayTab.toElement().attribute ( "delayDelay" ) );
	delayJitter->setText ( delayTab.toElement().attribute ( "delayJitter" ) );

	radioDelay = delayTab.toElement().attribute ( "radioDelay" );
	if ( radioDelay == "random" )
	{
		delayGroupChanged ( radioDelay );
		delayCorrelation->setValue ( 0 );
		delayDistribution->setCurrentIndex ( 0 );
	}
	else  if ( radioDelay == "correlation" )
	{
		delayGroupChanged ( radioDelay );
		value = delayTab.toElement().attribute ( "radioDelayValue" );
		val = value.toInt ( &ok, 10 );
		delayCorrelation->setValue ( val );
		delayDistribution->setCurrentIndex ( 0 );
	}
	else
	{
		delayGroupChanged ( radioDelay );
		delayCorrelation->setValue ( 0 );
		value = delayTab.toElement().attribute ( "radioDelayValue" );
		val = delayDistribution->findText( value );
		delayDistribution->setCurrentIndex ( val );
	}

	// 7) input datas in DD tab
	
	radioDD = DDTab.toElement().attribute ( "radioDD" );
	if ( radioDD == "none" )
	{
		dDGroupChanged ( radioDD );
		DD->setValue ( 0 );
		DDCorrelation->setValue ( 0 );
	}
	else  if ( radioDD == "drop" )
	{
		dDGroupChanged ( radioDD );
		value = DDTab.toElement().attribute ( "DD" );
		val = value.toInt ( &ok, 10 );
		DD->setValue ( val );
		value = DDTab.toElement().attribute ( "DDCorrelation" );
		val = value.toInt ( &ok, 10 );
		DDCorrelation->setValue ( val );
	}
	else
	{
		dDGroupChanged ( radioDD );
		value = DDTab.toElement().attribute ( "DD" );
		val = value.toInt ( &ok, 10 );
		DD->setValue ( val );
		value = DDTab.toElement().attribute ( "DDCorrelation" );
		val = value.toInt ( &ok, 10 );
		DDCorrelation->setValue ( val );
	}

	// 8) input datas in drop tab
	rateRate->setText ( rateTab.toElement().attribute ( "rateRate" ) );
//	rateMaxBurst->setText(rateTab.toElement().attribute("rateMaxBurst"));
	rateBurst->setText ( rateTab.toElement().attribute ( "rateBurst" ) );
}


void NMGSimulatorWidget::delFlowSlot()
{
	QTreeWidgetItem * delItem, * nextItem;	

	// 1) Check if there ain't no flows to delete || delItem is not a valid item
	delItem = flowList->currentItem();
	if(flowList->topLevelItemCount() == 0 || delItem == 0)
	{
		QMessageBox::warning(0,"No item selected","You have to select something to delete");
		oldName = "Flow 0";
		return;
	}

	// 2) Select de nextItem, the one that will be selected after the delete
	if(flowList->topLevelItemCount() >= 1)	
		nextItem = flowList->topLevelItem(0);
	if(nextItem == delItem)	
		nextItem = flowList->topLevelItem(1);

	// 3) delete item from flowList
	int index = flowList->indexOfTopLevelItem( delItem );
  	QTreeWidgetItem *ok = flowList->takeTopLevelItem ( index );

	if(!ok)	
	{
		cout << RED;
		cout << "[ERROR] NMGSimulator::delFlowSlot() couldn't remove the selected item" << ENDCOLOR << endl;
	}
	// 4) delete item from xml object
	QDomNode node = settings.firstChild();
	QString lastSelected = "Flow " + QString::number(index);
	int out = 0; 

	while (( !settings.isNull()) & (out != 1))
	{
		if ( node.toElement().attribute ( "name" ) != lastSelected )
			node = node.nextSibling();
		else 
			out = 1;	
	}
	settings.removeChild ( node );

	// 5) Update the id's

	if ( nextItem != 0 )
	{
		oldName = nextItem->text(0);
		updateFlowListIdentifier();
		updateXMLIdentifiers();
		nextItem->setSelected(true);
		flowList->setCurrentItem(nextItem);
 	}
/*	
	cout << " ********** DATA ( NMGSimulatorWidget::delFlowSlot() ) *********** " << endl;
	cout << qPrintable(doc.toString(1)) << endl << endl;
	cout << " ***************************************************************** " << endl << endl;
*/
}

/**
 * 
 */
void NMGSimulatorWidget::saveSettingsSlot()
{
	if ( flowList->topLevelItemCount() == 0 )
	{
		QMessageBox::critical ( 0, "Error", "There are no flows in the list \nIn order to save, you have to add flows." );
		return;
	}

	// 1) saving current flow settings in xml object before saving the file

	saveFlowSlot();
	
	// 2) selecting the file to be saved

	QString fileName = QFileDialog::getSaveFileName( this, tr( "Network Simulator: save flow settings" ), getLastTestPath(), tr( "XML (*.xml);;All files (*.*)" ));



	if ( fileName == 0 )
		return; //if there is no file chosen do nothing
	else
	{
		if(!fileName.contains( ".xml", Qt::CaseInsensitive )) fileName = fileName + ".xml";
		saveLastTestPathFromFilePath(fileName);
	}

	QFile file ( fileName );
 	QTextStream out ( &file );
	const int Indent = 4;

	router.setAttribute ( "nsAddress", nsAddress->text() );
	file.open ( QIODevice::WriteOnly );
	doc.save ( out, Indent );
}

void NMGSimulatorWidget::loadSettingsSlot()
{
	QString fileName;
	QTreeWidgetItem * flowItem;

//getting the file to be opened

 	fileName = QFileDialog::getOpenFileName( this, tr( "Select a configuration file" ), getLastTestPath() , tr( "XML (*.xml);;All files (*.*)" ) );


	if ( fileName == 0 )
		return; //if there is no file chosen do nothing
	else
	{
		saveLastTestPathFromFilePath(fileName);
	}
//inporting the settings
	//opening the xml file
	QFile file ( fileName );
	if ( !file.open ( QIODevice::ReadOnly ) )
	{
		QMessageBox::critical ( 0, "Error", "The file that you are trying to open is corrupt \nChose another file." );
		return;
	}
	// erasing the old settings

	flowList->clear();
/*
	doc.removeChild ( root );
*/
	root = doc.createElement ( "NetworkSimulatorSettings" );
	router = doc.createElement ( "router" );
	settings = doc.createElement ( "settings" );
	doc.appendChild ( root );
	root.appendChild ( router );
	root.appendChild ( settings );

	//creating the xml ducument object
	if ( !doc.setContent ( &file ) )
	{
		QMessageBox::critical ( 0, "Error", "The file that you are trying to open is corrupt \nChose another file." );
		return;
	}

	file.close();  //closing the xml file

	//getting the rest of the elements
	root = doc.documentElement();
	router = root.firstChild().toElement();
	settings = router.nextSibling().toElement();

	// writing the ns settings
	nsAddress->setAlignment ( Qt::AlignRight );
	nsStationObj = NMGHostInformationManager::self()->getStation ( router.attribute ( "nsAddress" ) );

	if ( nsStationObj == 0 )
	{
		cout << RED;
		cout << "[ERROR] There is no station with ip address " << qPrintable(router.attribute ( "nsAddress" ));
		cout << ENDCOLOR << endl;
		nsAddress->setText ( "0.0.0.0" );
		nsUser->setText ( " " );
		nsPort->setText ( "22" );
		nsAlias2->setText ( ". . ." );
	}
	else
	{
		nsHostObj = nsStationObj->getManagementHost();
		nsAddress->setText ( nsHostObj.getAddress() );

		nsUser->setAlignment ( Qt::AlignRight );
		nsUser->setText ( nsStationObj->getManagementUsername() );

		nsPort->setAlignment ( Qt::AlignRight );
		port.sprintf ( "%d", nsStationObj->getManagementAddressPort() ); //convert the integer value to string
		nsPort->setText ( port );

		nsHostObj = nsStationObj->getTestHost();
		nsAlias2->setText ( nsHostObj.getAlias() );
	}

	// Read the XML file:
	QDomNode node = settings.firstChild();

	while ( !settings.isNull() )
	{
		if ( node.isElement() && node.nodeName() == "Flow" )
		{
			flowItem = new QTreeWidgetItem ( flowList );
			flowItem->setText( 0, node.toElement().attribute ( "name" ) );
		}
		else break;

		node = node.nextSibling();
	}

	// Select an item:
	if( flowList->topLevelItemCount() != 0)
	{
		lastItem = 0;
		flowItem = flowList->topLevelItem( 0 );
		oldName = "Flow 0";
		flowList->setCurrentItem ( flowItem );
	}
}

void NMGSimulatorWidget::connectSlot()
{
	if ( nsAddress->text() == "0.0.0.0" )
	{
		QMessageBox::critical ( 0, "Error", "You have to choose a router to connect to." );
		return;
	}

	connectButton->setEnabled ( 0 );
	QString nsAddr = nsAddress->text();
	QString nsUsr = nsUser->text();
	QString nsPass = nsStationObj->getManagementPassword();
	QString nsPortValue = nsPort->text();
	QString nsProtocol;

	if ( nsHostObj.isHostIPv4() ) {nsProtocol = "IPv4";}
	else {nsProtocol = "IPv6";}

	QString xmlMessage = "<data> <nsAddress>" + nsAddr + "</nsAddress> <nsUsername>" + nsUsr + "</nsUsername> <nsPassword>" + nsPass + "</nsPassword> <nsPort>" + nsPortValue + "</nsPort> <nsProtocol>" + nsProtocol + "</nsProtocol> </data>";

	if ( sender->sendCommand ( Start, xmlMessage ) ) //sending the command
	{
		progressItem = 0;
		progressItem = ProgressManager::createProgressItem ( "Network Simulator Progress:" ); //creating the progressBar
		progressItem->setTotalItems ( 4 );
		progressItem->setStatus ( "Connecting to the host router ..." );
	}
	else
	{
		connectButton->setEnabled ( 1 );
		srcInterface->setEnabled ( 0 );
		pixmapLabel4->setEnabled ( 0 );
		destInterface->setEnabled ( 0 );
		applyButton->setEnabled ( 0 );
		saveButton->setEnabled ( 0 );
		loadButton->setEnabled ( 0 );
		srcInterface->clear();
		srcInterface->insertItem ( srcInterface->count(), "Any" );	// append
		destInterface->clear();
		destInterface->insertItem ( destInterface->count(), "Any" );	// append

// 		if(progressItem)	progressItem->cancel();
		MessageBar::longMessage ( "Command not started\nThere is no connection with the server!" );
	}
}

void NMGSimulatorWidget::applySettingsSlot()
{
	QString tag;
	int index, interfNumber;

	if ( flowList->topLevelItemCount() == 0 )
	{
		QMessageBox::critical ( 0, "Error", "There are no flows in the list, \nin order to apply settings, you need to create settings" );
		return;
	}

	QString nsAddr = nsAddress->text();
	QString nsUsr = nsUser->text();
	QString nsPass = nsStationObj->getManagementPassword();
	QString nsPortValue = nsPort->text();
	QString nsProtocol;

	if ( nsHostObj.isHostIPv4() ) {nsProtocol = "IPv4";}
	else {nsProtocol = "IPv6";}

	QString xmlMessage = "<data> <address>" + nsAddr + "</address> <nsUsername>" + nsUsr + "</nsUsername> <nsPassword>" + nsPass + "</nsPassword> <nsPort>" + nsPortValue + "</nsPort> <nsProtocol>" + nsProtocol + "</nsProtocol>";
	xmlMessage.append ( "<cleanInterf>" );//sending the interfaces for witch we have settings
	interfNumber = destInterface->count();

	for ( index = 1;index < interfNumber;index++ )
	{
		tag.sprintf ( "interface%d", index );
		xmlMessage.append ( "<" + tag + ">" );
		xmlMessage.append ( destInterface->itemText ( index ) );
		xmlMessage.append ( "</" + tag + ">" );
	}

	tag.sprintf ( "%d", interfNumber - 1 );
	xmlMessage.append ( "<interfaceNr>" + tag + "</interfaceNr>" );
	xmlMessage.append ( "</cleanInterf>" );
	xmlMessage.append ( "</data>" );//ending the data tree

	if ( sender->sendCommand ( Start, xmlMessage ) ) //sending the interfaces data
	{
		//starting the progres bar
		progressItem = 0;
		progressItem = ProgressManager::createProgressItem ( "Network Simulator Progress:" ); //creating the progressBar
		progressItem->setTotalItems ( flowList->topLevelItemCount() + 10 );
		progressItem->incCompletedItems();//incrementing the progressBar
		progressItem->updateProgress();
		progressItem->setStatus ( "Settings are being send ..." );

		//inactivating the buttons
		loadButton->setEnabled ( 0 );
		saveButton->setEnabled ( 0 );
		applyButton->setEnabled ( 0 );
		delFlowButton->setEnabled ( 0 );
		addFlowButton->setEnabled ( 0 );
	}
	else
	{
		connectButton->setEnabled ( 1 );
		srcInterface->setEnabled ( 0 );
		pixmapLabel4->setEnabled ( 0 );
		destInterface->setEnabled ( 0 );
		applyButton->setEnabled ( 0 );
		saveButton->setEnabled ( 0 );
		loadButton->setEnabled ( 0 );
		srcInterface->clear();
		srcInterface->insertItem ( srcInterface->count(), "Any" );		// append
		destInterface->clear();
		destInterface->insertItem ( destInterface->count(), "Any" );		// append

		MessageBar::longMessage ( "Settings not sent\nThere is no connection with the server!" );
	}
}

void NMGSimulatorWidget::sendSettingsSlot()
{
	bool ok, interfTable[20], exit;
	int index, i, interfNumber, outCheck, setNumber, val;
	QString tag, command, value, radioDelay, radioDD;
	QTreeWidgetItem * searchedItem;
	QDomNode node, source, destination;

	// saving current flow settings in xml object before saving the file

	QDomNode oldflow = settings.firstChild();
	outCheck = 0;
	
	// searching the item in the xml object
	while ( ( !settings.isNull() ) & ( outCheck == 0 ) )
	{
		if ( oldflow.toElement().attribute ( "name" ) != oldName )
		{
			oldflow = oldflow.nextSibling();
		}
		else { outCheck = 1;}
	}

	QDomElement newflow = doc.createElement ( "Flow" );
	QDomElement source1 = doc.createElement ( "Source" );
	QDomElement destination1 = doc.createElement ( "Destination" );
	QDomElement protocols1 = doc.createElement ( "Protocols" );
	QDomElement delayTab1 = doc.createElement ( "DelayTab" );
	QDomElement DDTab1 = doc.createElement ( "DDTab" );
	QDomElement rateTab1 = doc.createElement ( "RateTab" );

	settings.replaceChild ( newflow, oldflow );
	newflow.setAttribute ( "name", newName );

	newflow.appendChild ( source1 );	
	source1.setAttribute ( "srcAlias", srcAlias->text() );
	source1.setAttribute ( "srcAddress", srcAddress->text() );
	source1.setAttribute ( "srcMask", srcMask->text() );
	source1.setAttribute ( "srcPort", srcPort->text() );
	source1.setAttribute ( "srcInterface", srcInterface->currentIndex() );

	newflow.appendChild ( destination1 );
	destination1.setAttribute ( "destAlias", destAlias->text() );
	destination1.setAttribute ( "destAddress", destAddress->text() );
	destination1.setAttribute ( "destMask", destMask->text() );
	destination1.setAttribute ( "destPort", destPort->text() );
	destination1.setAttribute ( "destInterface", destInterface->currentIndex() );

	newflow.appendChild ( protocols1 );
	protocols1.setAttribute ( "l4Protocol", l4Protocol->currentIndex() );
	protocols1.setAttribute ( "l3Protocol", l3Protocol->currentIndex() );

	newflow.appendChild ( delayTab1 );
	delayTab1.setAttribute ( "delayDelay", delayDelay->text() );
	delayTab1.setAttribute ( "delayJitter", delayJitter->text() );

	radioDelay = returnDelayGroup();
	if ( radioDelay == "random" )
	{
		delayTab1.setAttribute ( "radioDelay", "random" );
		delayTab1.setAttribute ( "radioDelayValue", "random" );
	}
	else if ( radioDelay == "correlation" )
	{
		delayTab1.setAttribute ( "radioDelayValue", delayCorrelation->text() );
		delayTab1.setAttribute ( "radioDelay", "correlation" );
	}
	else
	{
		delayTab1.setAttribute ( "radioDelayValue", delayDistribution->currentIndex() );
		delayTab1.setAttribute ( "radioDelay", "distribution" );
	}

	newflow.appendChild ( DDTab1 );
	radioDD = returnDDGroup();
	if ( radioDD == "none" )
	{
		DDTab1.setAttribute ( "radioDD", "none" );
		DDTab1.setAttribute ( "DD", "none" );
		DDTab1.setAttribute ( "DDCorrelation", "none" );
	}
	else if ( radioDD == "drop" )
	{
		DDTab1.setAttribute ( "radioDD", "drop" );
		DDTab1.setAttribute ( "DD", DD->text() );
		DDTab1.setAttribute ( "DDCorrelation", DDCorrelation->text() );
	}
	else
	{
		DDTab1.setAttribute ( "radioDD", "duplicate" );
		DDTab1.setAttribute ( "DD", DD->text() );
		DDTab1.setAttribute ( "DDCorrelation", DDCorrelation->text() );
	}

	newflow.appendChild ( rateTab1 );
	rateTab1.setAttribute ( "rateRate", rateRate->text() );
	//rateTab1.setAttribute("rateMaxBurst",rateMaxBurst->text());
	rateTab1.setAttribute ( "rateBurst", rateBurst->text() );

	if ( newName != oldName ) //check if the item name was changed
	{
		QList<QTreeWidgetItem *> searchedItems = flowList->findItems ( oldName, Qt::MatchExactly );
		if ( searchedItem != 0 )
		{
			searchedItem = searchedItems.first();
			flowList->takeTopLevelItem ( flowList->indexOfTopLevelItem(searchedItem) );
			QTreeWidgetItem * newItem = new QTreeWidgetItem ( flowList );
			newItem->setText ( 0, newName );
			newItem->setSelected ( TRUE );
			flowList->setCurrentItem ( newItem );
			oldName = newItem->text ( 0 );
		}
	}

	//finish saving the settings

	QString nsAddr = nsAddress->text();
	QString nsUsr = nsUser->text();
	QString nsPass = nsStationObj->getManagementPassword();
	QString nsPortValue = nsPort->text();
	QString nsProtocol;

	if ( nsHostObj.isHostIPv4() ) {nsProtocol = "IPv4";}
	else {nsProtocol = "IPv6";}

	QString xmlMessage = "<data> <address>" + nsAddr + "</address> <nsUsername>" + nsUsr + "</nsUsername> <nsPassword>" + nsPass + "</nsPassword> <nsPort>" + nsPortValue + "</nsPort> <nsProtocol>" + nsProtocol + "</nsProtocol>";
	xmlMessage.append ( "<rootTc>" );//sending the interfaces for witch we have settings
	setNumber = flowList->topLevelItemCount();
	interfNumber = destInterface->count();

	
	node = settings.firstChild();
	for ( index = 0;index <= interfNumber;index++ )
	{
		interfTable[index] = FALSE;
	}

	index = 1;
	exit = FALSE;
	while ( ( index <= setNumber ) & ( exit == FALSE ) ) //searching for the used interfaces
	{
		source = node.firstChild();
		destination = source.nextSibling();

		value = destination.toElement().attribute ( "destInterface" );
		val = value.toInt ( &ok, 10 );

		if ( val == 0 )  //if we have a setting with the output interface Any
		{
			for ( i = 0;i < interfNumber;i++ )
				{interfTable[i] = TRUE;} //we consider used all the interfaces
			exit = TRUE;
		}
		else {interfTable[val] = TRUE;}

		node = node.nextSibling();//going to the next element
		index++;
	}

	i = 0;
	for ( index = 1;index < interfNumber;index++ )
	{
		if ( interfTable[index] )
		{
			i++;
			tag.sprintf ( "interface%d", i );
			xmlMessage.append ( "<" + tag + ">" );
			xmlMessage.append ( destInterface->itemText ( index ) );
			xmlMessage.append ( "</" + tag + ">" );

			tag.sprintf ( "clean%d", i );
			xmlMessage.append ( "<" + tag + ">" );
			if ( cleanInterface[index] == TRUE )
			{
				xmlMessage.append ( "YES" );
			}
			else
			{
				xmlMessage.append ( "NO" );
			}
			xmlMessage.append ( "</" + tag + ">" );
		}
	}

	tag.sprintf ( "%d", i );
	xmlMessage.append ( "<interfaceNr>" + tag + "</interfaceNr>" );

	xmlMessage.append ( "</rootTc>" );

	xmlMessage.append ( "</data>" );//ending the data tree

	if ( sender->sendCommand ( Start, xmlMessage ) ) //sending the interfaces data
	{
		cout << "****************************************************************" << endl;
	}
	else
	{
		connectButton->setEnabled ( 1 );
		srcInterface->setEnabled ( 0 );
		pixmapLabel4->setEnabled ( 0 );
		destInterface->setEnabled ( 0 );
		applyButton->setEnabled ( 0 );
		saveButton->setEnabled ( 0 );
		loadButton->setEnabled ( 0 );
		srcInterface->clear();
		srcInterface->insertItem ( srcInterface->count(), "Any" );		// append
		destInterface->clear();
		destInterface->insertItem ( destInterface->count(), "Any" );		// append

		MessageBar::longMessage ( "Settings not sent\nThere is no connection with the server!" );
	}
}

void NMGSimulatorWidget::nextSettingSlot ( int indexSet )
{
	bool ok;
	int i, j, val, offset, mark, hMajor, netemIndex, protocolNumber;

	QString  value, command, markString, sAddr, sMask, sPort, dAddr, dMask, dPort, l4Prot, iInterf, nextCommand, htbMajor, htbMinor, netemMajor, tag, result, responseDD;

	QDomNode node, source, destination, protocols, delayTab, DDTab, rateTab;

	cout << LIGHTBLUE <<"[INFO] Sending command nr: " << indexSet << ENDCOLOR << endl;

	//creating the xml message containing the settings
	QString nsAddr = nsAddress->text();
	QString nsUsr = nsUser->text();
	QString nsPass = nsStationObj->getManagementPassword();
	QString nsPortValue = nsPort->text();
	QString nsProtocol;

	if ( nsHostObj.isHostIPv4() ) {nsProtocol = "IPv4";}
	else {nsProtocol = "IPv6";}

	QString xmlMessage = "<data> <address>" + nsAddr + "</address> <nsUsername>" + nsUsr + "</nsUsername> <nsPassword>" + nsPass + "</nsPassword> <nsPort>" + nsPortValue + "</nsPort> <nsProtocol>" + nsProtocol + "</nsProtocol>";

	node = settings.firstChild();
	for ( i = 1;i < indexSet;i++ ) //going to the setting on the position indexSet
	{
		node = node.nextSibling();//going to the next element
	}

	source = node.firstChild();
	destination = source.nextSibling();
	protocols = destination.nextSibling();
	delayTab = protocols.nextSibling();
	DDTab = delayTab.nextSibling();
	rateTab = DDTab.nextSibling();

	offset = 1000;
	//*******************getting the iptables parameters
	sAddr = source.toElement().attribute ( "srcAddress" );
	sMask = source.toElement().attribute ( "srcMask" );
	sPort = source.toElement().attribute ( "srcPort" );
	value = source.toElement().attribute ( "srcInterface" );
	val = value.toInt ( &ok, 10 );
	iInterf = srcInterface->itemText ( val );

	dAddr = destination.toElement().attribute ( "destAddress" );
	dMask = destination.toElement().attribute ( "destMask" );
	dPort = destination.toElement().attribute ( "destPort" );

	value = protocols.toElement().attribute ( "l4Protocol" );
	val = value.toInt ( &ok, 10 );
	l4Prot = l4Protocol->itemText ( val );

	//Putting data in <iptablesBegin>
	command = " ";
	xmlMessage.append ( "<iptablesBegin>" );

	if (sAddr.compare("...")) command.append ( "-s " + sAddr + "/" + sMask );
	if (dAddr.compare("...")) command.append ( " -d " + dAddr + "/" + dMask );

	if (!sAddr.compare("...") && !dAddr.compare("..."))
	{
		cout << RED;	
		cout << "[ERROR] No source or destination selected for the settings" << ENDCOLOR << endl;
	} 

	if ( iInterf != "Any" )
	{
		command.append ( " -i " + iInterf );
	}

	xmlMessage.append ( command );
	xmlMessage.append ( "</iptablesBegin>" ); //ending the field <iptablesBegin>


	//Adding the protocols used
	protocolNumber = l4Protocol->count();
	if ( l4Prot == "All" )
	{
		for ( i = 1;i < protocolNumber;i++ )
		{
			tag.sprintf ( "protocol%d", i );
			xmlMessage.append ( "<" + tag + ">" );
			command = " ";
			command.append ( "-p " + l4Protocol->itemText ( i ) );
			if ( l4Protocol->itemText ( i ) != "ICMP" )
			{
				if ( sPort != "0" )
				{
					command.append ( " --sport " + sPort );
				}

				if ( dPort != "0" )
				{
					command.append ( " --dport " + dPort );
				}
			}
			xmlMessage.append ( command );
			xmlMessage.append ( "</" + tag + ">" );
		}
		xmlMessage.append ( "<protocolNr>3</protocolNr>" );
	}
	else
	{
		xmlMessage.append ( "<protocol1>" );
		command = " ";
		command.append ( "-p " + l4Prot );
		if ( l4Prot != "ICMP" )
		{
			if ( sPort != "0" )
			{
				command.append ( " --sport " + sPort );
			}

			if ( dPort != "0" )
			{
				command.append ( " --dport " + dPort );
			}
		}
		xmlMessage.append ( command );
		xmlMessage.append ( "</protocol1>" );
		xmlMessage.append ( "<protocolNr>1</protocolNr>" );
	}


	//Putting data in <iptablesEnd>
	command = " ";
	xmlMessage.append ( "<iptablesEnd>" );

	mark = offset + indexSet;
	markString.sprintf ( "%d", mark );
	command.append ( "-j MARK --set-mark " + markString );

	xmlMessage.append ( command );
	xmlMessage.append ( "</iptablesEnd>" );//ending the field <iptablesEnd>
	//ENd of getting the iptables parameters*****************


	/***********Getting the htbChild parameters*******************/
	xmlMessage.append ( "<htbChild>" );

	hMajor = 1;
	htbMajor.sprintf ( "%d", hMajor );
	htbMinor.sprintf ( "%d", indexSet );

	command = " ";
	command.append ( "parent " + htbMajor + ": " );
	command.append ( "classid " );
	command.append ( htbMajor + ":" + htbMinor + " htb rate " );
	//setting the rate and the ceil and eventualy burst and cburst
	if ( rateTab.toElement().attribute ( "rateRate" ) != "" )
	{
		command.append ( rateTab.toElement().attribute ( "rateRate" ) );
		command.append ( "kbit " );
		//command.append("ceil ");
		//command.append(rateTab.toElement().attribute("rateRate"));
		//command.append("kbit ");

		if ( rateTab.toElement().attribute ( "rateBurst" ) != "" )
		{
			command.append ( "burst " );
			command.append ( rateTab.toElement().attribute ( "rateBurst" ) );
			command.append ( "b " );
			//command.append("cburst ");
			//command.append(rateTab.toElement().attribute("rateBurst"));
			//command.append("b ");
		}
	}
	else {command.append ( "0.01kbit ceil 10000mbps " );}//if nothing is speciffied, we put a setting that will not affect the transmission, and we ignore the burst and cburst settings

	xmlMessage.append ( command );

	xmlMessage.append ( "</htbChild>" );
	/*****************htbChild********************/
	/*********************************************/

	/*****************Netem Qdiscs*********************/
	//making the netem qdiscs, first the drop qdisc, then the duplicate qdisc, and then the delay qdisc. This is the order, but it can be missing any of them
	netemIndex = 0;
	responseDD = DDTab.toElement().attribute ( "radioDD" );
	if ( responseDD != "none" ) //if we have drop or duplicate
	{
		netemIndex++;
		tag.sprintf ( "netem%d", netemIndex );
		xmlMessage.append ( "<" + tag + ">" );
		command = " ";
		command.append ( "parent " + htbMajor + ":" + htbMinor );

		netemMajor.sprintf ( "%d%d", indexSet, netemIndex );
		result = getDDSlot ( indexSet, netemMajor );

		command.append ( result );
		xmlMessage.append ( command );
		xmlMessage.append ( "</" + tag + ">" );

		if ( delayTab.toElement().attribute ( "delayDelay" ) != "" ) //if we have delay settigs
		{
			netemIndex++;
			tag.sprintf ( "netem%d", netemIndex );
			xmlMessage.append ( "<" + tag + ">" );
			command = " ";
			command.append ( "parent " + netemMajor + ":1" );

			netemMajor.sprintf ( "%d%d", indexSet, netemIndex );
			result = getDelaySlot ( indexSet, netemMajor );

			command.append ( result );
			xmlMessage.append ( command );
			xmlMessage.append ( "</" + tag + ">" );

		}

	}
	else 	 //if we don't have drop or duplicate
	{
		if ( delayTab.toElement().attribute ( "delayDelay" ) != "" ) //if we have delay settigs
		{
			netemIndex++;
			tag.sprintf ( "netem%d", netemIndex );
			xmlMessage.append ( "<" + tag + ">" );
			command = " ";
			command.append ( "parent " + htbMajor + ":" + htbMinor );

			netemMajor.sprintf ( "%d%d", indexSet, netemIndex );
			result = getDelaySlot ( indexSet, netemMajor );

			command.append ( result );
			xmlMessage.append ( command );
			xmlMessage.append ( "</" + tag + ">" );

		}

	}

	tag.sprintf ( "%d", netemIndex );
	xmlMessage.append ( "<netemNr>" + tag + "</netemNr>" );

	/*************************************************************/
	/*****************Putting the filter parrameters*************/

	xmlMessage.append ( "<filter>" );
	//tc filter add dev DEV...
	command = " ";
	command.append ( "parent " + htbMajor + ":" );
	command.append ( " protocol ip prio 1 handle " );
	command.append ( markString );
	command.append ( " fw classid " + htbMajor + ":" + htbMinor );

	xmlMessage.append ( command );
	xmlMessage.append ( "</filter>" );

	/*****************Ending the filter parrameters*************/
	/*************************************************************/

	/*********Sending the interfaces for witch this command is for******/
	xmlMessage.append ( "<interfaces>" );
	j = 0;
	value = destination.toElement().attribute ( "destInterface" );
	val = value.toInt ( &ok, 10 );
	if ( val == 0 )
	{
		j = destInterface->count();
		for ( i = 1;i < j;i++ )
		{
			tag.sprintf ( "interf%d", i );
			xmlMessage.append ( "<" + tag + ">" );
			xmlMessage.append ( destInterface->itemText ( i ) );
			xmlMessage.append ( "</" + tag + ">" );
		}
		tag.sprintf ( "%d", ( i - 1 ) );
		xmlMessage.append ( "<interfNr>" + tag + "</interfNr>" );
	}
	else
	{
		xmlMessage.append ( "<interf1>" + destInterface->itemText ( val ) + "</interf1>" );
		xmlMessage.append ( "<interfNr>1</interfNr>" );
	}

	xmlMessage.append ( "</interfaces>" );
	/*************************Interfaces********************************/
	/*******************************************************************/


	nextCommand = "";
	//sending the next command or NO if this was the last command
	i = indexSet + 1;
	if ( indexSet != flowList->topLevelItemCount() ) 	nextCommand.sprintf ( "%d", i );
	else 						nextCommand = "NO";

	xmlMessage.append ( "<nextCommandAvailable>" + nextCommand + "</nextCommandAvailable>" );
	xmlMessage.append ( "</data>" ); //ending the settings tree

	if ( sender->sendCommand ( Start, xmlMessage ) ) //sending the interfaces data
	{
		cout << "****************************************************************" << endl;
	}
	else
	{
		connectButton->setEnabled ( 1 );
		srcInterface->setEnabled ( 0 );
		pixmapLabel4->setEnabled ( 0 );
		destInterface->setEnabled ( 0 );
		applyButton->setEnabled ( 0 );
		saveButton->setEnabled ( 0 );
		loadButton->setEnabled ( 0 );
		srcInterface->clear();
		srcInterface->insertItem ( srcInterface->count(), "Any" );		// append
		destInterface->clear();
		destInterface->insertItem ( destInterface->count(), "Any" );		// append

// 		if(progressItem)	progressItem->cancel();
		MessageBar::longMessage ( "Settings not sent\nThere is no connection with the server!" );
	}
}

//getting datas from delay tab menu
QString NMGSimulatorWidget::getDelaySlot ( int currentIndex, QString netemMajor )
{
	bool ok;
	int i, val;
	QString command, value, response;
	QDomNode node, source, destination, protocols, delayTab;

	node = settings.firstChild();
	for ( i = 1;i < currentIndex;i++ ) //going to the setting on the position currentIndex
	{
		node = node.nextSibling();//going to the next element
	}

	source = node.firstChild();
	destination = source.nextSibling();
	protocols = destination.nextSibling();
	delayTab = protocols.nextSibling();

	command = "";
	command.append ( " handle " );
	command.append ( netemMajor );
	command.append ( ": netem delay " );
	command.append ( delayTab.toElement().attribute ( "delayDelay" ) + "ms " );

	/*******If we have jitter**************/
	//if we have jitter
	if ( delayTab.toElement().attribute ( "delayJitter" ) != "" )
	{
		command.append ( delayTab.toElement().attribute ( "delayJitter" ) );
		command.append ( "ms " );

		//if we have correlation or distribution value
		response = delayTab.toElement().attribute ( "radioDelay" );
		if ( response != "random" )
		{
			if ( response == "correlation" )
			{
				//if we have correlation
				command.append ( delayTab.toElement().attribute ( "radioDelayValue" ) );
				command.append ( "% " );
			}
			else  //if we have distribution
			{
				command.append ( "distribution " );
				value = delayTab.toElement().attribute ( "radioDelayValue" );
				val = value.toInt ( &ok, 10 );
				command.append ( delayDistribution->itemText ( val ) );
			}
		}
	}
	return command;
}

//getting data from drop tab menu
QString NMGSimulatorWidget::getDDSlot ( int currentIndex, QString netemMajor )
{
	int i;
	QString command, value;
	QDomNode node, source, destination, protocols, delayTab, DDTab;

	node = settings.firstChild();
	for ( i = 1;i < currentIndex;i++ ) //going to the setting on the position currentIndex
	{
		node = node.nextSibling();//going to the next element
	}

	source = node.firstChild();
	destination = source.nextSibling();
	protocols = destination.nextSibling();
	delayTab = protocols.nextSibling();
	DDTab = delayTab.nextSibling();

	command = "";
	command.append ( " handle " );
	command.append ( netemMajor );
	command.append ( ": netem " + DDTab.toElement().attribute ( "radioDD" ) + " " );
	command.append ( DDTab.toElement().attribute ( "DD" ) + "% " );

	/******************If we have correlation**********************/
	//if we have correlation value
	if ( DDTab.toElement().attribute ( "DDCorrelation" ) != "0" )
	{
		command.append ( DDTab.toElement().attribute ( "DDCorrelation" ) );
		command.append ( "% " );
	}
	return command;
}

void NMGSimulatorWidget::delayGroupChanged()
{
	delayCorrelation->setEnabled ( 0 );
	delayDistribution->setEnabled ( 0 );
	
	if ( dRadio1->isChecked() )
	{
		delayCorrelation->setValue ( 0 );
	}
	if ( dRadio2->isChecked() )
	{
		delayCorrelation->setEnabled ( 1 );
	}
	if ( dRadio3->isChecked() )
	{
		delayCorrelation->setValue ( 0 );
		delayDistribution->setEnabled ( 1 );
	}
}

void NMGSimulatorWidget::delayGroupChanged ( QString value )
{
	delayCorrelation->setEnabled ( 0 );
	delayDistribution->setEnabled ( 0 );

	if ( value == "random" )
	{
		dRadio1->setChecked ( TRUE );
		return;
	}
	else if ( value == "correlation" )
	{
		dRadio2->setChecked ( TRUE );
		delayCorrelation->setEnabled ( 1 );
		return;
	}
	else if ( value == "distribution" )
	{
		dRadio3->setChecked ( TRUE );
		delayDistribution->setEnabled ( 1 );
		return;
	}
}

void NMGSimulatorWidget::dDGroupChanged()
{
	DD->setEnabled ( 0 );
	DDCorrelation->setEnabled ( 0 );
	if ( DDR1->isChecked() )
	{
		DDAction->setText ( "None" );
		return;
	}
	else if ( DDR2->isChecked() )
	{
		DD->setEnabled ( 1 );
		DDCorrelation->setEnabled ( 1 );
		DDAction->setText ( "Drop" );
		return;
	}
	else if ( DDR3->isChecked() )
	{
		DD->setEnabled ( 1 );
		DDCorrelation->setEnabled ( 1 );
		DDAction->setText ( "Duplicate" );
		return;
	}
}

void NMGSimulatorWidget::dDGroupChanged ( QString value )
{
	DD->setEnabled ( 0 );
	DDCorrelation->setEnabled ( 0 );

	if ( value == "none" )
	{
		DDR1->setChecked ( TRUE );
		DDAction->setText ( "None" );
		return;
	}
	else if ( value == "drop" )
	{
		DDR2->setChecked ( TRUE );
		DD->setEnabled ( 1 );
		DDCorrelation->setEnabled ( 1 );
		DDAction->setText ( "Drop" );
		return;
	}
	else if ( value == "duplicate" )
	{
		DDR3->setChecked ( TRUE );
		DD->setEnabled ( 1 );
		DDCorrelation->setEnabled ( 1 );
		DDAction->setText ( "Duplicate" );
		return;
	}
}

QString NMGSimulatorWidget::returnDelayGroup()
{
	QString response;

	if ( dRadio1->isChecked() ) 		response = "random";
	else if ( dRadio2->isChecked() )		response = "correlation";
	else if ( dRadio3->isChecked() )		response = "distribution";

	return response;
}

QString NMGSimulatorWidget::returnDDGroup()
{
	QString response;

	if ( DDR1->isChecked() )		response = "none";
	else if ( DDR2->isChecked() )	response = "drop";
	else if ( DDR3->isChecked() )	response = "duplicate";

	return response;
}

void NMGSimulatorWidget::l4ProtocolChanged()
{
	srcPort->setEnabled ( 1 );
	destPort->setEnabled ( 1 );

	//int index = l4Protocol->findText("ICMP");

	if ( l4Protocol->currentText() == "ICMP" )
	{
		srcPort->setValue ( 0 );
		destPort->setValue ( 0 );
		srcPort->setEnabled ( 0 );
		destPort->setEnabled ( 0 );
	}
}

void NMGSimulatorWidget::l4ProtocolChanged ( QString value )
{
	srcPort->setEnabled ( 1 );
	destPort->setEnabled ( 1 );

	if ( value == "ICMP" )
	{
		srcPort->setValue ( 0 );
		destPort->setValue ( 0 );
		srcPort->setEnabled ( 0 );
		destPort->setEnabled ( 0 );
	}
}

/**
 * Returns the last test path (or "" if undefined)
 * @return 
 */
QString NMGSimulatorWidget::getLastTestPath()
{
	QString lastTestPath = preferencesManager->getValue( ID_PREFERENCE_LAST_TEST_PATH );
	if ( lastTestPath.isNull() or lastTestPath.isEmpty() ) lastTestPath = "";
	return lastTestPath;
}

/**
 * Saves the last test path from the filename path of the test
 * @param filePath 
 */
void NMGSimulatorWidget::saveLastTestPathFromFilePath(const QString & filePath)
{
	if(!filePath.isEmpty())
	{
		int lastPos = filePath.lastIndexOf("/");
		preferencesManager->addValue(ID_PREFERENCE_LAST_TEST_PATH,filePath.left(lastPos));
	}
}


#ifdef CMAKE_BS
#include "nmgsimulatorwidget.moc"
#endif
