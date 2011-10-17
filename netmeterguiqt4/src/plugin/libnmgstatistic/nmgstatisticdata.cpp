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
#include "nmgstatisticdata.h"
#include <nmgpreferencesmanager.h>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>

#include <iostream>
using namespace std;

/********************************************************************/
/** NMGStatisticPacketData **/
/********************************************************************/
NMGStatisticPacketData::NMGStatisticPacketData( NMGStatisticPacketData *inFlowData )
{
	if ( inFlowData )
	{
		type = PACKET;
		flow_name = inFlowData->flowName();
		flow_id = inFlowData->flowId();

		sent = inFlowData->sentPackets();
		dupl = inFlowData->duplicatedPackets();
		lost = inFlowData->lostPackets();
		outoforder = inFlowData->outOfOrder();
		duration_tx = inFlowData->duration();

		avg_delay = inFlowData->averageDelay();
		min_delay = inFlowData->minDelay();
		max_delay = inFlowData->maxDelay();
		stDev_delay = inFlowData->stDevDelay();
		delay_jitter = inFlowData->jitterDelay();
		delay_percentile50 = inFlowData->delay50Percentile();
		delay_percentile95 = inFlowData->delay95Percentile();
		delay_percentile99 = inFlowData->delay99Percentile();
		delay_percentile999 = inFlowData->delay999Percentile();

		avg_ipdv = inFlowData->averageIPDV();
		min_ipdv = inFlowData->minIPDV();
		max_ipdv = inFlowData->maxIPDV();
		stDev_ipdv = inFlowData->stDevIPDV();
		ipdv_jitter = inFlowData->jitterIPDV();
		ipdv_percentile50 = inFlowData->ipdv50Percentile();
		ipdv_percentile95 = inFlowData->ipdv95Percentile();
		ipdv_percentile99 = inFlowData->ipdv99Percentile();
		ipdv_percentile999 = inFlowData->ipdv999Percentile();
	}
}

/********************************************************************/
/** NMGStatisticData **/
/********************************************************************/
void NMGStatisticData::loadStatisticsFromXMLCache()
{
	QFile file( NMGPreferencesManager::self()->getConfigurationDirectory() + STATISTIC_CACHE );
	if ( !file.exists () ) return;
	else if ( file.open( QFile::ReadOnly | QFile::Text ) )
	{
		QXmlStreamReader xml( &file );
		NMGStatisticPacketData * flowData = 0;
		QString md5Id = QString();
		bool inDelay = false, inIPDV = false;
		while ( !xml.atEnd() )
		{
			xml.readNext();
			if ( xml.isStartElement() )
			{
				if ( xml.name() == TAG_FLOW )
				{
					flowData = new NMGStatisticPacketData();
					flowData->setFlowName( xml.attributes().value( ATTR_NAME ).toString() );
					flowData->setFlowId( xml.attributes().value( ATTR_ID ).toString().toInt() );
					md5Id = xml.attributes().value( ATTR_MD5 ).toString();
				}
				else if ( xml.name() == TAG_SENT )
					flowData->setSentPackets( xml.readElementText().toInt() );
				else if ( xml.name() == TAG_DURATION )
					flowData->setDuration( xml.readElementText().toDouble() );
				else if ( xml.name() == TAG_LOST )
					flowData->setLostPackets( xml.readElementText().toInt() );
				else if ( xml.name() == TAG_DUPLICATED )
					flowData->setDuplicatedPackets( xml.readElementText().toInt() );
				else if ( xml.name() == TAG_OUTOFORDER )
					flowData->setOutOfOrder( xml.readElementText().toInt() );
				
				else if ( xml.name() == TAG_DELAY ) inDelay = true;
				else if ( inDelay )
				{
					if ( xml.name() == TAG_AVERAGE ) flowData->setAverageDelay( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_MIN ) flowData->setMinDelay( xml.readElementText().toDouble( ) );
					else if ( xml.name() == TAG_MAX ) flowData->setMaxDelay( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_STDEV ) flowData->setStDevDelay( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_JITTER ) flowData->setJitterDelay( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P50 ) flowData->setDelay50Percentile( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P95 ) flowData->setDelay95Percentile( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P99 ) flowData->setDelay99Percentile( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P999 ) flowData->setDelay999Percentile( xml.readElementText().toDouble() );
				}

				else if ( xml.name() == TAG_IPDV ) inIPDV = true;
				else if ( inIPDV )
				{
					if ( xml.name() == TAG_AVERAGE ) flowData->setAverageIPDV( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_MIN ) flowData->setMinIPDV( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_MAX ) flowData->setMaxIPDV( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_STDEV ) flowData->setStDevIPDV( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_JITTER ) flowData->setJitterIPDV( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P50 ) flowData->setIPDV50Percentile( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P95 ) flowData->setIPDV95Percentile( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P99 ) flowData->setIPDV99Percentile( xml.readElementText().toDouble() );
					else if ( xml.name() == TAG_P999 ) flowData->setIPDV999Percentile( xml.readElementText().toDouble() );
				}
			}
			if ( xml.isEndElement() )
			{
				if ( xml.name() == TAG_FLOW ) insertMulti( md5Id, flowData );
				else if ( xml.name() == TAG_DELAY ) inDelay = false;
				else if ( xml.name() == TAG_IPDV ) inIPDV = false;
			}
		}
		if ( xml.hasError() )
			cerr << "[ERROR] Error loading Statistics' Cache From XML File" << endl;
	}
	else
		cerr << "[ERROR] Error opening Statistic File Cache" << endl;

	file.close();
}

