/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Pau Capella <pcapella@ac.upc.edu>               *
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
#ifndef POPUPMESSAGE_H
#define POPUPMESSAGE_H

#include "overlaywidget.h"

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QPixmap>
#include <QList>
#include <QTimerEvent>

class MessageItem
{
	public:
		inline MessageItem() {}
		inline MessageItem( const QString &text ) { m_text = text; }
		inline MessageItem( const QString &text, const QPixmap &pixmap ) { m_text = text; m_pixmap = pixmap; }

		QString m_text;
		QPixmap m_pixmap;
};

/**
* @class PopupMessage
* @short Widget that animates itself into a position relative to an anchor widget
*/
class PopupMessage : public OverlayWidget
{
		Q_OBJECT
	public:
		/**
		* @param anchor  : which widget to tie the popup widget to.
		* @param timeout : how long to wait before auto closing. A value of 0 means close only on pressing the closeButton or close() is called.
		*/
		PopupMessage( QWidget *parent, QWidget *anchor, int timeout = 5000 /*milliseconds*/, const char* name = 0 );

		void addWidget( QWidget *widget );
		void setShowCloseButton( const bool show );
		void setShowCounter( const bool show );
		void setImage( const QString &location );
		void setImage( const QPixmap &pix );
		void setText( const QString &text );
		void setTimeout( const int time ) { m_timeout = time; }

		void displayMessage( const QString &text );
		void nextMessage();

	public slots:
		void close();
		void display();

	private slots:
		void slotUpdateListMessage();

	protected:
		void timerEvent( QTimerEvent* );
		void countDown();
		void slideMask();

	private:
		QProgressBar *countDownBar;
		QWidget     *m_anchor;
		QWidget     *m_parent;
		QLabel	    *imageLabel;
		QLabel	    *txtLabel;
		QPushButton *closeButton;

		int      m_offset;
		int      m_stage;
		int      m_timeout;
		int      m_timerId;

		bool     m_showCounter;

		QList<MessageItem> m_list;
};

#endif
