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
#ifndef NMGMODULETOOLBOX_H
#define NMGMODULETOOLBOX_H

#include "nmgmoduletoolboxitem.h"

#include <nmgmodule.h>
#include <nmgmodulemanager.h>

#include <QToolBox>
#include <QWidget>
#include <QMap>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QSpacerItem>

typedef QMap<QString,QWidget *> NMGModuleToolBoxMap;
typedef NMGModuleToolBoxMap::iterator NMGModuleToolBoxMapIterator;

typedef QMap<QString,QSpacerItem *> NMGModuleToolBoxSpacerMap;
typedef NMGModuleToolBoxSpacerMap::iterator NMGModuleToolBoxSpacerMapIterator;

class NMGModuleToolBox : public QToolBox
{
		Q_OBJECT
	public:
		NMGModuleToolBox ( QWidget *parent = 0, const char *name = 0 );
		~NMGModuleToolBox();
		void mousePressEvent ( QMouseEvent * e );

	signals:
		void moduleActivated ( QString );
		void createInstaceModule ( const QString & );
		void unloadModule ( QString );
		void loadNewModule ( QString );

	public slots:
		void addModule ( QString moduleName, QString capabilities );

	private slots:
		void menuCreateInstance();
		void menuUnloadModule();
		void menuLoadModule();

	private:
		QString transformCapabilities ( QString capabilities );
		QString getModuleName ( NMModule * m );
		QString getModuleNameOfRightClickedWidget();
		void loadExistingModules();
		void addModuleWidgets ( QWidget * widget, QVBoxLayout *, QSpacerItem * spacer, QString moduleName );

	private:
		NMGModuleToolBoxMap capabilitiesMap;
		NMGModuleToolBoxSpacerMap spacerMap;
		QMenu menu;
		QWidget * widgetRightClicked;

		QAction * idMenuCreateInstance;
		QAction * idMenuUnloadModule;
};

#endif
