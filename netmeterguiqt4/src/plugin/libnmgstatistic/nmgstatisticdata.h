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
#ifndef NMGSTATISTICDATA_H
#define NMGSTATISTICDATA_H

#include <nmgmetadata.h>
#include "nmgstatisticxmltags.h"
#include <nmgmetadatatags.h>
#include <nmgpacketorientedtags.h>
#include <nmgaggregationorientedtags.h>

#include <QHash>
#include <QString>
#include <QList>

// To enable full precision with the maximum number of significant digits (trailing zeroes are omitted) remove { 'f', 3 }
/*! \def DOUBLE(n) Defines the conversion from a double to a QString with 3 decimal precision*/
#define DOUBLE(n)	QString::number( n, 'f', 3 )
/*! \def INTEGER(n) Defines the conversion from an integer to a QString*/
#define INTEGER(n)	QString::number( n )

// Used as QString in NMGMetaData to identify the typeOfTest
/*! \def TYPE_PACKETS Defines the "packets" type of test, related to MGen tests*/
#define TYPE_PACKETS	"packets"
/*! \def TYPE_TPUT Defines the "throughput" type of test, related to Netperf tests*/
#define TYPE_TPUT	"throughput"

/*! \def STATISTIC_CACHE Defines the "relative path to statistic cache file*/
#define STATISTIC_CACHE "/statistic_cache.xml"

/*! \brief NMGStatisticMetaData is a QHash table that contains NMGMetaData information of loaded tests.
 * The key is a QString with the MD5 of the source file
 * \sa NMGMetaData */
typedef QHash<QString, NMGMetaData *> NMGStatisticMetaData;

/*! \brief NMGStatisticItemData is a virtual parent class that defines each type of data that could be stored.
 * \sa NMGStatisticPacketData
 * \sa NMGStatisticThroughtputData
 * \sa NMGStatisticAggregationData */
class NMGStatisticItemData
{
	public:
		/*! Defines the type of test contained in the data. */
		enum itemType { PACKET, /*!< Packet tests (MGen). */
		                THROUGHTPUT, /*!< Throughtput tests (Netperf). */
		                THRAGGREGATION  /*!< Throughtput aggregated statistics (Statistics from multiple Netperf tests). */
	              };

		/*! \brief Default constructor. */
		NMGStatisticItemData() {}
	
		/*! \brief Default destructor. */
		virtual ~NMGStatisticItemData() {}
	
		/*! \brief Returns the type of test.
		 * \return NMGStatisticItemData::itemType type of test. */
		itemType getTypeOfData() const { return type; }

	protected:
		itemType type; /*!< Type of test. */
};

/*! \brief NMGStatisticItemDataList is a QList that contains the NMGStatisticItemData items.
 * \sa NMGStatisticItemData */
typedef QList <NMGStatisticItemData *> NMGStatisticItemDataList;
typedef QList <NMGStatisticItemData *>::iterator NMGStatisticItemDataListIterator;

/*! \brief NMGStatisticPacketData is a class that contains information of a packet (Mgen) tests.
 * \sa NMGStatisticItemData */
class NMGStatisticPacketData : public NMGStatisticItemData
{
	public:
		/*! \brief Default constructor. */
		inline NMGStatisticPacketData()
		{
			type = PACKET;
			flow_name = "";
			flow_id = sent = dupl = lost = outoforder = -1;
			duration_tx = -1.0;
			min_ipdv = max_ipdv = min_delay = max_delay = -1.0;
			avg_delay = avg_ipdv = stDev_delay = stDev_ipdv = -1.0;
			delay_jitter = ipdv_jitter = delay_percentile95 = delay_percentile99 = delay_percentile999 = -1.0;
			delay_percentile50 = ipdv_percentile50 = ipdv_percentile95 = ipdv_percentile99 = ipdv_percentile999 = -1.0;
		}
	
		/*! \brief Constructs a copy of another NMGStatisticPacketData.
		 * \param inFlowData NMGStatisticPacketData source that we want to copy */
		NMGStatisticPacketData ( NMGStatisticPacketData * inFlowData );
	
		/*! \brief Default destructor. */
		inline ~NMGStatisticPacketData() {}

