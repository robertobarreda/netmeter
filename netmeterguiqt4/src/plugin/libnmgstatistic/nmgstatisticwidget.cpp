/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#include "nmgstatisticwidget.h"

#include <nmgmoduleutils.h>
#include <messagebar.h>

#include <nmgmetadatatags.h>

#include <QStringList>
#include <QFileDialog>

#include <iostream>
using namespace std;

#define ID_PREFERENCE_LAST_STATISTIC_PATH	"StatisticLastTestPath"

NMGStatisticWidget::NMGStatisticWidget ( NMGModulePreferencesManager * prefMan, QWidget *parent, const char *name )
		: QWidget ( parent ), preferencesManager ( prefMan )
{
	setObjectName ( name );
	setupUi ( this );

	QStringList header;
	header.append ( "Test/Flow ID" );
	header.append ( "Test/Flow Name" );
	header.append ( "Source\n(IP/port)" );
	header.append ( "Destination\n(IP/port)" );
	header.append ( "Pattern" );
	header.append ( "Packet rate\n(packets/sec)" );
	header.append ( "Packet size\n(bytes)" );
	header.append ( "Duration" );
	statisticTreePacket->setHeaderLabels ( header );

	header.clear();
	header.append ( "Test ID" );
	header.append ( "Test Name" );
	header.append ( "Source" );
	header.append ( "Destination" );
	header.append ( "Duration" );
	header.append ( "Throughput\n(sender -> receiver)" );
	header.append ( "Protocol" );
	header.append ( "Send\nSocket Size" );
	header.append ( "Received\nSocket Size" );
	header.append ( "Send\nMessage Size" );
	statisticTreeAggregation->setHeaderLabels ( header );

	toolBar = 0;
	listMenu = 0;
	createContextualMenu();
	currentStatisticTree = statisticTreePacket;

	fileLoader = new NMGStatisticFileLoader();
	exportManager = new NMGStatisticExportManager ( this, preferencesManager );

	ngroup = 0;

	connect ( fileLoader, SIGNAL ( signalFileRead ( const QString&, const QString& ) ),
	          this, SLOT ( updateWidgetBecauseOfTestEnd ( const QString &, const QString & ) ) );

	connect ( statisticTreePacket, SIGNAL ( itemSelectionChanged () ),
	          this, SLOT ( updateWidgetBecauseSelectionChanged () ) );
	connect ( statisticTreeAggregation, SIGNAL ( itemSelectionChanged () ),
	          this, SLOT ( updateWidgetBecauseSelectionChanged () ) );
	connect ( tabTest, SIGNAL ( currentChanged ( int ) ),
	          this, SLOT ( updateWidgetBecauseSelectionChanged () ) );

	connect ( statisticTreePacket, SIGNAL ( customContextMenuRequested ( const QPoint & ) ),
	          this, SLOT ( showContextualMenu ( const QPoint & ) ) );
	connect ( statisticTreeAggregation, SIGNAL ( customContextMenuRequested ( const QPoint & ) ),
	          this, SLOT ( showContextualMenu ( const QPoint & ) ) );

	connect ( statisticTreePacket, SIGNAL ( itemRemoved ( const QString & ) ),
	          this, SLOT ( updateWidgetBecauseItemRemoved ( const QString &) ) );
	connect ( statisticTreeAggregation, SIGNAL ( itemRemoved ( const QString &) ),
	          this, SLOT ( updateWidgetBecauseItemRemoved ( const QString &) ) );
}

NMGStatisticWidget::~NMGStatisticWidget()
{
	if (exportManager) delete exportManager;
	if (fileLoader) delete fileLoader;
}

QToolBar * NMGStatisticWidget::getToolBar()
{
	if ( !toolBar )
	{
		toolBar = new QToolBar ( NMGModuleUtils::getMainWindow() );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "fileopen.png" ) ),
		                     tr ( "Load Test File" ), this, SLOT ( loadFiles() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "add_group.png" ) ),
				     tr ( "Add New Group" ), this, SLOT ( addNewGroup() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "reload.png" ) ),
		                     tr ( "Reload Test file" ), this, SLOT ( reloadFiles() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "remove-red.png" ) ),
		                     tr ( "Delete Item" ), this, SLOT ( removeSelectedItems() ) );
	}
	return toolBar;
}

