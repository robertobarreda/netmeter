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
#ifndef NMGTESTFILEDATA_H
#define NMGTESTFILEDATA_H

#include "nmgresultdata.h"
#include "nmgglobals.h"
#include <nmgmetadata.h>
using namespace Globals;

/**
 * \brief NMGTestFileData class includes all information in a test file using the NMGMetaData and
 * NMGResultData structures.
 *
 * This class has been designed to manage test files with different data and provide a single
 * interface for any test file supported. Its design has been thought in an efficient way. Is 
 * for this reason that NMGTestFileData works with pointers, to allow access to data without
 * extra copy costs.
 */
class NMGTestFileData
{
  public:

    /**
     * \brief Default constructor.
     * \param type Specifies the test file type that will manage the class. This is 
     * important because it defines the way NMGResultData will be inializated.
     */
    NMGTestFileData(SupportedType type);

    /** \brief Default destructor */
    ~NMGTestFileData();

    /**
     * \brief Returns a pointer to a NMGMetaData structure. 
     * 
     * This allows not only to look up its data, but also to modify it.
     * \sa setMetadata(NMGMetaData& metadata).
     */
    NMGMetaData* getMetadata();

    /**
     * \brief Returns a pointer to a NMGResultData structure.
     *
     * Because NMGTestFileData saves any type of supported test files, it is important to know
     * that NMGResultData* will be, in fact, a pointer to a specific result data subclass 
     * depending on the current type supported. The consequences are that in order 
     * to look up or modify result data obtained with this method, we must make a cast to the
     * specific NMGResultData subclass. For the current supported test files this cast will be:
     * <br>
     * (NMGFlowOrientedResultData *) for PACKETS<br>
     * (NMGAggregationOrientedResultData *) for AGGREGATION<br>
     */
    NMGResultData* getResultData();
    
    /** \brief Returns the test file type managed */
    SupportedType getCurrentSupportedType() const; 

    /**
     * \brief Allows to modify metadata contents.
     * \param metadata NMGMetaData that will replace the current metadata content.
     */
    void setMetadata(NMGMetaData& metadata);

  private:

    NMGMetaData* metadata;
    NMGResultData* result;
    SupportedType type;
};

#endif
