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
#include "nmgstatisticremotewidget.h"
#include "nmgstatisticxmltags.h"
#include <nmgmetadatatags.h>

#include <QStringList>
#include <QXmlStreamReader>

#include <iostream>
using namespace std;

/********************************************************************/
/** NMGStatisticRemoteContainer **/
/********************************************************************/
NMGStatisticRemoteContainer::NMGStatisticRemoteContainer ( QString & items, QString & statistics )
{
	error = false;
	innerCounter = 0;
	itemList = items.split ( ";" );
	
	if ( itemList.isEmpty() )
		itemList.append ( "ALL" );
	else
	{
		QRegExp rx ( "^[0-9]+..[0-9]+" );
		int index = itemList.indexOf ( rx );
		while ( index != -1 )
		{
			QString range = itemList.at ( index );
			int position = range.indexOf ( ".." );
			int begin = range.left ( position ).toInt();
			int end = range.right ( range.size() - ( position + 2 ) ).toInt();
			itemList.replace ( index, INTEGER ( begin ) );
			for ( int i = begin + 1; i <= end; ++i )
				itemList.insert ( ++index, INTEGER ( i ) );

			index = itemList.indexOf ( rx, ++index );
		}
	}

	QStringList statistisList = statistics.split ( ";" );

	if ( statistisList.isEmpty() )
	{
		basicStatistisList << TAG_SENT << TAG_RECEIVED << TAG_DURATION << TAG_LOST << TAG_LOSS_RATIO << TAG_DUPLICATED << TAG_DUPLICATED_RATIO << TAG_OUTOFORDER << TAG_OUTOFORDER_RATIO;
		delayStatistisList << TAG_AVERAGE << TAG_MIN << TAG_MAX << TAG_STDEV << TAG_JITTER << TAG_P50 << TAG_P95 << TAG_P99 << TAG_P999;
		ipdvStatistisList << TAG_AVERAGE << TAG_MIN << TAG_MAX << TAG_STDEV << TAG_JITTER << TAG_P50 << TAG_P95 << TAG_P99 << TAG_P999;
	}
	else
	{
		basicStatistisList = delayStatistisList = ipdvStatistisList = QStringList();

		for ( int i = 0; i < statistisList.count(); ++i )
		{
			if ( QString::compare ( statistisList.at ( i ), "ALL", Qt::CaseInsensitive ) == 0 )
			{
				basicStatistisList = QStringList() << TAG_SENT << TAG_RECEIVED << TAG_DURATION << TAG_LOST << TAG_LOSS_RATIO << TAG_DUPLICATED << TAG_DUPLICATED_RATIO << TAG_OUTOFORDER << TAG_OUTOFORDER_RATIO;
				delayStatistisList = QStringList() << TAG_AVERAGE << TAG_MIN << TAG_MAX << TAG_STDEV << TAG_JITTER << TAG_P50 << TAG_P95 << TAG_P99 << TAG_P999;
				ipdvStatistisList = QStringList() << TAG_AVERAGE << TAG_MIN << TAG_MAX << TAG_STDEV << TAG_JITTER << TAG_P50 << TAG_P95 << TAG_P99 << TAG_P999;
				break;
			}

			else if ( statistisList.at ( i ).startsWith ( TAG_DELAY, Qt::CaseInsensitive ) )
			{
				if ( delayStatistisList.isEmpty () )
				{
					QString str = statistisList.at ( i ).mid ( 6, statistisList.at ( i ).size() - 7 );
					if ( str.isEmpty () )
						delayStatistisList << TAG_AVERAGE << TAG_MIN << TAG_MAX << TAG_STDEV << TAG_JITTER << TAG_P50 << TAG_P95 << TAG_P99 << TAG_P999;
					else
						delayStatistisList = str.split ( "," );
				}
				else error = true;
			}

			else if ( statistisList.at ( i ).startsWith ( TAG_IPDV, Qt::CaseInsensitive ) )
			{
				if ( ipdvStatistisList.isEmpty () )
				{
					QString str = statistisList.at ( i ).mid ( 5, statistisList.at ( i ).size() - 6 );
					if ( str.isEmpty () )
						ipdvStatistisList << TAG_AVERAGE << TAG_MIN << TAG_MAX << TAG_STDEV << TAG_JITTER << TAG_P50 << TAG_P95 << TAG_P99 << TAG_P999;
					else
						ipdvStatistisList = str.split ( "," );
				}
				else error = true;
			}

			else
				basicStatistisList.append ( statistisList.at ( i ) );
		}
	}
}