QMenu * NMGStatisticWidget::getPopupMenu()
{
	if ( !listMenu )
	{
		listMenu = new QMenu();
		listMenu->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "fileopen.png" ) ), 
		                      "Load &Test file", this, SLOT ( loadFiles() ), Qt::CTRL + Qt::Key_O );
		listMenu->addAction ( reloadAction );
		listMenu->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "add_group.png" ) ),
		                      "Add New &Group", this, SLOT ( addNewGroup() ), Qt::CTRL + Qt::Key_G );
		listMenu->addSeparator();

		listMenu->addMenu ( exportSubMenu );
		listMenu->addAction ( "Statistic &Export Manager Preferences", this, SLOT ( showExportManager() ), Qt::CTRL + Qt::Key_E );
		listMenu->addSeparator();

		QMenu * cacheSubMenu = listMenu->addMenu ( "Manage Cache" );
		cacheSubMenu->addAction ( "Save Cache", this, SLOT ( saveCache() ) );
		cacheSubMenu->addAction ( "Clear Cache", this, SLOT ( deleteCache() ) );
	}
	return listMenu;
}

void NMGStatisticWidget::createContextualMenu()
{
	contextualMenu = new QMenu();
	
	contextualMenu->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "fileopen.png" ) ),
	                            "Load &Test file", this, SLOT ( loadFiles() ), Qt::CTRL + Qt::Key_O );
	reloadAction = contextualMenu->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "reload.png" ) ),
	                                           "&Reload Test file", this, SLOT ( reloadFiles() ), Qt::CTRL + Qt::Key_R );
	contextualMenu->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "add_group.png" ) ),
	                            "Add New &Group", this, SLOT ( addNewGroup() ), Qt::CTRL + Qt::Key_G );
	
	contextualMenu->addSeparator();

	exportSubMenu = contextualMenu->addMenu ( QIcon(*NMGModuleUtils::getPixmap("export.png")), "Export Test" );
	exportSubMenu->addAction ( "To XML" );
	exportSubMenu->addAction ( "To XHTML" );
	exportSubMenu->addAction ( "To TXT" );
	exportSubMenu->addSeparator();
	exportSubMenu->addAction ( "Applying XSLT to Statistics" );
	exportSubMenu->addAction ( "Applying XSLT to Test Source" );
	connect ( exportSubMenu, SIGNAL ( triggered ( QAction * ) ), this, SLOT ( exportTestTo ( QAction * ) ) );
	contextualMenu->addSeparator();

	QMenu * cacheSubMenu = contextualMenu->addMenu ( "Manage Cache" );
	cacheSubMenu->addAction ( "Save Cache", this, SLOT ( saveCache() ) );
	cacheSubMenu->addAction ( "Clear Cache", this, SLOT ( deleteCache() ) );

	reloadAction->setEnabled ( false );
	exportSubMenu->setEnabled ( false );
}

bool NMGStatisticWidget::loadData ( QString data )
{
	QXmlStreamReader xml ( data );

	/** \par Received Message Format to START command when the plugin is subscribed
	 * <data>
	 *  	<test>testPath</test>
	 * 	...
	 * </data>
	 **/

	bool error = true;
	QString filePath;
	
	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() )
		{
			if ( xml.name() == TAG_DATA ) error = false;
			else if ( xml.name() == TAG_TEST and !error )
			{
				filePath = xml.readElementText();
				if ( ( error = filePath.isEmpty() ) ) xml.raiseError ( "[ERROR] Empty test path" );
				else fileLoader->loadFile ( filePath );
			}
			else 
			{
				xml.raiseError ( "[ERROR] Error in XML format" );
				error = true;
			}
		}
	}
	if ( xml.hasError() )
		cout << qPrintable ( xml.errorString () ) << endl;

	return !error;
}

