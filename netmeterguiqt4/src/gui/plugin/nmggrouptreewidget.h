/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#ifndef NMGGROUPTREEWIDGET_H
#define NMGGROUPTREEWIDGET_H

#include "nmggrouptreewidgetitem.h"
#include <QHeaderView>
#include <QTreeWidget>

#include <QMenu>

#define SEPARATOR "#"

typedef QList<QTreeWidgetItem *> QTreeWidgetItemList;
typedef QList<QTreeWidgetItem *>::iterator QTreeWidgetItemListIterator;

class NMGGroupTreeWidget : public QTreeWidget
{
		Q_OBJECT

	public:
		NMGGroupTreeWidget( QWidget *parent = 0 );
		~NMGGroupTreeWidget();
		void addGroup( const QString & id );
		void addTest( const QString & id, const QString & group = "" );
		void addTest( const QString & id, const QStringList & values, const QString & group = "" );
		void addFlow( const QString & id, const QStringList & values );

		void setHeaderLabel( const QString & );
		void setHeaderLabels( const QStringList & );
		void hideColumn( int );
		void showColumn( int );
		
		NMGGroupTreeWidgetItem * findItemById(const QString &, NMGGroupTreeWidgetItem * startItem = 0);

	signals:
		void itemRemoved(const QString &);
		void itemMoved(const QString &);
	
	public slots:
		void removeSelectedItems();

	protected:
		void dragMoveEvent( QDragMoveEvent * );
		void dropEvent( QDropEvent * );
		void keyReleaseEvent( QKeyEvent * );

	private slots:
		void showContextualHeaderMenu( const QPoint & );
		void hideTreeColumn( QAction * action );

	private:
		void createContextualHeaderMenu();
		void dropOn(QDropEvent *, int *, QModelIndex *);
	
		NMGGroupTreeWidgetItem * lastTestItemAdded;
		QMenu * contextualHeaderMenu;
		QStringList headerLabels;
};

#endif