		/** Seters **/
		inline void setFlowName ( const QString & flowName ) { flow_name = flowName; }
		inline void setFlowId ( int flowId )  { flow_id = flowId; }

		inline void setSentPackets ( int sentPackets ) { sent = sentPackets; }
		inline void setDuplicatedPackets ( int duplPackets ) { dupl = duplPackets; }
		inline void setLostPackets ( int lostPackets ) { lost = lostPackets; }
		inline void setOutOfOrder ( int oooPackets ) { outoforder = oooPackets; }
		inline void setDuration ( double duration ) { duration_tx = duration; }

		inline void setAverageDelay ( double ave ) { avg_delay = ave; }
		inline void setMinDelay ( double min ) { min_delay = min; }
		inline void setMaxDelay ( double max ) { max_delay = max; }
		inline void setStDevDelay ( double stdev ) { stDev_delay = stdev; }
		inline void setJitterDelay ( double jitter ) { delay_jitter = jitter; }
		inline void setDelay50Percentile ( double p50delay ) { delay_percentile50 = p50delay; }
		inline void setDelay95Percentile ( double p95delay ) { delay_percentile95 = p95delay; }
		inline void setDelay99Percentile ( double p99delay ) { delay_percentile99 = p99delay; }
		inline void setDelay999Percentile ( double p999delay ) { delay_percentile999 = p999delay; }

		inline void setAverageIPDV ( double ave ) { avg_ipdv = ave; }
		inline void setMinIPDV ( double min ) { min_ipdv = min; }
		inline void setMaxIPDV ( double max ) { max_ipdv = max; }
		inline void setStDevIPDV ( double stdev ) { stDev_ipdv = stdev; }
		inline void setJitterIPDV ( double jitter ) { ipdv_jitter = jitter; }
		inline void setIPDV50Percentile ( double p50ipdv ) { ipdv_percentile50 = p50ipdv; }
		inline void setIPDV95Percentile ( double p95ipdv ) { ipdv_percentile95 = p95ipdv; }
		inline void setIPDV99Percentile ( double p99ipdv ) { ipdv_percentile99 = p99ipdv; }
		inline void setIPDV999Percentile ( double p999ipdv ) { ipdv_percentile999 = p999ipdv; }

		/** Geters **/
		inline const QString & flowName() { return flow_name; }
		inline int flowId() { return flow_id; }

		inline int sentPackets() { return sent; }
		inline int receivedPackets() { return ( sent - lost ); }
		inline int duplicatedPackets() { return dupl; }
		inline double duplicatedRatio() { return ( sent - dupl ) ? ( double ) dupl / ( sent - dupl ) : 0.0; }
		inline int lostPackets() { return lost; }
		inline double lossRatio() { return sent ? ( double ) lost / sent : 0.0; }
		inline int outOfOrder() { return outoforder; }
		inline double outOfOrderRatio() { return sent ? ( double ) outoforder / sent : 0.0; }
		inline double duration() { return duration_tx; }

		inline double averageDelay() { return avg_delay; }
		inline double minDelay() { return min_delay; }
		inline double maxDelay() { return max_delay; }
		inline double stDevDelay() { return stDev_delay; }
		inline double jitterDelay() { return delay_jitter; }
		inline double delay50Percentile() { return delay_percentile50; }
		inline double delay95Percentile() { return delay_percentile95; }
		inline double delay99Percentile() { return delay_percentile99; }
		inline double delay999Percentile() { return delay_percentile999; }

		inline double averageIPDV() { return avg_ipdv; }
		inline double minIPDV() { return min_ipdv; }
		inline double maxIPDV() { return max_ipdv; }
		inline double stDevIPDV() { return stDev_ipdv; }
		inline double jitterIPDV() { return ipdv_jitter; }
		inline double ipdv50Percentile() { return ipdv_percentile50; }
		inline double ipdv95Percentile() { return ipdv_percentile95; }
		inline double ipdv99Percentile() { return ipdv_percentile99; }
		inline double ipdv999Percentile() { return ipdv_percentile999; }

