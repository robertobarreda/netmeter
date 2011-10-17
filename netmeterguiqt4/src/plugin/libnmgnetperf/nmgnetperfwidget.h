/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#ifndef NMGNETPERFWIDGET_H
#define NMGNETPERFWIDGET_H

#include "ui_nmgnetperfwidgetbase.h"
#include <nmgmetadatatags.h>
#include <nmgaggregationorientedtags.h>

#include "nmgnetperfxmlinterpret.h"
#include "nmgnetperfwidgeterrornotifier.h"
#include <progressmanager.h>
#include <nmgimodulestationmanager.h>
#include <nmgimodulecommunication.h>
#include <nmgmodulepreferencesmanager.h>
#include <nmgisender.h>
#include <nmgmodtabwidgetmanager.h>

#include <QMenu>
#include <QToolBar>

#define DEFAULT_PORT 12865

#define ID_NETPERF_PATH		"NETPERF_NetperfPath"
#define ID_NETSERVER_PATH	"NETPERF_NetserverPath"

namespace Ui {
    class NMGNetperfWidgetBase;
} // namespace Ui

class NMGNetperfWidget : public QWidget, public Ui::NMGNetperfWidgetBase
{
		Q_OBJECT
	public:
		NMGNetperfWidget ( NMGISender * s, NMGIModuleCommunication * mc, NMGIModuleStationManager * sm,
		                   NMGModTabWidgetManager * tabWidgetManager, NMGModulePreferencesManager * prefMan,
		                   QWidget *parent = 0, const char *name = 0 );
		~NMGNetperfWidget();
		/**
		 * Returns the station manager
		 * @return stationManager
		 */
		inline NMGIModuleStationManager * getStationManager() { return stationManager; }
		void notifyResult ( QString data );
		void notifyError ( int errorType, QString errorMessage );
		void notifyStatus ( QString status );
		/**
		 * Returns TRUE if the test is running, FALSE otherwise
		 * @return
 		*/
		inline bool isTestRunning() { return cmdStop->isEnabled(); }
		bool isProtocolIPv4();

	public slots:
		void loadTestFromDisk();
		void saveTestToDisk();
		void buttonStartClicked();
		void buttonStopClicked();
		void buttonStatisticClicked();
		void buttonGraphicClicked();

		/**
		 * Returns the Pop-up menu of the widget
		 * @return menu
		 */
		inline QMenu * getPopupMenu() { return menu; }
		QToolBar * getToolBar();
		QString getLastTestPath();
		void saveLastTestPathFromFilePath ( const QString & filePath );
		/**
		 * Test cancelled using the progress bar widget
		 * @param item
		 */
		inline void testCancelled ( ProgressItem * ) { buttonStopClicked(); }
		void incrementProgressItemBar();

	private:
		void createMenu();
		bool allFieldsNeededCorrect();
		void updateWidgetBecauseOfTestEnd();
		int getMaximumEndTime();
		QString getLeftTimeTestString();

		QMenu * menu;
		QToolBar * toolBar;
	        NMGIModuleCommunication * moduleCommunication;
		NMGIModuleStationManager * stationManager;
		NMGISender * sender;
		NMGModTabWidgetManager * tabWidgetManager;
		NMGModulePreferencesManager * preferencesManager;
		NMGNetperfXMLInterpret interpret;
		ProgressItem * progressItem;
		int maxTestTime;
		int stepTime;
		int leftTime;
		QTimer progressTimer;
		bool runningNetperfAndNetserver;
		NMGNetperfWidgetErrorNotifier errorNotifier;

                QString idSubscribeStatistic, idSubscribeGraphic; // ID of the statistic and the graphic subscripted modules
                QString lastResultFile; // Stores the path to the last result file recived to send it to the statistic
};

#endif
