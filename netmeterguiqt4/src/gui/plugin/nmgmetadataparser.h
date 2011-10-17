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
#ifndef NMGMETADATAPARSER_H
#define NMGMETADATAPARSER_H

#include <QXmlStreamReader>
#include "nmgmetadata.h"

/**
 * \brief NMGMetaDataParser is a well-formed XML 1.0 parser for *.xff files.
 * 
 * NMGMetaDataParser parses the metadata information found in *.xff files and saves it into a 
 * NMGMetaData structure. Because this parser only deals with metadata information, the parsing 
 * process finishes when metadata has been read, saving us from the entire reading of the file.
 * If an error occures while parsing, parsing process is stopped and the kind of error that 
 * occurred is returned.
 *
 * \sa NMGMetaData
 */
class NMGMetaDataParser : public QXmlStreamReader
{

  public:

    /**
     * \brief Default constructor.
     * \param metadata NMGMetaData structure where the information read from the file is saved
     */
    NMGMetaDataParser(NMGMetaData& metadata);

    /** \brief Default destructor */
    ~NMGMetaDataParser() {}

    /**
     * \brief Allows to know if metadata information has been completely read by parser
     * \return True if metadata has been read or false if not
     */
    bool isMetaDataRead() const;

    /**
     * \brief Starts the parsing process
     * 
     * Parsing process can be aborted due to errors in the parsed file. These errors can be:
     * <i>The file is not a test file</i> and <i>The parser didn't find the expected metadata 
     * information</i>.
     * 
     * \param device File to be parsed.
     * \return False when an error ocurred during parsing.
     */
    bool read(QIODevice* device);
    
    /** \brief Stops the parsing process. NMGMetaData reaches an undefined state. */
    void stop();
    
    /**
     * \brief Allows to know aproximately the current percent of read metadata.
     * \return Percent between 0 and 100.
     */
    int getCompletedPercent() const;
    
    /**
     * \brief Allows to know the current parsing state.
     *
     * The possible states are: <i>Parser initialized</i>, <i>Reading metedata</i> and 
     * <i>Metadata read</i>.
     *
     * \return Current parsing state
     */
    QString getStatus() const;

  private:
    
    void readTest();
    void readMetadata();
    void readName();
    void readIdentifier();
    void readTypeOfFile();
    void readVersion();
    
    void readSource();
    void readSourceAddress();
    void readSourceManagement();
    void readSourceManagementAddress();
    void readSourceManagementPort();
    
    void readDestination();
    void readDestinationAddress();
    void readDestinationManagement();
    void readDestinationManagementAddress();
    void readDestinationManagementPort();
    
    void readUncommonElement();
    void readExtraContentUncommonElement(QString& content);
    void writeStartElement(QString name, QXmlStreamAttributes atrib, QString& content);
    void writeEndElement(QString name, QString& content);

    NMGMetaData& metadata;
    bool isMetadataFinished;
    
    bool stopParsing;
    int completedPercent;
    QString status;
};

#endif
