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
#ifndef NMGTESTREADERTHREAD_H
#define NMGTESTREADERTHREAD_H

#include "nmgmetadata.h"
#include "nmgresultdata.h"
#include "nmgtestfiledata.h"
#include "nmgglobals.h"
#include <nmgmetadataparser.h>
#include <nmgdataparser.h>
#include <nmthread.h>

/**
 * \brief NMGTestReaderThread reads a single test file.
 *
 * This class takes the appropiate actions for parsing the file accordingly to its type, and
 * saves locally all processed file data. To avoid extra copy costly costs of file data it has
 * been designed in a pointer-based way. It is important to know that this file data must be
 * get (getTestFileData()) and managed by the user because thread doesn't manage it and 
 * consequently, it doesn't delete space reserved dinamically during the population of file data.
 *
 * \sa NMGTestLoaderHandler
 */
class NMGTestReaderThread : public NMThread
{
  public:

    /** \brief Default constructor. */
    NMGTestReaderThread();

    /** \brief Default destructor. */
    ~NMGTestReaderThread();

    /**
     * \brief Saves the file path to be read when the thread starts.
     * \param fileName Path file.
     */
    void loadFile(const QString& fileName);

    /** \brief Returns file path that will be read. */
    QString getFileLoaded() const;

    /**
     * \brief Returns the NMGTestFileData structure where all processed file information has 
     * been saved.
     */
    NMGTestFileData* getTestFileData() const;

    /** \brief Returns the percentatge of parsing work done. */
    int getProcessPercent() const;

    /** \brief Returns a message with the current parsing state. */
    QString getProcessStatus() const;

    /**
     * \brief Returns if an error occured during parsing.
     * \return True if an error occured; False otherwise.
     */
    bool hasError() const;

    /**
     * \brief Returns an error message if an error occured during parsing.
     * \return Error message or empty string if no error.
     */
    QString getError() const;

    /** \brief Method invoked when the thread is started. */
    void run();

    /**
     * \brief Allows to stop the parsing process whatever parsing state it be. Thread ends 
     * with user cancellation error.
     */
    void kill();

  private:

    bool readMetadata(NMGMetaData& metadata);
    bool readData(Globals::SupportedType fileType, NMGResultData* data);

    QString fileName;
    NMGTestFileData* testData;
    NMGMetaDataParser* metaParser;
    NMGDataParser* dataParser;

    bool error;
    QString errorMsg;
};

#endif
