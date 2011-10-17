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
#include <QToolBar>
#include <QWidget>
#include <QFileDialog>
#include <QTabWidget>
#include <QMessageBox>
#include <QAction>

#include <messagebar.h>

#include "nmgmgenwidget.h"
#include <nmgpreferencesmanager.h>
#include <nmgmoduletabwidgetmanager.h>
#include <nmgmoduleutils.h>
#include "nmgmgenflow.h"
#include "nmgmgenbandwith.h"

#include <iostream>
#include <cstdlib>

using namespace std;

#define IS_ITEM_MODIFICATION(item)	(item and ((NMGMGenFlowListItem*)item)->getFlowItemType()==FLOW_LIST_ITEM_MODIFICATION)
#define IS_ITEM_FLOW(item)		(item and ((NMGMGenFlowListItem *)item)->getFlowItemType()==FLOW_LIST_ITEM_FLOW)
#define IS_ITEM_EXT_HEADER(item)	(item and ((NMGMGenFlowListItem*)item)->getFlowItemType()==FLOW_LIST_ITEM_EXTENSION_HEADER)

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGMGenWidget::NMGMGenWidget ( NMGISender * s, NMGIModuleCommunication * mc, NMGIModuleStationManager * sm, NMGMGenFlowManager * fm, NMGModTabWidgetManager * tabWidgetMan, NMGModulePreferencesManager * prefMan, QWidget *parent, const char *name )
		: QWidget ( parent ), interpret ( this ), errorNotifier ( this )
{
	mainWidget = new Ui::NMGMGenMainWidget();
	mainWidget->setupUi ( this );

	lastSelectedItem = 0;
	lastFlowId = 0;
	progressItem = 0;
	toolBar = 0;
	sender = s;
	moduleCommunication = mc;
	stationManager = sm;
	tabWidgetManager = tabWidgetMan;
	flowManager = fm;
	preferencesManager = prefMan;
	idSubscribeStatistic = idSubscribeGraphic = lastResultFile = QString();
        lastResultFile = "/home/rbarreda/proves/resultats/test_owd_20081027_084627_mxrk.xff";
	updateTabWithItem ( 0 );

	mainWidget->flowList->setRootIsDecorated ( true );
	mainWidget->flowList->setColumnCount ( 1 );

	mainWidget->dateStartTime->setDate ( QDate().currentDate() );
	mainWidget->timeStartTime->setTime ( QTime().currentTime() );
	int randomSourcePort = rand() % 1000 + 5000;
	mainWidget->txtSourcePort->setValue ( randomSourcePort );

	connect ( this, SIGNAL ( activatedFlowOrMod ( QTreeWidgetItem * ) ), this, SLOT ( showItemProperties ( QTreeWidgetItem * ) ) );
	createFlowListMenu();
	connect ( mainWidget->flowList, SIGNAL ( itemSelectionChanged() ), this, SLOT ( flowListItemSelected() ) );
	connect ( mainWidget->txtFlowRate, SIGNAL ( valueChanged ( int ) ), this, SLOT ( updateEthernetAndATMBAndwith ( int ) ) );
	connect ( mainWidget->txtFlowPacketSize, SIGNAL ( valueChanged ( int ) ), this, SLOT ( updateEthernetAndATMBAndwith ( int ) ) );
	connect ( mainWidget->txtModRate, SIGNAL ( valueChanged ( int ) ), this, SLOT ( updateEthernetAndATMBAndwith ( int ) ) );
	connect ( mainWidget->txtModPacketSize, SIGNAL ( valueChanged ( int ) ), this, SLOT ( updateEthernetAndATMBAndwith ( int ) ) );
	connect ( mainWidget->buttonStart, SIGNAL ( clicked() ), this, SLOT ( buttonStartClicked() ) );
	connect ( mainWidget->buttonStop, SIGNAL ( clicked() ), this, SLOT ( buttonStopClicked() ) );
	connect ( mainWidget->buttonOreneta, SIGNAL ( clicked() ), this, SLOT ( buttonOrenetaClicked() ) );
	connect ( &progressTimer, SIGNAL ( timeout() ), this, SLOT ( incrementProgressItemBar() ) );
	connect ( mainWidget->comboMgenVersion, SIGNAL ( activated ( const QString & ) ), this, SLOT ( activeBurstPattern ( const QString & ) ) );
	connect ( this, SIGNAL ( activatedMgenVersion ( const QString & ) ), this, SLOT ( activeBurstPattern ( const QString & ) ) );
	connect ( mainWidget->checkBoxTypeOfService, SIGNAL ( toggled ( bool ) ), this, SLOT ( checkRootToS ( bool ) ) );
	connect ( mainWidget->checkSync , SIGNAL ( toggled ( bool ) ), this, SLOT ( checkBoxChanged ( bool ) ) );

	mainWidget->flowList->setContextMenuPolicy ( Qt::CustomContextMenu );
	connect ( mainWidget->flowList, SIGNAL ( customContextMenuRequested ( const QPoint & ) ), this, SLOT ( showMenu ( const QPoint & ) ) );

//	connect(,SIGNAL(stationChanged(NMGStation * )),this,SLOT(sourceStationChanged(NMGStation* )));
//	sourceStationChanged(hostSourceCombo->getStationFromCombo());
}

/**
 * Destructor of the class
 * @return
 */
NMGMGenWidget::~NMGMGenWidget()
{
	if ( progressItem ) progressItem->setComplete();
	progressItem = 0;
}

/**
 * Creates the flow list menu
 */
void NMGMGenWidget::createFlowListMenu()
{
	flowListMenu = new QMenu();
	flowListMenu->addAction ( "Add &new flow", this, SLOT ( addNewFlow() ), Qt::CTRL + Qt::Key_N );
	flowListMenu->addAction ( "&Delete item", this, SLOT ( deleteItem() ), Qt::CTRL + Qt::Key_D );
	flowListMenu->addAction ( "&Delete all items", this, SLOT ( deleteAllItems() ), Qt::SHIFT + Qt::CTRL + Qt::Key_D );
	flowListMenu->addAction ( "Add &modification", this, SLOT ( addModificationToFlow() ), Qt::CTRL + Qt::Key_M );
	flowListMenu->addSeparator();
	flowListMenu->addAction ( "&Load test from disk", this, SLOT ( loadTestFromDisk() ), Qt::CTRL + Qt::Key_L );
	flowListMenu->addAction ( "&Save test to disk", this, SLOT ( saveTestToDisk() ), Qt::CTRL + Qt::Key_S );
}

/**
 * Slot used to display the QMenu when user clicks inside the QTreeWidget
 */
void NMGMGenWidget::showMenu ( const QPoint & pos )
{
	if ( !flowListMenu )
		createFlowListMenu();
	flowListMenu->exec ( QCursor::pos() );
}

/**
 * Returns the toolbar of the mgen widget
 * @return
 */
