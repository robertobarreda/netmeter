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
#ifndef NMGSTATISTICREMOTEWIDGET_H
#define NMGSTATISTICREMOTEWIDGET_H

#include "nmgstatisticfileloader.h"
#include <nmgmodulemanager.h>

#include <QString>

/*! \brief NMGStatisticRemoteContainer is a container class that stores the parsed information from the remote request
 * 
 * Parses received information (request) to load the list of items requested, as well as statistical values required to generate 
 * after the output file from these options. */
class NMGStatisticRemoteContainer
{
	public:
		/*! \brief Parses part of the XML Request to list the requested items and statistic values
		 * 
		 * This function is for "packets" tests
		 * \param items should be like a list of flowIds separated by semicolons like 1;2;3;...;N or ranges like 1..N or ALL.
		 * \param statistics should be a list of tags separated by semicolons.*/
		NMGStatisticRemoteContainer( QString & items, QString & statistics );
		
		/*! \brief Parses part of the XML Request to list the statistic requested values
		 * 
		 * This function is for "throughtput" tests
		 * \param statistics should be a list of tags separated by semicolons.*/
		NMGStatisticRemoteContainer( QString & statistics );
		
		/*! \brief Returns if there is an error on the parsing.
		 * \return TRUE if there is an error on the parsing, FALSE if there is no error.*/
		inline bool hasError() { return error; };
	
		QStringList itemList; /*!< List of items that want to parse. */
		QStringList basicStatistisList; /*!< List of basic statistics. */
		QStringList delayStatistisList; /*!< List of delay statistics. */
		QStringList ipdvStatistisList; /*!< List of ipdv statistics. */
		int innerCounter; /*!< Number of requested items counter. */

	private:
		bool error; /*!< Error on parsing. */
};

/*! \brief NMGStatisticRemoteWidget is the class is responsible for receiving the petition, parsing the XML, upload the data structures
 * and eventually generate the output from the configuration options requested. */
class NMGStatisticRemoteWidget : public QWidget
{
		Q_OBJECT
	public:
		/*! \brief Contructor of the class. Loads the statistic file loader and cache
		* \param modmanager Pointer to the Module manager.
		* \param currentTestId Identifier of the current module id. 
		* \sa NMGStatisticFileLoader */
		NMGStatisticRemoteWidget( NMGModuleManager * modmanager, QString currentTestId );

		/*! \brief Default destructor of the class. */
		~NMGStatisticRemoteWidget();

		/*! \brief Parses the XML request and loads requested tests
		* \param data QString with the request in XML format. */
		bool loadData( QString data );

	private slots:
		/*! \brief Slot executed when a test parsing is finished that builds the XML reply
		 * \param filePathId QString with path of the file test.
		 * \param fileMD5Id QString with the MD5 ID of the file test. */
		void updateWidgetBecauseOfTestEnd( const QString & filePathId, const QString & fileMD5Id );

	private:
		bool loadPacketData ( QXmlStreamReader * xml );
		bool loadThroughtputData ( QXmlStreamReader * xml );
		
		void generateFlow ( QXmlStreamWriter * xmlw, NMGStatisticPacketData * flow, const QString & filePathId );

		QString testId;
		QMap<QString, NMGStatisticRemoteContainer *> exportListMap;
		NMGStatisticFileLoader * fileLoader;
		NMGModuleManager * moduleManager;
};

#endif
