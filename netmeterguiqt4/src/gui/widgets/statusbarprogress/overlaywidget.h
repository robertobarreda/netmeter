/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2004 by David Faure <faure@kde.org>                     *
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
#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QHBoxLayout>
#include <QWidget>
#include <QFrame>
#include <QEvent>
#include <QResizeEvent>

class OverlayWidget : public QFrame
{
		Q_OBJECT

	public:
		OverlayWidget( QWidget *alignWidget, QWidget *parent, const char *name = 0 );
		~OverlayWidget();

		QWidget *alignWidget() { return m_alignwidget; }

	protected:
		void resizeEvent( QResizeEvent* ev );
		bool eventFilter( QObject* o, QEvent* e );
		virtual bool event ( QEvent* );

	private:
		void reposition();

	private:
		QHBoxLayout * m_hlayout;
		QWidget * m_alignwidget;

};

#endif