QToolBar * NMGMGenWidget::getToolBar()
{
	QAction * action;

	if ( !toolBar )
	{
		toolBar = new QToolBar ( NMGModuleUtils::getMainWindow() );

		action = new QAction ( QIcon ( *NMGModuleUtils::getPixmap ( "fileopen.png" ) ), tr ( "Load test from disk" ), 0 );
		connect ( action, SIGNAL ( triggered() ), this, SLOT ( loadTestFromDisk() ) );
		toolBar->addAction ( action );

		action = new QAction ( QIcon ( *NMGModuleUtils::getPixmap ( "filesave.png" ) ), tr ( "Save test to disk" ), 0 );
		connect ( action, SIGNAL ( triggered() ), this, SLOT ( saveTestToDisk() ) );
		toolBar->addAction ( action );

		action = new QAction ( QIcon ( *NMGModuleUtils::getPixmap ( "add.png" ) ), tr ( "Add new flow" ), 0 );
		connect ( action, SIGNAL ( triggered() ), this, SLOT ( addNewFlow() ) );
		toolBar->addAction ( action );

		action = new QAction ( QIcon ( *NMGModuleUtils::getPixmap ( "out.png" ) ), tr ( "Add new Modification" ), 0 );
		connect ( action, SIGNAL ( triggered() ), this, SLOT ( addModificationToFlow() ) );
		toolBar->addAction ( action );

		action = new QAction ( QIcon ( *NMGModuleUtils::getPixmap ( "remove-red.png" ) ), tr ( "Delete item" ), 0 );
		connect ( action, SIGNAL ( triggered() ), this, SLOT ( deleteItem() ) );
		toolBar->addAction ( action );

		action = new QAction ( QIcon ( *NMGModuleUtils::getPixmap ( "edittrash.png" ) ), tr ( "Delete all items" ), 0 );
		connect ( action, SIGNAL ( triggered() ), this, SLOT ( deleteAllItems() ) );
		toolBar->addAction ( action );

		action = new QAction ( QIcon ( *NMGModuleUtils::getPixmap ( "launch.png" ) ), tr ( "Start test" ), 0 );
		connect ( action, SIGNAL ( triggered() ), this, SLOT ( buttonStartClicked() ) );
		toolBar->addAction ( action );

		toolBar->addSeparator ();
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "statistic.png" ) ),
		                     tr ( "Send result to statistic" ), this, SLOT ( buttonStatisticClicked() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "graphicon.png" ) ),
		                     tr ( "Send result to graphic" ), this, SLOT ( buttonGraphicClicked() ) );
	}
	return toolBar;
}

/**
 * Slots whichs adds a new flow
 */
void NMGMGenWidget::addNewFlow()
{
	updateTabContentToItem ( getSelectedItem() );

	NMGMGenFlow * flow = new NMGMGenFlow ( flowManager, mainWidget->flowList );
	flow->setId ( 1 + lastFlowId++ );
	updateFlowListIdentifier();

	mainWidget->flowList->setCurrentItem ( flow->getListViewItem() );

	if ( !lastSelectedItem ) emit activatedFlowOrMod ( flow->getListViewItem() );
	lastSelectedItem = flow->getListViewItem();
}

/**
 * Returns the selected item of the flow list (or 0)
 * @return
 */
QTreeWidgetItem * NMGMGenWidget::getSelectedItem()
{
	return mainWidget->flowList->currentItem();
}

/**
 * Slots which deletes the selected item (if any)
 */
void NMGMGenWidget::deleteItem()
{
	QTreeWidgetItem * item = getSelectedItem();
	if ( item )	deleteItem ( item );
}

/**
 * Deletes the item of the list knowing that it's not empty (checked before)
 * @param item
 */
void NMGMGenWidget::deleteItem ( QTreeWidgetItem * item )
{
	QTreeWidgetItem *parent = item->parent();
	QTreeWidgetItem * firstItem;
	int index;

	if ( mainWidget->flowList->topLevelItemCount() >= 1 )
		firstItem = mainWidget->flowList->topLevelItem ( 0 );
	if ( firstItem == item )
		firstItem = mainWidget->flowList->topLevelItem ( 1 );

	if ( IS_ITEM_FLOW ( item ) )
	{
		// Delete flow modifications (if any)

		if ( item->childCount() > 0 )
			item->takeChildren();

		// Final step: Delete flow

		if ( parent )
		{
			index = parent->indexOfChild ( mainWidget->flowList->currentItem() );
			delete parent->takeChild ( index );
		}

		else
		{
			index = mainWidget->flowList->indexOfTopLevelItem ( mainWidget->flowList->currentItem() );
			delete mainWidget->flowList->takeTopLevelItem ( index );
		}

		lastSelectedItem = 0;
		updateFlowListIdentifier();

	}

	else if ( IS_ITEM_MODIFICATION ( item ) or IS_ITEM_EXT_HEADER ( item ) )
	{
		// Delete Mod:

		index = parent->indexOfChild ( mainWidget->flowList->currentItem() );
		delete parent->takeChild ( index );
		lastSelectedItem = 0;
		updateFlowListIdentifier();
	}

	else
	{
		QMessageBox::warning ( 0, "No item selected", "You have to select something to delete" );
		return;
	}

	if ( firstItem )
	{
		lastSelectedItem = firstItem;
		mainWidget->flowList->setCurrentItem ( firstItem );
		firstItem->setSelected ( true );
	}
	else emit activatedFlowOrMod ( 0 );
}

/**
 * Slot which deletes all the items of the test
 */
void NMGMGenWidget::deleteAllItems()
{
	deleteAllItemsWithResult();
}

/**
 * Deletes all the items
 * @return Returns true if all the items where deleted
 */
bool NMGMGenWidget::deleteAllItemsWithResult()
{
	if ( mainWidget->flowList->topLevelItemCount() == 0 ) return true;

	int reply = QMessageBox::QMessageBox::question ( 0, "Confirm operation",
	            "Do you really want to delete all the items?", QMessageBox::Yes | QMessageBox::Default,
	            QMessageBox::No | QMessageBox::Escape );

	if ( reply == QMessageBox::No ) return false;
	else
	{
		// Delete All Items:
		mainWidget->flowList->clear();
		showItemProperties ( 0 );
		lastSelectedItem = 0;

		return true;
	}
}

/**
 * Slots which adds a modification to an existing flow
 */
void NMGMGenWidget::addModificationToFlow()
{
	updateTabContentToItem ( getSelectedItem() );
	QTreeWidgetItem * item = getSelectedItem();

	if ( not IS_ITEM_FLOW ( item ) and not IS_ITEM_MODIFICATION ( item ) and not IS_ITEM_EXT_HEADER ( item ) )
		QMessageBox::warning ( 0, "No flow selected", "You can only add modifications to a flow" );
	else
	{
		NMGMGenFlow * flow;
		if ( IS_ITEM_MODIFICATION ( item ) or IS_ITEM_EXT_HEADER ( item ) )
			flow = ( NMGMGenFlow * ) item->parent();
		else
			flow = ( NMGMGenFlow * ) item;

		NMGMGenFlowModification * mod = new NMGMGenFlowModification ( flow );
		item->setExpanded ( true );
		mainWidget->flowList->setCurrentItem ( mod->getListViewItem() );
	}
}

/**
 * Slot which load a test from disk
 */
void NMGMGenWidget::loadTestFromDisk()
{
	QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Select test file" ), getLastTestPath(), tr ( "XML Flow Format Configuration File (*.xfc);;XML Flow Format Result File (*.xff);;XML (*.xml);;All files (*.*)" ) );

	if ( !fileName.isEmpty() )
	{
		saveLastTestPathFromFilePath ( fileName );

		if ( not deleteAllItemsWithResult() ) return;

		if ( !interpret.updateWidgetFromXMLDocument ( fileName ) )
			QMessageBox::warning ( 0, "Error while reading file", "The file is empty or not well-formed" );
		else
		{
			lastSelectedItem = 0;
			updateFlowListIdentifier();
			emit activatedMgenVersion ( mainWidget->comboMgenVersion->currentText() );

			if ( mainWidget->flowList->topLevelItemCount() >= 1 )
				mainWidget->flowList->setCurrentItem ( mainWidget->flowList->topLevelItem ( 0 ) );
		}
	}
}

