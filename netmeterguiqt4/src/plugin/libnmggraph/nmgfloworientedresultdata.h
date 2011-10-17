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
#ifndef NMGFLOWORIENTEDRESULTDATA_H
#define NMGFLOWORIENTEDRESULTDATA_H

#include "nmgresultdata.h"
#include "nmgflowdata.h"
#include <QMap>

/**
 * \brief NMGFlowOrientedResultData class is responsible to save all flows from a packet 
 * oriented test file. 
 *
 * Its design has been thought in an efficient way. Is for this reason that
 * NMGFlowOrientedResultData works with pointers, to avoid extra costly copies of NMGFlowData
 * information. Specifically, the class is formed with pairs of \<\e flow \e id, 
 * \e NMGFlowData * \>. Once new flow data is added as a new pair, NMGFlowOrientedResultData 
 * manages it completely.
 *
 * \sa NMGGraphPacketOrientedDataParser
 */
class NMGFlowOrientedResultData : public NMGResultData, public QMap<int,NMGFlowData*>
{
  public:
    /** \brief Default constructor. Construcs an empty flows collection*/
    NMGFlowOrientedResultData();

    /**
     * \brief Default destructor. Has the responsibility to delete all pointers to NMGFlowData 
     */
    virtual ~NMGFlowOrientedResultData();

    /**
     * \brief Copy constructor.
     * \param object NMGFlowOrientedResultData from which implicit parameter takes data
     */
    NMGFlowOrientedResultData(const NMGFlowOrientedResultData& object);

    /**
     * \brief Assignment operator.
     * \param object NMGFlowOrientedResultData from which implicit parameter takes data. 
     * Returns a reference to implicit parameter.
     */
    NMGFlowOrientedResultData& operator=(const NMGFlowOrientedResultData& object);

    /**
     * \brief Inserts a new item with the key \e flowId and the value \e flow. 
     * 
     * It doesn't control if key \e flowId is already added or not. Space reserved dinamically for
     * NMGFlowData is managed once it is added to the collection so the user only has to reserve 
     * space for it.
     * 
     * \param flowId Flow identifier
     * \param flow NMGFlowData pointer
     */
    void addFlowData(int flowId, NMGFlowData* flow);
    
    /**
     * \brief Deletes an existing item with the key \e flowId. 
     * 
     * If repeated keys were inserted, the most recently added is deleted. If the key
     * doesn't exist, this method has not effect.
     * 
     * \param flowId Flow identifier
     */
    void deleteFlowData(int flowId);
        
    /** 
     * \brief Determines if result data can be used without problems (valid) or if it is
     * programmer responsability to determine can to do with data (invalid).
     */
    bool isDataInvalid() const;
};

#endif
