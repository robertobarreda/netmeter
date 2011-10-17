/***************************************************************************
 *   Copyright (C) 2005 by Joan Sala                                       *
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

#ifndef NMGSYNCWIDGET_H
#define NMGSYNCWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>

#include <progressmanager.h>
#include "ui_nmgsyncwidgetbase.h"
#include <nmgisender.h>

namespace Ui {
    class NMGSyncWidgetBase;
} // namespace Ui
    

class NMGSyncWidget: public QWidget, public Ui::NMGSyncWidgetBase 
{
	Q_OBJECT

	public:
		NMGSyncWidget(NMGISender * s, QWidget *parent = 0, const char *name = 0);
		~NMGSyncWidget();
	
		QMenu * getPopupMenu();
		void resultReceived( QString data);
		void notifyError( int errorType, QString errorMessage);

	private slots:
		virtual void startSync();
		virtual void stopSync();
		
		void testCancelled( ProgressItem *);

	signals:
		void send(QString value);

	private:
		QMenu *menu;
		bool alredystarted;
		NMGISender *sender;
		ProgressItem * progressItem;
};

#endif
