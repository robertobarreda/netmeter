/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#include "nmggrouptreewidget.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QKeyEvent>
#include <QStringList>
#include <QMessageBox>
#include <QList>

NMGGroupTreeWidget::NMGGroupTreeWidget ( QWidget *parent )
		: QTreeWidget ( parent )
{
	setContextMenuPolicy ( Qt::CustomContextMenu );

	QHeaderView * pheader = header();
	pheader->setDefaultAlignment ( Qt::AlignVCenter | Qt::AlignHCenter );
	pheader->setResizeMode ( QHeaderView::ResizeToContents );
	pheader->setMovable ( true );
	pheader->setStretchLastSection ( false );
	pheader->setContextMenuPolicy ( Qt::CustomContextMenu );

	setAutoExpandDelay ( 750 );
	//setDragEnabled( true );
	setAcceptDrops ( true );
	setDropIndicatorShown ( true );
	setAutoScroll ( true );
	setDragDropMode ( QAbstractItemView::InternalMove );

	lastTestItemAdded = 0;
}

NMGGroupTreeWidget::~NMGGroupTreeWidget()
{
	if ( lastTestItemAdded ) delete lastTestItemAdded;
	delete contextualHeaderMenu;
}

void NMGGroupTreeWidget::setHeaderLabel ( const QString & label )
{
	setHeaderLabels ( QStringList ( label ) );
}

void NMGGroupTreeWidget::setHeaderLabels ( const QStringList & labels )
{
	headerLabels = labels;
	headerLabels.takeFirst();
	createContextualHeaderMenu();
	QTreeWidget::setHeaderLabels ( labels );
}

void NMGGroupTreeWidget::createContextualHeaderMenu()
{
	contextualHeaderMenu = new QMenu();
	foreach ( QString label, headerLabels )
	{
		QAction * act = contextualHeaderMenu->addAction ( label );
		act->setCheckable ( true );
		act->setChecked ( true );
	}
	connect ( contextualHeaderMenu, SIGNAL ( triggered ( QAction * ) ),
	          this, SLOT ( hideTreeColumn ( QAction * ) ) );
	connect ( header(), SIGNAL ( customContextMenuRequested ( const QPoint & ) ),
	          this, SLOT ( showContextualHeaderMenu ( const QPoint & ) ) );
}

void NMGGroupTreeWidget::showContextualHeaderMenu ( const QPoint & pos )
{
	contextualHeaderMenu->popup ( header()->mapToGlobal ( pos ) );
}

void NMGGroupTreeWidget::hideTreeColumn ( QAction * action )
{
	int column = headerLabels.indexOf ( action->text() ) + 1;
	if ( !action->isChecked() )
		QTreeWidget::hideColumn ( column );
	else
		QTreeWidget::showColumn ( column );
}

void NMGGroupTreeWidget::hideColumn ( int column )
{
	if ( column > 0 )
	{
		contextualHeaderMenu->actions().at ( column - 1 )->setChecked ( false );
		QTreeWidget::hideColumn ( column );
	}
}

void NMGGroupTreeWidget::showColumn ( int column )
{
	if ( column > 0 )
	{
		contextualHeaderMenu->actions().at ( column - 1 )->setChecked ( true );
		QTreeWidget::showColumn ( column );
	}
}

void NMGGroupTreeWidget::removeSelectedItems()
{
	QString id;
	QTreeWidgetItemList selectList = selectedItems ();

	// Remove the items
	for ( QTreeWidgetItemListIterator it = selectList.begin(); it != selectList.end(); ++it )
	{
		if ( *it )  id = ( ( NMGGroupTreeWidgetItem * ) *it )->getId();
		if ( *it == lastTestItemAdded ) lastTestItemAdded = 0;
		
		if ( !( *it ) || !( *it )->parent() )
			takeTopLevelItem ( indexOfTopLevelItem ( *it ) );
		else
		{
			QTreeWidgetItem * parent = ( *it )->parent();
			delete parent->takeChild ( parent->indexOfChild ( *it ) );
			
			if ( parent->childCount() == 0 and parent->type() == NMGGroupTreeWidgetItem::Test )
			{
				id = ( ( NMGGroupTreeWidgetItem * ) parent )->getId();
				if ( parent == lastTestItemAdded ) lastTestItemAdded = 0;

				if ( !parent->parent() )
					takeTopLevelItem ( indexOfTopLevelItem ( parent ) );
				else
					delete parent->parent()->takeChild ( parent->parent()->indexOfChild ( parent ) );
			}
		}
		emit itemRemoved (id);
	}
}

/** Item adders **/
void NMGGroupTreeWidget::addGroup ( const QString & id )
{
	new  NMGGroupTreeWidgetItem ( this, id, QStringList ( id ), NMGGroupTreeWidgetItem::Group );
}

