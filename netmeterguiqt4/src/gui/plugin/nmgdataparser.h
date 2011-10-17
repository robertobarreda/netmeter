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
#ifndef NMGDATAPARSER_H
#define NMGDATAPARSER_H

#include <QXmlStreamReader>

/**
 * \brief NMGDataParser is the base class for all data parsers used to read tests files. 
 * 
 * When a new specific data parser is needed, it must be created as a NMGDataParser derived class 
 * and it must redefine all virtual methods.
 */

class NMGDataParser : public QXmlStreamReader
{
  public:
    /** \brief Default constructor */
    NMGDataParser() {}
    
    /** \brief Default destructor */
    virtual ~NMGDataParser() {}
    
    /**
     * \brief Starts the parsing process.
     * \param device File to be parsed.
     * \return False when an error ocurred during parsing. True otherwise.
     */    
    virtual bool read(QIODevice* device) { return TRUE; }
    
    /**
     * \brief Allows to know if data information has been completely read by parser
     * \return True if data has been read or false if not
     */    
    virtual bool isDataRead() { return FALSE; }
    
    /** \brief Stops the parsing process. */    
    virtual void stop() {}
    
    /**
     * \brief Allows to know aproximately the current percent of read file.
     * \return Percent between 0 and 100.
     */    
    virtual int getCompletedPercent() const { return 0; }
    
    /**
     * \brief Allows to know the current parsing state.
     * \return Current parsing state
     */
    virtual QString getStatus() const { return ""; }
};

#endif
