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
#ifndef NMGFILECONTAINER_H
#define NMGFILECONTAINER_H

#include "nmgtestfiledata.h"
#include <QHash>

/**
 * \brief NMGFileContainer class has been conceived to contain a collection of test files
 * with their individual data. 
 *
 * The collection is formed with pairs of \<\a test \a filename, \a NMGTestFileData * \>. To be 
 * efficient, test data has been designed in a pointer-based way. Once new test data is added to 
 * the collection, NMGFileContainer manages the space reserved dinamically for NMGTestFileData, 
 * so the user only has to reserve space for it.
 */
class NMGFileContainer : public QHash<QString, NMGTestFileData*>
{
  public:

    /** \brief Default constructor */
    NMGFileContainer();

    /** \brief Default destructor */
    ~NMGFileContainer();

    /* 
     * NOTE Copy constructor and assignment operator should not be uncommented
     * unless NMGTestFileData copy constructor and assignment operator are implemented.
     * The behaviour of these methods hasn't been tested.
     */
    //NMGFileContainer(const NMGFileContainer& object);
    //NMGFileContainer& operator=(const NMGFileContainer& object);
};

#endif
