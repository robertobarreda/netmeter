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
#ifndef NMGSTATISTICEXPORTMANAGER_H
#define NMGSTATISTICEXPORTMANAGER_H

#include "nmgstatisticdata.h"
#include "ui_nmgstatisticexportoptionswidget.h"
#include <nmgmodulepreferencesmanager.h>

namespace Ui
{
	class NMGStatisticExportManagerBase;
} // namespace Ui

class QXmlStreamWriter;

/*! \brief NMGStatisticExportManager is the class responsible for managing the export functionalities
 * 
 * This class only deals with the passed information in custom structures and 
 * build an output according to the export selection made by the user.
 * This module is able to export the tests to XML, XHTML, TXT or apply an XSLT stylesheet to the source file test
 * or to the statistic file 
 * \sa NMGStatisticExportData
 * \sa NMGStatisticAggregationData
 * \sa NMGStatisticPacketData
 * \sa NMGStatisticThroughtputData
 * \sa NMGStatisticItemData */
class NMGStatisticExportManager : public QDialog, public Ui::NMGStatisticExportManagerBase
{
		Q_OBJECT

	public:
		/*! \brief Export formats*/
		enum exportFormat { XML, XHTML, TXT, XSLT };

		/*! \brief Default constructor. 
		 * \param parent Parent widget.
		 * \param perfMan Pointer to the NMGModulePreferencesManager. */
		NMGStatisticExportManager ( QWidget * parent, NMGModulePreferencesManager * prefMan );
		
		/*! \brief Default destructor. */
		~NMGStatisticExportManager();

		/*! \brief Generates de XHTML output from a \a list of items in a QString to display it in the text browser. 
		 * \param list List of items that will generate the output in XHTML.
		 * \return QString with the XHTML code. */
		QString displayTest ( NMGStatisticExportDataList list );
		
		/*! \brief Generates de XHTML output from a \a list of items in a QString to display it in the text browser. 
		 * Moreover, this function attaches the aggregated information of the listed items.
		 * \param aggregationData Aggregated statistic information of the items.
		 * \param list List of items that will generate the output in XHTML.
		 * \return QString with the XHTML code. */
		QString displayTest ( NMGStatisticAggregationData * aggregationData, NMGStatisticExportDataList list );

		/*! \brief Generates de output file/s from a \a list of items in the format specified in the \a format value.
		 * \param list List of items that will generate the output.
		 * \param format Specifies the export format according to NMGStatisticExportManager::exportFormat available options. *
		 * \return returns FALSE in case of error or TRUE if no error */
		bool exportTest ( NMGStatisticExportDataList list, exportFormat format );
	
		/*! \brief Generates de output file/s from a \a list of items in the format specified in the \a format value.
		 * \param aggregationData Aggregated statistic information of the items.
		 * \param list List of items that will generate the output.
		 * \param format Specifies the export format according to NMGStatisticExportManager::exportFormat available options. 
		 * \return returns FALSE in case of error or TRUE if no error */
		bool exportTest ( NMGStatisticAggregationData * aggregationData, NMGStatisticExportDataList list, exportFormat format );

		/*! \brief Applies an XSLT stylesheet to the source tests in the list. 
		 * \param list List of items that will generate the output.
		 * \return returns FALSE in case of error or TRUE if no error */
		bool applyXsltToSourceTest ( QStringList list );

	private slots:
		void loadPreferences();
		void savePreferences();
		bool selectResultDirectory();

	private:
		void exportTestToXml ( QXmlStreamWriter *, NMGStatisticExportData * );
		void exportTestToXml ( QXmlStreamWriter *, NMGStatisticAggregationData * );

		void exportTestToXhtml ( QXmlStreamWriter *, NMGStatisticExportData * );
		void exportTestToXhtml ( QXmlStreamWriter *, NMGStatisticAggregationData * );
		void exportTestToXhtml ( QXmlStreamWriter * xmlw, NMGStatisticExportDataList );

		QString exportTestToTxt ( NMGStatisticExportData * );
		QString exportTestToTxt ( NMGStatisticAggregationData * );


		void exportItemToXml ( QXmlStreamWriter *, NMGStatisticPacketData * );
		void exportItemToXml ( QXmlStreamWriter *, NMGStatisticThroughtputData * );

		void exportItemToXhtml ( QXmlStreamWriter *, NMGStatisticPacketData * );
		void exportItemToXhtml ( QXmlStreamWriter *, NMGStatisticItemDataList );
		void exportItemToXhtml ( QXmlStreamWriter *, NMGStatisticThroughtputData * );

		QString exportItemToTxt ( NMGStatisticPacketData * );
		QString exportItemToTxt ( NMGStatisticThroughtputData * );

		void generateIndex ( QString & );
		void writeHtmlHeader ( QXmlStreamWriter *, QString filename = "" );
		QString writeTxtHeader ( QString );

		enum TimeUnits { MILISECS, SECS, MINS, HOURS };
		QString simplifyTime ( int time, int startTimeUnit = MILISECS );

		NMGModulePreferencesManager * preferencesManager;
		QString txtseparator;
};

#endif
