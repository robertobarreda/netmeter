/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Pau Capella <pcapella@ac.upc.edu>,              *
 *   based in David Faure <faure@kde.org> software                         *
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
#include "progressdialog.h"
#include <nmgimagemanager.h>

#include <QApplication>
#include <QScrollBar>
#include <QTimer>
#include <QEvent>
#include <QHBoxLayout>

// TransactionItemView ----------------------------------------------------------------------------

TransactionItemView::TransactionItemView ( QWidget * parent, const char * name )
		: QScrollArea ( parent )
{
	setObjectName ( name );
	setFrameShape ( QFrame::NoFrame );
	m_bigbox = new QVBoxLayout ( this );
	setLayout ( m_bigbox );
	m_bigbox->setSpacing ( 5 );
	setWidgetResizable ( true ); //setResizePolicy( Q3ScrollView::AutoOneFit ); // Fit so that the box expands horizontally
	setSizePolicy ( QSizePolicy::Preferred, QSizePolicy::Fixed );
}

TransactionItem* TransactionItemView::addTransactionItem ( ProgressItem* item, bool first )
{
	//qDebug( "AddTransactionAdded" );
	TransactionItem *ti = new TransactionItem ( m_bigbox->parentWidget(), item, first );
	//ti->hide();
	//QTimer::singleShot( 1000, ti, SLOT( show() ) );
	m_bigbox->addWidget ( ti );
	resize ( m_bigbox->parentWidget()->width(), m_bigbox->parentWidget()->height() );
	return ti;
}

/*void TransactionItemView::resizeContents( int w, int h )
{
	resize( w, h );
	// Tell the layout in the parent (progressdialog) that our size changed
	updateGeometry();
	// Resize the parent (progressdialog) - this works but resize horizontally too often
	//parentWidget()->adjustSize();

	QApplication::sendPostedEvents( 0, QEvent::ChildAdded );
	QApplication::sendPostedEvents( 0, QEvent::LayoutRequest );
	QSize sz = parentWidget()->sizeHint();
	int currentWidth = parentWidget()->width();
	// Don't resize to sz.width() every time when it only reduces a little bit
	if (( currentWidth < sz.width() ) || ( currentWidth > sz.width() + 100 ) )
		currentWidth = sz.width();
	parentWidget()->resize( currentWidth, sz.height() );
}*/

void TransactionItemView::resizeEvent ( QResizeEvent *event )
{
	// Tell the layout in the parent (progressdialog) that our size changed
	updateGeometry();

	QSize sz = parentWidget()->sizeHint();
	int currentWidth = parentWidget()->width();

	// Don't resize to sz.width() every time when it only reduces a little bit
	if ( ( currentWidth < sz.width() ) || ( currentWidth > ( sz.width() + 100 ) ) )
		currentWidth = sz.width();
	parentWidget()->resize ( currentWidth, sz.height() );

	QScrollArea::resizeEvent ( event );
}


QSize TransactionItemView::sizeHint() const
{
	return minimumSizeHint();
}

QSize TransactionItemView::minimumSizeHint() const
{
	int f = 2 * frameWidth();
	// Make room for a vertical scrollbar in all cases, to avoid a horizontal one
	int vsbExt = verticalScrollBar()->sizeHint().width();
	int minw = topLevelWidget()->width() / 3;
	int maxh = topLevelWidget()->height() / 2;
	QSize sz ( m_bigbox->sizeHint() ); //QSize sz( m_bigbox->minimumSizeHint() );
	sz.setWidth ( qMax ( sz.width(), minw ) + f + vsbExt );
	sz.setHeight ( qMin ( sz.height(), maxh ) + f );
	return sz;
}


void TransactionItemView::slotLayoutFirstItem()
{
	//This slot is called whenever a TransactionItem is deleted, so this is a
	//good place to call updateGeometry(), so our parent takes the new size
	//into account and resizes.
	updateGeometry();

	/*
	   The below relies on some details in Qt's behaviour regarding deleting
	   objects. This slot is called from the destroyed signal of an item just
	   going away. That item is at that point still in the  list of chilren, but
	   since the vtable is already gone, it will have type QObject. The first
	   one with both the right name and the right class therefor is what will
	   be the first item very shortly. That's the one we want to remove the
	   hline for.
	*/
	TransactionItem *ti = m_bigbox->findChild<TransactionItem *> ( "TransactionItem" );
	if ( ti )
	{
		ti->hideHLine();
	}
}


