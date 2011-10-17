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
#ifndef NMGFLOWDATA_H
#define NMGFLOWDATA_H

#include <QString>
#include <QList>

/**
 * \brief NMGFlowData class is responsible to save all the information of a flow
 * from a packet oriented test file.
 *
 * This class saves not only information related to the flow as origin or destination, but also
 * all data in the flow, considering that it has previously been processed to be suitable for
 * visualization purposes.
 *
 * \sa NMGGraphPacketOrientedDataParser
 */
class NMGFlowData
{
  public:

    /** \brief Default constructor. */
    NMGFlowData();

    /** \brief Default destructor. */
    ~NMGFlowData() {};

    /**
     * \brief Saves the internal flow identifier.
     * \param id Flow identifier.
     */
    inline void setFlowId(int id) { flowId = id; }

    /**
     * \brief Saves the internal flow name.
     * \param name Flow name.
     */
    inline void setFlowName(QString name) { flowName = name; }
        
    /**
     * \brief Saves the source address, as IP format, from where packets have been sent.
     * \param source Source address.
     */
    inline void setSourceAddress(QString source) { sourceAddress = source; }

    /**
     * \brief Saves the source port from where packets have been sent.
     * \param port Source port.
     */
    inline void setSourcePort(int port) { sourcePort = port; }

    /**
     * \brief Saves the destination address, as IP format, where packets have been received.
     * \param destination Destination address.
     */
    inline void setDestinationAddress(QString destination) { destinationAddress = destination; }

    /**
     * \brief Saves the destination port where packets have been received.
     * \param port Destination port.
     */
    inline void setDestinationPort(int port) { destinationPort = port; }
    
    /**
     * \brief Saves the flow duration.
     * \param duration Elapsed time for the flow in miliseconds.
     */
    inline void setElapsedTimeInMiliSeconds(int duration) { time = duration; }

    /**
     * \brief Saves packet rate of packets in flow.
     * \param rate Packet rate in packets/second.
     */
    inline void setPacketRate(int rate) { this->rate = rate; }
        
    /**
     * \brief Saves packet size of packets in flow.
     * \param size Packet size in bytes.
     */
    inline void setPacketSize(int size) { this->size = size; }
    
    /**
     * \brief Saves packets generation pattern in flow.
     * \param pattern Generation pattern.
     */
    inline void setPattern(QString pattern) { this->pattern = pattern.toUpper(); }
    
    /**
     * \brief Appends the processed information extracted from a single packet to the lists,
     * ordered by its seqId.
     *
     * Processed informations are timestamp and delay, that are calculated as substraction 
     * between current packet transmission time and the first packet transmission time in the
     * same flow, and substraction between packet reception time and packet transmission time,
     * respectively. This method doesn't control repeated packets to avoid an extra time 
     * consumption.
     *
     * \param seqId Internal sequence packet identifier.
     * \param timestamp Packet timestamp.
     * \param delay Transmission time delay.
     */
    void addPacketInfoToList(const int& seqId, const double& timestamp, const double& delay);

    /**
     * \brief Returns the internal flow identifier.
     * \return Flow identifier or -1 if it is not set.
     */
    inline int getFlowId() const { return flowId; }

    /**
     * \brief Returns the internal flow name.
     * \return Flow name or empty string if it is not set.
     */
    inline QString getFlowName() const { return flowName; }
    
    /**
     * \brief Returns the source address, as IP format, from where packets have been sent.
     * \return Source address or empty string if it is not set.
     */
    inline QString getSourceAddress() const { return sourceAddress; }

    /**
     * \brief Returns the source port from where packets have been sent.
     * \return Source port or -1 if it is not set.
     */
    inline int getSourcePort() const { return sourcePort; }

    /**
     * \brief Returns the destination address, as IP format, where packets have been received.
     * \return Destination address or empty string it it is not set.
     */
    inline QString getDestinationAddress() const { return destinationAddress; }

    /**
     * \brief Returns the destination port where packets have been received.
     * \return Destination port or -1 if it is not set.
     */
    inline int getDestinationPort() const { return destinationPort; }
    
    /**
     * \brief Returns the flow duration.
     * \return Elapsed time for the flow in miliseconds or -1 if it is not set.
     */
    inline int getElapsedTimeInMiliSeconds() const { return time; }

    /**
     * \brief Returns packet rate of packets in flow.
     * \return Packet rate in packets/second.
     */
    inline int getPacketRate() { return rate; }

    /**
     * \brief Returns packet size of packets in flow.
     * \return Packet size in bytes.
     */
    inline int getPacketSize() { return size; }
    
    /**
     * \brief Returns packets generation pattern in flow.
     * \return Packet rate in packets/second.
     */
    inline QString getPattern() { return pattern; }

    /**
     * \brief Returns an STL-style iterator pointing to the first item in the sequence packet
     * identifiers collection.
     */
    inline QList<int>::const_iterator beginSequenceIdentifier() const 
    { return seqIdList.constBegin(); }

    /**
     * \brief Returns an STL-style iterator pointing to the imaginary item after the last item in 
     * the sequence packet identifiers collection.
     */
    inline QList<int>::const_iterator endSequenceIdentifier() const
    { return seqIdList.constEnd(); }
  
    /**
     * \brief Returns an STL-style iterator pointing to the first item in the packet timestamps 
     * collection.
     */
    inline QList<double>::const_iterator beginTimestamp() const
    { return timestampList.constBegin(); }

    /**
     * \brief Returns an STL-style iterator pointing to the imaginary item after the last item in 
     * the packet timestamps collection.
     */
    inline QList<double>::const_iterator endTimestamp() const
    { return timestampList.constEnd(); }

    /**
     * \brief Returns an STL-style iterator pointing to the first item in the packet delays 
     * collection.
     */
    inline QList<double>::const_iterator beginDelay() const
    { return delayList.constBegin(); }

    /**
     * \brief Returns an STL-style iterator pointing to the imaginary item after the last item in 
     * the packet delays collection.
     */
    inline QList<double>::const_iterator endDelay() const
    { return delayList.constEnd(); }

    /**
     * \brief Fills lists with lost sequence packets.
     *
     * Lost sequences identifiers are added to their correct position, sequence timestamp is 
     * calculated based on non-lost previous and posterior sequences near to the lost sequence,
     * and delay is set to 0.0. This method is usually used when no more packets are added. If
     * it is used before adding more packets, repeated packets can appear.
     */
    void fillLostSequences();
    
    /** 
     * \brief Returns if one or more sequence packets has a negative delay, that it
     * means not syncronized clocks between source and destination.
     */
    inline bool isDataInvalid() const { return isInvalid; }
    
    /** 
     * \brief Sets data invalidity in terms of negatives delays in sequence packets.
     * \param invalid True if one or more packets contain negatives delays. False otherwise.
     */
    inline void setDataInvalidity(bool invalid) { isInvalid = invalid; }

  private:

    int flowId;
    QString flowName;
    QString sourceAddress;
    int sourcePort;
    QString destinationAddress;
    int destinationPort;
    int time; /**< Flow duration in miliseconds. */
    int rate; /**< Flow packet rate in packets/sec. */
    int size; /**< Flow packet size in bytes. */
    QString pattern; /**< Packets generation pattern. */

    QList<int> seqIdList;
    QList<double> timestampList;
    QList<double> delayList;
    
    bool isInvalid;/**< Data validity. Used to determine negative delays in sequence packets. */
};

#endif
