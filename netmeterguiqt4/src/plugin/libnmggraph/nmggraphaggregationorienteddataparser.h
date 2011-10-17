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
#ifndef NMGGRAPHAGGREGATIONORIENTEDDATAPARSER_H
#define NMGGRAPHAGGREGATIONORIENTEDDATAPARSER_H

#include "nmgaggregationorientedresultdata.h"
#include <nmgaggregationorienteddataparser.h>

/**
 * \brief NMGGraphAggregationOrientedDataParser is a class with the specific behaviour needed
 * by the Graphic Analysis plugin to support aggregation oriented data parsing.
 * 
 * NMGGraphAggregationOrientedDataParser is a derived class of NMGAggregationOrientedDataParser 
 * that decides which treatment do with the aggregated values information. This derived class 
 * defines the virtual method doProcess() called when aggregated values has been read.
 */
class NMGGraphAggregationOrientedDataParser : public NMGAggregationOrientedDataParser
{
  public:

    /**
     * \brief Class constructor.
     * \param data Pointer to the structure to be filled during parsing. Class doesn't hold any
     * data.
     */
    NMGGraphAggregationOrientedDataParser(NMGAggregationOrientedResultData* data);
    
    /** \brief Default destructor. */
    ~NMGGraphAggregationOrientedDataParser();

    /** 
     * \brief Gets aggregated information and saves it into the NMGAggregationOrientedResultData
     * structure.
     */
    void doProcess();

  private:

    NMGAggregationOrientedResultData* data;

};

#endif
