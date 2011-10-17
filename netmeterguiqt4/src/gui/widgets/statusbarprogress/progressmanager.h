/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Pau Capella <pcapella@ac.upc.edu>, based in     *
 *   David Faure <faure@kde.org> akregator software                        *
 *                                                                         *
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
#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>
#include <QMultiHash>
#include <QString>

class ProgressItem;
class ProgressManager;

class ProgressItem : public QObject
{
		Q_OBJECT

		friend class ProgressManager;
		friend class QMultiHash< QString, ProgressItem >; // so it can be deleted from dicts

	public:

		const QString& id() const { return m_id; }

		const QString& label() const { return m_label; }
		void setLabel( const QString& v );

		const QString& status() const { return m_status; }
		void setStatus( const QString& v );

		bool canBeCanceled() const { return m_canbecanceled; }

		unsigned int progress() const { return m_progress; }
		void setProgress( unsigned int v );

		void setComplete();

		void reset() { setProgress( 0 ); setStatus( QString::null ); m_completed = 0; }
		void cancel();

		void setTotalItems( unsigned int v ) { m_total = v; }
		unsigned int totalItems() const { return m_total; }
		void setCompletedItems( unsigned int v ) { m_completed = v; }
		void incCompletedItems( unsigned int v = 1 ) { m_completed += v; }
		unsigned int completedItems() const { return m_completed; }

		void updateProgress() { setProgress( m_total ? m_completed*100 / m_total : 0 ); };

		bool canceled() const { return m_canceled; }

	signals:
		/**
		 * Emitted when a new ProgressItem is added.
		 * @param The ProgressItem that was added.
		 */
		void progressItemAdded( ProgressItem* );
		/**
		 * Emitted when the progress value of an item changes.
		 * @param  The item which got a new value.
		 * @param  The value, for convenience.
		 */
		void progressItemProgress( ProgressItem*, unsigned int );
		/**
		 * Emitted when a progress item was completed. The item will be
		 * deleted afterwards, so slots connected to this are the last
		 * chance to work with this item.
		 * @param The completed item.
		 */
		void progressItemCompleted( ProgressItem* );
		/**
		 * Emitted when an item was cancelled. It will _not_ go away immediately,
		 * only when the owner sets it complete, which will usually happen. Can be
		 * used to visually indicate the cancelled status of an item. Should be used
		 * by the owner of the item to make sure it is set completed even if it is
		 * cancelled. There is a ProgressManager::slotStandardCancelHandler which
		 * simply sets the item completed and can be used if no other work needs to
		 * be done on cancel.
		 * @param The canceled item;
		 */
		void progressItemCanceled( ProgressItem* );
		/**
		 * Emitted when the status message of an item changed. Should be used by
		 * progress dialogs to update the status message for an item.
		 * @param  The updated item.
		 * @param  The new message.
		 */
		void progressItemStatus( ProgressItem*, const QString& );
		/**
		 * Emitted when the label of an item changed. Should be used by
		 * progress dialogs to update the label of an item.
		 * @param  The updated item.
		 * @param  The new label.
		 */
		void progressItemLabel( ProgressItem*, const QString& );


	protected:
		/* Only to be used by our good friend the ProgressManager */
		ProgressItem( const QString& id, const QString& label, const QString& status, bool isCancellable );
		virtual ~ProgressItem();


	private:

		QString m_id;
		QString m_label;
		QString m_status;
		bool m_canbecanceled;
		unsigned int m_progress;
		unsigned int m_total;
		unsigned int m_completed;
		bool m_canceled;
};

class ProgressManager : public QObject
{
	Q_OBJECT

	public:

		~ProgressManager();
		static ProgressManager * self();

		/**
		 * Creates a ProgressItem with a unique id and the given label.
		 * This is the simplest way to aquire a progress item. It will not
		 * have a parent and will be set to be cancellable and not using crypto.
		 */
		static ProgressItem * createProgressItem( const QString &label )
		{
			return self()->createProgressItemImpl( uniqueId(), label, QString::null, true );
		}

