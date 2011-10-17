/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
 *                                                                         *
 *   This file is based on the Qt Designer components.                     *
 *   See http://www.trolltech.com/ for more information                    *
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
#include "nmgobjectinspector.h"

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QApplication>
#include <QHeaderView>
#include <QPixmap>

class NMGObjectInspectorDelegate: public QItemDelegate
{
  public:
    NMGObjectInspectorDelegate(QObject* parent = 0)
      : QItemDelegate(parent) {}

    ~NMGObjectInspectorDelegate() {}
    
    void paint(QPainter *painter, 
               const QStyleOptionViewItem &option, 
               const QModelIndex &index) const
    {
      QItemDelegate::paint(painter, option, index);

      QPen savedPen = painter->pen();
      QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
      painter->setPen(QPen(color));

      painter->drawLine(option.rect.x(), option.rect.bottom(),
                        option.rect.right(), option.rect.bottom());

      int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
      painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
      painter->setPen(savedPen);
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
      return QItemDelegate::sizeHint(option, index) + QSize(4,4);
    }
};

NMGObjectInspector::NMGObjectInspector(QWidget* parent)
  : QTreeWidget(parent)
{
  // setting tree widget properties
  setEnabled(FALSE);
  setItemDelegate(new NMGObjectInspectorDelegate(this));
  setAlternatingRowColors(true);
  setColumnCount(1);
  headerItem()->setText(0, tr("Object Inspector"));
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);  
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  header()->setResizeMode(0, QHeaderView::ResizeToContents);
  setContextMenuPolicy(Qt::NoContextMenu);

  connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
          this, SLOT(slotSelectionChanged()));
  connect(this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
          this, SLOT(slotSelectionChanged()));
  
  // setting tree widget entries
  labels = new QTreeWidgetItem(this, QStringList() << tr("Labels"));
  axes = new QTreeWidgetItem(this, QStringList() << tr("Axes"));
  chartWall = new QTreeWidgetItem(this, QStringList() << tr("Grid"));
  legend = new QTreeWidgetItem(this, QStringList() << tr("Legend"));
  series = new QTreeWidgetItem(this, QStringList() << tr("Series"));
  lastSeries = 0;  
}

static int level(QAbstractItemModel* model, const QModelIndex& index)
{
  int result = 0;
  QModelIndex parent = model->parent(index);
  while (parent.isValid()) {
    parent = model->parent(parent);
    ++result;
  }
  return result;
}

void NMGObjectInspector::drawBranches(QPainter *painter, 
                                      const QRect &rect, 
                                      const QModelIndex &index) const
{
  static const int windows_deco_size = 9;
  QStyleOptionViewItem option = viewOptions();

  if (model()->hasChildren(index)) {
    option.state |= QStyle::State_Children;
    const bool reverse = QApplication::isRightToLeft();
    int indent = level(model(), index)*indentation();
    QRect primitive(reverse ? rect.left() : rect.left() + indent - 2,
                    rect.top(), indentation(), rect.height());

    primitive.moveLeft(reverse ? primitive.left()
      : primitive.left() + (primitive.width() - windows_deco_size)/2);
    primitive.moveTop(primitive.top() + (primitive.height() - windows_deco_size)/2);
    primitive.setWidth(windows_deco_size);
    primitive.setHeight(windows_deco_size);

    option.rect = primitive;

    if (isExpanded(index)) option.state |= QStyle::State_Open;

    style()->drawPrimitive(QStyle::PE_IndicatorBranch, &option, painter, this);
  }
  QPen savedPen = painter->pen();
  QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
  painter->setPen(QPen(color));
  painter->drawLine(rect.x(), rect.bottom(), rect.right(), rect.bottom());
  painter->setPen(savedPen);
}

