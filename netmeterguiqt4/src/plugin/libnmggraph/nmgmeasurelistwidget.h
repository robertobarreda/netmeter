/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#ifndef NMGMEASURELISTWIDGET_H
#define NMGMEASURELISTWIDGET_H

#include "nmgtestloaderhandler.h"
#include "nmgtestfiledata.h"
#include "nmgflowdata.h"
#include "nmgglobals.h"
#include <nmggrouptreewidget.h>

/**
 * \brief NMGMeasureListWidget is a convenience class that provides a standard tree widget 
 * with a classic item-based interface to show tests' metadata.
 */
class NMGMeasureListWidget : public NMGGroupTreeWidget
{
  Q_OBJECT

  public:
    
    /** \brief Default constructor. */
    NMGMeasureListWidget(QWidget* parent = 0);
    
    /** \brief Default destructor. */
    ~NMGMeasureListWidget() {}
    
    /** 
     * \brief Adds test metadata information as a new child in the tree of tests.
     * \param fileName Path to the test file.
     * \param loader Structure where file information is saved.
     */
    void addNewTest(const QString& fileName, NMGTestLoaderHandler& loader);
    
    /** \brief Returns if measure list has some entry or not. */
    bool isEmpty();
    
  signals:
    
    /**
     * \brief This signal can be emitted (not always) whenever a movement 
     * in measures list is done.
     * 
     * The signal will be emitted only if the item moved is a valid atomic item.
     * We consider a valid atomic item, the one that has not children and its type 
     * is Flow or Test. If the item moved has children they are processed and extra 
     * individual signals are emitted for each valid atomic child.
     * \param id Internal item identifier.
     * \param position Returns and integer (between 0 and the maximum number of valid
     * atomic items available minus one) with the position occupied by the valid atomic
     * item in the measure list if all valid atomic items in the list are imaginary put
     * into a traditional list in order of appearance.
     */
    void signalValidAtomicItemMoved(const QString& id, int position);
    
  public slots:
    
    /** 
     * \brief This slot can be called to change the external text of a list item, if it is
     * available. It has not effect if item doesn't exist.
     * \param id Internal identifier for the searched list item
     * \param text New text.
     */
    void slotChangeMeasureListItemText(const QString& id, const QString& text);
    
    /** 
     * \brief This slot can be called to add a couloured decoration to a list item. It has not
     * effect if item doesn't exist.
     * \param id Internal identifier for the searched list item
     * \param color Decoration color
     */    
    void slotChangeMeasureListItemColor(const QString& id, const QColor& color);
    
    /** \brief This slot can be called to delete all items in the measure list. */
    void slotDeleteAllItems();
    
  private slots:
    
    void slotItemDeleted();
    void slotItemMoved(const QString& id);

  private:
    
    enum TimeUnits { MILISECS, SECS, MINS, HOURS };
    enum BinarySizeUnits { BYTES, KBYTES, MBYTES, GBYTES };
    enum DecimalSizeUnits { BITS, KBITS, MBITS, GBITS };

    void addHeaderInformation(Globals::SupportedType type);
    void addAggregationFileInformation(NMGTestFileData& data, const QString& fileName);
    void addPacketsFileInformation(NMGTestFileData& data, const QString& fileName);
    
    void readFlowMetadataInfo(NMGFlowData* flow, QString metatags);
    QString simplifyTime(double time, NMGMeasureListWidget::TimeUnits startUnit);
    QString simplifyBinarySize(double size, NMGMeasureListWidget::BinarySizeUnits startUnit);
    QString simplifyDecimalSize(double size, NMGMeasureListWidget::DecimalSizeUnits startUnit);
    
    // movement treatment methods
    void guessValidAtomicItemsMoved(const QString& id, QList<QString>& guessed);
    void guessValidAtomicItemsMovedRecursive(const QString& id, 
                                             QList<QString>& guessed,
                                             NMGGroupTreeWidgetItem* item);
    int guessMovementPosition(const QString& id);
    void guessMovementPositionRecursive(const QString& id, 
                                        int& position, 
                                        bool& found, 
                                        NMGGroupTreeWidgetItem* item);
    
    void selectAllItems(QAbstractItemModel* model, 
                        QItemSelectionModel* selectionModel,
                        const QModelIndex& parent);
    
    bool isHeaderSet;
};

#endif
