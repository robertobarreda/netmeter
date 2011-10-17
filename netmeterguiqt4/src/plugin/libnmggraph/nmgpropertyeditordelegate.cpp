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
#include "nmgpropertyeditordelegate.h"
#include "nmgglobals.h"

#include <QApplication>
#include <QKeyEvent>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QFontDatabase>

NMGPropertyEditorDelegate::NMGPropertyEditorDelegate(QObject* parent)
  : QItemDelegate(parent)
{}
    
void NMGPropertyEditorDelegate::paint(QPainter* painter, 
                                      const QStyleOptionViewItem& opt, 
                                      const QModelIndex& index) const
{
  QStyleOptionViewItem option = opt;

  if(index.column() == 1) option.state &= ~QStyle::State_Selected;
  option.state &= ~QStyle::State_HasFocus;

  QPen savedPen = painter->pen();
  QItemDelegate::paint(painter, option, index);

  QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
  painter->setPen(QPen(color));
  if(index.column() == 1) {
    int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
    painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
  }
  painter->drawLine(option.rect.x(), option.rect.bottom(),
                    option.rect.right(), option.rect.bottom());
  painter->setPen(savedPen);
}
    
QSize NMGPropertyEditorDelegate::sizeHint(const QStyleOptionViewItem& opt, 
                                          const QModelIndex& index) const
{
  return QItemDelegate::sizeHint(opt, index) + QSize(4, 4);
}

bool NMGPropertyEditorDelegate::eventFilter(QObject* object, QEvent* event)
{
  QWidget* editor = qobject_cast<QWidget*>(object);

  switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease: 
    {
      QKeyEvent* ke = static_cast<QKeyEvent*>(event);
      if (!(ke->modifiers() & Qt::ControlModifier) && 
            (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down)) 
      {
        event->ignore();
        return true;
      }
      break;
    } 
    default:
      break;
  }
  return QItemDelegate::eventFilter(editor ? editor : object, event);
}

QWidget* NMGPropertyEditorDelegate::createEditor(QWidget* parent,
                                                 const QStyleOptionViewItem& option,
                                                 const QModelIndex& index) const
{
  Q_UNUSED(option);
  QWidget* editor = 0;
  if(index.column() == 1)
  {
    const QTreeWidgetItem* property = static_cast<QTreeWidgetItem*>(index.internalPointer());
    int valueType = property->data(1, Qt::EditRole).type();
    QString propertyName = property->text(0);
    switch(valueType)
    {
      case QVariant::Bool:
      {
        QComboBox* combo = new QComboBox(parent);
        combo->setFrame(FALSE);
        combo->addItems(QStringList() << QString::fromUtf8("false") << QString::fromUtf8("true"));
        QObject::connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(sync()));
        editor = combo;
        break;
      }
      
      case QVariant::String:
      {
        if(propertyName == "fontFamily")
        {
          QFontDatabase fdb;
          QStringList list = fdb.families(QFontDatabase::Latin);
          QStringList result;
          for(int i = 0; i < list.size(); i++) 
          {
            if(fdb.isScalable(list.at(i))) result += list.at(i);
          }
          QComboBox* combo = new QComboBox(parent);
          combo->setFrame(FALSE);
          combo->addItems(result);
          QObject::connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(sync()));
          editor = combo;
        }
        else if(propertyName == "representation")
        {
          QComboBox* combo = new QComboBox(parent);
          combo->setFrame(FALSE);
          QStringList list;
          list << LINE_TYPE << AREA_TYPE << POINTS_TYPE << BARS_TYPE;
          combo->addItems(list);
          QObject::connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(sync()));
          editor = combo;
        }
        else if(propertyName == "position")
        {
          QComboBox* combo = new QComboBox(parent);
          combo->setFrame(FALSE);
          QStringList list;
          list << NORTH << NORTHEAST << EAST;
          list << SOUTHEAST << SOUTH << SOUTHWEST;
          list << WEST << NORTHWEST << CENTER;
          combo->addItems(list);
          QObject::connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(sync()));
          editor = combo;
        }
        else
        {
          QLineEdit* lineEdit = new QLineEdit(parent);
          lineEdit->setFrame(FALSE);
          QObject::connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(sync()));
          editor = lineEdit;
        }
        break;
      }
      
      case QVariant::Color:
      {
        QComboBox* combo = new QComboBox(parent);
        combo->setFrame(FALSE);
        
        QStringList colorNames = QColor::colorNames();
        for (int i = 0; i < colorNames.size(); ++i) {
          QColor color(colorNames[i]);
          combo->insertItem(i, colorNames[i]);
          combo->setItemData(i, color, Qt::DecorationRole);
        }
        QObject::connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(sync()));
        editor = combo;
        break;
      }
      
      case QVariant::Int:
      {
        if(propertyName == "fontSize")
        {
          QComboBox* combo = new QComboBox(parent);
          combo->setFrame(FALSE);
          QFontDatabase db;
          foreach(int size, db.standardSizes()) combo->addItem(QString::number(size));
          QObject::connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(sync()));
          editor = combo;
        }
        else if(propertyName == "pointSize")
        {
          QSpinBox* spin = new QSpinBox(parent);
          spin->setFrame(FALSE);
          spin->setMinimum(1);
          spin->setMaximum(3);
          QObject::connect(spin, SIGNAL(editingFinished()), this, SLOT(sync()));
          editor = spin;
        }
        break;
      }
    }
    if(editor) editor->installEventFilter(const_cast<NMGPropertyEditorDelegate*>(this));
  }
  return editor;
}

void NMGPropertyEditorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  const QTreeWidgetItem* property = static_cast<QTreeWidgetItem*>(index.internalPointer());
  QVariant var = property->data(1, Qt::EditRole);
  int valueType = var.type();
  switch(valueType)
  {
    case QVariant::Bool:
    {
      if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
        combo->setCurrentIndex(qVariantValue<bool>(var) ? 1 : 0);
      break;
    }
    
    case QVariant::String:
    {
      if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
      {
        QString value = qVariantValue<QString>(var);
        int idx = combo->findText(value);
        if(idx != -1) combo->setCurrentIndex(idx);
      }
      else if(QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor))
      {
        QString value = qVariantValue<QString>(var);
        if (lineEdit->text() != value) lineEdit->setText(value);
      }
      break;
    }
    
    case QVariant::Color:
    {
      if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
      {
        int idx = combo->findData(property->data(1, Qt::EditRole), Qt::DecorationRole);
        if(idx != -1) combo->setCurrentIndex(idx);
      }
      break;
    }
    
    case QVariant::Int:
    {
      QString propertyName = property->text(0);
      if(propertyName == "fontSize")
      {
        if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
        {
          int value = qVariantValue<int>(property->data(1,Qt::EditRole));
          int idx=combo->findText(QString::number(value));
          if(idx != -1) combo->setCurrentIndex(idx);
        }
      }
      else if(propertyName == "pointSize")
      {
        if(QSpinBox* spin = qobject_cast<QSpinBox*>(editor))
        {
          int value = qVariantValue<int>(property->data(1,Qt::EditRole));
          spin->setValue(value);
        }
      }
      break;
    }
  }
}

void NMGPropertyEditorDelegate::setModelData(QWidget* editor, 
                                             QAbstractItemModel* model, 
                                             const QModelIndex& index) const
{
  const QTreeWidgetItem* property = static_cast<QTreeWidgetItem*>(index.internalPointer());
  QVariant var = property->data(1, Qt::EditRole);
  int valueType = var.type();
  switch(valueType)
  {
    case QVariant::Bool:
    {
      if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
      {
        bool newValue = combo->currentIndex() ? true : false;
        model->setData(index, QVariant(newValue), Qt::EditRole);
      }
      break;
    }
    
    case QVariant::String:
    {
      if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
      {
        int idx = combo->currentIndex();
        if(idx != -1)
        {
          QString newValue = combo->itemText(idx);
          model->setData(index, QVariant(newValue), Qt::EditRole);
        }
      }
      else if(QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor)) 
      {
        QString newValue = lineEdit->text();
        model->setData(index, QVariant(newValue), Qt::EditRole);
      }
      break;
    }
    
    case QVariant::Color:
    {
      if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
      {
        int idx = combo->currentIndex();
        if(idx != -1)
        {
          QColor newValue = qVariantValue<QColor>(combo->itemData(idx, Qt::DecorationRole));
          model->setData(index, QVariant(newValue), Qt::EditRole);
        }
      }
      break;      
    }
    
    case QVariant::Int:
    {
      QString propertyName = property->text(0);
      if(propertyName == "fontSize")
      {
        if(QComboBox* combo = qobject_cast<QComboBox*>(editor))
        {
          int idx = combo->currentIndex();
          if(idx != -1)
          {
            int newValue = combo->itemText(idx).toInt();
            model->setData(index, QVariant(newValue), Qt::EditRole);
          }
        }
      }
      else if(propertyName == "pointSize")
      {
        if(QSpinBox* spin = qobject_cast<QSpinBox*>(editor))
        {
          int newValue = spin->value();
          model->setData(index, QVariant(newValue), Qt::EditRole);
        }
      }
      break;
    }
  }
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  emit const_cast<NMGPropertyEditorDelegate*>(this)->signalPropertyChanged(property->text(0), property->data(1, Qt::EditRole));
  QApplication::restoreOverrideCursor();
}

void NMGPropertyEditorDelegate::updateEditorGeometry(QWidget* editor,
                                                     const QStyleOptionViewItem& option, 
                                                     const QModelIndex& index) const
{
  QItemDelegate::updateEditorGeometry(editor, option, index);
  editor->setGeometry(editor->geometry().adjusted(0, 0, -1, -1));
}

// ********* //
// * SLOTS * //
// ********* //

void NMGPropertyEditorDelegate::sync()
{
  QWidget* w = qobject_cast<QWidget*>(sender());
  if (w == 0) return;
  emit commitData(w);
}

#include "nmgpropertyeditordelegate.moc"