// TransactionItem ----------------------------------------------------------------------------

TransactionItem::TransactionItem ( QWidget* parent, ProgressItem *item, bool first )
		: QFrame ( parent ), m_cancelbutton ( 0 ), m_item ( item )
{
	setObjectName ( "TransactionItem" );

	m_vlayout = new QVBoxLayout ( this );
	m_vlayout->setSpacing ( 2 );
	m_vlayout->setMargin ( 2 );
	setLayout ( m_vlayout );

	setSizePolicy ( QSizePolicy ( QSizePolicy::Preferred, QSizePolicy::Fixed ) );

	m_frame = new QFrame ( this );
	m_frame->setFrameShape ( QFrame::HLine );
	m_frame->setFrameShadow ( QFrame::Raised );
	m_frame->show();
	m_vlayout->setStretchFactor ( m_frame, 3 );
	m_vlayout->addWidget ( m_frame );

	QFrame * hframe = new QFrame ( this );
	QHBoxLayout * h = new QHBoxLayout ( hframe );
	hframe->setLayout ( h );
	h->setSpacing ( 5 );
	m_vlayout->addWidget ( hframe );

	m_itemlabel = new QLabel ( item->label(), hframe );
	h->addWidget ( m_itemlabel );
	hframe->setSizePolicy ( QSizePolicy ( QSizePolicy::Preferred, QSizePolicy::Fixed ) );

	m_progress = new QProgressBar ( hframe );
	m_progress->setRange ( 0, 100 );
	m_progress->setValue ( item->progress() );
	h->addWidget ( m_progress );

	if ( item->canBeCanceled() )
	{
		m_cancelbutton = new QPushButton ( QIcon ( *NMGImageManager::loadPixmap ( "no.png" ) ), "(Cancel)", hframe );
		m_cancelbutton->setToolTip ( "Cancel current test." );
		connect ( m_cancelbutton, SIGNAL ( clicked() ), this, SLOT ( slotItemCanceled() ) );
		h->addWidget ( m_cancelbutton );
	}

	hframe = new QFrame ( this );
	h = new QHBoxLayout ( hframe );
	hframe->setLayout ( h );
	h->setSpacing ( 5 );
	hframe->setSizePolicy ( QSizePolicy ( QSizePolicy::Preferred, QSizePolicy::Fixed ) );
	m_vlayout->addWidget ( hframe );

	m_itemstatus = new QLabel ( item->status(), hframe );
	h->addWidget ( m_itemstatus );

	if ( first ) hideHLine();
}

TransactionItem::~TransactionItem()
{}

void TransactionItem::hideHLine()
{
	m_frame->hide();
}

void TransactionItem::setProgress ( int progress )
{
	m_progress->setValue ( progress );
}

void TransactionItem::setLabel ( const QString& label )
{
	m_itemlabel->setText ( label );
}

void TransactionItem::setStatus ( const QString& status )
{
	m_itemstatus->setText ( status );
}

void TransactionItem::slotItemCanceled()
{
	if ( m_item )
		m_item->cancel();
}

void TransactionItem::addSubTransaction ( ProgressItem* /*item*/ )
{}


// ProgressDialog ---------------------------------------------------------------------------

ProgressDialog::ProgressDialog ( QWidget* alignWidget, QWidget* parent, const char* name )
		: OverlayWidget ( alignWidget, parent, name ), m_waslastshown ( false )
{
	setFrameStyle ( QFrame::Panel | QFrame::Sunken ); // QFrame
	setFrameShape( QFrame::StyledPanel );
	setAutoFillBackground(true);

	m_scrollview = new TransactionItemView ( this, "ProgressScrollView" );
	layout()->addWidget ( m_scrollview );
	/*
	 * Get the singleton ProgressManager item which will inform us of
	 * appearing and vanishing items.
	 */
	ProgressManager *pm = ProgressManager::self();
	connect ( pm, SIGNAL ( progressItemAdded ( ProgressItem* ) ),
	          this, SLOT ( slotTransactionAdded ( ProgressItem* ) ) );
	connect ( pm, SIGNAL ( progressItemCompleted ( ProgressItem* ) ),
	          this, SLOT ( slotTransactionCompleted ( ProgressItem* ) ) );
	connect ( pm, SIGNAL ( progressItemProgress ( ProgressItem*, unsigned int ) ),
	          this, SLOT ( slotTransactionProgress ( ProgressItem*, unsigned int ) ) );
	connect ( pm, SIGNAL ( progressItemStatus ( ProgressItem*, const QString& ) ),
	          this, SLOT ( slotTransactionStatus ( ProgressItem*, const QString& ) ) );
	connect ( pm, SIGNAL ( progressItemLabel ( ProgressItem*, const QString& ) ),
	          this, SLOT ( slotTransactionLabel ( ProgressItem*, const QString& ) ) );
	connect ( pm, SIGNAL ( showProgressDialog() ),
	          this, SLOT ( slotShow() ) );
}

