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
#ifndef NMGWIDGETFROMXMLEVENTHANDLER_H
#define NMGWIDGETFROMXMLEVENTHANDLER_H

#include <QObject>
#include <QMap>

#include "nmgwidgetfromxmlaction.h"
#include "nmgwidgetfromxmlbutton.h"
#include "nmgwidgetfromxmlinputwidget.h"

typedef QMap<QString, NMGWidgetFromXMLAction *> QActionMap;
typedef QActionMap::iterator ActionIterator;


typedef QMap<QString, NMGWidgetFromXMLButton *> QButtonMap;
typedef QButtonMap::iterator ButtonIterator;

class NMGWidgetFromXMLEventHandler : public QObject
{
		Q_OBJECT
	public:
		NMGWidgetFromXMLEventHandler( QObject *parent = 0, const char *name = 0 );
		~NMGWidgetFromXMLEventHandler();
		void addAction( QString id, NMGWidgetFromXMLAction * a );
		void addButton( QString id, NMGWidgetFromXMLButton * b );
		NMGWidgetFromXMLButton * getButton( QString id );

	public slots:
		void actionButton( QString buttonId );

	private:
		QActionMap actionMap;
		QButtonMap buttonMap;

};

#endif