NMGStatisticRemoteContainer::NMGStatisticRemoteContainer ( QString & statistics )
{
	error = false;
	innerCounter = 0;
	basicStatistisList = statistics.split ( ";" );

	if ( basicStatistisList.isEmpty() or basicStatistisList.contains ( "ALL", Qt::CaseInsensitive ) )
		basicStatistisList << TAG_AVERAGE << TAG_MIN << TAG_MAX;
}


/********************************************************************/
/** NMGStatisticRemoteWidget **/
/********************************************************************/
NMGStatisticRemoteWidget::NMGStatisticRemoteWidget ( NMGModuleManager * modmanager, QString currentTestId )
		: moduleManager ( modmanager )
{
	setObjectName ( "NMGStatisticRemoteWidget" );
	testId = currentTestId;
	fileLoader = new NMGStatisticFileLoader();
	connect ( fileLoader, SIGNAL ( signalFileRead ( const QString&, const QString& ) ),
	          this, SLOT ( updateWidgetBecauseOfTestEnd ( const QString &, const QString & ) ) );
}

NMGStatisticRemoteWidget::~NMGStatisticRemoteWidget()
{
	delete fileLoader;
}

// Data loading
bool NMGStatisticRemoteWidget::loadData ( QString data )
{
	QXmlStreamReader xml ( data );

	/** \par Received Message Format for "packets" tests
	 * <data>
	 *  	<test type="packets">
	 *		<path>testPath</path>
	 *  		<flows>1;3..6;8</flows>
	 *		<statistic>SENT;...;IPDV(P999)</statistic>
	 *  	</test>
	 * </data>
	 *
	 * \par Received Message Format for "throughput" tests
	 * <data>	
	 *  	<test type="throughput">
	 *		<paths>
	 *			<path>testPath_1</path>
	 *			...
	 *			<path>testPath_N</path>
	 *		</paths>
	 *		<statistic>MIN;AVE;MAX</statistic>
	 *  	</test>
	 * </data>
	 *
	 * \par More Information
	 * \note The parsers are Case Insensitive
	 * \note ATTR "type" of <test> should be "packets" (TYPE_PACKETS) or "throughput" (TYPE_TPUT)
	 * \note TAG <flows> should be like a list of flowIds separated by semicolons like 1;2;3;...;N or ranges like 1..N or ALL
	 * \note TAG <statistics> should be a list of tags separated by semicolons. Available tags are:
	 *	For "packets" tests:
	 *      	ALL (if we want ALL statistics, we only need this tag)
	 *		SENT,
	 *		RECEIVED,
	 *		DURATION,
	 *		LOST,
	 *		LOSS_RATIO,
	 *		DUPLICATED,
	 *		DUPLICATED_RATIO,
	 *		OUTOFORDER,
	 *		OUTOFORDER_RATIO,
	 *		DELAY(AVERAGE,MIN,MAX,STDEV,JITTER,P50,P95,P99,P999) or DELAY() for all values,
	 *		IPDV(AVERAGE,MIN,MAX,STDEV,JITTER,P50,P95,P99,P999) or IPDV() for all values
	 *	For "throughput" tests:
	 *      	ALL (if we want ALL statistics, we only need this tag)
	 *		MIN
	 *		MAX
	 *		AVE
	 **/

	bool error = true;

	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() )
		{
			if ( xml.name() == TAG_DATA ) error = false;
			else if ( xml.name() == TAG_TEST and !error )
			{
				if ( QString::compare ( xml.attributes().value ( ATTR_TYPE ).toString(), 
				                        TYPE_PACKETS, Qt::CaseInsensitive ) == 0 )
					error = loadPacketData ( &xml );
				else if ( QString::compare ( xml.attributes().value ( ATTR_TYPE ).toString(), 
							TYPE_TPUT, Qt::CaseInsensitive ) == 0 )
					error = loadThroughtputData ( &xml );
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

// Data loading: Packets
bool NMGStatisticRemoteWidget::loadPacketData ( QXmlStreamReader * xml )
{
	QString filePath, items, statistics;
	bool error = true;

	while ( !xml->atEnd() )
	{
		xml->readNext();
		if ( xml->isStartElement() )
		{
			if ( xml->name() == TAG_PATH )
			{
				filePath = xml->readElementText();
				if ( ( error = filePath.isEmpty() ) ) xml->raiseError ( "[ERROR] Empty test path" );
			}
			else if ( xml->name() == TAG_FLOWS and !error )
				items = xml->readElementText();
			else if ( xml->name() == TAG_STATISTIC and !error )
				statistics = xml->readElementText();
			else
			{
				xml->raiseError ( "[ERROR] Unknown Tag" );
				return true;
			}
		}
		else if ( xml->isEndElement() )
		{
			if ( xml->name() == TAG_TEST )
			{
				NMGStatisticRemoteContainer * container = new NMGStatisticRemoteContainer ( items, statistics );
				if ( ( error = container->hasError() ) ) xml->raiseError ( "[ERROR] Error in statistics or flows requested" );
				else
				{
					exportListMap.insert ( filePath, container );
					fileLoader->loadFile ( filePath );
				}
				break;
			}
		}
	}

	return error;
}

// Data loading: Throughput
bool NMGStatisticRemoteWidget::loadThroughtputData ( QXmlStreamReader * xml )
{
	QStringList filePaths;
	QString statistics;
	bool error = true;

	while ( !xml->atEnd() )
	{
		xml->readNext();
		if ( xml->isStartElement() )
		{
			if ( xml->name() == TAG_PATHS )
			{
				filePaths = QStringList();
				error = false;
			}
			else if ( xml->name() == TAG_PATH and !error )
			{
				QString path = xml->readElementText();
				if ( ( error = path.isEmpty() ) ) xml->raiseError ( "[ERROR] Empty test path" );
				else filePaths.append ( path );
			}
			else if ( xml->name() == TAG_STATISTIC and !error )
				statistics = xml->readElementText();
			else
			{
				xml->raiseError ( "[ERROR] Unknown Tag" );
				return true;
			}
		}
		else if ( xml->isEndElement() )
		{
			if ( xml->name() == TAG_TEST )
			{
				NMGStatisticRemoteContainer * container = new NMGStatisticRemoteContainer ( statistics );
				container->itemList = filePaths;
				container->innerCounter = filePaths.count();

				foreach (QString path, filePaths)
				{
					exportListMap.insert ( path, container );
					fileLoader->loadFile ( path );
				}
				break;
			}
		}
	}

	return error;
}

// Test parsed
void NMGStatisticRemoteWidget::updateWidgetBecauseOfTestEnd ( const QString & filePathId, const QString & fileMD5Id )
{
	QString result;
	QXmlStreamWriter xmlw ( &result );
	xmlw.setAutoFormatting ( true );

	/** \par Reply Message Format For "packets" tests
	*   <data>
	*	<test path="testPath" >
	*		<flow id="1">
	*			<sent>0</sent>
	*			<received>0</received>
	*			<duration>0.000</duration>
	*			<lost>0</lost>
	*			<loss_ratio>0.000</loss_ratio>
	*			<duplicated>0</duplicated>
	*			<duplicated_ratio>0.000</duplicateed_ratio>
	*			<outoforder>0</outoforder>
	*			<outoforder_ratio>0.000</outoforder_ratio>
	*			<delay>
	*				<average>0.000</average>
	*				<min>0.000</min>
	*				<max>0.000</max>
	*				<stdev>0.000</stdev>
	*				<jitter>0.000</jitter>
	*				<p50>0.000</p50>
	*				<p95>0.000</p95>
	*				<p99>0.000</p99>
	*				<p999>0.000</p999>
	*			</delay>
	*			<ipdv>
	*				<average>0.000</average>
	*				<min>0.000</min>
	*				<max>0.000</max>
	*				<stdev>0.000</stdev>
	*				<jitter>0.000</jitter>
	*				<p50>0.000</p50>
	*				<p95>0.000</p95>
	*				<p99>0.000</p99>
	*				<p999>0.000</p999>
	*			</ipdv>
	*		</flow>
	*	</test>
	* </data>
	* 
	* \par Reply Message Format For "throughput" tests
	*  <data>
	*	 <test>
	*		<paths>
	*			<path>testPath_1</path>
	*			...
	*			<path>testPath_N</path>
	*		</paths>
	*		<statistic>
	*			<average>0.000</average>
	*			<min>0.000</min>
	*			<max>0.000</max>
	*		</statistic>
	*	</test>
	*   </data>
	**/

	xmlw.writeStartElement ( TAG_DATA );
	xmlw.writeStartElement ( TAG_TEST );
	
	if (fileLoader->getTypeOfTest(fileMD5Id) == TYPE_PACKETS)
	{
		xmlw.writeAttribute ( TAG_PATH, filePathId );
	
		if ( QString::compare ( ( exportListMap.value ( filePathId ) )->itemList.at ( 0 ), "ALL", Qt::CaseInsensitive ) == 0 )
		{
			NMGStatisticItemDataList flowList;
			try
			{
				flowList = fileLoader->getStatistics ( filePathId, fileMD5Id );
			}
			catch ( NMGStatisticError e ) { e.raiseError(); }
	
			NMGStatisticItemDataListIterator it = flowList.end();
			while ( it != flowList.begin() )
			{
				--it;
				xmlw.writeStartElement ( TAG_FLOW );
				xmlw.writeAttribute ( ATTR_ID, INTEGER ( ( ( NMGStatisticPacketData * ) *it )->flowId() ) );
				generateFlow ( &xmlw, ( NMGStatisticPacketData * ) *it, filePathId );
				xmlw.writeEndElement();
			}
		}
		else
		{
			foreach ( QString flowId, ( exportListMap.value ( filePathId ) )->itemList )
			{
				NMGStatisticPacketData * flow;
				try
				{
					flow = ( NMGStatisticPacketData * ) fileLoader->getStatistics ( filePathId, flowId.toInt(), fileMD5Id );
				}
				catch ( NMGStatisticError e ) { e.raiseError(); }
	
				xmlw.writeStartElement ( TAG_FLOW );
				xmlw.writeAttribute ( ATTR_ID, flowId );
				generateFlow ( &xmlw, flow, filePathId );
				xmlw.writeEndElement();
			}
		}
	}
	else if (fileLoader->getTypeOfTest(fileMD5Id) == TYPE_TPUT)
	{
		(exportListMap.value( filePathId )->innerCounter)--;
		if ((exportListMap.value( filePathId ))->innerCounter == 0)
		{
			xmlw.writeStartElement ( TAG_PATHS );

			QStringList list = ( exportListMap.value ( filePathId ) )->itemList;
			foreach ( QString path, list )
				xmlw.writeTextElement ( TAG_PATH, path );
			
			xmlw.writeEndElement();
			
			NMGStatisticAggregationData * aggregatedData;
			try
			{
				aggregatedData = fileLoader->getStatistics( list );
			}
			catch ( NMGStatisticError e ) { e.raiseError(); }
			
			xmlw.writeStartElement ( TAG_STATISTIC );

			list = ( exportListMap.value ( filePathId ) )->basicStatistisList;
			foreach ( QString statisticItem, list )
			{
				// Basic Statistics
				if ( QString::compare ( statisticItem, TAG_AVERAGE, Qt::CaseInsensitive ) == 0 )
					xmlw.writeTextElement ( TAG_AVERAGE, DOUBLE ( aggregatedData->averageThroughtput() ) );
				else if ( QString::compare ( statisticItem, TAG_MIN, Qt::CaseInsensitive ) == 0 )
					xmlw.writeTextElement ( TAG_MIN, DOUBLE ( aggregatedData->minThroughtput() ) );
				else if ( QString::compare ( statisticItem, TAG_MAX, Qt::CaseInsensitive ) == 0 )
					xmlw.writeTextElement ( TAG_MAX, DOUBLE ( aggregatedData->maxThroughtput() ) );
			}
			
			xmlw.writeEndElement();
		}
	}
	
	xmlw.writeEndElement();
	xmlw.writeEndElement();

	moduleManager->emitFinishedGUIRequest ( testId, result );
}

// Test parsed: generate packet item
void NMGStatisticRemoteWidget::generateFlow ( QXmlStreamWriter * xmlw, NMGStatisticPacketData * flow, const QString & filePathId )
{
	QStringList statisticItemList = ( exportListMap.value ( filePathId ) )->basicStatistisList;
	foreach ( QString statisticItem, statisticItemList )
	{
		// Basic Statistics
		if ( QString::compare ( statisticItem, TAG_SENT, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_SENT, INTEGER ( flow->sentPackets() ) );
		if ( QString::compare ( statisticItem, TAG_RECEIVED, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_RECEIVED, INTEGER ( flow->receivedPackets() ) );
		if ( QString::compare ( statisticItem, TAG_DURATION, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_DURATION, DOUBLE ( flow->duration() ) );
		else if ( QString::compare ( statisticItem, TAG_LOST, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_LOST, INTEGER ( flow->lostPackets() ) );
		else if ( QString::compare ( statisticItem, TAG_LOSS_RATIO, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_LOSS_RATIO, DOUBLE ( flow->lossRatio() ) );
		else if ( QString::compare ( statisticItem, TAG_DUPLICATED, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_DUPLICATED, INTEGER ( flow->duplicatedPackets() ) );
		else if ( QString::compare ( statisticItem, TAG_DUPLICATED_RATIO, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_DUPLICATED_RATIO, DOUBLE ( flow->duplicatedRatio() ) );
		else if ( QString::compare ( statisticItem, TAG_OUTOFORDER, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_OUTOFORDER, INTEGER ( flow->outOfOrder() ) );
		else if ( QString::compare ( statisticItem, TAG_OUTOFORDER_RATIO, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_OUTOFORDER_RATIO, DOUBLE ( flow->outOfOrderRatio() ) );
	}

	statisticItemList = ( exportListMap.value ( filePathId ) )->delayStatistisList;
	if ( !statisticItemList.isEmpty() ) xmlw->writeStartElement ( TAG_DELAY );
	foreach ( QString statisticItem, statisticItemList )
	{
		// Delay
		if ( QString::compare ( statisticItem, TAG_MAX, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_MAX, DOUBLE ( flow->maxDelay() ) );
		else if ( QString::compare ( statisticItem, TAG_MIN, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_MIN, DOUBLE ( flow->minDelay() ) );
		else if ( QString::compare ( statisticItem, TAG_AVERAGE, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_AVERAGE, DOUBLE ( flow->averageDelay() ) );
		else if ( QString::compare ( statisticItem, TAG_STDEV, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_STDEV, DOUBLE ( flow->stDevDelay() ) );
		else if ( QString::compare ( statisticItem, TAG_JITTER, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_JITTER, DOUBLE ( flow->jitterDelay() ) );
		else if ( QString::compare ( statisticItem, TAG_P50, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P50, DOUBLE ( flow->delay50Percentile() ) );
		else if ( QString::compare ( statisticItem, TAG_P95, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P95, DOUBLE ( flow->delay95Percentile() ) );
		else if ( QString::compare ( statisticItem, TAG_P99, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P99, DOUBLE ( flow->delay99Percentile() ) );
		else if ( QString::compare ( statisticItem, TAG_P999, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P999, DOUBLE ( flow->delay999Percentile() ) );
	}
	if ( !statisticItemList.isEmpty() ) xmlw->writeEndElement ();

	statisticItemList = ( exportListMap.value ( filePathId ) )->ipdvStatistisList;
	if ( !statisticItemList.isEmpty() ) xmlw->writeStartElement ( TAG_IPDV );
	foreach ( QString statisticItem, statisticItemList )
	{
		// IPDV
		if ( QString::compare ( statisticItem, TAG_MAX, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_MAX, DOUBLE ( flow->maxIPDV() ) );
		else if ( QString::compare ( statisticItem, TAG_MIN, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_MIN, DOUBLE ( flow->minIPDV() ) );
		else if ( QString::compare ( statisticItem, TAG_AVERAGE, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_AVERAGE, DOUBLE ( flow->averageIPDV() ) );
		else if ( QString::compare ( statisticItem, TAG_STDEV, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_STDEV, DOUBLE ( flow->stDevIPDV() ) );
		else if ( QString::compare ( statisticItem, TAG_JITTER, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_JITTER, DOUBLE ( flow->jitterIPDV() ) );
		else if ( QString::compare ( statisticItem, TAG_P50, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P50, DOUBLE ( flow->ipdv50Percentile() ) );
		else if ( QString::compare ( statisticItem, TAG_P95, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P95, DOUBLE ( flow->ipdv95Percentile() ) );
		else if ( QString::compare ( statisticItem, TAG_P99, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P99, DOUBLE ( flow->ipdv99Percentile() ) );
		else if ( QString::compare ( statisticItem, TAG_P999, Qt::CaseInsensitive ) == 0 )
			xmlw->writeTextElement ( TAG_P999, DOUBLE ( flow->ipdv999Percentile() ) );
	}
	if ( !statisticItemList.isEmpty() ) xmlw->writeEndElement ();
}

#include "nmgstatisticremotewidget.moc"
