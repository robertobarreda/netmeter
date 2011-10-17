/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#ifndef NMGGRAPHICSWIDGET_H
#define NMGGRAPHICSWIDGET_H

#include "ui_nmggraphicswidgetbase.h"
#include "nmgtoolbar.h"
#include "nmgtestloaderhandler.h"
#include "nmggraphicserror.h"
#include <nmgmodulepreferencesmanager.h>

class NMGGraphicsWidget : public QWidget, public Ui::NMGGraphicsWidgetBase
{
  Q_OBJECT

  public:

    NMGGraphicsWidget(QWidget *parent = 0, 
                      const char *name = 0, 
                      NMGModulePreferencesManager * preferences = 0);

    virtual ~NMGGraphicsWidget();
    NMGToolBar* getToolBar();
    bool loadData ( QString data );

  public slots:

    void slotAddTests();
    void slotDeleteAllMeasures();
    void slotExportTo();

  private slots:

    void slotProcessFileRead(const QString& fileName);

  private:

    void createComponents();
    void createConnections();

    NMGModulePreferencesManager* preferences;
    NMGToolBar* toolbar;
    NMGTestLoaderHandler* loader;
};

#endif