	private:
		QString flow_name;
		int flow_id, sent, dupl, lost, outoforder;
		// All result times are stored in miliseconds (aka msec or ms)
		double duration_tx;
		double min_ipdv, max_ipdv, min_delay, max_delay;
		double avg_delay, avg_ipdv, stDev_delay, stDev_ipdv;
		double delay_jitter, ipdv_jitter, delay_percentile95, delay_percentile99, delay_percentile999;
		double delay_percentile50, ipdv_percentile50, ipdv_percentile95, ipdv_percentile99, ipdv_percentile999;
};

/*! \brief NMGStatisticThroughtputData is a class that contains information of a throughtput (Netperf) test.
 \sa NMGStatisticItemData */
class NMGStatisticThroughtputData : public NMGStatisticItemData
{
	public:

		/*! \brief Default constructor. */
		inline NMGStatisticThroughtputData()
		{
			type = THROUGHTPUT;
			rss = sss = sms = okMsgSent = koMsgSent = okMsgRecv = -1;
			time = tputSent = tputRecv = -1.0;
			sourceAddress = destinationAddress = typeOfProtocol = QString();
		};

		/*! \brief Default destructor. */
		inline ~NMGStatisticThroughtputData() {};

		/*! \brief Saves the address, usually as IP format, of test origin.
		 * \param source Source address. */
		inline void setSourceAddress ( QString source ) { sourceAddress = source; }

		/*! \brief Saves the address, usually as IP format, of test destination.
		 * \param destination Destination address. */
		inline void setDestinationAddress ( QString destination ) { destinationAddress = destination; }

		/*! \brief Saves the socket size of the receiver host.
		 * \param rss Socket size in bytes. */
		inline void setReceivingSocketSizeInBytes ( int rss ) { this->rss = rss; }

		/*! \brief Saves the socket size of the sender host.
		 * \param sss Socket size in bytes. */
		inline void setSendingSocketSizeInBytes ( int sss ) { this->sss = sss; }

		/*! \brief Saves the size of messages sent by the sender host.
		 * \param sms Size of messages. */
		inline void setSendingMessagesSizeInBytes ( int sms ) { this->sms = sms; }

		/*! \brief Save the duration of the perfomance test.
		 * \param time Test duration. */
		inline void setElapsedTimeInSeconds ( double time ) { this->time = time; }

		/*! \brief Saves the aggregated value of performance, throughput, of the sender host.
		 *
		 * This value has to be defined only when the perfomance test is done with UDP packets
		 * transmission patterns.
		 * \param tput Throughput is expressed in megabits per second. */
		inline void setSendingThroughputInKBitsPerSecond ( double tput ) { tputSent = tput; }

		/*! \brief Saves the aggregated value of performance, throughput, of the receiver host.
		 *
		 * This value is defined when the performance test is indistinctly done with UDP/TCP packets
		 * transmission patterns.
		 * \param tput Throughput is expressed in megabits per second. */
		inline void setReceivingThroughputInKBitsPerSecond ( double tput ) { tputRecv = tput; }

		/*! \brief Saves the number of correct messages sent by sender host.
		 *
		 * This value has to be defined only when the perfomance test is done with UDP packets
		 * transmission patterns.
		 * \param msg Number of correct messages. */
		inline void setSendingOkMessages ( int msg ) { okMsgSent = msg; }

		/*! \brief Saves the number of incorrect messages sent by sender host.
		 *
		 * This value has to be defined only when the perfomance test is done with UDP packets
		 * transmission patterns.
		 * \param msg Number of incorrect messages. */
		inline void setSendingKoMessages ( int msg ) { koMsgSent = msg; }

		/*!\brief Saves the number of correct messages received by receiver host.
		 *
		 * This value has to be defined only when the perfomance test is done with UDP packets
		 * transmission patterns.
		 * \param msg Number of correct messages. */
		inline void setReceivingOkMessages ( int msg ) { okMsgRecv = msg; }

		/*! \brief Saves the pattern used in the perfomance test, TCP or UDP.
		 * \param protocol Protocol used. */
		inline void setTypeOfProtocol ( QString protocol ) { typeOfProtocol = protocol.toLower(); }

