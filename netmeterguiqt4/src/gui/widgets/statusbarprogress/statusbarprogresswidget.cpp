/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Pau Capella <pau.capella@ac.upc.edu> 	   *
 *    and kmail authors							   *
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
#include "statusbarprogresswidget.h"

#include <nmgimagemanager.h>

#include <QPixmap>
#include <QMouseEvent>

StatusbarProgressWidget::StatusbarProgressWidget( ProgressDialog* progressdialog, QWidget* parent, bool button )
		: QFrame( parent ), m_currentitem( 0 ), m_progressdialog( progressdialog ),
		m_delaytimer( 0 ), m_busytimer( 0 )
{
	setObjectName ( "StatusbarProgressWidget" );
	m_showbutton = button;
	int w = fontMetrics().width( " 999.9 kB/s 00:00:01 " );
	m_box = new QHBoxLayout( this );
	m_box->setMargin( 0 );
	m_box->setSpacing( 0 );

	m_button = new QPushButton( this );
	m_button->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
	m_button->setIcon( QIcon( *NMGImageManager::loadPixmap( "up.png" ) ) );
	m_button->setToolTip( "Open Tests detailed progress dialog" );
	m_button->setMaximumHeight( fontMetrics().height() );

	m_box->addWidget( m_button );

	m_stack = new QStackedWidget( this );
	m_stack->setMaximumHeight( fontMetrics().height() );

	m_box->addWidget( m_stack );

	m_progressbar = new QProgressBar( this );
	//Uncomment the following lines if you want to customize the Progress Bar's Style
	//m_progressbar->setStyleSheet( "QProgressBar { border: 5px solid grey; border-radius: 5px; text-align: center; }" );
	//m_progressbar->setStyleSheet( "QProgressBar::chunk { background-color: #CD96CD; width: 10px; margin: 0.5px; }" );
	
	m_progressbar->installEventFilter( this );
	m_progressbar->setMinimumWidth( w );
	m_stack->addWidget( m_progressbar );

	m_label = new QLabel( QString::null, this );
	m_label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
	m_label->installEventFilter( this );
	m_label->setMinimumWidth( w );
	m_stack->addWidget( m_label );

	m_mode = None;
	setMode();

	connect( m_button, SIGNAL( clicked() ),
	         m_progressdialog, SLOT( slotToggleVisibility() ) );
	connect( ProgressManager::self(), SIGNAL( progressItemAdded( ProgressItem * ) ),
	         this, SLOT( slotProgressItemAdded( ProgressItem * ) ) );
	connect( ProgressManager::self(), SIGNAL( progressItemCompleted( ProgressItem * ) ),
	         this, SLOT( slotProgressItemCompleted( ProgressItem * ) ) );
	connect( m_progressdialog, SIGNAL( visibilityChanged( bool ) ),
	         this, SLOT( slotProgressDialogVisible( bool ) ) );

	m_delaytimer = new QTimer( this );
	connect( m_delaytimer, SIGNAL( timeout() ),
	         this, SLOT( slotShowItemDelayed() ) );

	setMinimumWidth( minimumSizeHint().width() );
	setMaximumWidth(m_progressbar->width () + m_button->width ());
}

// There are three cases: no progressitem, one progressitem (connect to it directly),
// or many progressitems (display busy indicator). Let's call them 0,1,N.
// In slot..Added we can only end up in 1 or N.
// In slot..Removed we can end up in 0, 1, or we can stay in N if we were already.

void StatusbarProgressWidget::slotProgressItemAdded( ProgressItem *item )
{
	if ( item->parent() ) return; // we are only interested in top level items
	connectSingleItem(); // if going to 1 item
	if ( m_currentitem )
	{
		// Exactly one item
		delete m_busytimer;
		m_busytimer = 0;
		m_delaytimer->setSingleShot( true );
		m_delaytimer->start( 1000 );
	}
	else
	{
		// N items
		if ( !m_busytimer )
		{
			m_busytimer = new QTimer( this );
			connect( m_busytimer, SIGNAL( timeout() ),
			         this, SLOT( slotBusyIndicator() ) );
			m_delaytimer->setSingleShot( true );
			m_delaytimer->start( 1000 );
		}
	}
}