void NMGGroupTreeWidget::addTest ( const QString & id, const QString & group )
{
	//TODO group treatment pendent
	lastTestItemAdded = new  NMGGroupTreeWidgetItem ( this, id, NMGGroupTreeWidgetItem::Test );
	setCurrentItem ( lastTestItemAdded );
}

void NMGGroupTreeWidget::addTest ( const QString & id, const QStringList & values, const QString & group )
{
	//TODO group treatment pendent
	lastTestItemAdded = new  NMGGroupTreeWidgetItem ( this, id, values, NMGGroupTreeWidgetItem::Test );
	setCurrentItem ( lastTestItemAdded );
}

void NMGGroupTreeWidget::addFlow ( const QString & id, const QStringList & values )
{
	new  NMGGroupTreeWidgetItem ( lastTestItemAdded, lastTestItemAdded->getId()
	                              + SEPARATOR + id, values, NMGGroupTreeWidgetItem::Flow );
}

/** Drag & Drop stuff **/
void NMGGroupTreeWidget::dragMoveEvent ( QDragMoveEvent *event )
{
	NMGGroupTreeWidgetItem * currentItemSelected = ( NMGGroupTreeWidgetItem * ) currentItem();
	NMGGroupTreeWidgetItem * currentItemOver = ( NMGGroupTreeWidgetItem * ) ( QTreeWidget::itemAt ( event->pos() ) );

	if ( currentItemOver )
	{
		QTreeWidget::dragMoveEvent ( event );

		switch ( currentItemOver->type() )
		{
			case NMGGroupTreeWidgetItem::Group:
				switch ( currentItemSelected->type() )
				{
					case NMGGroupTreeWidgetItem::Group:
						if ( ( ( dropIndicatorPosition() == QAbstractItemView::AboveItem )
						        or ( dropIndicatorPosition() == QAbstractItemView::BelowItem ) )
							and indexOfTopLevelItem( currentItemOver ) != -1 )
							event->setDropAction ( Qt::MoveAction );
						else
							event->ignore();
						break;
					case NMGGroupTreeWidgetItem::Flow:
						if ( dropIndicatorPosition() == QAbstractItemView::OnItem )
							event->setDropAction ( Qt::MoveAction );
						else
							event->ignore();
						break;
					default:
						event->setDropAction ( Qt::MoveAction );
						break;
				}
				break;

			case NMGGroupTreeWidgetItem::Test:
				switch ( currentItemSelected->type() )
				{
					case NMGGroupTreeWidgetItem::Group:
						if ( ( ( dropIndicatorPosition() == QAbstractItemView::AboveItem )
						        or ( dropIndicatorPosition() == QAbstractItemView::BelowItem ) )
							and indexOfTopLevelItem( currentItemOver ) != -1 )
							event->setDropAction ( Qt::MoveAction );
						else
							event->ignore();
						break;
					case NMGGroupTreeWidgetItem::Test:
						if ( ( dropIndicatorPosition() == QAbstractItemView::AboveItem )
						        or ( dropIndicatorPosition() == QAbstractItemView::BelowItem ) )
							event->setDropAction ( Qt::MoveAction );
						else
							event->ignore();
						break;
					case NMGGroupTreeWidgetItem::Flow:
						if ( currentItemSelected->getId().contains ( currentItemOver->getId() )
						        and ( dropIndicatorPosition() == QAbstractItemView::OnItem ) )
							event->setDropAction ( Qt::MoveAction );
						else
							event->ignore();
						break;
					default:
						event->ignore();
						break;
				}
				break;

			case NMGGroupTreeWidgetItem::Flow:
				if ( ( currentItemSelected->type() == NMGGroupTreeWidgetItem::Flow )
				        and ( currentItemSelected->getId().left ( currentItemSelected->getId().lastIndexOf ( SEPARATOR ) )
				              == currentItemOver->getId().left ( currentItemOver->getId().lastIndexOf ( SEPARATOR ) ) )
				        and ( ( dropIndicatorPosition() == QAbstractItemView::AboveItem )
				              or ( dropIndicatorPosition() == QAbstractItemView::BelowItem ) ) )
					event->setDropAction ( Qt::MoveAction );
				else
					event->ignore();
				break;
			default:
				event->ignore();
				break;
		}
	}
	else
	{
		switch ( currentItemSelected->type() )
		{
			case NMGGroupTreeWidgetItem::Flow:
				event->ignore();
				break;
			default:
				QTreeWidget::dragMoveEvent ( event );
				break;
		}
	}
}

