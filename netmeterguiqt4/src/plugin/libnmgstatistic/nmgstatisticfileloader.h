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
#ifndef NMGSTATISTICFILELOADER_H
#define NMGSTATISTICFILELOADER_H

#include "nmgstatisticthread.h"
#include "nmgstatisticerror.h"

#include <progressmanager.h>

#include <QTimer>
#include <QStringList>

/*! \brief NMGStatisticFileLoader is the class responsible for loading test files into the plugin.
 *
 * This class not only deals with the loading aspects but also it stores the processed information
 * contained in the files loaded. */
class NMGStatisticFileLoader : public QObject
{
		Q_OBJECT

	public:
		/*! \brief Default constructor. */
		NMGStatisticFileLoader();
	
		/*! \brief Default destructor. */
		~NMGStatisticFileLoader();

		/*! \brief Notifies to the loader that new file must be loaded.
		 *
		 * The new files will be loaded one by one in an execution thread.
		 * \param fileName Path where file to be loaded is located. */
		void loadFile ( const QString & fileName );
	
		/*! \brief Notifies to the loader that new files must be loaded.
		 *
		 * The new files will be loaded one by one in an execution thread.
		 * \param fileNames Path list where files to be loaded are located. */
		void loadFiles ( const QStringList & fileNames );

		/*! \brief Returns the type of a test identified by its MD5 ID.
		 * \param fileMD5Id MD5 of the source file (UID).
		 * \return QString with the type of test that could be "packets" (TYPE_PACKETS) or "throughput" (TYPE_TPUT) */
		QString getTypeOfTest ( const QString & fileMD5Id );

		inline NMGStatisticMetaData * getMetadata() { return metadata; }
	
		/*! \brief Returns the metadata information of the test.
		 *
		 * The test is searched in the metadata by its MD5 ID (calculated from the source file).
		 * If the fileMD5Id parameter is not defined, is calculated from the file specified in the \a testPathId.
		 * \param testPathId Path of the test.
		 * \param fileMD5Id MD5 ID of the test (optional).
		 * \return NMGMetaData with the metadata information of the test.
		 * \throw <NMGStatisticError> { throws a NMGStatisticError::CRITICAL exception when the test isn't find }*/
		NMGMetaData * getMetadata ( const QString & testPathId, const QString & fileMD5Id = "" ) 
		throw ( NMGStatisticError );
	
		/*! \brief Removes the metadata information of the test identified with
		 * \a id from the container.
		 * \param id MD5 ID of the test.
		 * \return the number of removed items if the test is founded and removed propertly 
		 * 	   or 0 is the item doesn't exist or no item is removed. */
		int removeMetadataItem ( const QString & id );

		inline NMGStatisticData * getStatistics() { return statistics; }
	
		/*! \brief Returns the statistic information of all items of the test.
		 *
		 * The test is searched in the statistics by its MD5 ID (calculated from the source file).
		 * If the \a fileMD5Id parameter is not defined, is calculated from the file specified in the \a testPathId.
		 * \param testPathId Path of the test.
		 * \param fileMD5Id MD5 ID of the test (optional).
		 * \return NMGStatisticItemDataList with a list of all NMGStatisticItemData (items) of the test.
		 * \throw <NMGStatisticError> { throws a NMGStatisticError::CRITICAL exception when the test isn't found }*/
		NMGStatisticItemDataList getStatistics ( const QString & testPathId, const QString & fileMD5Id = "" )
		throw ( NMGStatisticError );
	
		/*! \brief Returns the statistic information of one item of test.
		 *
		 * The test is searched in the statistics by its MD5 ID (calculated from the source file).
		 * If the \a fileMD5Id parameter is not defined, is calculated from the file specified in the \a testPathId.
		 * Furthermore, if \a flowId is -1, the value of the most recently inserted one is returned.
		 * Else, the item with the indicated value is searched and returned.
		 * If there's no item with the specified flowId causes an error.
		 * \param testPathId Path of the test.
		 * \param flowId Desired flow if flowId > 0. The most recently inserted if flowId == -1
		 * \param fileMD5Id MD5 ID of the test (optional).
		 * \return NMGStatisticItemData with selected item of the test.
		 * \throw <NMGStatisticError> { throws a NMGStatisticError::CRITICAL exception when the test or the item isn't found }*/
		NMGStatisticItemData * getStatistics ( const QString & testPathId, int flowId, const QString & fileMD5Id = "" ) 
		throw ( NMGStatisticError );
	
		/*! \brief Returns the aggregated statistic information of the throughtput tests in the list.
		 *
		 * Dynamically calculates the statistical information from the throughtput tests specified in the list 
		 * and returns a filled structure.
		 * \param idList MD5 ID list of the tests
		 * \return NMGStatisticAggregationData with the aggregated statistic information of the throughtput tests in the list
		 * \throw <NMGStatisticError> { throws a NMGStatisticError::CRITICAL exception if any of the tests in the list isn't found }*/
		NMGStatisticAggregationData * getStatistics ( const QStringList & idList ) 
		throw ( NMGStatisticError );

	signals:
		void signalFileRead ( const QString &, const QString & );
		void newFilesAdded();

	private slots:
		void loadPendingFiles();
		void slotUpdateProgress();
		void slotLoadCancelled ( ProgressItem * );

	private:
		QStringList filesToLoad; /*!< List of file paths pending to load */

		NMGStatisticData * statistics; /*!< Statistic data container of all test in cache */
		NMGStatisticMetaData * metadata; /*!< Metadata of loaded tests*/

		QTimer * timer;
		NMGStatisticThread * readerThread;
		ProgressItem * progress;
		bool userCancelled;
};

#endif