/**
 * Slot which saves flows to disk
 */
void NMGMGenWidget::saveTestToDisk()
{
	updateTabContentToItem ( getSelectedItem() );
	if ( mainWidget->flowList->topLevelItemCount() == 0 ) QMessageBox::warning ( 0, "No flows", "There are no flows on the list." );
	else
	{
		if ( mainWidget->hostSourceCombo->getStationFromCombo() == 0 )
			QMessageBox::warning ( 0, "No Source Host", "Source host is not defined." );
		else if ( mainWidget->hostDestinationCombo->getStationFromCombo() == 0 )
			QMessageBox::warning ( 0, "No Destination Host", "Destination host is not defined." );
		else
		{
			QString testContent = interpret.generateXMLTestDocument ( false );

			QString fileName = QFileDialog::getSaveFileName ( this, tr ( "File to save the flows" ),
			                   getLastTestPath(), tr ( "XML Flow Format Configuration File (*.xfc);;XML Flow Format Result File (*.xff);;XML (*.xml);;All files (*.*)" ) );

			if ( !fileName.isEmpty() )
			{
				if ( !fileName.contains ( ".xfc", Qt::CaseInsensitive ) ) fileName = fileName + ".xfc";

				saveLastTestPathFromFilePath ( fileName );
				NMGXMLConfigurationFile file;
				file.setPath ( fileName );
				file.getDomDocument().setContent ( testContent );
				if ( !file.writeToDisk() )
					cerr << "Error while writing the file to disk(" << qPrintable ( file.getError() ) << ").";
			}
		}
	}
}

/**
 * Slot executed when the user clicks with the right button on the flow list
 * @param item Item clicked (0 if there is no such item)
 * @param point Position of the mouse
 * @param
 */
void NMGMGenWidget::flowListRightButtonPressed ( QTreeWidgetItem * , const QPoint & point, int )
{
	flowListMenu->popup ( point );
}


/**
 * Slot executed when an item is selected
 * @param item Item selected (if any)
 */
void NMGMGenWidget::flowListItemSelected()
{
	QTreeWidgetItem * item = mainWidget->flowList->currentItem();

	if ( lastSelectedItem != item and mainWidget->flowList->topLevelItemCount() > 1 )
		updateTabContentToItem ( lastSelectedItem );

	showItemProperties ( item );
	lastSelectedItem = item;
}

/**
 * Shows the information of the flow
 * @param flow
 */
void NMGMGenWidget::showItemProperties ( QTreeWidgetItem * item )
{
	int index;

	if ( !item ) return;

	updateTabWithItem ( item );

	if ( IS_ITEM_FLOW ( item ) )
	{
		NMGMGenFlow *flow = ( NMGMGenFlow * ) item;
		mainWidget->txtFlowName->setText ( flow->getName() );
		mainWidget->txtFlowStart->setValue ( flow->getStartTime() );
		mainWidget->txtFlowDuration->setValue ( flow->getDuration() );
		mainWidget->txtFlowDestinationPort->setValue ( flow->getPort() );
		index = mainWidget->comboFlowPattern->findText ( flow->getPattern() );
		mainWidget->comboFlowPattern->setCurrentIndex ( index );
		mainWidget->txtFlowRate->setValue ( flow->getRate() );
		mainWidget->txtFlowPacketSize->setValue ( flow->getPacketSize() );

		if ( flow->isActiveBurst() && mainWidget->comboMgenVersion->currentText() == "4" )
		{
			mainWidget->checkBoxPatternBurst->setChecked ( true );

			index = mainWidget->comboInterval->findText ( flow->getBurstIntervalTxt() );
			mainWidget->comboInterval->setItemText ( index, flow->getBurstIntervalTxt() );
			mainWidget->txtInterval->setValue ( flow->getBurstInterval() );

			index = mainWidget->comboDuration->findText ( flow->getBurstDurationTxt() );
			mainWidget->comboDuration->setItemText ( index, flow->getBurstDurationTxt() );
			mainWidget->txtDuration->setValue ( flow->getBurstDuration() );
		}
		else
		{
			mainWidget->checkBoxPatternBurst->setChecked ( false );

			index = mainWidget->comboInterval->findText ( DEFAULT_BURST_INTERVAL );
			mainWidget->comboInterval->setItemText ( index, DEFAULT_BURST_INTERVAL );
			mainWidget->txtInterval->setValue ( 0 );

			index = mainWidget->comboDuration->findText ( DEFAULT_BURST_DURATION );
			mainWidget->comboDuration->setItemText ( index, DEFAULT_BURST_DURATION );
			mainWidget->txtDuration->setValue ( 0 );
		}
		if ( flow->getTypeOfService() < 0 )
		{
			mainWidget->checkBoxTypeOfService->setChecked ( false );
			mainWidget->txtTypeOfService->setText ( "" );
		}
		else
		{
			mainWidget->checkBoxTypeOfService->setChecked ( true );
			mainWidget->txtTypeOfService->setText ( QString ( "%1" ).arg ( flow->getTypeOfService() ) );
		}
		if ( flow->getRSVP().isActive() )
		{
			mainWidget->checkBoxRSVPParameters->setChecked ( true );
			mainWidget->txtRSVPParameters->setText ( flow->getRSVP().getSourceParam() );

			if ( flow->getRSVP().getResvStyleAsShortString() == "WF" )
				mainWidget->radioRSVPWF->setChecked ( true );

			else if ( flow->getRSVP().getResvStyleAsShortString() == "FF" )
				mainWidget->radioRSVPFF->setChecked ( true );

			else
				mainWidget->radioRSVPSE->setChecked ( true );
			mainWidget->txtRESVParameters->setText ( flow->getRSVP().getDestParam() );
		}
		else
		{
			mainWidget->checkBoxRSVPParameters->setChecked ( false );
			mainWidget->txtRSVPParameters->setText ( "" );
			mainWidget->txtRESVParameters->setText ( "" );
			mainWidget->radioRSVPWF->setChecked ( true );
		}
		if ( flow->getTrafficClass() < 0 )
		{
			mainWidget->checkBoxTrafficClass->setChecked ( false );
			mainWidget->txtTrafficClass->setText ( "" );
		}
		else
		{
			mainWidget->checkBoxTrafficClass->setChecked ( true );
			mainWidget->txtTrafficClass->setText ( QString ( "%1" ).arg ( flow->getTrafficClass() ) );
		}
	}
	else if ( IS_ITEM_MODIFICATION ( item ) )
	{
		NMGMGenFlowModification *mod = ( NMGMGenFlowModification * ) item;
		mainWidget->txtModStart->setValue ( mod->getStartTime() );
		mainWidget->txtModRate->setValue ( mod->getRate() );
		mainWidget->txtModPacketSize->setValue ( mod->getPacketSize() );

		index = mainWidget->comboModPattern->findText ( mod->getPattern() );
		mainWidget->comboModPattern->setCurrentIndex ( index );

		if ( mod->isActiveBurst() && mainWidget->comboMgenVersion->currentText() == "4" )
		{
			mainWidget->checkBoxPatternBurstMod->setChecked ( true );

			index = mainWidget->comboIntervalMod->findText ( mod->getBurstIntervalTxt() );
			mainWidget->comboIntervalMod->setItemText ( index, mod->getBurstIntervalTxt() );
			mainWidget->txtIntervalMod->setValue ( mod->getBurstInterval() );

			index = mainWidget->comboDurationMod->findText ( mod->getBurstDurationTxt() );
			mainWidget->comboDurationMod->setItemText ( index, mod->getBurstDurationTxt() );
			mainWidget->txtDurationMod->setValue ( mod->getBurstDuration() );
		}
		else
		{
			mainWidget->checkBoxPatternBurstMod->setChecked ( false );

			index = mainWidget->comboIntervalMod->findText ( DEFAULT_BURST_INTERVAL );
			mainWidget->comboIntervalMod->setItemText ( index, DEFAULT_BURST_INTERVAL );
			mainWidget->txtIntervalMod->setValue ( 0 );

			index = mainWidget->comboDurationMod->findText ( DEFAULT_BURST_DURATION );
			mainWidget->comboDurationMod->setItemText ( index, DEFAULT_BURST_DURATION );
			mainWidget->txtDurationMod->setValue ( 0 );
		}
		if ( mod->getTypeOfService() < 0 )
		{
			mainWidget->checkBoxTypeOfService->setChecked ( false );
			mainWidget->txtTypeOfService->setText ( "" );
		}
		else
		{
			mainWidget->checkBoxTypeOfService->setChecked ( true );
			mainWidget->txtTypeOfService->setText ( QString ( "%1" ).arg ( mod->getTypeOfService() ) );
		}
		if ( mod->getRSVP().isActive() )
		{
			mainWidget->checkBoxRSVPParameters->setChecked ( true );
			mainWidget->txtRSVPParameters->setText ( mod->getRSVP().getSourceParam() );

			if ( mod->getRSVP().getResvStyleAsShortString() == "WF" )
				mainWidget->radioRSVPWF->setChecked ( true );

			else if ( mod->getRSVP().getResvStyleAsShortString() == "FF" )
				mainWidget->radioRSVPFF->setChecked ( true );

			else
				mainWidget->radioRSVPSE->setChecked ( true );

			mainWidget->txtRESVParameters->setText ( mod->getRSVP().getDestParam() );
		}
		else
		{
			mainWidget->checkBoxRSVPParameters->setChecked ( false );
			mainWidget->txtRSVPParameters->setText ( "" );
			mainWidget->txtRESVParameters->setText ( "" );
			mainWidget->radioRSVPWF->setChecked ( true );
		}
		if ( mod->getTrafficClass() < 0 )
		{
			mainWidget->checkBoxTrafficClass->setChecked ( false );
			mainWidget->txtTrafficClass->setText ( "" );
		}
		else
		{
			mainWidget->checkBoxTrafficClass->setChecked ( true );
			mainWidget->txtTrafficClass->setText ( QString ( "%1" ).arg ( mod->getTrafficClass() ) );
		}
	}
	else if ( IS_ITEM_EXT_HEADER ( item ) )
	{
		NMGMGenFlowExtensionHeader * header = ( NMGMGenFlowExtensionHeader * ) item;
		mainWidget->txtExtHeaderStartTime->setValue ( header->getOptionStartTime() );
		mainWidget->txtExtHeaderStopTime->setValue ( header->getOptionStopTime() );

		index = mainWidget->comboBoxOptionHeader->findText ( header->getExtensionHeaderTypeAsString() );
		mainWidget->comboBoxOptionHeader->setItemText ( index, header->getExtensionHeaderTypeAsString() );

		mainWidget->txtExtHeaderOptionalParameter->setText ( header->getOptionalParameter() );
		/* labelExtHeaderOptionalParameter */
	}
}

