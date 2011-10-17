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
#ifndef NMGRESULTDATA_H
#define NMGRESULTDATA_H
#include <Qt>

/**
 * \brief NMGResultData is the base class for all result data classes that contain result data 
 * used in test files. 
 * 
 * When a new specific data appears, a result data subclass must be created as a NMGResultData 
 * derived class.
 */
class NMGResultData
{
  public:
    /** \brief Default constructor */
    NMGResultData() {}
    
    /** \brief Default destructor */
    virtual ~NMGResultData() {}
        
    /** 
     * \brief Determines if result data can be used without problems (valid) or if it is
     * programmer responsability to determine can to do with data (invalid).
     * 
     * This is a method provide for convinience for subclasses that need to reimplement it.
     */
    virtual bool isDataInvalid() const { return FALSE; }
};

#endif
