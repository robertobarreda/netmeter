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
#ifndef NMGPLOTWIDGET_H
#define NMGPLOTWIDGET_H

#include "nmgtestloaderhandler.h"
#include "nmgchartscene.h"
#include <nmgmodulepreferencesmanager.h>
#include <QGraphicsView>

/**
 * \brief The NMGPlotWidget class provides a widget for displaying the contents of a 
 * NMGChartScene.
 */
class NMGPlotWidget : public QGraphicsView
{
  Q_OBJECT

  public:
    
    /** \brief Default constructor. */
    NMGPlotWidget(QWidget* parent = 0);
    
    /** \brief Default destructor. */
    ~NMGPlotWidget() {}
    
    /**
     * \brief Method provided for convinience to manage external actions for adding new tests.
     * \param fileName Test path to be added.
     * \param loader Structure where the information for \e fileName test is saved.
     */
    void addNewTest(const QString& fileName, NMGTestLoaderHandler& loader);
  
  signals:
    
    /** \brief This signal is emitted when the NMGPlotWidget is resized. */
    void signalResize(const int& width, const int& height);
    
    /**
     * \brief This signal is emitted when there aren't series and a new one is created successfully
     * or when chart type is changed programmatically.
     * 
     * It notifies that the type managed will be \e type from now on.
     * \param supported Test type managed
     * \param type Chart type used to build scene.
     */
    void signalChartTypeChanged(SupportedType supported, ChartType type);
    
    /** 
     * \brief This signal is emitted when a new graphical element has been 
     * added to NMGPlotWidget. 
     */
    void signalAddedNewChartItem(NMGChartItem* item);
    
    /** 
     * \brief This signal is emitted when a set of new graphical elements have been 
     * added to NMGPlotWidget. 
     */
    void signalAddedNewChartItems(QList<NMGChartItem*>& itemsList);
        
    /** 
     * \brief This signal is emitted to notify to independent external components that text in 
     * a series has been changed.
     */
    void signalSeriesTextChanged(const QString& id, const QString& text);
    
    /** 
     * \brief This signal is emitted to notify to independent external components that color in 
     * a series has been changed.
     */
    void signalSeriesColorChanged(const QString& id, const QColor& color);
    
    /** 
     * \brief This signal is emitted to notify to independent external components that series
     * identified by \e id has been deleted from scene.
     */
    void signalSeriesDeleted(const QString& id);
    
    /** \brief This signal is emitted to notify that NMGPlotWidget has anything to paint. */
    void signalEmptyPlot();
    
    /**
     * \brief This signal is emitted each time that graphical axis change their ranges.
     * \param currentMinX This parameter holds the current minimum value taken by x-axis.
     * \param currentMaxX This parameter holds the current maximum value taken by x-axis.
     * \param xNumRefValues Number of labels that appear in the x-axis.
     * \param currentMinY This parameter holds the current minimum value taken by y-axis.
     * \param currentMaxY This parameter holds the current maximum value taken by y-axis.
     * \param yNumRefValues Number of labels that appear in the y-axis.
     * \param type Chart type used to build scene.
     */
    void signalAxisRangesChanged(const double& currentMinX, const double& currentMaxX, int xNumRefValues,
                                 const double& currentMinY, const double& currentMaxY, int yNumRefValues,
                                 ChartType type);
    
    /** 
     * \brief This signal is emitted when zoom scene needs to be restored to the initial values.
     */
    void signalZoomRestoringNeeded();
    
  public slots:
    
    /** 
     * \brief This slot can be called whatever an exportation is needed. 
     * \param preferences If defined exportation path will be update accordingly to
     * the last path used by the user. Otherwise, exportation path will be the user
     * home directory.
     */
    void slotExportTo(NMGModulePreferencesManager* preferences = 0);
    
    /** 
     * \brief This slot sets the chart type used to paint data in scene
     * \param type Chart type. 
     */
    inline void slotSetChartType(ChartType type) { scene.setChartType(type); }
    
    /** 
     * \brief This slot can be called to delete a series previously loaded.
     * \param id Internal id. It will be deleted only the series identified into the scene
     * with \e id . If identifier doesn't exist between available series' identifiers,
     * the slot has not effect.
     */
    inline void slotDeleteSeries(const QString& id) { scene.slotDeleteSeries(id); }
    
    /**
     * \brief This slot can be called to reorder an individual series taking into account
     * a new order position.
     * (position will be reinterpret as depth or movement, depending on the tests type).
     * 
     * \param seriesId Internal series identifier.
     * \param order New order position.
     */
    inline void slotOrderSeries(const QString& seriesId, int order) 
    { 
      scene.slotOrderSeries(seriesId, order); 
    }
    
    /**
     * \brief This slot can be called to zoom data in plot.
     * 
     * \param minX Minimum value that x-axis has to take once zoom is done.
     * \param maxX Maximum value that x-axis has to take once zoom is done.
     * \param minY Minimum value that y-axis has to take once zoom is done.
     * \param maxY Maximum value that y-axis has to take once zoom is done.
     */
    inline void slotZoom(const double& minX, const double& maxX,
                         const double& minY, const double& maxY)
    {
      scene.slotZoom(minX, maxX, minY, maxY);
    }
    
    /** \brief This slot can be called to paint all data into the plot. */
    inline void slotRestoreZoom() { scene.slotRestoreZoom(); }
    
  protected:
    
    /** \brief This event handler receives NMGPlotWidget resize events. */
    void resizeEvent(QResizeEvent *event);
    
    /** \brief Surface where the NMGChartItem elements are added. */
    NMGChartScene scene;
    
  private:
    
    void exportToPNG(QString filePath);
    void exportToJPG(QString filePath);
    void exportToSVG(QString filePath);
    void exportToPDF(QString filePath);
};

#endif
