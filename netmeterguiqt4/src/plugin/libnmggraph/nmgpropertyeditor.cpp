/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
 *                                                                         *
 *   This file is based on the Qt Designer components and the KDevelop     *
 *   designer components.                                                  *
 *   See http://www.trolltech.com/ and http://www.kdevelop.org/ for        *
 *   more information.                                                     *
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
#include "nmgpropertyeditor.h"
#include "nmgpropertyeditordelegate.h"

#include <QApplication>
#include <QHeaderView>
#include <QMetaObject>
#include <QMetaProperty>

NMGPropertyEditor::NMGPropertyEditor(QWidget* parent)
  : QTreeWidget(parent), currentInspectedObject(0)
{
  // setting tree widget properties
  delegate = new NMGPropertyEditorDelegate(this);
  setItemDelegate(delegate);
  setAlternatingRowColors(true);
  setColumnCount(2);
  headerItem()->setText(0, tr("Property"));
  headerItem()->setText(1, tr("Value"));
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);  
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  header()->setResizeMode(0, QHeaderView::ResizeToContents);
  setContextMenuPolicy(Qt::NoContextMenu);
  setEditTriggers(QAbstractItemView::CurrentChanged|QAbstractItemView::SelectedClicked);
  
  connect(delegate, SIGNAL(signalPropertyChanged(const QString&, const QVariant&)), 
          this, SLOT(slotPropertyChanged(const QString&, const QVariant&)));
}

NMGPropertyEditor::~NMGPropertyEditor()
{
  if(delegate) delete delegate;
}

void NMGPropertyEditor::drawBranches(QPainter* painter, 
                                     const QRect& rect, 
                                     const QModelIndex& index) const
{
  static const int windows_deco_size = 9;
  QStyleOptionViewItem opt = viewOptions();

  if (model()->hasChildren(index)) {
    opt.state |= QStyle::State_Children;
    
    QRect primitive(rect.left(), rect.top(), indentation(), rect.height());
    primitive.moveLeft(primitive.left() + (primitive.width() - windows_deco_size)/2);
    primitive.moveTop(primitive.top() + (primitive.height() - windows_deco_size)/2);
    primitive.setWidth(windows_deco_size);
    primitive.setHeight(windows_deco_size);
    opt.rect = primitive;

    if (isExpanded(index)) opt.state |= QStyle::State_Open;
    style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, painter, this);
  }
  QPen savedPen = painter->pen();
  QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
  painter->setPen(QPen(color));
  painter->drawLine(rect.x(), rect.bottom(), rect.right(), rect.bottom());
  painter->setPen(savedPen);
}

void NMGPropertyEditor::focusInEvent(QFocusEvent* event)
{
  QAbstractScrollArea::focusInEvent(event);
  viewport()->update();
}

// ********* //
// * SLOTS * //
// ********* //
   
void NMGPropertyEditor::slotSetupProperties(NMGChartItem* item)
{
  clear();
  if(item)
  {
    currentInspectedObject = item;
    if(const QMetaObject* metaObject = item->metaObject())
    {
      QTreeWidgetItem* treeItem;
      for(int i = 1; i < metaObject->propertyCount(); i++)
      {
        QMetaProperty metaPropertyInfo = metaObject->property(i);
        if(metaPropertyInfo.isValid() && 
            metaPropertyInfo.isWritable() && 
            metaPropertyInfo.isDesignable(item))
        { // a QTreeWidgetItem can be added
          treeItem = new QTreeWidgetItem(this);
          treeItem->setFlags(treeItem->flags() | Qt::ItemIsEditable);
          treeItem->setText(0, QString(metaPropertyInfo.name()));
          treeItem->setData(1, Qt::EditRole, metaPropertyInfo.read(item));
        }
      }
    }
  }
  else currentInspectedObject = 0;
}

void NMGPropertyEditor::slotRebuildProperties()
{
  if(currentInspectedObject) slotSetupProperties(currentInspectedObject);
}

void NMGPropertyEditor::slotPropertyChanged(const QString& name, const QVariant& value)
{
  if(currentInspectedObject)
  {
    if(QMetaObject* metaObject = const_cast<QMetaObject*>(currentInspectedObject->metaObject()))
    {
      int index = metaObject->indexOfProperty(name.toUtf8().data());
      if(index != -1)
      {
        QMetaProperty metaPropertyInfo = metaObject->property(index);
        metaPropertyInfo.write(currentInspectedObject, value);
      }
    }
  }
}

#include "nmgpropertyeditor.moc"