		/*! \brief Returns the source address, usually as IP format.
		 * \return Source address or empty string if it is not set. */
		inline QString getSourceAddress() const { return sourceAddress; }

		/*! \brief Returns the destination address, usually as IP format.
		 * \return Destination address or empty string if it is not set. */
		inline QString getDestinationAddress() const { return destinationAddress; }

		/*! \brief Returns the socket size of the receiver host.
		 * \return Socket size in bytes or -1 if it is not set. */
		inline int getReceivingSocketSizeInBytes() const { return rss; }

		/*! \brief Returns the socket size of the sender host.
		 * \return Socket size in bytes or -1 if it is not set. */
		inline int getSendingSocketSizeInBytes() const { return sss; }

		/*! \brief Returns the size of messages sent by the sender host.
		 * \return Size of messages or -1 if it is not set. */
		inline int getSendingMessagesSizeInBytes() const { return sms; }

		/*! \brief Returns the duration of the perfomance test.
		 * \return Time in seconds or -1.0 if it is not set. */
		inline double getElapsedTimeInSeconds() const { return time; }

		/*! \brief Returns the aggregated value of performance, throughput, of the sender host.
		 *
		 * This value is only defined when the perfomance test is done with UDP packets transmission
		 * patterns.
		 *
		 * \return Throughput in megabits per second or -1.0 if transmission pattern
		 * isn't of UDP type or data isn't set yet. */
		inline double getSendingThroughputInKBitsPerSecond() const { return tputSent; }

		/*! \brief Returns the aggregated value of performance, throughput, of the receiver host.
		 *
		 * This value is defined when the performance test is indistinctly done with UDP/TCP packets
		 * transmission patterns.
		 *
		 * \return Throughput in megabits per second or -1.0 if it is not set. */
		inline double getReceivingThroughputInKBitsPerSecond() const { return tputRecv; }

		/*! \brief Returns the number of correct messages sent by sender host.
		 *
		 * This value is only defined when the perfomance test is done with UDP packets transmission
		 * patterns.
		 *
		 * \return Number of correct messages or -1 if transmission pattern isn't of UDP type
		 * or data isn't set yet. */
		inline int getSendingOkMessages() const { return okMsgSent; }

		/*! \brief Returns the number of incorrect messages sent by sender host.
		 *
		 * This value is only defined when the perfomance test is done with UDP packets transmission
		 * patterns.
		 *
		 * \return Number of incorrect messages or -1 if transmission pattern isn't of UDP type
		 * or data isn't set yet. */
		inline int getSendingKoMessages() const { return koMsgSent; }

		/*! \brief Returns the number of correct messages received by receiver host.
		 *
		 * This value is only defined when the perfomance test is done with UDP packets transmission
		 * patterns.
		 *
		 * \return Number of correct messages or -1 if transmission pattern isn't of UDP type
		 * or data isn't set yet. */
		inline int getReceivingOkMessages() const { return okMsgRecv; }

		/*! \brief Returns the pattern used in the perfomance test, TCP or UDP.
		 * \return Protocol used or empty string if it is not set. */
		inline QString getTypeOfProtocol() const { return typeOfProtocol; }

	private:
		QString sourceAddress; /*!< Source address of sender host. */
		QString destinationAddress; /*!< Destination address of receiver host. */
		QString typeOfProtocol; /*!< Protocol pattern for test. */
		int rss; /*!< Receiving socket size. */
		int sss; /*!< Sending socket size. */
		int sms; /*!< Sending messages size. */
		double time; /*!< Elapsed time for test. */
		double tputSent; /*!< Throughput for sender host. */
		double tputRecv; /*!< Throughput for receiver host. */
		int okMsgSent; /*!< Number of correct messages sent. */
		int koMsgSent; /*!< Number of incorrect messages sent. */
		int okMsgRecv; /*!< Number of correct messages received. */
};

/*! \brief NMGStatisticAggregationData is a class that contains information of an statistic aggregation of throughtput result tests.
 \sa NMGStatisticItemData */
class NMGStatisticAggregationData : public NMGStatisticItemData
{
	public:
		/*! \brief Default constructor. */
		inline NMGStatisticAggregationData()
		{
			type = THRAGGREGATION;
			ave_throughtput = min_throughtput = max_throughtput = -1.0;
		}
	
