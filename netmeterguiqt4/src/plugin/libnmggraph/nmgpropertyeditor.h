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
#ifndef NMGPROPERTYEDITOR_H
#define NMGPROPERTYEDITOR_H

#include "nmgchartitem.h"
#include <QTreeWidget>

class NMGPropertyEditorDelegate;

/**
 * \brief NMGPropertyEditor class is a user interface that allows to change the available
 * properties for a specific NMGChartItem.
 * 
 * It is used together with NMGObjectInspector.
 * \sa NMGObjectInspector
 */
class NMGPropertyEditor: public QTreeWidget
{
  Q_OBJECT
      
  public:
    
    /** 
     * \brief Default constructor.
     * \param parent NMGPropertyEditor parent, if any.
     */
    NMGPropertyEditor(QWidget* parent = 0);
    
    /** \brief Default destructor. */
    ~NMGPropertyEditor();
            
  public slots:
    
    /** 
     * \brief This slot can be called to inspect the available properties for an existent
     * \e item and build the user interface to interact with them.
     */
    void slotSetupProperties(NMGChartItem* item);
    
    /**
     * \brief This slot is provided for convenience and forces to rebuild the available
     * properties for the current inspected chart item.
     */
    void slotRebuildProperties();
        
  protected:
    
    /** 
     * \brief Draws the branches in the property editor.
     * 
     * This is used to change the painting predefined way in a customized one.
     */
    void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const;
    
    /** 
     * \brief This function is called with the given event when the widget obtains 
     * the focus. Property editor is repainted.
     */
    void focusInEvent(QFocusEvent* event);

  private slots:
    
    void slotPropertyChanged(const QString& name, const QVariant& value);
    
  private:
    
    NMGChartItem* currentInspectedObject;
    NMGPropertyEditorDelegate* delegate;

};

#endif