void NMGStatisticWidget::loadFiles()
{
	QStringList fileNamesList = QFileDialog::getOpenFileNames ( this, tr ( "Select test files" ), getLastTestPath(),
	                            tr ( "XML Flow Format (*.xff);;GZiped XML Flow Format (*.xff.gz);;XML Statistic Format (*.xsf);;XML (*.xml);;All files (*.*)" ) );

	if ( !fileNamesList.isEmpty() )
	{
		saveLastTestPathFromFilePath ( fileNamesList.last() );
		fileLoader->loadFiles ( fileNamesList );
	}
}

void NMGStatisticWidget::reloadFiles()
{
	QStringList fileNamesList;
	QTreeWidgetItemList selectList = currentStatisticTree->selectedItems();

	for ( QTreeWidgetItemListIterator it = selectList.begin(); it != selectList.end(); ++it )
	{
		if ( ( *it ) and ( *it )->type() != NMGGroupTreeWidgetItem::Group )
		{
			// Get selected item's ID and add the REPARSE_FLAG before appendig to the list
			QString id = ( ( NMGGroupTreeWidgetItem * ) * it )->getId();
			id = REPARSE_FLAG + paths.value ( id.left ( id.lastIndexOf ( SEPARATOR ) ) );
			if ( !fileNamesList.contains ( id ) ) fileNamesList.append ( id );

			// Remove items reloaded
			if ( ! ( *it ) or ! ( *it )->parent() )
				currentStatisticTree->takeTopLevelItem ( currentStatisticTree->indexOfTopLevelItem ( *it ) );
			else
			{
				QTreeWidgetItem * parent = ( *it )->parent();

				if ( parent->type() == NMGGroupTreeWidgetItem::Test )
				{
					if ( !parent->parent() )
						currentStatisticTree->takeTopLevelItem ( currentStatisticTree->indexOfTopLevelItem ( parent ) );
					else
						delete parent->parent()->takeChild ( parent->parent()->indexOfChild ( parent ) );
				}
				else if ( parent->type() == NMGGroupTreeWidgetItem::Group )
					delete parent->takeChild ( parent->indexOfChild ( *it ) );
			}
		}
	}

	if ( !fileNamesList.isEmpty() )
		fileLoader->loadFiles ( fileNamesList );
}

void NMGStatisticWidget::updateWidgetBecauseItemRemoved ( const QString & id )
{
	#ifdef DEBUG
		cout << "[ITEM REMOVED] " << qPrintable ( id ) << endl;
	#endif
	fileLoader->removeMetadataItem( id );
}

void NMGStatisticWidget::updateWidgetBecauseSelectionChanged ()
{
	// We perform this check every time we change the selection or the tab
	// currentStatisticTree contains the current visible tree, so we don't need to check it every time
	if ( tabTest->currentIndex() == TAB_PACKETS )
		currentStatisticTree = statisticTreePacket;
	else if ( tabTest->currentIndex() == TAB_TPUT )
		currentStatisticTree = statisticTreeAggregation;
	else // Future use, if we have more tabs
		return;

	bool on = ( currentStatisticTree->selectedItems().count() > 0 );
	reloadAction->setEnabled ( on );
	exportSubMenu->setEnabled ( on );
	if ( !on ) statisticView->setPlainText ( "" );
	else
	{
		if ( tabTest->currentIndex() == TAB_TPUT
		        and statisticTreeAggregation->currentItem()->type() == NMGGroupTreeWidgetItem::Group
		        and statisticTreeAggregation->currentItem()->childCount() > 0 )
			displayAggregationStatisticsOnTextBrowser ( ( NMGGroupTreeWidgetItem * ) statisticTreeAggregation->currentItem() );
		else
		{
			NMGStatisticExportDataList exportList;
			createSelectedList ( ( NMGGroupTreeWidgetItem * ) currentStatisticTree->currentItem(), &exportList );
			statisticView->setHtml ( exportManager->displayTest ( exportList ) );
		}
	}
}

