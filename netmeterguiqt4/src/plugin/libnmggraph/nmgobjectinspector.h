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
#ifndef NMGOBJECTINSPECTOR_H
#define NMGOBJECTINSPECTOR_H

#include "nmgchartitem.h"
#include <QTreeWidget>

/**
 * \brief NMGObjectInspector class is a user interface that allows to inspect the graphical
 * chart items available.
 * 
 * It is used together with NMGPropertyEditor.
 * \sa NMGPropertyEditor
 */
class NMGObjectInspector: public QTreeWidget
{
  Q_OBJECT
      
  public:
    
    /** 
     * \brief Default constructor.
     * \param parent NMGObjectInspector parent, if any.
     */
    NMGObjectInspector(QWidget* parent = 0);
    
    /** \brief Default destructor. */
    ~NMGObjectInspector() {}
    
  signals:
    
    /** \brief This signal is emitted when a chart item is selected for inspection. */
    void signalSelectedChartItem(NMGChartItem* item);
    
  public slots:
    
    /** 
     * \brief This slot can be called to notify the object inspector that a new
     * graphical chart item is available for inspection.
     */
    void slotAddNewChartItem(NMGChartItem* item);
    
    /**
     * \brief This slot is provided for convenience.
     * \sa slotAddNewChartItem
     */
    void slotAddNewChartItems(QList<NMGChartItem*>& itemsList);
    
    /**
     * \brief This slot must be called each time a series is deleted from the
     * plotting component.
     * 
     * If this slot is not called when the series is deleted, it can result in 
     * a fatal error if the user wants to inspect a residual entry into the
     * object inspector for a non-existent series.
     */
    void slotDeleteSeries(const QString& seriesId);
    
    /**
     * \brief This slot can be called to change the visual text seen by the user
     * in any object inspector entry that is classified as a series. It is used to
     * maintain data consistence between other components.
     */
    void slotChangeSeriesText(const QString& seriesId, const QString& text);
    
    /**
     * \brief This slot can be called to change the decoration color seen by the user
     * in any object inspector entry that is classified as a series. It is used to
     * maintain color consistence between other components.
     */
    void slotChangeSeriesColor(const QString& seriesId, const QColor& color);
    
    /**
     * \brief This slot can be called to reorder a series to a new order position.
     * 
     * This method assumes the existence of the series to be ordered an a valid order
     * position. Otherwise it has not effect. It is used to maintain order consistence 
     * between other components.
     */
    void slotChangeSeriesOrder(const QString& seriesId, int newOrder);
    
  protected:
    
    /** 
     * \brief Draws the branches in the object inspector.
     * 
     * This is used to change the painting predefined way in a customized one.
     */
    void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const;

  private slots:
    
    void slotSelectionChanged();

  private:
        
    QTreeWidgetItem* labels;
    QTreeWidgetItem* axes;
    QTreeWidgetItem* chartWall;
    QTreeWidgetItem* legend;
    QTreeWidgetItem* series; /**< Pointer to the data series parent. */
    QTreeWidgetItem* lastSeries; /**< Pointer to the last series added, if any has been added.*/
    
};

#endif
