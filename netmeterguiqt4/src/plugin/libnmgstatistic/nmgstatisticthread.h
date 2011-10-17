/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#ifndef NMGSTATISTICTHREAD_H
#define NMGSTATISTICTHREAD_H

#include "nmgstatisticparser.h"
#include <nmgmetadataparser.h>
#include <nmthread.h>

/*! \def REPARSE_FLAG Descrives which is the reparse flag character */
#define REPARSE_FLAG		"+"
/*! \def TESTALREADYPARED Returned error message when the test is already parsed */
#define TESTALREADYPARED	"File already parsed. Not parsed again"

/*! \brief NMGStatisticThread reads a single test file.
 *
 * This class takes the appropiate actions for parsing the file accordingly to its type, and
 * saves all processed file data. To avoid extra copy costly costs of file data it has
 * been designed in a pointer-based way and the thread fills the global structure.
 * 
 * \sa NMGStatisticFileLoader
 * \sa NMGStatisticMetaData
 * \sa NMGStatisticData
 * \sa NMGMetaDataParser
 * \sa NMGDataParser */
class NMGStatisticThread : public NMThread
{
	public:
		/*! \brief Default constructor.
		 * \param stats[out] Statistics container reference to.
		 * \param meta[out] Metadata container reference to.
		 * \param pathid[in] file path */
		NMGStatisticThread ( NMGStatisticData * stats, NMGStatisticMetaData * meta, QString & pathid );
	
		/*!  \brief Default destructor. */
		~NMGStatisticThread();

		/*!  \brief Returns file path that will be read.
		 * \return QString with the path of the parsed file. */
		inline QString getFileLoaded() const { return filePathId; }
	
		/*!  \brief Returns file path that will be read.
		 * \return QString with the file MD5 ID of the parsed file. */
		inline QString getFileLoadedMD5Id() const { return fileMD5Id; }

		/*!  \brief Returns the percentatge of parsing work done.
		 * \return int [0-100] stating the percentatge of the test that has been parsed. */
		int getProcessPercent() const;
			
		/*!  \brief Returns a message with the current parsing state.
		 * \return QString with the status message. */
		QString getProcessStatus() const;

		/*! \brief Returns if an error occured during parsing.
		 * \return True if an error occured; False otherwise. */
		inline bool hasError() const { return error; }
			
		/*! \brief Returns, if an error occured during parsing, the error message.
		 * \return QString with the error message.*/
		inline QString getError() const { return errorMsg; }

		/*! \brief Method invoked to start the thread. */
		void run();
			
		/*! \brief Stops the parsing process whatever parsing state it be. Thread ends
		 * with user cancellation error. */
		void kill();

	private:
		/*! \brief Internal method invoked to parse the metadata.
		 * With this method the threads can guess the type of test
		 * \return TRUE is everything is correct, FALSE if there is an error */
		bool readMetadata();
			
		/*! \brief Internal method invoked to parse the result data.
		 * With this method parses the result depending on the type of test
		 * \return TRUE is everything is correct, FALSE if there is an error */
		bool readResult();

		//Data Containers
		QString filePathId, fileMD5Id, testType;
		NMGMetaData * metadataTest;
		NMGStatisticMetaData * metadata;
		NMGStatisticData * statistics;

		//Parsers
		NMGMetaDataParser * metaParser;
		NMGDataParser * statisticParser;

		bool error;
		QString errorMsg;
};

#endif