void NMGStatisticWidget::updateWidgetBecauseOfTestEnd ( const QString & filePathId, const QString & fileMD5Id )
{
	paths.insert ( fileMD5Id, filePathId ); // Because we want to keep the full source path

	displayMetadataOnTreeWidget ( filePathId, fileMD5Id );
	displayStatisticsOnTextBrowser ( filePathId, fileMD5Id );
}

void NMGStatisticWidget::displayMetadataOnTreeWidget ( const QString & filePathId, const QString & fileMD5Id )
{
	NMGMetaData * metadataInfo;
	try
	{
		metadataInfo = fileLoader->getMetadata ( filePathId, fileMD5Id );
	}
	catch ( NMGStatisticError e ) { e.raiseError(); }

	if ( metadataInfo->getTypeOfTest() == TYPE_PACKETS )
	{
		statisticTreePacket->addTest ( fileMD5Id, QStringList() << filePathId.mid ( filePathId.lastIndexOf ( "/" ) + 1 ) << metadataInfo->getTestName() );

		QString sourcePort;
		QMap<QString, QStringList *> flows;

		QXmlStreamReader xml ( metadataInfo->getUncommonElement ( TAG_CONF_MGEN ) );

		while ( !xml.atEnd() )
		{
			xml.readNext();
			if ( xml.isStartElement() )
			{
				if ( xml.name() == TAG_PORT ) sourcePort = xml.readElementText();
				else if ( xml.name() == TAG_ON )
				{
					QStringList * on = new QStringList ( xml.attributes().value ( ATTR_FLOW_ID ).toString() );
					on->append ( xml.attributes().value ( ATTR_NAME ).toString() );
					on->append ( QString ( metadataInfo->getSourceAddress() + "/" + sourcePort ) );
					on->append ( xml.attributes().value ( ATTR_DEST ).toString() );
					on->append ( xml.attributes().value ( ATTR_PATTERN ).toString() );
					on->append ( xml.attributes().value ( ATTR_RATE ).toString() );
					on->append ( xml.attributes().value ( ATTR_SIZE ).toString() );
					on->append ( xml.attributes().value ( ATTR_TIME ).toString() );
					flows.insert ( xml.attributes().value ( ATTR_FLOW_ID ).toString(), on );
				}
				else if ( xml.name() == TAG_OFF )
				{
					QStringList * off = flows.value ( xml.attributes().value ( ATTR_FLOW_ID ).toString() );
					off->append ( simplifyTime ( xml.attributes().value ( ATTR_TIME ).toString().toInt() - off->takeLast().toInt() ) );
				}
			}
		}

		for ( QMap<QString, QStringList *>::iterator i = flows.begin(); i != flows.end(); ++i )
			statisticTreePacket->addFlow ( ( **i ).at ( 0 ), **i );

		tabTest->setCurrentIndex ( TAB_PACKETS );
	}

	else if ( metadataInfo->getTypeOfTest() == TYPE_TPUT )
	{
		NMGStatisticThroughtputData * item;

		try
		{
			item = ( NMGStatisticThroughtputData * ) fileLoader->getStatistics ( filePathId, -1, fileMD5Id );
		}
		catch ( NMGStatisticError e ) { e.raiseError(); }

		QStringList values = QStringList ( filePathId.mid ( filePathId.lastIndexOf ( "/" ) + 1 ) );
		values.append ( metadataInfo->getTestName() );
		values.append ( item->getSourceAddress() );
		values.append ( item->getDestinationAddress() );
		values.append ( simplifyTime ( ( int ) item->getElapsedTimeInSeconds(), SECS ) );
		values.append ( DOUBLE ( item->getReceivingThroughputInKBitsPerSecond() ) + " kbps" );
		values.append ( item->getTypeOfProtocol().toUpper() );
		values.append ( simplifySize ( item->getSendingSocketSizeInBytes(), BYTES ) );
		values.append ( simplifySize ( item->getReceivingSocketSizeInBytes(), BYTES ) );
		values.append ( simplifySize ( item->getSendingMessagesSizeInBytes(), BYTES ) );

		statisticTreeAggregation->addTest ( fileMD5Id, values );

		tabTest->setCurrentIndex ( TAB_TPUT );
	}
	else
		return;
}