/**
 * Updates the tab widget flow with a new flow
 * @param flow
 */
void NMGMGenWidget::updateTabWithItem ( QTreeWidgetItem * item )
{
	if ( IS_ITEM_FLOW ( item ) )
	{
		mainWidget->tabFlowProperties->setEnabled ( true );
		removeAllTabFlowPages();
		mainWidget->tabFlowProperties->addTab ( mainWidget->pageFlowProperties, TAB_FLOW_PROPERTIES );

		if ( isProtocolIPv4() )
			mainWidget->tabFlowProperties->addTab ( mainWidget->tabPageIPV4Options, TAB_IPV4_OPTIONS );
		else
			mainWidget->tabFlowProperties->addTab ( mainWidget->tabPageIPV6Options, TAB_IPV6_OPTIONS );

		mainWidget->tabFlowProperties->setCurrentIndex ( mainWidget->tabFlowProperties->indexOf ( mainWidget->pageFlowProperties ) );
	}

	else if ( IS_ITEM_MODIFICATION ( item ) )
	{
		mainWidget->tabFlowProperties->setEnabled ( true );
		removeAllTabFlowPages();
		mainWidget->tabFlowProperties->addTab ( mainWidget->pageModificationProperties, TAB_MOD_FLOW_PROPERTIES );


		if ( isProtocolIPv4() )
			mainWidget->tabFlowProperties->addTab ( mainWidget->tabPageIPV4Options, TAB_IPV4_OPTIONS );
		else
			mainWidget->tabFlowProperties->addTab ( mainWidget->tabPageIPV6Options, TAB_IPV6_OPTIONS );

		mainWidget->tabFlowProperties->setCurrentIndex ( mainWidget->tabFlowProperties->indexOf ( mainWidget->pageModificationProperties ) );
	}

	else if ( IS_ITEM_EXT_HEADER ( item ) )
	{
		removeAllTabFlowPages();
		mainWidget->tabFlowProperties->addTab ( mainWidget->tabPageIPv6ExtensionsHeaders, TAB_IPV6_EXT_HEADERS );
		mainWidget->tabFlowProperties->setCurrentIndex ( mainWidget->tabFlowProperties->indexOf ( mainWidget->tabPageIPv6ExtensionsHeaders ) );
	}
	else mainWidget->tabFlowProperties->setEnabled ( false );
}

/**
 * Removes the tab pages which are present in the tabFlowProperties
 */
void NMGMGenWidget::removeAllTabFlowPages()
{
	mainWidget->tabFlowProperties->clear();
}

/**
 * Updates the item (flow or modification from the existing data in the tab widget)
 * @param item Item to update
 */
