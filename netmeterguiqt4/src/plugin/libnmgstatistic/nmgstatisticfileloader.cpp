/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#include "nmgstatisticfileloader.h"
#include <nmgmd5.h>
#include <messagebar.h>
#include <cfloat>

NMGStatisticFileLoader::NMGStatisticFileLoader()
{
	metadata = new NMGStatisticMetaData();
	statistics = new NMGStatisticData();
	filesToLoad = QStringList();
	readerThread = 0;
	progress = 0;
	userCancelled = false;
	timer = new QTimer;
	connect ( timer, SIGNAL ( timeout() ), this, SLOT ( slotUpdateProgress() ) );
	connect ( this, SIGNAL ( newFilesAdded() ), this, SLOT ( loadPendingFiles() ) );
}

NMGStatisticFileLoader::~NMGStatisticFileLoader()
{
	if ( readerThread ) delete readerThread;
	if ( progress ) progress->setComplete();
	progress = 0;
	if ( timer ) delete timer;
	statistics->saveStatisticsToXMLCache();
	if ( metadata ) delete metadata;
	if ( statistics ) delete statistics;
}

QString NMGStatisticFileLoader::NMGStatisticFileLoader::getTypeOfTest ( const QString & fileMD5Id )
{
	if ( !metadata->contains ( fileMD5Id ) ) return QString();
	return metadata->value ( fileMD5Id )->getTypeOfTest();
}

NMGMetaData * NMGStatisticFileLoader::getMetadata ( const QString & testPathId, const QString & fileMD5Id )
throw ( NMGStatisticError )
{
	QString id = fileMD5Id.isEmpty() ? DMD5Hash::hashFile ( testPathId ) : fileMD5Id;
	if ( !metadata->contains ( id ) )
		throw NMGStatisticError ( QObject::tr ( "Loader error" ),
		                          QObject::tr ( "Metadata from file " ) + testPathId +
		                          QObject::tr ( " is unavailable or the file hasn't been loaded previously" ),
		                          NMGStatisticError::CRITICAL );

	return metadata->value ( id );
}

int NMGStatisticFileLoader::removeMetadataItem ( const QString & id )
{
	if ( !metadata->contains ( id ) ) return 0;
	else
	{
		// delete because we are using pointers to structures
		// and we must free memory before delete item
		delete metadata->find ( id ).value();

		//metadata->take ( id );
		return metadata->remove ( id );
	}
}

NMGStatisticItemDataList NMGStatisticFileLoader::getStatistics ( const QString & testPathId, const QString & fileMD5Id )
throw ( NMGStatisticError )
{
	QString id = fileMD5Id.isEmpty() ? DMD5Hash::hashFile ( testPathId ) : fileMD5Id;
	if ( !statistics->contains ( id ) )
	{
		throw NMGStatisticError ( QObject::tr ( "Loader error" ),
		                          QObject::tr ( "Data from file " ) + testPathId +
		                          QObject::tr ( " is unavailable or the file hasn't been loaded previously" ),
		                          NMGStatisticError::CRITICAL );
	}
	return statistics->values ( id );
}

NMGStatisticItemData * NMGStatisticFileLoader::getStatistics ( const QString & testPathId, int flowId, const QString & fileMD5Id )
throw ( NMGStatisticError )
{
	QString id = fileMD5Id.isEmpty() ? DMD5Hash::hashFile ( testPathId ) : fileMD5Id;
	if ( !statistics->contains ( id ) )
		throw NMGStatisticError ( QObject::tr ( "Loader error" ),
		                          QObject::tr ( "Data from file " ) + testPathId +
		                          QObject::tr ( " is unavailable or the file hasn't been loaded previously" ),
		                          NMGStatisticError::CRITICAL );
	if ( flowId == -1 )
		return statistics->value ( id );
	else
	{
		NMGStatisticItemDataListIterator flowListIterator = statistics->values ( id ).begin();
		NMGStatisticItemDataListIterator flowListIteratorEnd = statistics->values ( id ).end();
		while ( ( flowListIterator != flowListIteratorEnd )
		        and ( ( ( NMGStatisticPacketData * ) *flowListIterator )->flowId() != flowId ) ) ++flowListIterator;

		if ( flowListIterator == flowListIteratorEnd )
			throw NMGStatisticError ( QObject::tr ( "Loader error" ),
			                          QObject::tr ( "Data from file " ) + testPathId + QObject::tr ( " and flow " ) + INTEGER ( flowId ) +
			                          QObject::tr ( " is unavailable or the file hasn't been loaded previously" ),
			                          NMGStatisticError::CRITICAL );
		return *flowListIterator;
	}
}