void NMGStatisticWidget::displayStatisticsOnTextBrowser ( const QString & filePathId, const QString & fileMD5Id )
{
	NMGStatisticExportData * exportData  = new NMGStatisticExportData();
	try
	{
		exportData->fileName = filePathId;
		exportData->metadata = fileLoader->getMetadata ( filePathId, fileMD5Id );
		exportData->itemList = fileLoader->getStatistics ( filePathId, fileMD5Id );
	}
	catch ( NMGStatisticError e ) { e.raiseError(); }

	NMGStatisticExportDataList list;
	list.append ( exportData );
	statisticView->setHtml ( exportManager->displayTest ( list ) );
}

void NMGStatisticWidget::displayAggregationStatisticsOnTextBrowser ( NMGGroupTreeWidgetItem * selectedItem )
{
	NMGStatisticExportDataList exportList;
	NMGStatisticAggregationData * exportAggregatedData = createAggreagtedList ( selectedItem, &exportList );

	statisticView->setHtml ( exportManager->displayTest ( exportAggregatedData, exportList ) );
	delete exportAggregatedData;
}

void NMGStatisticWidget::exportTestTo ( QAction * action )
{
	bool ret;
	QTreeWidgetItemList selectList = currentStatisticTree->selectedItems();
		
	QString type = action->text();
	if ( type == "Applying XSLT to Test Source" )
	{
		QStringList exportList;

		for ( QTreeWidgetItemListIterator it = selectList.begin(); it != selectList.end(); ++it )
			createSelectedPathList ( ( NMGGroupTreeWidgetItem * ) ( *it ), &exportList );
		ret = exportManager->applyXsltToSourceTest ( exportList );
	}
	else
	{
		if ( tabTest->currentIndex() == TAB_TPUT
		        and statisticTreeAggregation->currentItem()->type() == NMGGroupTreeWidgetItem::Group
		        and statisticTreeAggregation->currentItem()->childCount() > 0 )
		{
			NMGStatisticExportDataList exportList;
			NMGStatisticAggregationData * exportAggregatedData = createAggreagtedList ( ( NMGGroupTreeWidgetItem * ) statisticTreeAggregation->currentItem(), &exportList );

			if ( type == "To XML" )
				ret = exportManager->exportTest ( exportAggregatedData, exportList, NMGStatisticExportManager::XML );
			else if ( type == "To XHTML" )
				ret = exportManager->exportTest ( exportAggregatedData, exportList, NMGStatisticExportManager::XHTML );
			else if ( type == "To TXT" )
				ret = exportManager->exportTest ( exportAggregatedData, exportList, NMGStatisticExportManager::TXT );
			else if ( type == "Applying XSLT to Statistics" )
				ret = exportManager->exportTest ( exportAggregatedData, exportList, NMGStatisticExportManager::XSLT );
			delete exportAggregatedData;
		}
		else
		{
			NMGStatisticExportDataList exportList;

			for ( QTreeWidgetItemListIterator it = selectList.begin(); it != selectList.end(); ++it )
				createSelectedList ( ( NMGGroupTreeWidgetItem * ) ( *it ), &exportList );

			if ( type == "To XML" )
				ret = exportManager->exportTest ( exportList, NMGStatisticExportManager::XML );
			else if ( type == "To XHTML" )
				ret = exportManager->exportTest ( exportList, NMGStatisticExportManager::XHTML );
			else if ( type == "To TXT" )
				ret = exportManager->exportTest ( exportList, NMGStatisticExportManager::TXT );
			else if ( type == "Applying XSLT to Statistics" )
				ret = exportManager->exportTest ( exportList, NMGStatisticExportManager::XSLT );
		}
	}

	if ( ret ) MessageBar::longMessage ( tr ( "File export process finished" ) );
}

