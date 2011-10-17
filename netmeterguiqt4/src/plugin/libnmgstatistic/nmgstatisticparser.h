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
#ifndef NMGSTATISTICPARSER_H
#define NMGSTATISTICPARSER_H

#include "nmgstatisticdata.h"

#include <nmgpacketorienteddataparser.h>
#include <nmgaggregationorienteddataparser.h>

#include <QTime>
#include <QBitArray>
#include <QList>
#include <QFile>

#include <math.h>

/*! \def MSECS Conversion factor from microseconds (us) to miliseconds (ms) [double] */
#define MSECS	1000.0
/*! \def USECS Conversion factor from seconcs (secs) to microseconds (us) */
#define USECS	1000000

/*! \brief NMGPacketStatisticParser is a class with the specific behaviour needed
 * by the Statistic plugin to support packet oriented data parsing.
 *
 * NMGPacketStatisticParser is a derived class of NMGPacketOrientedDataParser
 * that decides which treatment do with each packet value information. This derived class
 * defines the virtual methods doProcess() and doWhenFlowFinished() that are called 
 * when sequence and flow values has been read respectively. */
class NMGPacketStatisticParser : public NMGPacketOrientedDataParser
{
	public:
		/** \brief Default constructor.
		 * \param stat[out] Statistics container reference to.
		 * \param md5id[in] MD5 file id
		 * \param mgenConf[in] MGen configuration in XML extracted from metadata information to calculate 
		 * the number of packets of each flow */
		inline NMGPacketStatisticParser( NMGStatisticData * stats, QString & md5id, QString mgenConf )
				: statistics( stats ), fileMD5Id( md5id )
		{ /*getNumPackets( mgenConf );*/ cleanValues(); };

		/** \brief Default destructor. */
		inline ~NMGPacketStatisticParser() {};

		/** \brief Prepares the parsed information to make statistical operations. */
		void doProcess();

		/** \brief Fills the container with the statistic information processed. */
		void doWhenFlowFinished();

	private:
		/** \brief Does the statistic operations when all seq of a flow are parsed. */
		void makeCalc();

		/** \brief Attaches the statistic information to the general container. */
		void addStatisticFlowData();

		// Packets
		inline int getSentPackets() { return count + getLostPackets(); }
		inline int getDuplicatedPackets() { return dupl; }
		inline int getLostPackets() { return seqList.count( false ); }
		inline int getOutOfOrderPackets() { return outoforder; }
		inline double getDuration() { return ( double ) (endTx - startTx) / MSECS; } // in miliseconds
	
		// Delay
		// Results in miliseconds (aka msecs or ms)
		inline double getAverageDelay() { return numDelays ? ( double )( avgDelay / numDelays ) / MSECS : 0.0; }
		inline double getMinDelay() { return ( double ) minDelay / MSECS; }
		inline double getMaxDelay() { return ( double ) maxDelay / MSECS; }
		inline double getStDevDelay() { return stDevDelay / MSECS; }
		inline double getJitterDelay() { return ( double ) delayJitter / MSECS; }
		inline double getDelay50Percentile() { return ( double ) delayPercentile50 / MSECS; }
		inline double getDelay95Percentile() { return ( double ) delayPercentile95 / MSECS; }
		inline double getDelay99Percentile() { return ( double ) delayPercentile99  / MSECS; }
		inline double getDelay999Percentile() { return ( double ) delayPercentile999 / MSECS; }

		// IPDV
		// Results in miliseconds (aka msecs or ms)
		inline double getAverageIPDV() { return numIPDV ? ( double )( avgIPDV / numIPDV ) / MSECS : 0.0; }
		inline double getMinIPDV() { return ( double ) minIPDV / MSECS; }
		inline double getMaxIPDV() { return ( double ) maxIPDV / MSECS; }
		inline double getStDevIPDV() { return stDevIPDV / MSECS; }
		inline double getJitterIPDV() { return ( double ) ipdvJitter / MSECS; }
		inline double getIPDV50Percentile() { return ( double ) ipdvPercentile50 / MSECS; }
		inline double getIPDV95Percentile() { return ( double ) ipdvPercentile95 / MSECS; }
		inline double getIPDV99Percentile() { return ( double ) ipdvPercentile99 / MSECS; }
		inline double getIPDV999Percentile() { return ( double ) ipdvPercentile999 / MSECS; }

		inline bool isDelayNegative() { return negativeDelay; }

		/** \brief From the XML Metadata MGen configuration calculates the number of packets that 
		 * must be sent at each flow (in order to calculate duplicated and lost sequences. */
		void getNumPackets( QString & );
	
		/** \brief Cleans local values. */
		void cleanValues();

		// Data containers
		NMGStatisticData * statistics;
		QString fileMD5Id;

		// Local Variables
		QHash<int, QString> flowNames;

		QList<int> numOfPacketsList, delayList, ipdvList;
		QList<QPair<long long,int> > oooList;
		QBitArray seqList; /*!< List of bits as long as the number of sequences a flow has. 
		                    * Used to calculate duplicated and lost packets */

		int oldSeq, dupl, count, outoforder, numDelays, numIPDV;
		long long currentTx, currentRx, startTx, endTx;
		int currentDelay, oldDelay;
		int minIPDV, maxIPDV, minDelay, maxDelay;
		long long avgDelay, avgIPDV;
		double stDevDelay, stDevIPDV;
		int delayJitter, ipdvJitter;
		int delayPercentile50, delayPercentile95, delayPercentile99, delayPercentile999;
		int ipdvPercentile50, ipdvPercentile95, ipdvPercentile99, ipdvPercentile999;

		bool negativeDelay;
};

/*! \brief NMGAggregationStatisticParser is a class with the specific behaviour needed
 * by the Statistic plugin to support aggregation oriented data parsing.
 *
 * NMGAggregationStatisticParser is a derived class of NMGAggregationOrientedDataParser
 * that decides which treatment do with the aggregated values information. This derived class
 * defines the virtual method doProcess() called when aggregated values has been read. */
class NMGAggregationStatisticParser : public NMGAggregationOrientedDataParser
{
	public:
		/*! \brief Class constructor.
		 * \param stat[out] Statistics container reference to.
		 * \param md5id[in] MD5 file id */
		inline NMGAggregationStatisticParser( NMGStatisticData * stats, QString & md5id )
				: statistics( stats ), fileMD5Id( md5id ) {}

		/*! \brief Default destructor. */
		inline ~NMGAggregationStatisticParser() {}

		/*! \brief Gets aggregated information and saves it into the NMGAggregationOrientedResultData structure.*/
		void doProcess();

	private:
		//Data containers
		NMGStatisticData * statistics;
		QString fileMD5Id;

};
#endif
