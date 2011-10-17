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
#include "nmgstatisticparser.h"
#include <QMap>
#include <QStringList>

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

/********************************************************************/
/** NMGPacketStatisticParser **/
/********************************************************************/
void NMGPacketStatisticParser::doProcess( )
{
#ifdef DEBUG
	cout << "SeqID = " << seqId << endl;
#endif

	//Lost packets method
	if ( seqId >= seqList.size() )
	{
		seqList.resize ( seqId + 1 );
#ifdef DEBUG
		cerr << "[WARNING] Bitarray in NMGPacketStatisticParser ( MD5 = " << qPrintable ( fileMD5Id );
		cerr << " ) resized for flowID=" << flowId << ". Incorrect Metadata information" << endl;
#endif
	}
	if ( seqList.testBit ( seqId ) ) dupl++;
	seqList.setBit ( seqId );

	int hoursTx, minsTx, secsTx, microsecsTx;
	int hoursRx, minsRx, secsRx, microsecsRx;

	//TXTime
	sscanf ( txTime.toAscii().data(), "%d:%d:%d.%d", &hoursTx, &minsTx, &secsTx, &microsecsTx );

	//RXTime
	sscanf ( rxTime.toAscii().data(), "%d:%d:%d.%d", &hoursRx, &minsRx, &secsRx, &microsecsRx );

	if ( microsecsTx > -1 && microsecsRx > -1 )
	{
		//Operations calculated in microseconds due to float precision problems
		currentDelay = ( ( ( hoursRx * 3600 ) + ( minsRx * 60 ) + secsRx )
		                 - ( ( hoursTx * 3600 ) + ( minsTx * 60 ) + secsTx ) ) * USECS + ( microsecsRx  - microsecsTx );

		if ( currentDelay < 0 )
		{
			raiseError ( "There are negative delays in this test at flow ("
			             + INTEGER ( flowId ) + "):seq (" + INTEGER ( seqId ) + ")" );
			negativeDelay = TRUE;
		}

		long long tx = ( long long ) ( ( hoursTx * 3600 ) + ( minsTx * 60 ) + secsTx ) * USECS + microsecsTx;
		long long rx = ( long long ) ( ( hoursRx * 3600 ) + ( minsRx * 60 ) + secsRx ) * USECS + microsecsRx;

		// Real Duration
		if ( startTx == 0 ) startTx = endTx = tx;
		else if ( tx > endTx ) endTx = tx;

		// Out-of-order
		oooList.append ( qMakePair ( rx, seqId ) );

		delayList.append ( currentDelay );

		avgDelay += currentDelay;

		if ( ( count > 0 ) && ( seqId == oldSeq + 1 ) )
		{
			ipdvList.append ( abs ( currentDelay - oldDelay ) );
			avgIPDV += abs ( ( currentDelay - oldDelay ) );
		}
		oldDelay = currentDelay;
		count++;
	}
	oldSeq = seqId;
}

void NMGPacketStatisticParser::doWhenFlowFinished()
{
	//Make calcs before processing new flows
	makeCalc();
	addStatisticFlowData();
	cleanValues();
}