void NMGStatisticWidget::createSelectedList ( NMGGroupTreeWidgetItem * selectedItem, NMGStatisticExportDataList * exportList )
{
	NMGStatisticExportData * exportData = new NMGStatisticExportData();

	if ( ( selectedItem->type() == NMGGroupTreeWidgetItem::Group or selectedItem->type() == NMGGroupTreeWidgetItem::Test )
	        and selectedItem->childCount() > 0 )
	{
		for ( int i = 0; i < selectedItem->childCount(); ++i )
			createSelectedList ( ( NMGGroupTreeWidgetItem * ) selectedItem->child ( i ), exportList );
	}
	else if ( selectedItem->type() == NMGGroupTreeWidgetItem::Test and selectedItem->childCount()  == 0 )
	{
		exportData->fileName = selectedItem->text ( 0 );
		try
		{
			exportData->metadata = fileLoader->getMetadata ( exportData->fileName, selectedItem->getId() );
			exportData->itemList = fileLoader->getStatistics ( exportData->fileName, selectedItem->getId() );
		}
		catch ( NMGStatisticError e ) { e.raiseError(); }
		exportList->append ( exportData );
	}
	else if ( selectedItem->type() == NMGGroupTreeWidgetItem::Flow )
	{
		NMGStatisticPacketData * flow;
		exportData->fileName = selectedItem->parent()?selectedItem->parent()->text ( 0 ):"";
		try
		{
			flow = ( NMGStatisticPacketData * ) fileLoader->getStatistics ( exportData->fileName,
			        selectedItem->text ( 0 ).toInt(),
			        selectedItem->getId().left ( selectedItem->getId().lastIndexOf ( SEPARATOR ) ) );
		}
		catch ( NMGStatisticError e ) { e.raiseError(); }

		//exportList->indexOf( exportData );
		int index = 0;
		for ( NMGStatisticExportDataListIterator i = exportList->begin(); i != exportList->end(); ++i )
		{
			if ( ( *i )->fileName == exportData->fileName ) { break; }
			++index;
		}

		if ( index == exportList->count() )
		{
			try
			{
				exportData->metadata = fileLoader->getMetadata ( exportData->fileName,
				                       selectedItem->getId().left ( selectedItem->getId().lastIndexOf ( SEPARATOR ) ) );
			}
			catch ( NMGStatisticError e ) { e.raiseError(); }
			exportData->itemList.append ( flow );
			exportList->append ( exportData );
		}
		else
		{
			//if (!exportList->at(index)->itemList.contains(flow))
			NMGStatisticItemDataListIterator j = exportList->at ( index )->itemList.begin();
			NMGStatisticItemDataListIterator end2 = exportList->at ( index )->itemList.end();

			while ( j != end2 )
			{
				if ( flow->flowId() > ( ( NMGStatisticPacketData * ) *j )->flowId() )
				{
					exportList->at ( index )->itemList.insert ( j, flow );
					break;
				}
				++j;
			}

			if ( j == end2 ) exportList->at ( index )->itemList.append ( flow );
		}
	}
	else return;
}

NMGStatisticAggregationData * NMGStatisticWidget::createAggreagtedList ( NMGGroupTreeWidgetItem * groupItem, NMGStatisticExportDataList * exportList )
{
	QStringList list;
	NMGStatisticExportData * exportData;
	NMGStatisticAggregationData * exportAggregatedData;

	for ( int i = 0; i < groupItem->childCount(); ++i )
	{
		QString id = ( ( NMGGroupTreeWidgetItem * ) groupItem->child ( i ) )->getId();

		exportData = new NMGStatisticExportData();
		exportData->fileName = groupItem->child ( i )->text ( 0 );
		try
		{
			exportData->metadata = fileLoader->getMetadata ( exportData->fileName, id );
			exportData->itemList = fileLoader->getStatistics ( exportData->fileName, id );
		}
		catch ( NMGStatisticError e ) { e.raiseError(); }

		exportList->append ( exportData );
		list.append ( id );
	}

	try
	{
		exportAggregatedData = fileLoader->getStatistics ( list );
	}
	catch ( NMGStatisticError e ) { e.raiseError(); }
	
	return exportAggregatedData;
}

