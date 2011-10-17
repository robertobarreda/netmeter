/***************************************************************************
 *   Copyright (C) 2005 by Josep Ramon Benet                               *
 *   josep.ramon.benet@upc.edu                                             *
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
#include "nmgbatchprocesswidget.h"
#include <qlistview.h>

#define ID_COL 1
#define NAME_COL 2
#define PLUGIN_COL 3
#define STATE_COL 4

NMGBatchProcessWidget::NMGBatchProcessWidget(QWidget *parent, const char *name)
    :NMGBatchProcessWidgetBase(parent, name)
{
	contTasks=0;
	
	//Centrem la llista 
// 	batchListView->setColumnAlignment(ID_COL, Qt::AlignHCenter);
// 	batchListView->setColumnAlignment(NAME_COL, Qt::AlignHCenter);
// 	batchListView->setColumnAlignment(PLUGIN_COL, Qt::AlignHCenter);
// 	batchListView->setColumnAlignment(STATE_COL, Qt::AlignHCenter);
}


NMGBatchProcessWidget *NMGBatchProcessWidget::self() 
{
	if(nmgbatchprocesswidget)
		return nmgbatchprocesswidget;
	return nmgbatchprocesswidget = new NMGBatchProcessWidget(0,"nmgbatchprocesswidget"); 
}

#include "nmgbatchprocesswidget.moc"
