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
#include "overlaywidget.h"

OverlayWidget::OverlayWidget( QWidget* alignwidget, QWidget* parent, const char* name )
		: QFrame( parent ), m_alignwidget( alignwidget )
{
	setObjectName( name );
	parent->installEventFilter( this );

	m_hlayout = new QHBoxLayout ( this );
	m_hlayout->setSpacing( 0 ); // QHBoxLayout
	m_hlayout->setMargin( 1 );
}

OverlayWidget::~OverlayWidget() {}

void OverlayWidget::reposition()
{
	setMaximumSize( parentWidget()->size() );
	adjustSize();

	// p is in the alignWidget's coordinates
	QPoint p;
	// We are always above the alignWidget, right-aligned with it.
	p.setX( m_alignwidget->width() - width() );
	p.setY( -height() );
	// Position in the toplevelwidget's coordinates
	QPoint pTopLevel = m_alignwidget->mapTo( topLevelWidget(), p );
	// Position in the widget's parentWidget coordinates
	QPoint pParent = parentWidget()->mapFrom( topLevelWidget(), pTopLevel );
	// keep it on the screen
	if ( pParent.x() < 0 )
		pParent.rx() = 0;
	// Move 'this' to that position.
	move( pParent );
}

bool OverlayWidget::eventFilter( QObject* o, QEvent* e )
{
	if (  ( e->type() == QEvent::Move || e->type() == QEvent::Resize ) )
		reposition();
	return QFrame::eventFilter( o, e );
}

void OverlayWidget::resizeEvent( QResizeEvent* ev )
{
	reposition();
	QFrame::resizeEvent( ev );
}

bool OverlayWidget::event( QEvent *e )
{
	if ( e->type() == QEvent::ChildAdded )
		adjustSize();
	return QFrame::event( e );
}

#include "overlaywidget.moc"