void NMGMGenWidget::updateTabContentToItem ( QTreeWidgetItem * item )
{
//	if(mainWidget->flowList->topLevelItemCount()>=1 and !item)
//		item = mainWidget->flowList->currentItem();

	if ( !item ) return;

	if ( item and ( ( NMGMGenFlowListItem* ) item )->getFlowItemType() == FLOW_LIST_ITEM_MODIFICATION )
	{
		// item is a flow modification
		NMGMGenFlowModification * mod = ( NMGMGenFlowModification * ) item;
		mod->setStartTime ( mainWidget->txtModStart->value() );
		mod->setRate ( mainWidget->txtModRate->value() );
		mod->setPacketSize ( mainWidget->txtModPacketSize->value() );
		mod->setPattern ( mainWidget->comboModPattern->currentText() );

		if ( mainWidget->checkBoxTypeOfService->isChecked() )
			mod->setTypeOfService ( mainWidget->txtTypeOfService->text().toInt ( 0, 0 ) );
		else mod->setTypeOfService ( -1 );

		if ( mainWidget->checkBoxPatternBurstMod->isChecked() && mainWidget->comboMgenVersion->currentText() == "4" )
		{
			mod->setBurstInterval ( mainWidget->comboIntervalMod->currentText(), mainWidget->txtIntervalMod->value() );
			mod->setBurstDuration ( mainWidget->comboDurationMod->currentText(), mainWidget->txtDurationMod->value() );
			mod->setBurst ( true );
		}
		else
		{
			mod->setBurstInterval ( DEFAULT_BURST_INTERVAL, 0 );
			mod->setBurstDuration ( DEFAULT_BURST_DURATION, 0 );
			mod->setBurst ( false );
		}

		if ( mainWidget->checkBoxRSVPParameters->isChecked() )
		{
			mod->getRSVP().setActive ( true );
			mod->getRSVP().setSourceParam ( mainWidget->txtRSVPParameters->text() );
			mod->getRSVP().setResvStyleAsShortString ( mainWidget->radioRSVPWF->isChecked() ? "WF" : mainWidget->radioRSVPFF->isChecked() ? "FF" : "SE" );
			mod->getRSVP().setDestParam ( mainWidget->txtRESVParameters->text() );
		}
		else mod->getRSVP().setActive ( false );

		if ( mainWidget->checkBoxTrafficClass->isChecked() ) mod->setTrafficClass ( mainWidget->txtTrafficClass->text().toInt() );
		else mod->setTrafficClass ( -1 );
	}

	else if ( item and ( ( NMGMGenFlowListItem * ) item )->getFlowItemType() == FLOW_LIST_ITEM_FLOW )
	{
		NMGMGenFlow * flow = ( NMGMGenFlow * ) item;
		flow->setName ( mainWidget->txtFlowName->text() );
		flow->setStartTime ( mainWidget->txtFlowStart->value() );
		flow->setDuration ( mainWidget->txtFlowDuration->value() );
		flow->setPattern ( mainWidget->comboFlowPattern->currentText() );
		flow->setRate ( mainWidget->txtFlowRate->value() );
		flow->setPacketSize ( mainWidget->txtFlowPacketSize->value() );
		if ( flow->getPort() != mainWidget->txtFlowDestinationPort->value() )
		{
			if ( not flow->getFlowManager()->isPortAvailable ( mainWidget->txtFlowDestinationPort->value() ) )
			{
				QMessageBox::warning ( 0, "Port used", "The port " + QString ( "%1" ).arg ( mainWidget->txtFlowDestinationPort->value() ) +
				                       " is already used." );
			}
			else flow->setPort ( mainWidget->txtFlowDestinationPort->value() );
		}

		if ( mainWidget->checkBoxPatternBurst->isChecked() && mainWidget->comboMgenVersion->currentText() == "4" )
		{
			flow->setBurstInterval ( mainWidget->comboInterval->currentText(), mainWidget->txtInterval->value() );
			flow->setBurstDuration ( mainWidget->comboDuration->currentText(), mainWidget->txtDuration->value() );
			flow->setBurst ( true );
		}

		else
		{
			flow->setBurstInterval ( DEFAULT_BURST_INTERVAL, 0 );
			flow->setBurstDuration ( DEFAULT_BURST_DURATION, 0 );
			flow->setBurst ( false );
		}

		if ( mainWidget->checkBoxTypeOfService->isChecked() )
			flow->setTypeOfService ( mainWidget->txtTypeOfService->text().toInt ( 0, 0 ) );

		else
			flow->setTypeOfService ( -1 );

		if ( mainWidget->checkBoxRSVPParameters->isChecked() )
		{
			flow->getRSVP().setActive ( true );
			flow->getRSVP().setSourceParam ( mainWidget->txtRSVPParameters->text() );
			flow->getRSVP().setResvStyleAsShortString ( mainWidget->radioRSVPWF->isChecked() ? "WF" : mainWidget->radioRSVPFF->isChecked() ? "FF" : "SE" );
			flow->getRSVP().setDestParam ( mainWidget->txtRESVParameters->text() );
		}
		else
			flow->getRSVP().setActive ( false );

		if ( mainWidget->checkBoxTrafficClass->isChecked() ) flow->setTrafficClass ( mainWidget->txtTrafficClass->text().toInt() );
		else flow->setTrafficClass ( -1 );
	}
	else if ( IS_ITEM_EXT_HEADER ( item ) )
		{ /* TODO */ }
}

/**
 * Slot executed when the user clicks the start button to start a new test
 */
void NMGMGenWidget::buttonStartClicked()
{
	updateFlowListIdentifier();
	updateTabContentToItem ( getSelectedItem() );
	progressItem = 0;

	// 1) Check that all the info is correct:

	if ( not allFieldsNeededCorrect() )
	{
		MessageBar::longMessage ( "You must enter all the info before sending the start command" );
		return;
	}

	// 2) Generate XML doc:

	QString xmlTest = interpret.generateXMLTestDocument ( true );
	QString title = mainWidget->txtTestName->text() == "" ? "One way delay" : mainWidget->txtTestName->text();
	tabWidgetManager->setTabImage ( "run.png" );

	// 3) SYNC send Start Command (if checkbox is checked):

	if ( syncActivated )
	{
		notifyStatus ( "<data><status>0</status></data>" );	// update progressbar ("test started");

		xmlSync = interpret.generateXMLSync();
		idSync = moduleCommunication->subscribe ( "NMGSync", xmlSync );

		if ( !idSync.isEmpty() )	
			cout << GREEN <<"[OK] Subscripted to NMGSync events" << ENDCOLOR << endl;
		else			
			cout << RED <<"[ERROR] Failed to subscript to NMGSync events" << ENDCOLOR <<endl;

		mainWidget->buttonStop->setEnabled ( true );
		mainWidget->buttonStart->setEnabled ( false );

		progressItem = ProgressManager::createProgressItem ( title );
		progressItem->setTotalItems ( 100 );
		connect ( progressItem, SIGNAL ( progressItemCanceled ( ProgressItem* ) ), this, SLOT ( testCancelled ( ProgressItem * ) ) );

		notifyStatus ( "<data><status>5</status></data>" );	// update progressbar ("Sync started");
		sender->sendGUICommand ( Start, xmlSync );			// START SYNC
		sleep ( 5 );						// wait untill sync core is ready

		cout << LIGHTBLUE;
		cout << "NMGMgen --- NMSync::READY -> sending start to mgen" << endl;
		cout << ENDCOLOR;
	}
	else
	{
		progressItem = ProgressManager::createProgressItem ( title );
		progressItem->setTotalItems ( 100 );
		connect ( progressItem, SIGNAL ( progressItemCanceled ( ProgressItem* ) ), this, SLOT ( testCancelled ( ProgressItem * ) ) );
	}

	// 4) MGEN send Start Command:

	if ( sender->sendCommand ( Start, xmlTest ) )
	{
		mainWidget->buttonStop->setEnabled ( true );
		mainWidget->buttonStart->setEnabled ( false );
		maxTestTime = getMaximumFlowEndTime();
		progressItem->setStatus ( "Preparing test..." );
		finishedMgen = false;
	}
	else
	{
		progressItem->cancel();
		MessageBar::longMessage ( "Command not started\nThere is no connection with the server!" );
	}

	xmlSync = "";
}

/**
 *  Checkbox to request for sync data
*/
void NMGMGenWidget::checkBoxChanged ( bool on )
{
	if ( on )
	{
		syncActivated = true;
		if ( moduleCommunication->existsModule ( "NMGSync" ) )
			cout << "--- NMGMgen <-> NMGSync --- : ";
		else
			MessageBar::longMessage ( "NMGSync not found\nPlease load the module!" );
	}
	else
	{
		syncActivated = false;

		if ( idSync != "" )	
			moduleCommunication->endSubscribe ( idSync );
		else			
			cout << RED << "[ERROR] Failed to unsubscribe to NMGSync events" << endl << ENDCOLOR;
	}
}

/**
 * Checks if all the fields are correct to send a start message to the server
 * If they are not correct a message box is shown to the server
 * @return
 */