		/**
		 * Creates a new progressItem with the given parent, id, label and initial
		 * status.
		 *
		 * @param parent Specify an already existing item as the parent of this one.
		 * @param id Used to identify this operation for cancel and progress info.
		 * @param label The text to be displayed by progress handlers
		 * @param status Additional text to be displayed for the item.
		 * @param canBeCanceled can the user cancel this operation?
		 * Cancelling the parent will cancel the children as well (if they can be
		 * cancelled) and ongoing children prevent parents from finishing.
		 * @return The ProgressItem representing the operation.
		 */
		/*     static ProgressItem * createProgressItem( ProgressItem* parent,
		                                               const QString& id,
		                                               const QString& label,
		                                               const QString& status = QString::null,
		                                               bool canBeCanceled = true ) {
		       return instance()->createProgressItemImpl( parent, id, label, status,
		                                                  canBeCanceled, usesCrypto );
		     }
		*/
		/**
		 * Use this version if you have the id string of the parent and want to
		 * add a subjob to it.
		 */
//      static ProgressItem * createProgressItem( const QString& parent,
//                                                const QString& id,
//                                                const QString& label,
//                                                const QString& status = QString::null,
//                                                bool canBeCanceled = true,
//                                                bool usesCrypto = false ) {
//        return instance()->createProgressItemImpl( parent, id, label,
//                                                  status, canBeCanceled, usesCrypto );
//      }

		/**
		 * Version without a parent.
		 */
//      static ProgressItem * createProgressItem( const QString& id,
//                                                const QString& label,
//                                                const QString& status = QString::null,
//                                                bool canBeCanceled = true ) {
//        return instance()->createProgressItemImpl( 0, id, label, status,
//                                                   canBeCanceled, usesCrypto );
//      }


		/**
		 * @return true when there is no more progress item
		 */
		bool isEmpty() const { return m_transactions.isEmpty(); }

		/**
		 * @return the only top level progressitem when there's only one.
		 * Returns 0 if there is no item, or more than one top level item.
		 */
		ProgressItem* singleItem() const;

		/**
		 * Ask all listeners to show the progress dialog, because there is
		 * something that wants to be shown.
		 */
		static void emitShowProgressDialog()
		{
			self()->emitShowProgressDialogImpl();
		}

	signals:
		/** @see ProgressItem::progressItemAdded() */
		void progressItemAdded( ProgressItem* );
		/** @see ProgressItem::progressItemProgress() */
		void progressItemProgress( ProgressItem*, unsigned int );
		/** @see ProgressItem::progressItemCompleted() */
		void progressItemCompleted( ProgressItem* );
		/** @see ProgressItem::progressItemCanceled() */
		void progressItemCanceled( ProgressItem* );
		/** @see ProgressItem::progressItemStatus() */
		void progressItemStatus( ProgressItem*, const QString& );
		/** @see ProgressItem::progressItemLabel() */
		void progressItemLabel( ProgressItem*, const QString& );

		/**
		 * Emitted when an operation requests the listeners to be shown.
		 * Use emitShowProgressDialog() to trigger it.
		 */
		void showProgressDialog();
	public slots:

		/**
		 * Calls setCompleted() on the item, to make sure it goes away.
		 * Provided for convenience.
		 * @param item the canceled item.
		 */
		void slotStandardCancelHandler( ProgressItem* item );

		/**
		 * Aborts all running jobs. Bound to "Esc"
		 */
		void slotAbortAll();

	private slots:
		void slotTransactionCompleted( ProgressItem *item );

	private:
		ProgressManager();
		static QString uniqueId() { return QString::number( ++m_id ); }
		// prevent unsolicited copies

		virtual ProgressItem* createProgressItemImpl( const QString& id, const QString& label, const QString& status, bool cancellable );

		void emitShowProgressDialogImpl();

		QMultiHash< QString, ProgressItem *> m_transactions;
		static ProgressManager *m_instance;
		static unsigned int m_id;
};

#endif