void NMGPacketStatisticParser::makeCalc()
{
	double stDevSum, tmpAve;
	int maxSeqId = -1;

	// Out-of-order
	qSort ( oooList );
	for ( int i = 0; i < oooList.size(); ++i )
	{
		if ( oooList.at ( i ).second < maxSeqId ) ++outoforder;
		else maxSeqId = oooList.at ( i ).second;
	}

	// Delay
	if ( !delayList.isEmpty() )
	{
		qSort ( delayList );

		minDelay = delayList.first();
		maxDelay = delayList.last();

		numDelays = delayList.count();

		stDevSum = 0.;
		tmpAve = ( double ) ( avgDelay / numDelays );
		for ( QList<int>::iterator itd = delayList.begin(); itd != delayList.end(); ++itd )
			stDevSum += pow ( ( ( double ) ( *itd ) - tmpAve ), 2. );

		stDevDelay = ( numDelays == 1 ) ? 0. : sqrt ( stDevSum / ( numDelays - 1 ) );

		//delayPercentile50 = delayList.at ( ( int ) ceil ( 0.5 * numDelays ) - 1 );
		if ( numDelays % 2 == 0 )
			delayPercentile50 = ( delayList.at ( ( numDelays / 2 ) - 1 ) + delayList.at ( numDelays / 2 ) ) / 2;
		else
			delayPercentile50 = delayList.at ( numDelays / 2 );
		delayPercentile95 = delayList.at ( ( int ) ceil ( .95 * numDelays ) - 1 );
		delayPercentile99 = delayList.at ( ( int ) ceil ( .99 * numDelays ) - 1 );
		delayPercentile999 = delayList.at ( ( int ) ceil ( .999 * numDelays ) - 1 );

		delayJitter = delayPercentile95 - delayPercentile50;
	}

	// IPDV
	if ( !ipdvList.isEmpty() )
	{
		qSort ( ipdvList );

		minIPDV = ipdvList.first();
		maxIPDV = ipdvList.last();

		numIPDV = ipdvList.count();

		stDevSum = 0.;
		tmpAve = ( double ) ( avgIPDV / numIPDV );
		for ( QList<int>::iterator itipdv = ipdvList.begin(); itipdv != ipdvList.end(); ++itipdv )
			stDevSum += pow ( ( ( double ) ( *itipdv ) - tmpAve ), 2. );

		stDevIPDV = ( numIPDV == 1 ) ? 0. : sqrt ( stDevSum / ( numIPDV - 1 ) );

		//ipdvPercentile50 = ipdvList.at ( ( int ) ceil ( 0.5 * numIPDV ) - 1 );
		if ( numIPDV % 2 == 0 )
			ipdvPercentile50 = ( ipdvList.at ( ( numIPDV / 2 ) - 1 ) + ipdvList.at ( numIPDV / 2 ) ) / 2;
		else
			ipdvPercentile50 = ipdvList.at ( numIPDV / 2 );
		ipdvPercentile95 = ipdvList.at ( ( int ) ceil ( .95 * numIPDV ) - 1 );
		ipdvPercentile99 = ipdvList.at ( ( int ) ceil ( .99 * numIPDV ) - 1 );
		ipdvPercentile999 = ipdvList.at ( ( int ) ceil ( .999 * numIPDV ) - 1 );

		ipdvJitter = ipdvPercentile95 - ipdvPercentile50;
	}
}

void NMGPacketStatisticParser::addStatisticFlowData()
{
	NMGStatisticPacketData * currentData = new NMGStatisticPacketData();

	currentData->setFlowName ( flowNames.value ( flowId ) );
	currentData->setFlowId ( flowId );

	currentData->setSentPackets ( getSentPackets() );
	currentData->setDuplicatedPackets ( getDuplicatedPackets() );
	currentData->setLostPackets ( getLostPackets() );
	currentData->setOutOfOrder ( getOutOfOrderPackets() );
	currentData->setDuration ( getDuration() );

	currentData->setAverageDelay ( getAverageDelay() );
	currentData->setMinDelay ( getMinDelay() );
	currentData->setMaxDelay ( getMaxDelay() );
	currentData->setStDevDelay ( getStDevDelay() );
	currentData->setJitterDelay ( getJitterDelay() );
	currentData->setDelay50Percentile ( getDelay50Percentile() );
	currentData->setDelay95Percentile ( getDelay95Percentile() );
	currentData->setDelay99Percentile ( getDelay99Percentile() );
	currentData->setDelay999Percentile ( getDelay999Percentile() );

	currentData->setAverageIPDV ( getAverageIPDV() );
	currentData->setMinIPDV ( getMinIPDV() );
	currentData->setMaxIPDV ( getMaxIPDV() );
	currentData->setStDevIPDV ( getStDevIPDV() );
	currentData->setJitterIPDV ( getJitterIPDV() );
	currentData->setIPDV50Percentile ( getIPDV50Percentile() );
	currentData->setIPDV95Percentile ( getIPDV95Percentile() );
	currentData->setIPDV99Percentile ( getIPDV99Percentile() );
	currentData->setIPDV999Percentile ( getIPDV999Percentile() );

	statistics->insertMulti ( fileMD5Id, currentData );
}