bool NMGMGenWidget::allFieldsNeededCorrect()
{
	QString errorMessage = "";

	if ( mainWidget->hostSourceCombo->getStationAddressFromCombo() == "" )
		errorMessage += "The source address is not an optional field.\n";
	else
	{
		NMGStation * sourceStation = mainWidget->hostSourceCombo->getStationFromCombo();
		if ( !sourceStation )
			errorMessage += "The source host is not optional.\n";
		else
		{
			switch ( ( mainWidget->comboMgenVersion->currentText() ).toInt() )
			{
				case 3:
					if ( sourceStation->getStationFieldsManager().getField ( getStationManager()->getModuleStationKey(), ID_MGEN_PATH ) == "" )
						errorMessage += "Mgen path is not an optional field for the source host.\n";
					break;
				case 4:
					if ( sourceStation->getStationFieldsManager().getField ( getStationManager()->getModuleStationKey(), ID_MGEN4_PATH ) == "" )
						errorMessage += "Mgen path for version 4 is not an optional field for the source host.\n";
					break;
			}
			if ( sourceStation->getManagementUsername() == "" )
				errorMessage += "Management username is not an optional field for the source host.\n";
		}
	}


	if ( mainWidget->hostDestinationCombo->getStationAddressFromCombo() == "" )
		errorMessage += "The destination address is not an optional field.\n";
	else
	{
		NMGStation * destionationStation = mainWidget->hostDestinationCombo->getStationFromCombo();
		if ( !destionationStation )
			errorMessage += "The destination host is not optional.\n";
		else
		{
			switch ( ( mainWidget->comboMgenVersion->currentText() ).toInt() )
			{
				case 3:
					if ( destionationStation->getStationFieldsManager().
					        getField ( getStationManager()->getModuleStationKey(), ID_DREC_PATH ) == "" )
						errorMessage += "Drec path is not an optional field for the destination host.\n";
					if ( destionationStation->getStationFieldsManager().getField ( getStationManager()->getModuleStationKey(), ID_DECODE_PATH ) == "" )
						errorMessage += "Decode path is not an optional field for the destination host.\n";
					break;
				case 4:
					if ( destionationStation->getStationFieldsManager().getField ( getStationManager()->getModuleStationKey(), ID_MGEN4_PATH ) == "" )
						errorMessage += "Mgen path for version 4 is not an optional field for the destination host.\n";
					break;
			}
			if ( destionationStation->getManagementUsername() == "" )
				errorMessage += "Management username is not an optional field for the destination host.\n";
		}
	}


	if ( mainWidget->flowList->topLevelItemCount() == 0 )
		errorMessage += "There are no flows in the test.\n";

	if ( errorMessage != "" )
		QMessageBox::warning ( 0, "Command not started", errorMessage );
	return errorMessage == "" ? true : false;
}

/**
 * Test cancelled using the progress bar widget
 * @param item
 */
void NMGMGenWidget::testCancelled ( ProgressItem * )
{
	buttonStopClicked();
}

/**
 * Slot executed when the user clicks the stop button to stop a test which is being executed
 */
void NMGMGenWidget::buttonStopClicked()
{
	sender->sendCommand ( Stop, "" );
	updateWidgetBecauseOfTestEnd();
}

/**
 * Updates the widgets because the end has ended
 */
void NMGMGenWidget::updateWidgetBecauseOfTestEnd()
{
	mainWidget->buttonStop->setEnabled ( false );
	mainWidget->buttonStart->setEnabled ( true );
	tabWidgetManager->removeTabImage();

	if ( progressItem )
	{
		progressItem->updateProgress();
		progressItem->setComplete();
	}
}

/**
 * Slot executed when the user wants to send the test to the statistic module
 */
void NMGMGenWidget::buttonStatisticClicked()
{
	if ( idSubscribeStatistic.isEmpty() )
	{
		// Subscribe in advance to the statistic plugin if you haven't done it before
		idSubscribeStatistic = moduleCommunication->subscribe ( "NMGStatistic", "<data><subscribe/></data>" );

		if ( !idSubscribeStatistic.isEmpty() ) 
			cout << GREEN << "[OK] Subscripted to NMGStatistic" << ENDCOLOR << endl;
		else 
			cout << RED <<"[ERROR] Failed to subscript to NMGStatistic " << ENDCOLOR <<endl;
	}

	if ( !sender->sendGUICommand ( Start, "<data><test>" + lastResultFile + "</test></data>", idSubscribeStatistic ) )
	{
                // If there's an error sending the command it's because the subscription is finished. 
                // Retry to subscribribe to a new instance of the plugin
		idSubscribeStatistic = QString();
                buttonStatisticClicked();
	}
}

/**
 * Slot executed when the user wants to send the test to the graphic module
 */
void NMGMGenWidget::buttonGraphicClicked()
{
	if ( idSubscribeGraphic.isEmpty() )
	{
		// Subscribe in advance to the graphic plugin if you haven't done it before
		idSubscribeGraphic = moduleCommunication->subscribe ( "NMGGraphics", "<data><subscribe/></data>" );

		if ( !idSubscribeGraphic.isEmpty() ) 
			cout << GREEN << "[OK] Subscripted to NMGGraphics" << ENDCOLOR<<endl;
		else 
			cout << RED << "[ERROR] Failed to subscript to NMGGraphics " << ENDCOLOR<< endl;
	}

	if ( !sender->sendGUICommand ( Start, "<data><test>" + lastResultFile + "</test></data>", idSubscribeGraphic ) )
	{
                // If there's an error sending the command it's because the subscription is finished. 
                // Retry to subscribribe to a new instance of the plugin
		idSubscribeGraphic = QString();
                buttonGraphicClicked();
	}
}
/**
 * Slot executed when the users clkick the Oreneta button
 */
void NMGMGenWidget::buttonOrenetaClicked()
{
	if ( !moduleCommunication->existsModule ( ORENETA_MODULE_NAME ) )
		QMessageBox::warning ( 0, "Oreneta is not loaded", "Oreneta module is not loaded ito memory.\nPlease load the module if you want to execute it." );
	else
	{

		if ( mainWidget->hostSourceCombo->getStationAddressFromCombo() == "" or
		        mainWidget->hostDestinationCombo->getStationAddressFromCombo() == "" )
		{
			QMessageBox::warning ( 0, "Insufficient parameters",
			                       "Source and destination address are needed by Oreneta module." );
			return;
		}


		QString data = "<data><source>" + mainWidget->hostSourceCombo->getStationAddressFromCombo() +
		               "</source><destination>" + mainWidget->hostDestinationCombo->getStationAddressFromCombo() + "</destination></data>";
		QString id;
		if ( ( id = moduleCommunication->subscribe ( ORENETA_MODULE_NAME, data ) ) != "" ) 
			cout << GREEN << "[OK] Execution of Oreneta Succesful(id: " << qPrintable ( id ) << ")." << endl << ENDCOLOR;
		else 
			cerr << RED << "[ERROR] while subscribing Oreneta to the MGEn Module" << endl << ENDCOLOR;
	}
}

/**
 * Updates the bandwiths of ATM and Ehernet
 * @param
 */
void NMGMGenWidget::updateEthernetAndATMBAndwith ( int )
{
	/* flow tab widgets  */
	int bwATM = NMGMGenBandwith::getATMBandwith ( mainWidget->txtFlowRate->value(), mainWidget->txtFlowPacketSize->value() );
	int bwEth = NMGMGenBandwith::getEthernetBandwith ( mainWidget->txtFlowRate->value(), mainWidget->txtFlowPacketSize->value() );

	mainWidget->txtBandwithATM->setText ( QString ( "%1" ).arg ( bwATM ) + " kbps" );
	mainWidget->txtBandwithEthernet->setText ( QString ( "%1" ).arg ( bwEth ) + " kbps" );

	/* mod tab widgets  */
	bwATM = NMGMGenBandwith::getATMBandwith ( mainWidget->txtModRate->value(), mainWidget->txtModPacketSize->value() );
	bwEth = NMGMGenBandwith::getEthernetBandwith ( mainWidget->txtModRate->value(), mainWidget->txtModPacketSize->value() );

	mainWidget->txtModBandwithATM->setText ( QString ( "%1" ).arg ( bwATM ) + " kbps" );
	mainWidget->txtModBandwithEthernet->setText ( QString ( "%1" ).arg ( bwEth ) + " kbps" );
}

