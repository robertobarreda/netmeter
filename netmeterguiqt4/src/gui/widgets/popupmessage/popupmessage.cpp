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
#include "popupmessage.h"

#include <QTextBrowser>
#include <QPainter>
#include <QTimer>
#include <QIcon>
#include <QHBoxLayout>
#include <QPalette>
#include <QToolTip>

#include <config.h>
#include <nmgimagemanager.h>

#include <iostream>
using namespace std;

PopupMessage::PopupMessage( QWidget *parent, QWidget *anchor, int timeout, const char *name )
		: OverlayWidget( parent, anchor, name )
		, m_anchor( anchor )
		, m_parent( parent )
		, m_offset( 0 )
		, m_stage( 1 )
		, m_timeout( timeout )
		, m_showCounter( true )
{
	setFrameStyle( QFrame::Panel | QFrame::Raised );
	setFrameShape( QFrame::StyledPanel );
	setWindowFlags( Qt::X11BypassWindowManagerHint );
	setAutoFillBackground(true);

	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

	setMaximumWidth (anchor->width());

	delete layout(); //Note: We must first delete the existing layout manager to add our layout

	QHBoxLayout *hbox = new QHBoxLayout( this );
	hbox->setSpacing( 12 );
	setLayout(hbox);

	//Vertical Progress Bar
	countDownBar = new QProgressBar( this );
	countDownBar->setFixedSize( 7, 30 );
	//countDownBar->setStyleSheet( "QProgressBar {border: 1px solid grey; border-radius: 5px; }" );
	//countDownBar->setStyleSheet( "QProgressBar::chunk {background-color: #05B8CC;}");
	countDownBar->setTextVisible( false );
	countDownBar->setRange( 0, 50 );
	countDownBar->setOrientation( Qt::Vertical );
	countDownBar->setInvertedAppearance( TRUE );
	countDownBar->setValue( 0 );
	hbox->addWidget( countDownBar );

	//Label to place an image
	imageLabel = new QLabel( this );
	imageLabel->setObjectName( "image" );
	hbox->addWidget( imageLabel );

	//Label to place the text message
	txtLabel = new QLabel( this );
	txtLabel->setObjectName( "label" );
	//txtLabel->setTextFormat( Qt::RichText ); //Uncomment for RichText Popup labels
	txtLabel->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );
	hbox->addWidget( txtLabel );

	hbox->addItem( new QSpacerItem( 4, 4, QSizePolicy::Expanding, QSizePolicy::Preferred ) );

	//Close button for the PopupMessage
	closeButton = new QPushButton( QIcon( *NMGImageManager::loadPixmap( "close.png" ) ), "(Close)", this );
	closeButton->setObjectName( "closeButton" );
	hbox->addWidget( closeButton );
	connect( closeButton, SIGNAL( clicked() ), this, SLOT( close() ) );
}

void PopupMessage::addWidget( QWidget *widget )
{
	layout()->addWidget( widget );
	adjustSize();
}

void PopupMessage::setShowCloseButton( const bool show )
{
	closeButton->setVisible( show );
	adjustSize();
}

void PopupMessage::setShowCounter( const bool show )
{
	m_showCounter = show;
	countDownBar->setVisible( show );
	adjustSize();
}

void PopupMessage::setText( const QString &text )
{
	txtLabel->setText( text );
	adjustSize();
}

void PopupMessage::setImage( const QString &location )
{
	imageLabel->setPixmap( QPixmap( location ) );
	adjustSize();
}

void PopupMessage::setImage( const QPixmap &pix )
{
	imageLabel->setPixmap( pix );
	adjustSize();
}

////////////////////////////////////////////////////////////////////////
//     Public Slots
////////////////////////////////////////////////////////////////////////

void PopupMessage::close() //SLOT
{
	m_stage = 3;
	killTimer( m_timerId );
	m_timerId = startTimer( 6 );
}

void PopupMessage::display() //SLOT
{
	m_timerId = startTimer( 6 );
	show();
}

////////////////////////////////////////////////////////////////////////
//     Protected
////////////////////////////////////////////////////////////////////////

void PopupMessage::timerEvent( QTimerEvent* )
{
	slideMask();
}

void PopupMessage::countDown()
{
	if ( !m_timeout )
	{
		killTimer( m_timerId );
		return;
	}

	if ( !testAttribute( Qt::WA_UnderMouse ) ) {
		if ( countDownBar->value() <  countDownBar->maximum() )
			countDownBar->setValue( countDownBar->value() + 1 );
		else
		{
			m_stage = 3;
			killTimer( m_timerId );
			m_timerId = startTimer( 6 );
		}
	}
}

void PopupMessage::slideMask()
{
	switch ( m_stage )
	{
		case 1: //raise
			move( 0, m_parent->y() - m_offset );
			m_offset++;
			if ( m_offset > height() )
			{
				killTimer( m_timerId );

				if ( m_timeout )
				{
					m_timerId = startTimer( 40 );
					m_stage = 2;
				}
			}
			break;

		case 2: //fill in pause timer bar
			countDown();
			break;

		case 3: //lower
			m_offset--;
			move( 0, m_parent->y() - m_offset );

			if ( m_offset < 0 )
			{
				//Stop and restart. Netmetergui's implementation
				killTimer( m_timerId );
				m_stage = 1;
				m_offset = 0;
				countDownBar->reset();
				countDownBar->setValue(0);
				hide();
				QTimer::singleShot( 500, this, SLOT( slotUpdateListMessage() ) );
			}
			break;
	}
}

void PopupMessage::displayMessage( const QString &text )
{
	m_list.append( MessageItem( text ) );
	if ( m_list.count() == 1 )
		nextMessage();
}

void PopupMessage::nextMessage()
{
	if ( !m_list.count() ) return;

	MessageItem item = m_list.first();

	raise();
	setText( item.m_text );
	reposition();
	display();
}

void PopupMessage::slotUpdateListMessage()
{
	m_list.pop_front();
	if ( m_list.count() > 0 )
		nextMessage();
}

#include "popupmessage.moc"
