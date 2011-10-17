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
#ifndef NMGMAINWINDOWMENUBAR_H
#define NMGMAINWINDOWMENUBAR_H

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QDialog>

namespace Ui {
    class NMGAboutWidget;
} // namespace Ui

class NMGMainWindow;

class NMGMainWindowMenuBar : public QMenuBar
{
		Q_OBJECT
	public:
		NMGMainWindowMenuBar ( NMGMainWindow *parent = 0, const char *name = 0 );
		~NMGMainWindowMenuBar();
		void deleteModuleFromPluginMenu ( QString moduleName );
		void blockQAccelSignalsOfMenu ( QMenu * menu, bool blockSignals );
		QAction * addModuleMenu(QMenu * moduleMenu);

	public slots:
		void setConnected ( bool connected );
		void addNewModuleAddedToMenu ( QString moduleName, QString capabilities );
		void showAboutWidget();

/*TODO (remove)
	signals:
		void closeCurrentModuleTab();
		void closeAllModuleTabs();
		void closeAllModuleTabsButCurrent();*/

	private:
		void createMenuPlugins ( NMGMainWindow *parent );
		QString transformCapabilities ( QString capabilities );

	private:
		QMenu * fileMenu;
		QMenu * pluginsMenu;
		QMenu * settingsMenu;
		QMenu * aboutMenu;

		QAction * connectAct;
		QAction * disconnectAct;

		QSignalMapper * pluginsSignalMapper;

		QDialog w;
		Ui::NMGAboutWidget * aboutWidget;
};

#endif