NMGStatisticAggregationData * NMGStatisticFileLoader::getStatistics ( const QStringList & idList )
throw ( NMGStatisticError )
{
	double throughtput, average = 0.0, min = DBL_MAX, max = DBL_MIN;

	foreach ( QString id, idList )
	{
		if ( QFile::exists ( id ) ) id = DMD5Hash::hashFile ( id );
		if ( !statistics->contains ( id ) )
			throw NMGStatisticError ( QObject::tr ( "Loader error" ),
			                          QObject::tr ( "Data from file " ) + id +
			                          QObject::tr ( " is unavailable or the file hasn't been loaded previously" ),
			                          NMGStatisticError::CRITICAL );
		else
		{
			throughtput = ( ( NMGStatisticThroughtputData * ) statistics->value ( id ) )->getReceivingThroughputInKBitsPerSecond();
			average += throughtput;
			if ( throughtput < min ) min = throughtput;
			if ( throughtput > max ) max = throughtput;
		}
	}
	return new NMGStatisticAggregationData ( ( average / idList.count() ), min, max );
}

void NMGStatisticFileLoader::loadFile ( const QString & fileName )
{
	filesToLoad.append ( fileName );
	emit newFilesAdded();
}

void NMGStatisticFileLoader::loadFiles ( const QStringList & fileNames )
{
	filesToLoad += fileNames;
	emit newFilesAdded();
}

void NMGStatisticFileLoader::loadPendingFiles()
{
	if ( !filesToLoad.isEmpty() )
	{
		if ( !readerThread ) //This must be changed if we want multithreading parsing
		{
			QString filePathId = filesToLoad.takeFirst();

			readerThread = new NMGStatisticThread ( statistics, metadata, filePathId );

			progress = ProgressManager::createProgressItem (
			               QString ( tr ( "Loading %1 " ) ).arg ( filePathId.right ( filePathId.lastIndexOf ( "/" ) + 1 ) ) );
			progress->setTotalItems ( 100 );
			connect ( progress, SIGNAL ( progressItemCanceled ( ProgressItem* ) ),
			          this, SLOT ( slotLoadCancelled ( ProgressItem* ) ) );

			readerThread->start();
			timer->start ( 100 );
		}
	}
	else
		MessageBar::longMessage ( tr ( "All files loaded" ) );
}

void NMGStatisticFileLoader::slotUpdateProgress()
{
	if ( readerThread->isFinished() )
	{
		timer->stop();
		progress->setComplete();
		progress = 0;

		if ( !readerThread->hasError() )
			emit signalFileRead ( readerThread->getFileLoaded(), readerThread->getFileLoadedMD5Id() );
		else if ( userCancelled )
		{
			MessageBar::longMessage ( tr ( "Load cancelled by the user" ) );
			userCancelled = false;
		}
		else if ( readerThread->getError() == TESTALREADYPARED )
			MessageBar::longMessage ( tr ( TESTALREADYPARED ) );
		else
		{
			NMGStatisticError ( tr ( "Loader error" ), readerThread->getError(), NMGStatisticError::CRITICAL ).raiseError();
			MessageBar::longMessage ( tr ( "File loading aborted" ) );
		}

		delete readerThread;
		readerThread = 0;
		emit newFilesAdded();
	}
	else
	{
		progress->setStatus ( readerThread->getProcessStatus() );
		progress->setCompletedItems ( readerThread->getProcessPercent() );
		progress->updateProgress();
	}
}

void NMGStatisticFileLoader::slotLoadCancelled ( ProgressItem * )
{
	userCancelled = true;
	readerThread->kill();
}

#include "nmgstatisticfileloader.moc"