void NMGGroupTreeWidget::dropEvent ( QDropEvent *event )
{
	NMGGroupTreeWidgetItem * currentItm = ( NMGGroupTreeWidgetItem * ) currentItem(); // Current Item

	QModelIndex topIndex;
	int row = -1; //Row inside the destination
	dropOn ( event, &row, &topIndex );
	NMGGroupTreeWidgetItem * destItem = ( NMGGroupTreeWidgetItem * ) itemFromIndex ( topIndex ); // Destination Item

	if ( destItem and ( destItem->type() == NMGGroupTreeWidgetItem::Group )
	        and ( currentItm->type() == NMGGroupTreeWidgetItem::Flow ) )
	{
		NMGGroupTreeWidgetItem * testItem = 0;

		// Check if there's already a Test with the same ID
		for ( int i = 0; i < destItem->childCount(); ++i )
		{
			if ( currentItm->getId().contains ( ( ( NMGGroupTreeWidgetItem * ) destItem->child ( i ) )->getId() ) )
				testItem = ( NMGGroupTreeWidgetItem * ) destItem->child ( i );
		}

		QTreeWidgetItem * parentOfCurrentItem = currentItm->parent();
		int currentRow = indexFromItem ( currentItm ).row();

		if ( !testItem )
		{
			QStringList values;
			for ( int i = 0; i < parentOfCurrentItem->columnCount(); ++i )
				values.append ( parentOfCurrentItem->text ( i ) );

			testItem = new  NMGGroupTreeWidgetItem ( destItem,
			        currentItm->getId().left ( currentItm->getId().lastIndexOf ( SEPARATOR ) ),
			        values, NMGGroupTreeWidgetItem::Test );
		}

		testItem->addChild ( parentOfCurrentItem->takeChild ( currentRow ) );

		// Check if you left an empty Test
		if ( parentOfCurrentItem->childCount() == 0 and parentOfCurrentItem->type() == NMGGroupTreeWidgetItem::Test )
		{
			if ( !parentOfCurrentItem->parent() )
				takeTopLevelItem ( indexFromItem ( parentOfCurrentItem ).row() );
			else
				delete parentOfCurrentItem->parent()->takeChild ( indexFromItem ( parentOfCurrentItem ).row() );
		}

		event->ignore();
	}
	else
	{
		QTreeWidgetItem * parentOfCurrentItem = currentItm->parent();
		QTreeWidget::dropEvent ( event );
		// Check if you left an empty Test
		if ( parentOfCurrentItem and parentOfCurrentItem->childCount() == 0
		        and ( parentOfCurrentItem->type() == NMGGroupTreeWidgetItem::Test )
		        and ( currentItm->type() == NMGGroupTreeWidgetItem::Flow ) )
		{
			if ( !parentOfCurrentItem->parent() )
				takeTopLevelItem ( indexFromItem ( parentOfCurrentItem ).row() );
			else
				delete parentOfCurrentItem->parent()->takeChild ( indexFromItem ( parentOfCurrentItem ).row() );
		}
	}
	emit itemMoved ( currentItm->getId() );
}

void NMGGroupTreeWidget::dropOn ( QDropEvent *event, int *dropRow, QModelIndex *dropIndex )
{
	if ( event->isAccepted() )
		return;

	QModelIndex index;
	// rootIndex() (i.e. the viewport) might be a valid index
	if ( viewport()->rect().contains ( event->pos() ) )
	{
		index = indexAt ( event->pos() );
		if ( !index.isValid() || !visualRect ( index ).contains ( event->pos() ) )
			index = rootIndex();
	}

	// If we are allowed to do the drop
	if ( model()->supportedDropActions() & event->proposedAction() )
	{
		if ( index != rootIndex() )
		{
			switch ( dropIndicatorPosition() )
			{
				case QAbstractItemView::AboveItem:
					*dropRow = index.row();
					index = index.parent();
					break;
				case QAbstractItemView::BelowItem:
					*dropRow = index.row() + 1;
					index = index.parent();
					break;
				case QAbstractItemView::OnItem:
				case QAbstractItemView::OnViewport:
					*dropRow = -1;
					break;
			}
		}
		*dropIndex = index;
	}
}

NMGGroupTreeWidgetItem * NMGGroupTreeWidget::findItemById ( const QString & id, NMGGroupTreeWidgetItem * startItem )
{
	NMGGroupTreeWidgetItem * root;
	if ( startItem )
		if ( startItem->getId() == id ) return startItem;
		else root = startItem;
	else
		root = ( NMGGroupTreeWidgetItem * ) invisibleRootItem ();

	for ( int i = 0; i < root->childCount(); ++i )
	{
		NMGGroupTreeWidgetItem *item = ( NMGGroupTreeWidgetItem * ) root->child ( i );
		if ( item->getId() == id ) return item;
		else if ( item->childCount() > 0 )
		{
			item = findItemById ( id, item );
			if ( item ) return item;
			else continue;
		}
	}

	return 0;
}

void NMGGroupTreeWidget::keyReleaseEvent ( QKeyEvent *event )
{
	switch ( event->key() )
	{
		case Qt::Key_Delete:
			removeSelectedItems();
			break;
		default:
			QTreeWidget::keyReleaseEvent ( event );
			break;
	}
}

#include "nmggrouptreewidget.moc"
