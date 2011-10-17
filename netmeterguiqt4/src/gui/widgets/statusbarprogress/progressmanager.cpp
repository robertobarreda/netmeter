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
#include "progressmanager.h"

ProgressManager * ProgressManager::m_instance = 0;
unsigned int ProgressManager::m_id = 1;


ProgressItem::ProgressItem( const QString& id, const QString& label, const QString& status, bool canbecanceled )
		: m_id( id ), m_label( label ), m_status( status ),
		m_canbecanceled( canbecanceled ), m_progress( 0 ), m_total( 0 ),
		m_completed( 0 ), m_canceled( false )
{}

ProgressItem::~ProgressItem()
{}

void ProgressItem::setComplete()
{
	if ( !m_canceled )
		setProgress( 100 );
	emit progressItemCompleted( this );
	deleteLater();
}

void ProgressItem::cancel()
{
	if ( m_canceled || !m_canbecanceled ) return;

	m_canceled = true;
	emit progressItemCanceled( this );
}

void ProgressItem::setProgress( unsigned int v )
{
	m_progress = v;
	emit progressItemProgress( this, m_progress );
}

void ProgressItem::setLabel( const QString& v )
{
	m_label = v;
	emit progressItemLabel( this, m_label );
}

void ProgressItem::setStatus( const QString& v )
{
	m_status = v;
	emit progressItemStatus( this, m_status );
}

//***************ProgressManager *********************

ProgressManager::ProgressManager() : QObject()
{
	m_instance = this;
}

ProgressManager::~ProgressManager() { m_instance = 0; }


ProgressManager* ProgressManager::self()
{
	if ( !m_instance )
	{
		m_instance = new ProgressManager();
	}
	return m_instance;
}

ProgressItem* ProgressManager::createProgressItemImpl( const QString& id, const QString &label, const QString &status, bool cancellable )
{
	ProgressItem *t = 0;
	if ( !(m_transactions.value( id ) ) )
	{
		t = new ProgressItem( id, label, status, cancellable );
		m_transactions.insert( id, t );

		// connect all signals
		connect( t, SIGNAL( progressItemCompleted( ProgressItem* ) ),
		         this, SLOT( slotTransactionCompleted( ProgressItem* ) ) );
		connect( t, SIGNAL( progressItemProgress( ProgressItem*, unsigned int ) ),
		         this, SIGNAL( progressItemProgress( ProgressItem*, unsigned int ) ) );
		connect( t, SIGNAL( progressItemAdded( ProgressItem* ) ),
		         this, SIGNAL( progressItemAdded( ProgressItem* ) ) );
		connect( t, SIGNAL( progressItemCanceled( ProgressItem* ) ),
		         this, SIGNAL( progressItemCanceled( ProgressItem* ) ) );
		connect( t, SIGNAL( progressItemStatus( ProgressItem*, const QString& ) ),
		         this, SIGNAL( progressItemStatus( ProgressItem*, const QString& ) ) );
		connect( t, SIGNAL( progressItemLabel( ProgressItem*, const QString& ) ),
		         this, SIGNAL( progressItemLabel( ProgressItem*, const QString& ) ) );

		emit progressItemAdded( t );
	}
	else
	{
		// Hm, is this what makes the most sense?
		t = m_transactions.value( id );
	}
	return t;
}

void ProgressManager::emitShowProgressDialogImpl()
{
	emit showProgressDialog();
}


void ProgressManager::slotTransactionCompleted( ProgressItem *item )
{
	m_transactions.remove( item->id() );
	emit progressItemCompleted( item );
}

void ProgressManager::slotStandardCancelHandler( ProgressItem *item )
{
	item->setComplete();
}

ProgressItem* ProgressManager::singleItem() const
{
	ProgressItem *item = 0;

	QHashIterator<QString, ProgressItem* > it( m_transactions );
	while ( it.hasNext() )
	{
		it.next();
		if ( !( it.value()->parent() ) )
		{
			if ( item )
				return 0; // we found more than one
			else
				item = it.value();
		}
	}
	return item;
}

void ProgressManager::slotAbortAll()
{
	QHashIterator<QString, ProgressItem *> it( m_transactions );
	while ( it.hasNext() )
	{
		it.next();
		(*it.value()).cancel();
	}
}

#include "progressmanager.moc"
