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
#include "messagebar.h"
#include "popupmessage.h"

#include <config.h>

#include <QHBoxLayout>

/**
	rbarreda: All commented code is unused since nmg-qt3 version. It is ready to be removed
**/

MessageBar *MessageBar::m_instance = 0;

MessageBar::MessageBar ( PopupMessage *popupmessage, QWidget *parent, const char *name )
	: QWidget ( parent ), m_popupmessage ( popupmessage )
{
	setObjectName ( name );
	//QBoxLayout *mainlayout = new QHBoxLayout( this, 2, /*spacing*/5 );
	//we need extra spacing due to the way we paint the surrounding boxes
	//QBoxLayout *layout = new QHBoxLayout( mainlayout, /*spacing*/5 );
	m_instance = this;

	QHBoxLayout *mainlayout = new QHBoxLayout ( this );
	mainlayout->setMargin( 1 );
	mainlayout->setSpacing ( 5 );
	m_mainTextLabel = new QLabel ( this );
	m_mainTextLabel->setObjectName ( "MainTextLabel" );
	m_mainTextLabel->setText ( " Disconnected " );
	//m_mainTextLabel->show();

	mainlayout->addWidget ( m_mainTextLabel );

	//mainlayout->setStretchFactor( m_mainTextLabel, 3 );
	//layout->add( m_mainTextLabel);
}

MessageBar *MessageBar::self()
{
	if ( !m_instance )
		return 0;
	return m_instance;
}

/**
\fn MessageBar::message( const QString &text)
\brief Print a popup message with text text.
*/
void MessageBar::longMessage ( const QString &text )
{
	if ( text.isEmpty() )
		return;

	MessageBar *instance = MessageBar::self();
	if ( !instance )
		return;

	instance->popupMessage()->displayMessage ( text );

// 	PopupMessage *popupmessage = instance->popupMessage();
// 	popupmessage->raise();
// 	popupmessage->setText( text );
// 	popupmessage->reposition();
// 	popupmessage->display();
// 	instance->raise();
}

/**
\fn MessageBar::longMessage( const QString &text)
\brief Print a popup message with text text and image pixmap
*/
void MessageBar::longMessage ( const QString &text, const QPixmap &pixmap )
{
	if ( text.isEmpty() )
		return;

	MessageBar *instance = MessageBar::self();
	if ( !instance )
		return;

	PopupMessage *popupmessage = instance->popupMessage();
	popupmessage->raise();
	popupmessage->setText ( text );
	popupmessage->setImage ( pixmap );
	popupmessage->reposition();
	popupmessage->display();

	instance->raise();
}

/**
\fn MessageBar::shortMessage( const QString &text)
\brief Print a message with text text in a status bar.
*/
void MessageBar::shortMessage ( const QString &text )
{
	if ( text.isEmpty() )
		return;

	MessageBar *instance = MessageBar::self();
	if ( !instance )
		return;

	instance->m_mainTextLabel->setText ( text );
	instance->m_mainTextLabel->setToolTip ( text );
	//Per si hi error!.
	//instance->m_mainTextLabel->setPalette( QToolTip::palette() );
}

#include "messagebar.moc"
