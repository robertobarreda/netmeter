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
#ifndef NMGGROUPTREEWIDGETITEM_H
#define NMGGROUPTREEWIDGETITEM_H

#include <QTreeWidgetItem>
#include <QString>

class NMGGroupTreeWidgetItem : public QTreeWidgetItem
{
	public:
		enum ItemType { Group = 10, Test = 20, Flow = 30 };

		inline NMGGroupTreeWidgetItem( QTreeWidget * parent, const QString & identifier, int type = Group ) 
			: QTreeWidgetItem( parent, type ), id( identifier) {};
	
		inline NMGGroupTreeWidgetItem( QTreeWidget * parent, const QString & identifier, 
		                               const QStringList & values, int type = Group ) 
			: QTreeWidgetItem( parent, values, type ), id( identifier) {};
	
		inline NMGGroupTreeWidgetItem( QTreeWidgetItem * parent, const QString & identifier, int type = Test ) 
			: QTreeWidgetItem( parent, type ), id( identifier) {};
	
		inline NMGGroupTreeWidgetItem( QTreeWidgetItem * parent, const QString & identifier, 
		                               const QStringList & values, int type = Test ) 
			: QTreeWidgetItem( parent, values, type ), id( identifier) {};

		inline QString & getId() { return id; };
		void printType();
	
		/*!defined on QTreeWidgetItem
		 * \fn ItemType type() const */
	
	private:
		QString id;
};

#endif