void NMGStatisticData::saveStatisticsToXMLCache()
{
	QFile file( NMGPreferencesManager::self()->getConfigurationDirectory() + STATISTIC_CACHE );
	if ( file.open( QFile::WriteOnly | QFile::Text ) )
	{
		QXmlStreamWriter xmlw( &file );
		xmlw.setAutoFormatting( true );
		xmlw.writeStartDocument();
		xmlw.writeStartElement( TAG_STATISTIC );

		QHash<QString, NMGStatisticItemData *>::iterator it = end();
		while ( it-- != begin() )
		{
			if ( it.value()->getTypeOfData() == NMGStatisticItemData::PACKET )
			{
				NMGStatisticPacketData * flowData = ( NMGStatisticPacketData * ) it.value();

				xmlw.writeStartElement( TAG_FLOW );
				xmlw.writeAttribute( ATTR_ID, INTEGER( flowData->flowId() ) );
				xmlw.writeAttribute( ATTR_NAME, flowData->flowName() );
				xmlw.writeAttribute( ATTR_MD5, it.key() );

				xmlw.writeTextElement( TAG_SENT, INTEGER( flowData->sentPackets() ) );
				xmlw.writeTextElement( TAG_DURATION, DOUBLE( flowData->duration() ) );
				xmlw.writeTextElement( TAG_LOST, INTEGER( flowData->lostPackets() ) );
				xmlw.writeTextElement( TAG_DUPLICATED, INTEGER( flowData->duplicatedPackets() ) );
				xmlw.writeTextElement( TAG_OUTOFORDER, INTEGER( flowData->outOfOrder() ) );
				
				xmlw.writeStartElement( TAG_DELAY );
				xmlw.writeTextElement( TAG_AVERAGE, DOUBLE( flowData->averageDelay() ) );
				xmlw.writeTextElement( TAG_MIN, DOUBLE( flowData->minDelay() ) );
				xmlw.writeTextElement( TAG_MAX, DOUBLE( flowData->maxDelay() ) );
				xmlw.writeTextElement( TAG_STDEV, DOUBLE( flowData->stDevDelay() ) );
				xmlw.writeTextElement( TAG_JITTER, DOUBLE( flowData->jitterDelay() ) );
				xmlw.writeTextElement( TAG_P50, DOUBLE( flowData->delay50Percentile() ) );
				xmlw.writeTextElement( TAG_P95, DOUBLE( flowData->delay95Percentile() ) );
				xmlw.writeTextElement( TAG_P99, DOUBLE( flowData->delay99Percentile() ) );
				xmlw.writeTextElement( TAG_P999, DOUBLE( flowData->delay999Percentile() ) );
				xmlw.writeEndElement();

				xmlw.writeStartElement( TAG_IPDV );
				xmlw.writeTextElement( TAG_AVERAGE, DOUBLE( flowData->averageIPDV() ) );
				xmlw.writeTextElement( TAG_MIN, DOUBLE( flowData->minIPDV() ) );
				xmlw.writeTextElement( TAG_MAX, DOUBLE( flowData->maxIPDV() ) );
				xmlw.writeTextElement( TAG_STDEV, DOUBLE( flowData->stDevIPDV() ) );
				xmlw.writeTextElement( TAG_JITTER, DOUBLE( flowData->jitterIPDV() ) );
				xmlw.writeTextElement( TAG_P50, DOUBLE( flowData->ipdv50Percentile() ) );
				xmlw.writeTextElement( TAG_P95, DOUBLE( flowData->ipdv95Percentile() ) );
				xmlw.writeTextElement( TAG_P99, DOUBLE( flowData->ipdv99Percentile() ) );
				xmlw.writeTextElement( TAG_P999, DOUBLE( flowData->ipdv999Percentile() ) );
				xmlw.writeEndElement();

				xmlw.writeEndElement();
			}
		}
		xmlw.writeEndDocument();
	}
	file.close();
}

bool NMGStatisticData::deleteXMLCache()
{
	clear();
	return QFile( NMGPreferencesManager::self()->getConfigurationDirectory() + STATISTIC_CACHE ).remove();
}