// ********* //
// * SLOTS * //
// ********* //
void NMGObjectInspector::slotAddNewChartItem(NMGChartItem* item)
{
  int itemType = item->getTypeOfChartItem();
  QTreeWidgetItem* treeItem;
  QPixmap decoration = QPixmap(16,16);
  switch(itemType)
  {
    case NMGChartItem::LABEL:
      treeItem = new QTreeWidgetItem(labels);
      treeItem->setData(0, QTreeWidgetItem::UserType, qVariantFromValue((QObject*)item));
      switch(((NMGChartLabel*)item)->getTypeOfLabel())
      {
        case NMGChartLabel::TITLE:
          treeItem->setText(0, tr("Title"));
          break;
        case NMGChartLabel::AXIS_X:
          treeItem->setText(0, tr("X-Axis"));
          break;
        case NMGChartLabel::AXIS_Y:
          treeItem->setText(0, tr("Y-Axis"));
          break;
      }
      expandItem(labels);
      break;
      
    case NMGChartItem::WALL:
      chartWall->setData(0, QTreeWidgetItem::UserType, qVariantFromValue((QObject*)item));
      break;
      
    case NMGChartItem::SERIES:
      if(lastSeries) treeItem = new QTreeWidgetItem(series, lastSeries);
      else treeItem = new QTreeWidgetItem(series);
      
      treeItem->setData(0, QTreeWidgetItem::UserType, qVariantFromValue((QObject*)item));
      treeItem->setData(0, QTreeWidgetItem::UserType+1, ((NMGChartSeries*)item)->getId()); // used for deletion
      treeItem->setText(0, ((NMGChartSeries*)item)->getText());
      // decoration
      decoration.fill(((NMGChartSeries*)item)->getBaseColor());
      treeItem->setData(0, Qt::DecorationRole, qVariantFromValue(decoration));
    
      lastSeries = treeItem;
      expandItem(series);
      setEnabled(TRUE);
      break;
      
    case NMGChartItem::AXIS:
      treeItem = new QTreeWidgetItem(axes);
      treeItem->setData(0, QTreeWidgetItem::UserType, qVariantFromValue((QObject*)item));
      if(dynamic_cast<NMGChartXAxis*>(item)) treeItem->setText(0, tr("X-Axis"));
      else treeItem->setText(0, tr("Y-Axis"));
      expandItem(axes);
      break;
      
    case NMGChartItem::LEGEND:
      legend->setData(0, QTreeWidgetItem::UserType, qVariantFromValue((QObject*)item));
      break;
  }
}

void NMGObjectInspector::slotAddNewChartItems(QList<NMGChartItem*>& itemsList)
{
  QList<NMGChartItem*>::iterator it;
  for(it = itemsList.begin(); it != itemsList.end(); it++)
    slotAddNewChartItem(*it);
}

void NMGObjectInspector:: slotDeleteSeries(const QString& seriesId)
{
  if(series)
  {
    bool found = FALSE;
    int i;
    for(i = 0; i < series->childCount() && !found; i++)
    {
      QString id = qvariant_cast<QString>(series->child(i)->data(0, QTreeWidgetItem::UserType+1));
      if(id == seriesId) found = TRUE;
    }
    if(found)
    {
      delete series->takeChild(i-1);
      if(series->childCount()>0) lastSeries = series->child(series->childCount()-1);
      else
      {
        lastSeries = 0;
        setEnabled(FALSE);
      }
      clearSelection();
      emit signalSelectedChartItem(0);
    }
  }
}

void NMGObjectInspector::slotChangeSeriesText(const QString& seriesId, const QString& text)
{
  if(series)
  {
    bool found = FALSE;
    int i;
    for(i = 0; i < series->childCount() && !found; i++)
    {
      QObject *object = qvariant_cast<QObject*>(series->child(i)->data(0, 1000));
      NMGChartSeries* childSeries = qobject_cast<NMGChartSeries*>(object);
      if(childSeries)
      {
        if(childSeries->getId() == seriesId) found = TRUE;
      }
    }
    if(found) series->child(i-1)->setText(0, text);
  }
}

void NMGObjectInspector::slotChangeSeriesColor(const QString& seriesId, const QColor& color)
{
  if(series)
  {
    bool found = FALSE;
    int i;
    for(i = 0; i < series->childCount() && !found; i++)
    {
      QObject *object = qvariant_cast<QObject*>(series->child(i)->data(0, QTreeWidgetItem::UserType));
      NMGChartSeries* childSeries = qobject_cast<NMGChartSeries*>(object);
      if(childSeries)
      {
        if(childSeries->getId() == seriesId) found = TRUE;
      }
    }
    if(found)
    {
      QPixmap decoration = QPixmap(16,16);
      decoration.fill(color);
      series->child(i-1)->setData(0, Qt::DecorationRole, qVariantFromValue(decoration));
    }
  }
}

void NMGObjectInspector::slotChangeSeriesOrder(const QString& seriesId, int newOrder)
{
  if(series)
  {
    bool found = FALSE;
    int i;
    for(i = 0; i < series->childCount() && !found; i++)
    {
      QObject *object = qvariant_cast<QObject*>(series->child(i)->data(0, QTreeWidgetItem::UserType));
      NMGChartSeries* childSeries = qobject_cast<NMGChartSeries*>(object);
      if(childSeries)
      {
        if(childSeries->getId() == seriesId) found = TRUE;
      }
    }
    if(found && (newOrder >= 0 && newOrder < series->childCount()))
    {
      series->insertChild(newOrder, series->takeChild(i-1));
      lastSeries = series->child(series->childCount()-1);
      clearSelection();
      emit signalSelectedChartItem(0);
    }
  }
}

void NMGObjectInspector::slotSelectionChanged()
{
  QList<QTreeWidgetItem*> items = selectedItems();

  foreach (QTreeWidgetItem* item, items) {
    QObject *object = qvariant_cast<QObject*>(item->data(0, QTreeWidgetItem::UserType));

    NMGChartItem* item = qobject_cast<NMGChartItem*>(object);
    if (item) emit signalSelectedChartItem(item);
    else emit signalSelectedChartItem(0);
  }
}

#include "nmgobjectinspector.moc"
