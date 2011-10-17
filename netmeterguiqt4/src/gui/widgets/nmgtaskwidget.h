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
#ifndef NMGTASKWIDGET_H
#define NMGTASKWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QProgressBar>
#include <QLabel>
#include <QAction>
#include <QHBoxLayout>
#include <QMouseEvent>

#define MAXPROGRESS 100
#define MAXWIDTHBAR 200
#define MINWIDTHBAR 100
#define MAXHEIGHTBAR 16

class NMGTaskWidget : public QWidget
{
		Q_OBJECT
	public:
		NMGTaskWidget ( QWidget *parent = 0, const char *name = 0 );
		~NMGTaskWidget();
		void mousePressEvent ( QMouseEvent * e );
		void setToolTip ( QString toolTip );
		void setId ( QAction * id );
		QAction * getId();
		void setName ( QString s );

	public slots:
		void advance();

	private:
		QAction * id;
		QProgressBar * bar;
		QLabel * label;
		QHBoxLayout * layout;
};

#endif