/**
 * Returns true if the protocol used in the test is IPv4
 * @return
 */
bool NMGMGenWidget::isProtocolIPv4()
{
	NMGStation * station = mainWidget->hostDestinationCombo->getStationFromCombo();
	if ( !station ) return true;
	else return station->getTestHost().isHostIPv4();
}

/**
 * Updates the flows identifiers
 */
void NMGMGenWidget::updateFlowListIdentifier()
{
	lastFlowId = 1;
	int n = mainWidget->flowList->topLevelItemCount();
	int i = 0;
	int fills;

	QString numstring;

	while ( i < n )
	{
		NMGMGenFlow *flow = ( NMGMGenFlow * ) mainWidget->flowList->topLevelItem ( i );
		flow->setId ( lastFlowId++ );
		fills = flow->childCount();

		if ( fills > 0 )
		{
			for ( int j = 0; j < fills; j++ )
			{
				numstring = QString ( "%1" ).arg ( j + 1 );
				if ( numstring.length() == 1 )
					flow->child ( j )->setText ( 0, "Mod 0" + numstring );
				else
					flow->child ( j )->setText ( 0, "Mod " + numstring );
			}
		}
		i++;
	}
}

/**
 * Returns the op up menu of the widget
 * @return
 */
QMenu * NMGMGenWidget::getPopupMenu()
{
	return flowListMenu;
}

/**
 * Returns the station manager
 * @return
 */
NMGIModuleStationManager * NMGMGenWidget::getStationManager()
{
	return stationManager;
}

/**
 * Returns the flow manager
 * @return
 */
NMGMGenFlowManager * NMGMGenWidget::getFlowManager()
{
	return flowManager;
}

/**
 * Returns the last test path (or "" if undefined)
 * @return
 */
QString NMGMGenWidget::getLastTestPath()
{
	QString lastTestPath = preferencesManager->getValue ( ID_PREFERENCE_LAST_TEST_PATH );
	if ( lastTestPath.isNull() or lastTestPath.isEmpty() ) lastTestPath = "";
	return lastTestPath;
}

/**
 * Saves the last test path from the filename path of the test
 * @param filePath
 */
void NMGMGenWidget::saveLastTestPathFromFilePath ( const QString & filePath )
{
	if ( !filePath.isEmpty() )
	{
		int lastPos = filePath.lastIndexOf ( "/" );
		preferencesManager->addValue ( ID_PREFERENCE_LAST_TEST_PATH, filePath.left ( lastPos ) );
	}
}

/**
 * Notifies the arrival of the reply of the start message
 * @param data XML message to be parsed
 */
void NMGMGenWidget::notifyResult ( QString data )
{
	if ( syncActivated )
	{
		if ( !finishedMgen )
		{
			updateFinishedMgen ( data );		// get mgen result
			finishedMgen = true;
		}
	}
	else
	{
		updateFinishedMgen ( data );
	}
}

void NMGMGenWidget::updateFinishedMgen ( QString data )
{
	lastResultFile = interpret.getResultFilePathFromResult ( data );

	if ( lastResultFile.isEmpty() )
	{
		MessageBar::longMessage ( "The execution of the Mgen test wasn't succesful." );
		cout << RED << "[ERROR] The execution of the Mgen test wasn't succesful" << endl<<ENDCOLOR;
		
		if(progressItem)
		{
			progressItem->cancel();
			progressItem = 0;
		}
		updateWidgetBecauseOfTestEnd();
	}
	else
	{
		updateWidgetBecauseOfTestEnd();
		MessageBar::longMessage ( "MGen test finished\nDecode result at " + lastResultFile + "." );
		cout << GREEN << "[OK] MGen test finished --> Decode result at " << qPrintable(lastResultFile) << ENDCOLOR << endl;
	}
}

/**
 * Notifies the error of the core module
 * @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
 * @param errorMessage Error Message
 */
void NMGMGenWidget::notifyError ( int errorType, QString errorMessage )
{
	if ( errorType >= 1 and errorType <= 5 ) updateWidgetBecauseOfTestEnd();

	cout << RED << "[ERROR] NMGMGenWidget::notifyError --> " << qPrintable(errorMessage) << ENDCOLOR << endl;
	errorNotifier.notifyErrorToUser ( errorType, errorMessage );
}

/**
 * Notifies an status reply form the core
 * @param status XMl document with root data
 */
