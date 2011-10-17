/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Pau Capella <pcapella@ac.upc.edu>,              *
 *   based in David Faure <faure@kde.org> akregator software               *
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
#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include "overlaywidget.h"
#include "progressmanager.h"

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QScrollArea>

class TransactionItem;
class ProgressItem;

class TransactionItemView : public QScrollArea
{
		Q_OBJECT
	public:
		TransactionItemView( QWidget * parent = 0, const char * name = 0 );
		virtual ~TransactionItemView(){}
		TransactionItem* addTransactionItem( ProgressItem *item, bool first );
		QSize sizeHint() const;
		QSize minimumSizeHint() const;

	public slots:
		void slotLayoutFirstItem();

	protected:
		virtual void resizeEvent( QResizeEvent *event );

	private:
		QVBoxLayout * m_bigbox;
};


class TransactionItem : public QFrame
{
		Q_OBJECT

	public:
		TransactionItem( QWidget * parent, ProgressItem* item, bool first );
		~TransactionItem();
		void hideHLine();
		void setProgress( int progress );
		void setLabel( const QString& );
		void setStatus( const QString& );
		void setCrypto( bool );
		ProgressItem* item() const { return m_item; }
		void addSubTransaction( ProgressItem *item );

		// The progressitem is deleted immediately, we take 5s to go out,
		// so better not use mItem during this time.
		void setItemComplete() { m_item = 0; }

	public slots:
		void slotItemCanceled();

	protected:
		QProgressBar * m_progress;
		QPushButton *  m_cancelbutton;
		QLabel *       m_itemlabel;
		QLabel *       m_itemstatus;
		QFrame *       m_frame;
		QVBoxLayout *  m_vlayout;
		ProgressItem * m_item;
};


class ProgressDialog : public OverlayWidget
{
		Q_OBJECT

	public:
		ProgressDialog( QWidget* alignWidget, QWidget* parent, const char* name = 0 );
		~ProgressDialog();
		void setVisible( bool b );

	public slots:
		void slotToggleVisibility();

	protected slots:
		void slotTransactionAdded( ProgressItem *item );
		void slotTransactionCompleted( ProgressItem *item );
		void slotTransactionCanceled( ProgressItem *item );
		void slotTransactionProgress( ProgressItem *item, unsigned int progress );
		void slotTransactionStatus( ProgressItem *item, const QString& );
		void slotTransactionLabel( ProgressItem *item, const QString& );

		void slotClose();
		void slotShow();
		void slotHide();

	signals:
		void visibilityChanged( bool );

	protected:
		virtual void closeEvent( QCloseEvent* );

		TransactionItemView* m_scrollview;
		TransactionItem* m_previousitem;
		QMap< const ProgressItem*, TransactionItem* > m_transactionstolistviewitems;
		bool m_waslastshown;
};

#endif
