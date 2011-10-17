/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
 *   Copyright (C) 2007 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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
#ifndef NMGMGENWIDGET_H
#define NMGMGENWIDGET_H

#include "nmgmgenxmltags.h"
#include "nmgxmlflowsortedlist.h"

#include <QTreeWidget>
#include <QMenu>
#include <QWidget>
#include <QPoint>
#include <QTimer>
#include <QToolBar>

#include <qdom.h>

#include <progressmanager.h>

#include "nmgmgenflow.h"
#include "nmgmgenflowextensionheader.h"
#include "ui_nmgmgenmainwidget.h"
#include "nmgmgenxmlinterpret.h"
#include "nmgmgenwidgeterrornotifier.h"

#include <nmgimodulestationmanager.h>
#include <nmgmodulepreferencesmanager.h>
#include <nmgimodulecommunication.h>
#include <nmgisender.h>
#include <nmgxmlconfigurationfile.h>
#include <nmgimagemanager.h>
#include <nmgmodtabwidgetmanager.h>

#define LEFT_BUTTON	1
#define RIGHT_BUTTON	2
#define MID_BUTTON	4

#define TAB_FLOW_PROPERTIES	"Flow properties"
#define TAB_MOD_FLOW_PROPERTIES	"Modification properties"
#define TAB_EXTENDED_ATTRIBUTES	"Extended attributes"
#define TAB_IPV4_OPTIONS	"IPv4 Options"
#define TAB_IPV6_OPTIONS	"IPv6 Options"
#define TAB_IPV6_EXT_HEADERS	"IPv6 Extension Headers"
#define TAB_RESULT_FILE		"Result file"
#define ORENETA_MODULE_NAME	"nmgoreneta"

namespace Ui
{
	class NMGMGenMainWidget;
}

class NMGMGenWidget : public QWidget
{
		Q_OBJECT

	public:
		NMGMGenWidget ( NMGISender * s, NMGIModuleCommunication * mc, NMGIModuleStationManager * sm, NMGMGenFlowManager * fm,
		                NMGModTabWidgetManager * tabWidgetManager, NMGModulePreferencesManager * prefMan,
		                QWidget *parent = 0, const char *name = 0 );

		virtual ~NMGMGenWidget();

		NMGIModuleStationManager * getStationManager();
		NMGMGenFlowManager * getFlowManager();
		void notifyResult ( QString data );
		void notifyError ( int errorType, QString errorMessage );
		void notifyStatus ( QString status );
		bool isTestRunning();

		// QT4 widget stuff
		QWidget *w;
		bool syncActivated;
		Ui::NMGMGenMainWidget * mainWidget;

	public slots:

		void addNewFlow();
		void deleteItem();
		void deleteAllItems();
		void addModificationToFlow();
		void loadTestFromDisk();
		void saveTestToDisk();
		void flowListRightButtonPressed ( QTreeWidgetItem *, const QPoint &, int );
		void flowListMouseButtonClicked ( QTreeWidgetItem * item, int c );
		void flowListItemSelected();
		void showItemProperties ( QTreeWidgetItem * );
		void updateEthernetAndATMBAndwith ( int val = 0 );
		void buttonStartClicked();
		void buttonStopClicked();
                void buttonStatisticClicked();
		void buttonGraphicClicked();
		void buttonOrenetaClicked();
		void checkBoxChanged ( bool on );
		//void sourceStationChanged(NMGStation * station);
		QMenu * getPopupMenu();
		QToolBar * getToolBar();
		QString getLastTestPath();
		void saveLastTestPathFromFilePath ( const QString & filePath );
		void testCancelled ( ProgressItem * );
		void incrementProgressItemBar();
		void activeBurstPattern ( const QString & mgenVersion );
		void checkRootToS ( bool on );
		void showMenu ( const QPoint & pos );

	signals:
		void activatedFlowOrMod ( QTreeWidgetItem * );
		void activatedMgenVersion ( const QString & );

	private:
		void createFlowListMenu();
		bool deleteAllItemsWithResult();
		void deleteItem ( QTreeWidgetItem * );
		void updateTabWithItem ( QTreeWidgetItem * );
		void updateTabContentToItem ( QTreeWidgetItem * );
		void removeAllTabFlowPages();
		bool allFieldsNeededCorrect();
		bool isProtocolIPv4();
		void updateFlowListIdentifier();
		void updateWidgetBecauseOfTestEnd();
		void updateFinishedMgen ( QString data );
		QTreeWidgetItem * getSelectedItem();
		int getMaximumFlowEndTime();
		QString getLeftTimeTestString();

		QMenu * flowListMenu;
		QToolBar * toolBar;
		QTreeWidgetItem * lastSelectedItem;
		NMGIModuleStationManager * stationManager;
		NMGISender * sender;
		bool finishedMgen;

		NMGMGenFlowManager * flowManager;
		NMGModTabWidgetManager * tabWidgetManager;
		NMGIModuleCommunication * moduleCommunication;
		NMGModulePreferencesManager * preferencesManager;
		int lastFlowId;
		NMGMGenXMLInterpret interpret;
		ProgressItem * progressItem;
		int maxTestTime;
		int stepTime;
		int leftTime;
		QTimer progressTimer;
		QString idSync, xmlSync;
		bool runningMgenAndDrec;
		NMGMgenWidgetErrorNotifier errorNotifier;

                QString idSubscribeStatistic, idSubscribeGraphic; // ID of the statistic and the graphic subscripted modules
		QString lastResultFile; // Stores the path to the last result file recived to send it to the statistic
};

#endif
