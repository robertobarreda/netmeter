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
#ifndef NMGMAINTABBAR_H
#define NMGMAINTABBAR_H

#include <QTabBar>
#include <QMenu>
#include <QMouseEvent>

class NMGMainTabBar : public QTabBar
{
		Q_OBJECT
	public:
		NMGMainTabBar( QWidget *parent = 0, const char *name = 0 );
		~NMGMainTabBar();

	public slots:
		void deleteCurrent();
		void deleteAll();
		void deleteOthers();

	signals:
		void deleteCurrentTab();
		void deleteAllTabs();
		void deleteOthersTabs();

	private:
		void mousePressEvent( QMouseEvent *e );
		QMenu menu;
};

#endif
