/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Joan Sala                                       *
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
#ifndef NMGMGENFLOWLISTITEM_H
#define NMGMGENFLOWLISTITEM_H

#include <QTreeWidget>
#include <QTreeWidgetItem>

#define FLOW_LIST_ITEM_UNKNOWN			0
#define FLOW_LIST_ITEM_FLOW			1
#define FLOW_LIST_ITEM_MODIFICATION		2
#define FLOW_LIST_ITEM_EXTENSION_HEADER		3

/**
@author Ernest Nieto
*/
class NMGMGenFlowListItem : public QTreeWidgetItem
{
public:
    NMGMGenFlowListItem(QTreeWidget * parent);
    NMGMGenFlowListItem(QTreeWidgetItem * parent);
    ~NMGMGenFlowListItem();
    
    virtual int getFlowItemType();

};

#endif