void NMGMGenWidget::notifyStatus ( QString status )
{
	typedef enum
	{
		NotStarted = 0,
		SyncStarted = 5,
		SendingDrecScriptFile = 10,
		DrecStarted = 20,
		SendingMgenScriptFile = 30,
		MgenStarted = 40,
		MgenFinished = 50,
		DecodeStarted = 60,
		DecodeFinished = 70,
		ReceivingDecodeResultFile = 80,
		TestFinished = 90,
		TestError = 100
	}
	MgenStatusReply;


	if ( !progressItem ) 
		cerr << RED << "[ERROR] Status arrived without item at the progress bar!" << ENDCOLOR <<endl;
	else
	{
		int statusNum = interpret.parseStatusMessage ( status );
		if ( statusNum < 0 ) 	cerr << RED << "[ERROR] at status message." << ENDCOLOR << endl;
		else
		{
			MgenStatusReply reply = ( MgenStatusReply ) statusNum;
			cout << LIGHTBLUE << "[INFO] NMGMGenWidget::notifyStatus --> ";

			switch ( reply )
			{
				case NotStarted: //0%
					progressItem->setStatus ( "Test not started" );
					cout << "Test not started" << ENDCOLOR << endl;
					break;

				case SyncStarted: //5%
					progressItem->updateProgress();
					progressItem->setProgress ( 5 );
					progressItem->setStatus ( "Sync started" );
					cout << "Sync started" << ENDCOLOR << endl;
					break;

				case SendingDrecScriptFile: //15%
					progressItem->updateProgress();
					progressItem->setProgress ( 15 );
					if ( mainWidget->comboMgenVersion->currentText() == "4" )
					{
						progressItem->setStatus ( "Sending Mgen (DST) script file" );
						cout <<  "Sending Mgen (DST) script file" << ENDCOLOR << endl;
					}
					else
					{
						progressItem->setStatus ( "Sending Drec script file" );
						cout << "Sending Drec script file" << ENDCOLOR << endl;
					}
					break;

				case DrecStarted: //20%
					progressItem->updateProgress();
					progressItem->setProgress ( 20 );
					if ( mainWidget->comboMgenVersion->currentText() == "4" )
					{
						progressItem->setStatus ( "Mgen (DST) started listening" );
						cout << "Mgen (DST) started listening" << ENDCOLOR << endl;
					}
					else
					{
						progressItem->setStatus ( "Drec started" );
						cout << "Drec Started" << ENDCOLOR << endl;
					}
					break;

				case SendingMgenScriptFile: //30%
					progressItem->updateProgress();
					progressItem->setProgress ( 30 );
					runningMgenAndDrec = false;
					progressItem->setStatus ( "Sending Mgen script file" );
					cout << "Sending Mgen script file" << ENDCOLOR << endl;
					break;

				case MgenStarted: // 31% to 75% (44 increments to do)
					progressItem->setCompletedItems ( 30 );
					if ( !runningMgenAndDrec )
					{
						//start the timer to increase the progress bar
						stepTime = maxTestTime / 44;
						leftTime = maxTestTime;
						progressTimer.start ( stepTime );
						runningMgenAndDrec = true;
					}
					if ( mainWidget->comboMgenVersion->currentText() == "4" )
					{
						progressItem->setStatus ( "Mgen running in SRC and DST" );
						cout << "Mgen running in SRC and DST" << ENDCOLOR << endl;	
					}
					else
					{
						progressItem->setStatus ( "Mgen and Drec running" );
						cout << "Mgen and Drec running" << ENDCOLOR << endl;
					}
					break;

				case MgenFinished: // 76%
					progressTimer.stop();
					runningMgenAndDrec = false;
					progressItem->updateProgress();
					progressItem->setProgress ( 76 );
					progressItem->setStatus ( "Mgen finished" );
					cout << "Mgen finished" << ENDCOLOR << endl;
					break;

				case DecodeStarted: //80%
					if ( syncActivated )
					{
						// we need to send a "double-stop" because of mgen and sync connection
						// and because there's a libnetmeter bug...
						cout << "NMGMGenWidget :: Stopping Sync..." <<  ENDCOLOR << endl;
						sender->sendCommand ( Stop, "" );
						//sleep(3);
						//sender->sendCommand(Stop, "");
					}
					progressItem->updateProgress();
					progressItem->setProgress ( 80 );
					progressItem->setStatus ( "Decode started" );
					cout << "Decode started" << ENDCOLOR << endl;
					break;

				case DecodeFinished: //83%
					progressItem->updateProgress();
					progressItem->setProgress ( 83 );
					progressItem->setStatus ( "Decode finished" );
					cout << "Decode finished" << ENDCOLOR << endl;
					break;

				case ReceivingDecodeResultFile: //90%
					progressItem->updateProgress();
					progressItem->setProgress ( 90 );
					progressItem->setStatus ( "Receiving decode result file" );
					cout << "Receiving decode result file" << ENDCOLOR << endl;
					break;

				case TestFinished: //95%
					progressItem->updateProgress();
					progressItem->setProgress ( 95 );
					progressItem->setStatus ( "Test finished:" );
					cout << "Test finished: Writing file to disk... this might take a while"  << ENDCOLOR << endl;
					MessageBar::longMessage ( "Writing file to disk... this might take a while" );
					if ( syncActivated )
					{
						updateWidgetBecauseOfTestEnd(); 	// get mgen result
						finishedMgen = true;
					}
					break;

				case TestError:
					progressTimer.stop();
					runningMgenAndDrec = false;
					progressItem->setStatus ( "Error executing test" );
					cout << ENDCOLOR << endl;
					cout << RED << "[ERROR] executing test" << ENDCOLOR << endl;
					break;

				default:
					progressItem->setStatus ( "Unknown status!" );
					cout << ENDCOLOR << endl;
					cout << RED << "[ERROR] Unknown status!" << ENDCOLOR << endl;
			}
		}
	}
}

/**
 * Increments the progress bar
 */
void NMGMGenWidget::incrementProgressItemBar()
{
	if ( progressItem and leftTime > 1 )
	{
		leftTime -= stepTime;
		progressItem->incCompletedItems();
		progressItem->updateProgress();
		if ( mainWidget->comboMgenVersion->currentText() == "4" )
			progressItem->setStatus ( "Mgen (SRC and DST) running (" + getLeftTimeTestString() + " left)" );
		else
			progressItem->setStatus ( "Mgen and Drec running (" + getLeftTimeTestString() + " left)" );
	}
}

/**
 * Active checkbox for burst pattern
 */
void NMGMGenWidget::activeBurstPattern ( const QString & mgenVersion )
{
	if ( mgenVersion == "4" )
	{
		mainWidget->checkBoxPatternBurst->setEnabled ( true );
		mainWidget->checkBoxPatternBurstMod->setEnabled ( true );
	}
	else
	{
		mainWidget->checkBoxPatternBurst->setEnabled ( false );
		mainWidget->checkBoxPatternBurstMod->setEnabled ( false );

		if ( mainWidget->checkBoxPatternBurst->isChecked() )
		{
			mainWidget->checkBoxPatternBurst->setChecked ( false );
			mainWidget->txtInterval->setEnabled ( false );
			mainWidget->txtDuration->setEnabled ( false );
			mainWidget->comboInterval->setEnabled ( false );
			mainWidget->comboDuration->setEnabled ( false );

		}
		if ( mainWidget->checkBoxPatternBurstMod->isChecked() )
		{
			mainWidget->checkBoxPatternBurstMod->setChecked ( false );
			mainWidget->txtIntervalMod->setEnabled ( false );
			mainWidget->txtDurationMod->setEnabled ( false );
			mainWidget->comboIntervalMod->setEnabled ( false );
			mainWidget->comboDurationMod->setEnabled ( false );
		}
	}
}

/**
 * Check if user is "root" in order to change ToS
 */
void NMGMGenWidget::checkRootToS ( bool on )
{
	if ( on && mainWidget->hostSourceCombo->getStationFromCombo() != 0 )
	{
		if ( mainWidget->hostSourceCombo->getStationFromCombo()->getManagementUsername() != "root" )
			QMessageBox::warning ( 0, "ToS Warning", "To set ToS you must be root." );
	}
	else if ( on && mainWidget->hostSourceCombo->getStationFromCombo() == 0 )
		QMessageBox::warning ( 0, "ToS Warning", "To set ToS you must be root." );
}

/**
 * Returns the string corresponding to the left execution time of the test (in a friendly way hoiur, min, ...);
 * @return
 */
QString NMGMGenWidget::getLeftTimeTestString()
{
	if ( leftTime <= 0 ) leftTime = 1;
	if ( leftTime < 1000 ) //less than a second --> ms
		return QString ( "%1" ).arg ( leftTime ) + " ms";
	else if ( leftTime < 60*1000 ) //less than a minute --> sec
		return QString ( "%1" ).arg ( leftTime / 1000 ) + " sec";
	else if ( leftTime < 60*60*1000 ) //less than an hour --> minutes
		return QString ( "%1" ).arg ( leftTime / ( 60*1000 ) ) + " min " + QString ( "%1" ).arg ( leftTime % ( 60*1000 ) / 1000 ) + " sec";
	else return QString ( "%1" ).arg ( leftTime / ( 60*60*1000 ) ) + " h " + QString ( "%1" ).arg ( leftTime % ( 60*60*1000 ) / ( 60*1000 ) ) + " min";;

}


/**
 * Returns the maximum end time of all the flows of the test
 * @return
 */
int NMGMGenWidget::getMaximumFlowEndTime()
{
	int n = mainWidget->flowList->topLevelItemCount();
	int i = 0;
	int max = 0;

	while ( i < n )
	{
		NMGMGenFlow *flow = ( NMGMGenFlow * ) mainWidget->flowList->topLevelItem ( i );
		if ( max < ( flow->getStartTime() + flow->getDuration() ) )
			max = flow->getStartTime() + flow->getDuration();
		i++;
	}

	return max;
}

/**
 * Returns true if the test is running
 * @return
 */
bool NMGMGenWidget::isTestRunning()
{
	return mainWidget->buttonStop->isEnabled();
}


#include "nmgmgenwidget.moc"
