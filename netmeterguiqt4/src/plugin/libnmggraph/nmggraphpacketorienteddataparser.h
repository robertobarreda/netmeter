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
#ifndef NMGGRAPHPACKETORIENTEDDATAPARSER_H
#define NMGGRAPHPACKETORIENTEDDATAPARSER_H

#include "nmgresultdata.h"
#include "nmgfloworientedresultdata.h"
#include "nmgflowdata.h"
#include <nmgpacketorienteddataparser.h>

/**
 * \brief NMGGraphPacketOrientedDataParser is a class with the specific behaviour needed
 * by the Graphic Analysis plugin to support packet oriented data parsing.
 * 
 * NMGGraphPacketOrientedDataParser is a derived class of NMGPacketOrientedDataParser 
 * that decides which treatment do with the packets/flows information. This derived class 
 * defines the virtual method doProcess() called when aggregated values has been read and
 * the virtual method doWhenFlowFinished() called each time a flow parsing is ended.
 */
class NMGGraphPacketOrientedDataParser : public NMGPacketOrientedDataParser
{
  public:

    /**
     * \brief Class constructor.
     * \param data Pointer to the structure to be filled during parsing. Class doesn't hold any
     * data.
     */
    NMGGraphPacketOrientedDataParser(NMGFlowOrientedResultData* data);

    /** \brief Default destructor. */
    ~NMGGraphPacketOrientedDataParser();

    /**
     * \brief Gets packet information, processes it and saves it as a sequence packet in a
     * NMGFlowData structure.
     */
    void doProcess();

    /**
     * \brief Saves all the information related to the flow finished into the
     * NMGFlowOrientedResultData structure.
     */
    void doWhenFlowFinished();

  private:

    void saveFlowInformation();
    void saveSequenceInformation();
    double timeDifference(const QString &start, const QString &end);

    NMGFlowOrientedResultData* data;
    NMGFlowData* currentFlow;
    int currentFlowId;
    QString flowStartTime;

};

#endif
