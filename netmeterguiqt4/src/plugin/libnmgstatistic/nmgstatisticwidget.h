/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda\ac.upc.edu>           *
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
#ifndef NMGSTATISTICWIDGET_H
#define NMGSTATISTICWIDGET_H

#include "nmgstatisticfileloader.h"
#include "nmgstatisticexportmanager.h"
#include "ui_nmgstatisticwidgetbase.h"

#include <QMenu>
#include <QToolBar>

namespace Ui
{
	class NMGStatisticWidgetBase;
} // namespace Ui

/*! \brief NMGStatisticWidget holds the main widget interface of the plugin. */
class NMGStatisticWidget : public QWidget, public Ui::NMGStatisticWidgetBase
{
		Q_OBJECT
	public:
		/*! \brief Contructor of the class
		* \param prefMan Pointer to the preferences manager NMGModulePreferencesManager.
		* \param parent Pointer to the parent QWidget (optional)
		* \param name Name for the QObject (optional) */
		NMGStatisticWidget ( NMGModulePreferencesManager * prefMan, QWidget *parent = 0, const char *name = 0 );

		/*! \brief Default destructor of the class. */
		~NMGStatisticWidget();

		/*! \brief Returns the toolbar of the statistic widget
		* \return QToolBar pointer to the plugin's toolbar */
		QToolBar * getToolBar();

		/*! \brief Returns the pop up menu of the widget
		* \return QMenu pointer to the plugin's menu */
		QMenu * getPopupMenu();
	
		/*! \brief Parses the XML request and loads subscribe requested tests
		* \param data QString with the subscribe request in XML format. */
		bool loadData( QString data );

	private slots:
		/*! \brief Shows the contextual menu at the point Pos
		* \param pos QPoint where map the contextual menu. */
		inline void showContextualMenu ( const QPoint & pos ) { contextualMenu->popup ( statisticTreePacket->mapToGlobal ( pos ) ); }

		/*! \brief Show a dialog window to select the files that must be treated. */
		void loadFiles();
	
		/*! \brief Sends the selected tests to be reloaded again. */
		void reloadFiles();

		/*! \brief Executes the command in the file loader to save the cache. */
		inline void saveCache() { fileLoader->getStatistics()->saveStatisticsToXMLCache(); };
	
		/*! \brief Executes the command in the file loader to delete the cache. */
		inline void deleteCache() { fileLoader->getStatistics()->deleteXMLCache(); };
	
		/*! \brief Removes the selected item from the visualization. */
		inline void removeSelectedItems() { currentStatisticTree->removeSelectedItems(); }

		/*! \brief Slot executed when the widget receives a signal of a deleted test.
		 * \param id QString with the MD5 ID of the file test. */
		void updateWidgetBecauseItemRemoved ( const QString & id );
	
		/*! \brief Slot executed when the test selection changes to configure the visualization. */
		void updateWidgetBecauseSelectionChanged ();
	
		/*! \brief Slot executed when a test parsing is finished to configure the visualization of it. 
		 * \param filePathId QString with path of the file test.
		 * \param fileMD5Id QString with the MD5 ID of the file test. */
		void updateWidgetBecauseOfTestEnd ( const QString & filePathId, const QString & fileMD5Id );

		/*! \brief Slot executed to show the export manager options. */
		inline void showExportManager() { exportManager->show(); };
		/*! \brief Slot executed when an option of the export menu is selected to execute the proper export choice.
		 * \param action QAction with the selected export option. */
		void exportTestTo ( QAction * action );

		/*! \brief Slot executed to add a new group on the current tree visualization. */
		inline void addNewGroup() { currentStatisticTree->addGroup ( "Group " + INTEGER ( ngroup++ ) ); };

	private:
		enum tabs{ TAB_PACKETS = 0, TAB_TPUT = 1 };
	
		/*! \brief Creates the contextual menu. */
		void createContextualMenu();
	
		/*! \brief Method executed to display the metadata information of the selected test on the tree widget.
		 * \param filePathId QString with path of the file test.
		 * \param fileMD5Id QString with the MD5 ID of the file test. */
		void displayMetadataOnTreeWidget ( const QString & filePathId, const QString & fileMD5Id );
	
		/*! \brief Method executed to display the statistic data of the selected test on the text browser.
		 * \param filePathId QString with path of the file test.
		 * \param fileMD5Id QString with the MD5 ID of the file test. */
		void displayStatisticsOnTextBrowser ( const QString & filePathId, const QString & fileMD5Id );
	
		/*! \brief Method executed to display aggregated statistics of throughput tests on the text browser.
		 * \param selectedItem QAction with the selected export option. */
		void displayAggregationStatisticsOnTextBrowser ( NMGGroupTreeWidgetItem * selectedItem );

		/*! \brief Method that builds a list with the selected items.
		 * \param selectedItem[in] NMGGroupTreeWidgetItem is the parent selected item on the tree widget.
		 * \param exportList[out] NMGStatisticExportDataList is the list that will be filled in this function and that
		 * will contain a reference to every item. */
		void createSelectedList ( NMGGroupTreeWidgetItem * selectedItem, NMGStatisticExportDataList * exportList );
	
		/*! \brief Slot executed when an option of the export menu is selected to execute the proper export choice.
		 * \param groupItem[in] NMGGroupTreeWidgetItem is the parent selected (group) item on the tree widget.
		 * \param exportList[out] NMGStatisticExportDataList is the list that will be filled in this function and that
		 * will contain a reference to every item. 
		 * \return NMGStatisticAggregationData structure with the aggreged statistic data.
		 */
		NMGStatisticAggregationData * createAggreagtedList ( NMGGroupTreeWidgetItem * groupItem, NMGStatisticExportDataList * exportList );
	
		/*! \brief Slot executed when an option of the export menu is selected to execute the proper export choice.
		 * \param selectedItem[in] NMGGroupTreeWidgetItem is the parent selected (group) item on the tree widget.
		 * \param exportPathList QList of QStrings with the paths of tests that will be parsed. */
		void createSelectedPathList ( NMGGroupTreeWidgetItem * selectedItem, QStringList * exportPathList );

		/*! \brief Returns the last test path (or "" if undefined).
		* \return QString with the pas test path or "" if undefined. */
		QString getLastTestPath( );

		/*! \brief Saves the last test path from the filename path of the test
		 * \param filePath QString with the last test's path*/
		void saveLastTestPathFromFilePath ( const QString & filePath );

		enum TimeUnits { MILISECS, SECS, MINS, HOURS };
		enum SizeUnits { BITS, BYTES, KBYTES, MBYTES, GBYTES };

		QString simplifyTime ( int time, int startTimeUnit = MILISECS );
		QString simplifySize ( double unit, int startSizeUnit = BITS );

		QMenu * listMenu, * contextualMenu, * exportSubMenu;
		QAction * reloadAction;
		QToolBar * toolBar;
		NMGGroupTreeWidget * currentStatisticTree;

		NMGModulePreferencesManager * preferencesManager;
		NMGStatisticExportManager * exportManager;
		NMGStatisticFileLoader * fileLoader;
		QHash<QString, QString> paths;

		int ngroup;
};

#endif