void NMGStatisticWidget::createSelectedPathList ( NMGGroupTreeWidgetItem * selectedItem, QStringList * exportPathList )
{
	if ( selectedItem->type() == NMGGroupTreeWidgetItem::Group && selectedItem->childCount() > 0 )
	{
		for ( int i = 0; i < selectedItem->childCount(); ++i )
			createSelectedPathList ( ( NMGGroupTreeWidgetItem * ) selectedItem->child ( i ), exportPathList );
	}
	else if ( ( selectedItem->type() == NMGGroupTreeWidgetItem::Test ) or ( selectedItem->type() == NMGGroupTreeWidgetItem::Flow ) )
	{
		QString path = paths.value ( selectedItem->getId().left ( selectedItem->getId().lastIndexOf ( SEPARATOR ) ) );
		if ( !exportPathList->contains ( path ) )  exportPathList->append ( path );
	}
	else return;
}

QString NMGStatisticWidget::getLastTestPath()
{
	return preferencesManager->getValue ( ID_PREFERENCE_LAST_STATISTIC_PATH );
}

void NMGStatisticWidget::saveLastTestPathFromFilePath ( const QString & filePath )
{
	if ( !filePath.isEmpty() )
	{
		int lastPos = filePath.lastIndexOf ( "/" ) + 1;
		preferencesManager->addValue ( ID_PREFERENCE_LAST_STATISTIC_PATH, filePath.left ( lastPos ) );
	}
}

QString NMGStatisticWidget::simplifyTime ( int time, int startTimeUnit )
{
	QString simplification;
	switch ( startTimeUnit )
	{
		case MILISECS:
			if ( time >= 1000 )
			{
				simplification = simplifyTime ( time / 1000, SECS );
				if ( time % 1000 > 0 ) simplification += QString ( tr ( " %1 ms" ) ).arg ( time % 1000 );
			}
			else simplification = QString ( tr ( "%1 ms" ) ).arg ( time );
			break;
		case SECS:
			if ( time >= 60 )
			{
				simplification = simplifyTime ( time / 60, MINS );
				if ( time % 60 > 0 ) simplification += QString ( tr ( " %1 sec" ) ).arg ( time % 60 );
			}
			else simplification = QString ( tr ( "%1 sec" ) ).arg ( time );
			break;
		case MINS:
			if ( time >= 60 )
			{
				simplification = simplifyTime ( time / 60, HOURS );
				if ( time % 60 > 0 ) simplification += QString ( tr ( " %1 min" ) ).arg ( time % 60 );
			}
			else simplification = QString ( tr ( "%1 min" ) ).arg ( time );
			break;
		case HOURS:
			simplification = QString ( tr ( "%1 hr" ) ).arg ( time );
			break;
	}
	return simplification;
}

QString NMGStatisticWidget::simplifySize ( double unit, int startSizeUnit )
{
	QString simplification;
	switch ( startSizeUnit )
	{
		case BITS:
			if ( unit >= 8 ) simplification = simplifySize ( unit / 8, BYTES );
			else simplification = QString ( tr ( "%1 bits" ) ).arg ( unit, 0, 'f', 2 );
			break;
		case BYTES:
			if ( unit >= 1024 ) simplification = simplifySize ( unit / 1024, KBYTES );
			else simplification = QString ( tr ( "%1 Bytes" ) ).arg ( unit, 0, 'f', 2 );
			break;
		case KBYTES:
			if ( unit >= 1024 ) simplification = simplifySize ( unit / 1024, MBYTES );
			else simplification = QString ( tr ( "%1 KB" ) ).arg ( unit, 0, 'f', 2 );
			break;
		case MBYTES:
			if ( unit >= 1024 ) simplification = simplifySize ( unit / 1024, GBYTES );
			else simplification = QString ( tr ( "%1 MB" ) ).arg ( unit, 0, 'f', 2 );
			break;
		case GBYTES:
			simplification = QString ( tr ( "%1 GB" ) ).arg ( unit, 0, 'f', 2 );
			break;
	}
	return simplification;
}

#include "nmgstatisticwidget.moc"
