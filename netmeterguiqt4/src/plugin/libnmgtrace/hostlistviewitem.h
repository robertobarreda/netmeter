/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella                                     *
 *   pcapella@ac.upc.edu                                                   *
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
#ifndef HOSTLISTVIEWITEM_H
#define HOSTLISTVIEWITEM_H

#include <QTreeWidgetItem>
#include <QColorGroup>

/**
	@author Pau Capella <pcapella@ac.upc.edu>
	@brief Subclass of listViewItem that supports background colors
*/
class HostListViewItem : public QTreeWidgetItem
{
public:
	HostListViewItem(QTreeWidget *parent);
	~HostListViewItem();

	QColor backgroundColor();
	void clearColor();

private:
	QColor m_color;
	QColor m_defaultcolor;
};

#endif