void StatusbarProgressWidget::slotProgressItemCompleted( ProgressItem *item )
{
	if ( item->parent() ) return; // we are only interested in top level items
	connectSingleItem(); // if going back to 1 item
	if ( ProgressManager::self()->isEmpty() )
	{
		// No item
		// Done. In 5s the progress-widget will close, then we can clean up the statusbar
		QTimer::singleShot( 5000, this, SLOT( slotClean() ) );
	}
	else if ( m_currentitem )
	{
		// Exactly one item
		delete m_busytimer;
		m_busytimer = 0;
		activateSingleItemMode();
	}
}

void StatusbarProgressWidget::connectSingleItem()
{
	if ( m_currentitem )
	{
		disconnect( m_currentitem, SIGNAL( progressItemProgress( ProgressItem *, unsigned int ) ),
		            this, SLOT( slotProgressItemProgress( ProgressItem *, unsigned int ) ) );
		m_currentitem = 0;
	}
	m_currentitem = ProgressManager::self()->singleItem();
	if ( m_currentitem )
	{
		connect( m_currentitem, SIGNAL( progressItemProgress( ProgressItem *, unsigned int ) ),
		         this, SLOT( slotProgressItemProgress( ProgressItem *, unsigned int ) ) );
	}
}

void StatusbarProgressWidget::activateSingleItemMode()
{
	m_progressbar->setRange( 0, 100 );
	m_progressbar->setValue( m_currentitem->progress() );
	m_progressbar->setTextVisible( true );
}

void StatusbarProgressWidget::slotShowItemDelayed()
{
	bool noItems = ProgressManager::self()->isEmpty();
	if ( m_currentitem )
	{
		activateSingleItemMode();
	}
	else if ( !noItems )
	{
		// N items
		m_progressbar->setMaximum( 0 );
		m_progressbar->setTextVisible( false );
		Q_ASSERT( m_busytimer );
		if ( m_busytimer )
			m_busytimer->start( 100 );
	}

	if ( !noItems && m_mode == None )
	{
		m_mode = Progress;
		setMode();
	}
}

void StatusbarProgressWidget::slotBusyIndicator()
{
	int p = m_progressbar->value();
	m_progressbar->setValue( p + 10 );
}

void StatusbarProgressWidget::slotProgressItemProgress( ProgressItem *item, unsigned int value )
{
	Q_ASSERT( item == m_currentitem );  // the only one we should be connected to
	m_progressbar->setValue( value );
}

void StatusbarProgressWidget::setMode()
{
	switch ( m_mode )
	{
		case None:
			if ( m_showbutton )
				m_button->hide();

			m_stack->show();
			m_stack->setCurrentWidget( m_label );
			break;

		case Progress:
			m_stack->show();
			m_stack->setCurrentWidget( m_progressbar );
			if ( m_showbutton )
				m_button->show();
			break;
	}
}

void StatusbarProgressWidget::slotClean()
{
	// check if a new item showed up since we started the timer. If not, clear
	if ( ProgressManager::self()->isEmpty() )
	{
		m_progressbar->setValue( 0 );
		m_mode = None;
		setMode();
	}
}

bool StatusbarProgressWidget::eventFilter( QObject *, QEvent *ev )
{
	if ( ev->type() == QEvent::MouseButtonPress )
	{
		QMouseEvent *e = ( QMouseEvent* ) ev;

		if ( e->button() == Qt::LeftButton && m_mode != None )
		{
			// toggle view on left mouse button
			// Consensus seems to be that we should show/hide the fancy dialog when the user
			// clicks anywhere in the small one.
			m_progressdialog->slotToggleVisibility();
			return true;
		}
	}
	return false;
}

void StatusbarProgressWidget::slotProgressDialogVisible( bool b )
{
	// Update the hide/show button when the detailed one is shown/hidden
	if ( b )
	{
		m_button->setIcon( QIcon( *NMGImageManager::loadPixmap( "down.png" ) ) );
		m_button->setToolTip( "Hide detailed progress window" );
		setMode();
	}
	else
	{
		m_button->setIcon( QIcon( *NMGImageManager::loadPixmap( "up.png" ) ) );
		m_button->setToolTip( "Show detailed progress window" );
	}
}

#include "statusbarprogresswidget.moc"
