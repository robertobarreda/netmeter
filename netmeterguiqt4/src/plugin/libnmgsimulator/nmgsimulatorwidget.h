/***************************************************************************
 *   Copyright (C) 2005 by Rus Andrei Bogdan                               *
 *   bogdanrus2004@yahoo.com                                               *
 *   Copyright (C) 2008 by Joan Sala Calero                                *
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

#ifndef NMGSIMULATORWIDGET_H
#define NMGSIMULATORWIDGET_H

#define ID_PREFERENCE_LAST_TEST_PATH	"NMGSimulatorLastTestPath"


#include <nmgisender.h>
#include <progressmanager.h>
#include <nmgmodtabwidgetmanager.h>
#include <nmgmodulepreferencesmanager.h>
#include <messagebar.h>
#include <iostream>

#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QDomElement>
#include <QDomDocument> 
#include <QPushButton>
#include <QMessageBox>
#include <QString>
#include <QComboBox> 
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QSpinBox>
#include <QToolBar>
#include <QMenu>

#include "ui_nmgsimulatorwidgetbase.h"
#include "nmgutil.h"

namespace Ui {
    class NMGMGenMainWidget;
}

class NMGSimulatorWidget : public QWidget, public Ui::NMGSimulatorWidgetBase 
{
Q_OBJECT

public:
	NMGSimulatorWidget(NMGISender * sender, NMGModTabWidgetManager * tabWidgetManager, NMGModulePreferencesManager * prefMan, QWidget *parent = 0, const char *name = 0);
	~NMGSimulatorWidget();

	ProgressItem * progressItem;
	QString newName;

	bool cleanInterface[100];
	void createFlowListMenu();
	void updateFlowListIdentifier();
	void updateXMLIdentifiers();
	QString getLastTestPath();
	void saveLastTestPathFromFilePath(const QString & filePath);

public slots:
	
	void addSource();
	void addNS();
	void addDestination();
	void addFlowSlot();
	void saveFlowSlot();
	void showItemSelected();
	void showItem(QTreeWidgetItem * itemShow);
	void delFlowSlot();
	void saveSettingsSlot();
	void loadSettingsSlot();
	void connectSlot();
	void applySettingsSlot();
	void sendSettingsSlot();
	void nextSettingSlot(int indexSet);
	void showMenu(const QPoint & pos);

	QToolBar * getToolBar();
	QMenu * getPopupMenu();

private slots:
 
	QString getDelaySlot(int currentIndex, QString netemMajor);
	QString getDDSlot(int currentIndex, QString netemMajor);
	void delayGroupChanged();
	void delayGroupChanged(QString value);
	void dDGroupChanged();
	void dDGroupChanged(QString value);
	QString returnDelayGroup();
	QString returnDDGroup();
	void l4ProtocolChanged(); 
	void l4ProtocolChanged(QString value);

private:
	QDomDocument doc;
	QDomElement root, router, settings;
	QTreeWidgetItem * lastItem;
	QString port, oldName, clean;
	QToolBar * toolBar;
	QMenu * flowListMenu;

	NMGISender * sender;
	NMGModTabWidgetManager * tabWidgetManager;
	NMGModulePreferencesManager * preferencesManager;
};

#endif
