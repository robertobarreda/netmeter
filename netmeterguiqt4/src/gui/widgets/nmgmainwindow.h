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
#ifndef NMGMAINWINDOW_H
#define NMGMAINWINDOW_H

#include "nmgmoduletoolbox.h"
#include "nmgtaskprogressbar.h"
#include "nmgmaintabwidget.h"
#include "nmgmainwindowtoolbar.h"
#include "nmgmainwindowmenubar.h"

#include <QAction>
#include <QMainWindow>
#include <QObject>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QTabWidget>
#include <QStatusBar>
#include <QSplitter>

#define STATUS_APPLICATION_STARTED	0
#define STATUS_APPLICATION_EXIT_SENT	1

#define MAIN_WINDOW_SIZE_LAST_WIDTH	"MainWindowSizeLastWidth"
#define MAIN_WINDOW_SIZE_LAST_HEIGHT	"MainWindowSizeLastHeight"

#define MAIN_WINDOW_SIZE_DEFAULT_WIDTH	677
#define MAIN_WINDOW_SIZE_DEFAULT_HEIGHT	426

class NMGMainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		NMGMainWindow ( QWidget *parent = 0, const char *name = 0 );
		~NMGMainWindow();
		QToolBar *  createNewToolBar();
		NMGModule * getCurrentModule();
		NMGModuleToolBox * getModuleToolBox();
		NMGMainTabWidget * getTabWidget();

	public slots:
		void showModule ( QString s );
		void createNewInstanceModule ( const QString &  module );
		void unloadAllInstancesOfModule ( QString module );
		void closeCurrentTab();
		void closeAllTabs();
		void closeOtherTabs();
		void showTask ( QString id, QString moduleName, QString data );
		void currentTabChanged ();
		void addWidgetFromExistingModule ( NMGModule * mod );
		void connectionStatusChanged ( NMGStatus status );
		void connectToServer();
		void disconnectFromServer();
		void quitApplication();
		void closeEvent ( QCloseEvent *e );

	private:
		void createWidgets();
		void showCurrentToolbarAndMenu ( QString name,NMGModule * m );
		void createNewModuleTab ( QString name, QWidget * w, QString icon );
		void connectWidgets();

	private:
		NMGModuleToolBox * moduleToolBox;
		NMGMainTabWidget * tabWidget;
		NMGMainWindowToolBar * toolBar;
		NMGMainWindowMenuBar * menuBar;

 		QToolBar * toolBarModule;
		QAction * menuIdentifier;
		QMenu * previousMenu;
		int applicationStatus;
};

#endif
