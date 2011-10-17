/***************************************************************************
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
#ifndef NMGAGGREGATIONORIENTEDRESULTDATA_H
#define NMGAGGREGATIONORIENTEDRESULTDATA_H

#include "nmgresultdata.h"
#include <QString>

/**
 * \brief NMGAggregationOrientedResultData class is responsible to save all result data from 
 * an aggregation oriented test file.
 *
 * \sa NMGGraphAggregationOrientedDataParser
 */
class NMGAggregationOrientedResultData : public NMGResultData
{
  public:

    /** \brief Default constructor */
    NMGAggregationOrientedResultData();

    /** \brief Default destructor */
    virtual ~NMGAggregationOrientedResultData();

    /**
     * \brief Saves the address, usually as IP format, of test origin.
     * \param source Source address.
     */
    inline void setSourceAddress(QString source) { sourceAddress = source; }

    /**
     * \brief Saves the address, usually as IP format, of test destination.
     * \param destination Destination address.
     */
    inline void setDestinationAddress(QString destination) { destinationAddress = destination; }

    /**
     * \brief Saves the socket size of the receiver host.
     * \param rss Socket size in bytes.
     */
    inline void setReceivingSocketSizeInBytes(int rss) { this->rss = rss; }

    /**
     * \brief Saves the socket size of the sender host.
     * \param sss Socket size in bytes.
     */
    inline void setSendingSocketSizeInBytes(int sss) { this->sss = sss; }

    /**
     * \brief Saves the size of messages sent by the sender host.
     * \param sms Size of messages.
     */
    inline void setSendingMessagesSizeInBytes(int sms) { this->sms = sms; }

    /**
     * \brief Save the time length of the perfomance test.
     * \param time Test duration.
     */
    inline void setElapsedTimeInSeconds(double time) { this->time = time; }

    /**
     * \brief Saves the aggregated value of performance, throughput, of the sender host.
     *
     * This value has to be defined only when the perfomance test is done with UDP packets 
     * transmission patterns.
     * \param tput Throughput is expressed in kilobits per second.
     */
    inline void setSendingThroughputInKBitsPerSecond(double tput) { tputSent = tput; }

    /**
     * \brief Saves the aggregated value of performance, throughput, of the receiver host.
     *
     * This value is defined when the performance test is indistinctly done with UDP/TCP packets 
     * transmission patterns.
     * \param tput Throughput is expressed in kilobits per second.
     */
    inline void setReceivingThroughputInKBitsPerSecond(double tput) { tputRecv = tput; }

    /**
     * \brief Saves the number of correct messages sent by sender host.
     *
     * This value has to be defined only when the perfomance test is done with UDP packets 
     * transmission patterns.
     * \param msg Number of correct messages.
     */
    inline void setSendingOkMessages(int msg) { okMsgSent = msg; }

    /**
     * \brief Saves the number of incorrect messages sent by sender host.
     *
     * This value has to be defined only when the perfomance test is done with UDP packets 
     * transmission patterns.
     * \param msg Number of incorrect messages.
     */
    inline void setSendingKoMessages(int msg) { koMsgSent = msg; }

    /**
     * \brief Saves the number of correct messages received by receiver host.
     *
     * This value has to be defined only when the perfomance test is done with UDP packets
     * transmission patterns.
     * \param msg Number of correct messages.
     */
    inline void setReceivingOkMessages(int msg) { okMsgRecv = msg; }

    /**
     * \brief Saves the pattern used in the perfomance test, TCP or UDP.
     * \param protocol Protocol used.
     */
    inline void setTypeOfProtocol(QString protocol) { typeOfProtocol = protocol.toUpper(); }

    /**
     * \brief Returns the source address, usually as IP format.
     * \return Source address or empty string if it is not set.
     */
    inline QString getSourceAddress() const { return sourceAddress; }

    /**
     * \brief Returns the destination address, usually as IP format.
     * \return Destination address or empty string if it is not set.
     */
    inline QString getDestinationAddress() const { return destinationAddress; }

    /**
     * \brief Returns the socket size of the receiver host.
     * \return Socket size in bytes or -1 if it is not set.
     */
    inline int getReceivingSocketSizeInBytes() const { return rss; }

    /**
     * \brief Returns the socket size of the sender host.
     * \return Socket size in bytes or -1 if it is not set.
     */
    inline int getSendingSocketSizeInBytes() const { return sss; }

    /**
     * \brief Returns the size of messages sent by the sender host.
     * \return Size of messages or -1 if it is not set.
     */
    inline int getSendingMessagesSizeInBytes() const { return sms; }

    /**
     * \brief Returns the time length of the perfomance test.
     * \return Time in seconds or -1.0 if it is not set.
     */
    inline double getElapsedTimeInSeconds() const { return time; }

    /**
     * \brief Returns the aggregated value of performance, throughput, of the sender host.
     *
     * This value is only defined when the perfomance test is done with UDP packets transmission
     * patterns.
     *
     * \return Throughput in kilobits per second or -1.0 if transmission pattern 
     * isn't of UDP type or data isn't set yet.
     */
    inline double getSendingThroughputInKBitsPerSecond() const { return tputSent; }

    /**
     * \brief Returns the aggregated value of performance, throughput, of the receiver host.
     *
     * This value is defined when the performance test is indistinctly done with UDP/TCP packets 
     * transmission patterns.
     *
     * \return Throughput in kilobits per second or -1.0 if it is not set.
     */
    inline double getReceivingThroughputInKBitsPerSecond() const { return tputRecv; }

    /**
     * \brief Returns the number of correct messages sent by sender host.
     *
     * This value is only defined when the perfomance test is done with UDP packets transmission
     * patterns.
     *
     * \return Number of correct messages or -1 if transmission pattern isn't of UDP type
     * or data isn't set yet.
     */
    inline int getSendingOkMessages() const { return okMsgSent; }

    /**
     * \brief Returns the number of incorrect messages sent by sender host.
     *
     * This value is only defined when the perfomance test is done with UDP packets transmission
     * patterns.
     *
     * \return Number of incorrect messages or -1 if transmission pattern isn't of UDP type
     * or data isn't set yet.
     */
    inline int getSendingKoMessages() const { return koMsgSent; }

    /**
     * \brief Returns the number of correct messages received by receiver host.
     *
     * This value is only defined when the perfomance test is done with UDP packets transmission
     * patterns.
     *
     * \return Number of correct messages or -1 if transmission pattern isn't of UDP type
     * or data isn't set yet.
     */
    inline int getReceivingOkMessages() const { return okMsgRecv; }

    /**
     * \brief Returns the pattern used in the perfomance test, TCP or UDP.
     * \return Protocol used or empty string if it is not set.
     */
    inline QString getTypeOfProtocol() const { return typeOfProtocol; }
    
  private:

    QString sourceAddress; /**< Source address of sender host. */
    QString destinationAddress; /**< Destination address of receiver host. */
    int rss; /**< Receiving socket size. */
    int sss; /**< Sending socket size. */
    int sms; /**< Sending messages size. */
    double time; /**< Elapsed time for test. */
    double tputSent; /**< Throughput for sender host. */
    double tputRecv; /**< Throughput for receiver host. */
    int okMsgSent; /**< Number of correct messages sent. */
    int koMsgSent; /**< Number of incorrect messages sent. */
    int okMsgRecv; /**< Number of correct messages received. */

    QString typeOfProtocol; /**< Protocol pattern for test. */
};

#endif