void NMGPacketStatisticParser::getNumPackets ( QString & mgenConf )
{
	numOfPacketsList.clear();
	flowNames.clear();

	QMap<QString, QStringList> parsingList;

	QXmlStreamReader xml ( mgenConf );

	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() )
		{
			if ( xml.name() == TAG_ON )
			{
				flowNames.insert ( xml.attributes().value ( ATTR_FLOW_ID ).toString().toInt(), xml.attributes().value ( ATTR_NAME ).toString() );
				parsingList.insert ( xml.attributes().value ( ATTR_FLOW_ID ).toString(),
				                     QStringList() << xml.attributes().value ( ATTR_TIME ).toString() << xml.attributes().value ( ATTR_RATE ).toString() );
			}
			else if ( xml.name() == TAG_MOD )
				parsingList[xml.attributes().value ( ATTR_FLOW_ID ).toString() ]  << ( xml.attributes().value ( ATTR_TIME ).toString() ) << ( xml.attributes().value ( ATTR_RATE ).toString() );
			else if ( xml.name() == TAG_OFF )
				parsingList[xml.attributes().value ( ATTR_FLOW_ID ).toString() ] << ( xml.attributes().value ( ATTR_TIME ).toString() );
		}
	}
	if ( xml.hasError() )
		raiseError ( QObject::tr ( "Error in MGEN configuration:\n" ) + xml.errorString() );

	for ( QMap<QString, QStringList>::iterator i = parsingList.begin(); i != parsingList.end(); ++i )
	{
		int numPaq = 0;

		for ( int j = 0; j < ( ( *i ).size() - 1 ); j += 2 )
			numPaq += ( ( ( *i ).at ( j + 2 ) ).toInt() - ( ( *i ).at ( j ) ).toInt() ) * ( ( *i ).at ( j + 1 ) ).toInt();
		// Assuming Vt = 100 Mbps
		numOfPacketsList.append ( ( int ) numPaq / 1000 );
	}
}

void NMGPacketStatisticParser::cleanValues()
{
	oldSeq = -1;
	count = dupl = outoforder = 0;
	currentTx = currentRx = startTx = endTx = 0;
	currentDelay = oldDelay = 0;
	avgDelay = avgIPDV = 0;
	numDelays = numIPDV = 0;
	minIPDV = maxIPDV = minDelay = maxDelay = 0;
	avgDelay = avgIPDV = 0;
	stDevDelay = stDevIPDV = 0.0;
	delayJitter = ipdvJitter = 0;
	delayPercentile50 = delayPercentile95 = delayPercentile99 = delayPercentile999 = 0;
	ipdvPercentile50 = ipdvPercentile95 = ipdvPercentile99 = ipdvPercentile999 = 0;

	negativeDelay = FALSE;

	//if ( !numOfPacketsList.isEmpty() )
	//	seqList = QBitArray ( numOfPacketsList.takeFirst() );
	seqList = QBitArray ( 1 );
	
#ifdef DEBUG
	cout << "NUM_PACKETS = " << seqList.size() << endl;
#endif
	delayList.clear();
	ipdvList.clear();
	oooList.clear();
}

/********************************************************************/
/** NMGAggregationStatisticParser **/
/********************************************************************/
void NMGAggregationStatisticParser::doProcess()
{
	NMGStatisticThroughtputData * currentData = new NMGStatisticThroughtputData();

	// Save all common data (from TCP or UPD test)
	currentData->setSourceAddress ( sourceAddress );
	currentData->setDestinationAddress ( destinationAddress );
	currentData->setReceivingSocketSizeInBytes ( rss );
	currentData->setSendingSocketSizeInBytes ( sss );
	currentData->setSendingMessagesSizeInBytes ( sms );
	currentData->setElapsedTimeInSeconds ( time );
	currentData->setReceivingThroughputInKBitsPerSecond ( tputRecv );
	currentData->setTypeOfProtocol ( typeOfProtocol );

	// Save uncommon data (from UDP test)
	if ( typeOfProtocol == TAG_UDP )
	{
		currentData->setSendingThroughputInKBitsPerSecond ( tputSent );
		currentData->setSendingOkMessages ( okMsgSent );
		currentData->setSendingKoMessages ( koMsgSent );
		currentData->setReceivingOkMessages ( okMsgRecv );
	}

	statistics->insert ( fileMD5Id, currentData );
}
