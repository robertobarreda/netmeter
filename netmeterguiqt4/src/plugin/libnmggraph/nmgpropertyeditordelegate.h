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
#ifndef NMGPROPERTYEDITORDELEGATE_H
#define NMGPROPERTYEDITORDELEGATE_H

#include <QItemDelegate>
#include <QPainter>

/** 
 * \brief NMGPropertyEditorDelegate class provides editing facilities for the different
 * type of properties supported in NMGPropertyEditor.
 * 
 * \sa NMGPropertyEditor
 */
class NMGPropertyEditorDelegate : public QItemDelegate
{
  Q_OBJECT
      
  public:
    
    /**
     * \brief Default constructor.
     * 
     * This method shouldn't be used directly because it is called whenever is needed by the 
     * property editor according to the Model/View approach.
     */
    NMGPropertyEditorDelegate(QObject* parent = 0);
    
    /** \brief Default destructor. */
    ~NMGPropertyEditorDelegate() {}

    /** 
     * \brief Returns true if the given editor is a valid QWidget and the given event 
     * is handled; Otherwise returns false.
     */
    bool eventFilter(QObject* object, QEvent* event);

    /** 
     * \brief Renders the delegate using the given painter and style option for the 
     * item specified by index.
     */
    void paint(QPainter* painter, 
                const QStyleOptionViewItem& option,
                const QModelIndex& index) const;

    /**
     * \brief Returns the size needed by the delegate to display the item specified 
     * by index, taking into account the style information provided by option.
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

    /**
     * \brief Returns the widget used to edit the item specified by index for editing. 
     * 
     * The parent widget and style option are used to control how the editor widget appears.
     */
    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    /** 
     * \brief Sets the data to be displayed and edited by the editor for the item 
     * specified by index.
     */
    void setEditorData(QWidget* editor, const QModelIndex& index) const;

    /** 
     * \brief Sets the data for the specified model and item index from that supplied 
     * by the editor.
     */
    void setModelData(QWidget* editor, 
                      QAbstractItemModel* model, 
                      const QModelIndex& index) const;

    /** 
     * \brief Updates the editor for the item specified by index according to 
     * the style option given.
     */
    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const;
    
  signals:
    
    /** \brief This signal is emitted whenever a property has changed its value. */
    void signalPropertyChanged(const QString& name, const QVariant& value);

  private slots:
    
    /** \brief This slot is called every time some commit event in the delegate is done.*/
    void sync();

};

#endif