void ProgressDialog::closeEvent ( QCloseEvent* e )
{
	e->accept();
	hide();
}

/**
 *  Destructor
 **/
ProgressDialog::~ProgressDialog()
{
	// no need to delete child widgets.
}

void ProgressDialog::slotTransactionAdded ( ProgressItem *item )
{
	/*TransactionItem *parent = 0;
	if ( item->parent() )
	{
		if ( m_transactionstolistviewitems.contains( item->parent() ) )
		{
			parent = m_transactionstolistviewitems[ item->parent()];
			parent->addSubTransaction( item );
		}
	}
	else
	{
		const bool first = m_transactionstolistviewitems.empty();
		TransactionItem *ti = m_scrollview->addTransactionItem( item, first );
		if ( ti )
			m_transactionstolistviewitems.insert( item, ti );
		if ( first && m_waslastshown )
			QTimer::singleShot( 1000, this, SLOT( slotShow() ) );

	}*/

	//TODO (nmg's alternative)
	TransactionItem *ti = m_scrollview->addTransactionItem ( item, m_transactionstolistviewitems.empty() );
	if ( ti )
	{
		m_transactionstolistviewitems.remove ( item );
		m_transactionstolistviewitems.insert ( item, ti );
	}
}

void ProgressDialog::slotTransactionCompleted ( ProgressItem *item )
{
	if ( m_transactionstolistviewitems.contains ( item ) )
	{
		TransactionItem *ti = m_transactionstolistviewitems[ item ];
		m_transactionstolistviewitems.remove ( item );
		ti->setItemComplete();
		QTimer::singleShot ( 3000, ti, SLOT ( deleteLater() ) );
		// see the slot for comments as to why that works
		connect ( ti, SIGNAL ( destroyed() ),
		          m_scrollview, SLOT ( slotLayoutFirstItem() ) );
	}
	// This was the last item, hide.
	if ( m_transactionstolistviewitems.empty() )
		QTimer::singleShot ( 3000, this, SLOT ( slotHide() ) );
}

void ProgressDialog::slotTransactionCanceled ( ProgressItem* )
{}

void ProgressDialog::slotTransactionProgress ( ProgressItem *item,
        unsigned int progress )
{
	if ( m_transactionstolistviewitems.contains ( item ) )
	{
		TransactionItem *ti = m_transactionstolistviewitems[ item ];
		ti->setProgress ( progress );
	}
}

void ProgressDialog::slotTransactionStatus ( ProgressItem *item,
        const QString& status )
{
	if ( m_transactionstolistviewitems.contains ( item ) )
	{
		TransactionItem *ti = m_transactionstolistviewitems[ item ];
		ti->setStatus ( status );
	}
}

void ProgressDialog::slotTransactionLabel ( ProgressItem *item,
        const QString& label )
{
	if ( m_transactionstolistviewitems.contains ( item ) )
	{
		TransactionItem *ti = m_transactionstolistviewitems[ item ];
		ti->setLabel ( label );
	}
}

void ProgressDialog::slotShow()
{
	setVisible ( true );
}

void ProgressDialog::slotHide()
{
	// check if a new item showed up since we started the timer. If not, hide
	if ( m_transactionstolistviewitems.isEmpty() )
		setVisible ( false );
}

void ProgressDialog::slotClose()
{
	m_waslastshown = false;
	setVisible ( false );
}

void ProgressDialog::setVisible ( bool b )
{
	OverlayWidget::setVisible ( b );
	emit visibilityChanged ( b );
}

void ProgressDialog::slotToggleVisibility()
{
	/* Since we are only hiding with a timeout, there is a short period of
	 * time where the last item is still visible, but clicking on it in
	 * the statusbarwidget should not display the dialog, because there
	 * are no items to be shown anymore. Guard against that.
	 */
	m_waslastshown = isHidden();
	if ( !isHidden() || !m_transactionstolistviewitems.isEmpty() )
		setVisible ( isHidden() );
}

#include "progressdialog.moc"