		/*! \brief Default constructor with setters.
		 * \param ave Average throughtput.
		 * \param min Minimum throughtput.
		 * \param max Maximum throughtput. */
		inline NMGStatisticAggregationData ( double ave, double min, double max )
		{
			type = THRAGGREGATION;
			ave_throughtput = ave;
			min_throughtput = min;
			max_throughtput = max;
		}

		/*! \brief Default destructor. */
		inline ~NMGStatisticAggregationData() {}

		/*! \brief Save the average throughtput of the aggregation.
		 * \param ave Average throughtput. */
		inline void setAverageThroughtput ( double ave ) { ave_throughtput = ave; }
	
		/*! \brief Save the minimum throughtput of the aggregation.
		 * \param min Minimum throughtput. */
		inline void setMinThroughtput ( double min ) { min_throughtput = min; }
	
		/*! \brief Save the maximum throughtput of the aggregation.
		 * \param max Maximum throughtput. */
		inline void setMaxThroughtput ( double max ) { max_throughtput = max; }

		/*! \brief Returns the average throughtput of the aggregated tests.
		 * \return Returns the average throughtput of the aggregated tests or -1 if data isn't set yet. */
		inline double averageThroughtput() { return ave_throughtput; }
	
		/*! \brief Returns the minimum throughtput of the aggregated tests.
		 * \return Returns the minimum throughtput of the aggregated tests or -1 if data isn't set yet. */
		inline double minThroughtput() { return min_throughtput; }
	
		/*! \brief Returns the maximum throughtput of the aggregated tests.
		 * \return Returns the maximum throughtput of the aggregated tests or -1 if data isn't set yet. */
		inline double maxThroughtput() { return max_throughtput; }

	private:
		double min_throughtput; /*!< Minimum throughtput. */
		double max_throughtput; /*!< Maximum throughtput. */
		double ave_throughtput; /*!< Average throughtput. */
};

/*!
 * \brief NMGStatisticExportData is a container class to store information about the test items that will be exported.
 * This class stores in a QString the source file path, in NMGMetaData the metadata information and a list of NMGStatisticItemData items.
 * \sa NMGMetaData
 * \sa NMGStatisticItemData */
struct NMGStatisticExportData
{
	QString fileName; /*!< Source file path. */
	NMGMetaData * metadata; /*!< Metadata information of the test. */
	NMGStatisticItemDataList itemList; /*!< List of items to be exported. */
};

/*! \brief NMGStatisticExportDataList is a QList that contains the NMGStatisticExportData items.
 * \sa NMGStatisticExportData */
typedef QList <NMGStatisticExportData *> NMGStatisticExportDataList;
typedef QList <NMGStatisticExportData *>::iterator NMGStatisticExportDataListIterator;

/*!
 * \brief NMGStatisticData is a container class to store statistic information from items of different tests.
 * This class stores in each key (Test File MD5) statistic information calculed from each file
 * QHash ( <QString md5_of_test_file, NMGStatisticItemData> )
 * (N possible items per ID if it is a NMGStatisticPacketData or 1 if is a NMGStatisticThroughtputData).
 */
class NMGStatisticData: public QHash<QString, NMGStatisticItemData *>
{
	public:
		/*! \brief Default constructor. *
		 * Automatically tries to fill the cache with the information stored in the cache file */
		inline NMGStatisticData() { loadStatisticsFromXMLCache(); }

		/*! \brief Default destructor. */
		inline ~NMGStatisticData() {}

		/*! \brief Loads the statistic information from the XML file cache.
		 * Only loads in cache information of packets tests (NMGStatisticPacketData). */
		void loadStatisticsFromXMLCache();
	
		/*! \brief Saves the statistic information to the XML file cache".
		 * Only loads in cache information of packets tests (NMGStatisticPacketData). */
		void saveStatisticsToXMLCache();
	
		/*! \brief Deletes the statistic cache file". *
		 * \return TRUE if procedure runs ok or FALSE otherwise. */
		bool deleteXMLCache();
};

#endif
