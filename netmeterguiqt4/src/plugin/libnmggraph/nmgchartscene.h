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
#ifndef NMGCHARTSCENE_H
#define NMGCHARTSCENE_H

#include "nmgchartitem.h"
#include "nmgtestloaderhandler.h"
#include "nmgglobals.h"
#include <QGraphicsScene>
#include <QPointer>
using namespace Globals;

/**
 * \brief NMGChartScene class provides a surface for managing graphical 2D chart items.
 * 
 * The class serves as a container for NMGChartItem elements. It is used together with 
 * NMGPlotWidget for visualizing the graphical items, such as axes, labels, series, and 
 * so on, on a 2D surface. Contrary to the normal use of QGraphicsScene, this class has
 * been conceived to be able to show all chart items in the available surface. This has
 * been achieved managing and distributing the available space in the surface at each
 * moment, between all the graphical elements. Furthermore if, for some reason, one or
 * more NMGChartItem elements change its size as an answer to some external event, 
 * NMGChartScene is notified to reallocated all the elements to be shown correctly.
 * The current class is able to show One Way Delay and Bulk Transport Capacity tests and
 * its derivates.
 */
class NMGChartScene : public QGraphicsScene
{
  Q_OBJECT

  public:
    
    /** \brief Default constructor. */
    NMGChartScene(QObject* parent = 0);
    
    /** \brief Default destructor. */
    ~NMGChartScene() {}
    
    /** 
     * \brief Method for adding new tests to the graphical surface. This method assumes that
     * the test added have the same test type to the previous ones added, if any. Otherwise 
     * the result is undefined.
     * \param fileName Test path to be added.
     * \param loader Structure where the information for \e fileName test is saved.
     */
    void addNewTest(const QString& fileName, NMGTestLoaderHandler& loader);
    
    /** \brief Returns the chart type used to paint data into scene. */
    inline ChartType getChartType() const { return buildingPattern; }
    
    /** 
     * \brief Sets the chart type used to paint data in scene in different ways and update all
     * data according to the new type.
     * \param type Chart type. It is important to know that not all the types can be set. It
     * depends on the tests nature, added previously to the scene, and this is because some 
     * ways of data painting has not sense. If \e type can not be set due to the own nature
     * of tests, this method do nothing.
     */
    void setChartType(ChartType type);
    
  signals:
    
    /**
     * \brief This signal is emitted when there aren't series and a new one is created successfully
     * or when chart type is changed programmatically.
     * 
     * It notifies that the type managed will be \e type from now on.
     * \param supported Test type managed
     * \param type Chart type used to build scene.
     */
    void signalChartTypeChanged(SupportedType supported, ChartType type);

    
    /** \brief This signal is emitted when a new chart item has been added to NMGChartScene. */
    void signalAddedNewChartItem(NMGChartItem* item);
    
    /** 
     * \brief This signal is emitted when a set of new chart items have been added to 
     * NMGChartScene. 
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
     * visibility identified by \e id has been changed.
     */
    void signalSeriesVisibilityChanged(const QString& id, bool visible);
    
    /** 
     * \brief This signal is emitted to notify to independent external components that series
     * identified by \e id has been deleted
     */
    void signalSeriesDeleted(const QString& id);
    
    /** \brief This signal is emitted to notify that there aren't elements into scene to be painted. */ 
    void signalEmptyScene();
    
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
     * \brief This signal is emitted when some basic operations in scene are done and the scene
     * is currently zoomed.
     * 
     * Suitable connected, this signal will allow to restore the scene to an unzoomed state, and
     * mantain data consistence to do the basic operations correctly. Basic operations are:
     * to add a test, change visualization type, reorder series and remove series.
     */
    void signalZoomRestoringNeeded();
      
  public slots:
    
    /** 
     * \brief This slot is called when NMGPlotWidget is resized by the user.
     * 
     * It reallocates all graphical elements whatever surface size is. If available space
     * in the surface isn't enough to show correctly all elements, scrollbars appear.
     */
    void slotResizeScene(const int& width, const int& height);
    
    /** 
     * \brief This slot can be called to delete a series previously loaded. 
     * \param id Internal identifier. The identifier must exist between the available series'
     * identifiers, otherwise the slot has not effect.
     */
    void slotDeleteSeries(const QString& id);
    
    /**
     * \brief This slot can be called to reorder a series to a new order position.
     * 
     * This method assumes the existence of the series to be ordered an a valid order
     * position. Otherwise it has not effect.
     * \param seriesId Internal series identifier.
     * \param newOrder New order position. Must be between 0 and the maximum number of series
     * minus one.
     */
    void slotOrderSeries(const QString& seriesId, int newOrder);
    
    /**
     * \brief This slot can be called to zoom scene.
     * 
     * It affects only to the series and axis.
     * \param minX Minimum value that x-axis has to take once zoom is done.
     * \param maxX Maximum value that x-axis has to take once zoom is done.
     * \param minY Minimum value that y-axis has to take once zoom is done.
     * \param maxY Maximum value that y-axis has to take once zoom is done.
     */
    void slotZoom(const double& minX, const double& maxX, const double& minY, const double& maxY);
    
    /** \brief This slot can be called to zoom scene to 100%. */
    void slotRestoreZoom();
        
  private slots:
    
    /** 
     * \brief Slot provided for convinience to catch size changes in any chart 
     * graphical item.
     */
    void slotGiveMoreSpaceToItems();
    
  private:
    
    void createConnections();
    
    // adding methods
    void addNewTest(const QString& fileName, NMGTestFileData& data);
    void prepareContextualElements(QString& chartTitleText, 
                                   QString& chartXAxisText, 
                                   QString& chartYAxisText,
                                   QString lastTestName);

    /** 
     * \brief Builds all necessary data to create a series.
     * \return False if data in \e xData and \e yData cannot be calculated. True otherwise.
     * If an error occurs, \e xData and \e yData are cleared.
     */
    bool prepareSeriesData(int seriesNumber,
                           const QString& fileName,
                           NMGTestFileData& data, 
                           QString& id, 
                           QString& text, 
                           QList<double>& xData, 
                           QList<double>& yData);
    int numberOfSeries(NMGTestFileData& data);
    void copyList(QList<double>& destList,
                  QList<double>::const_iterator sourceListBegin,
                  QList<double>::const_iterator sourceListEnd);
    void copyList(QList<double>& destList,
                  QList<int>::const_iterator sourceListBegin,
                  QList<int>::const_iterator sourceListEnd);
    
    /** 
     * \brief Generates a percentual distribution. 
     * 
     * This method assumes that \e srcList has ascending order.
     */
    void generateDistributionPattern(const QList<double>& srcList, 
                                     QList<double>& valueList, 
                                     QList<double>& percentList);
    /** 
     * \brief Generates an ipdv list.
     * 
     * You has to notice that in ipdv lists, lost packets and its continguous packets 
     * aren't taken into account. This method assumes that \e packetsList and \e delaysList
     * have the same size.
     * \return True if the ipdv list has been successfully generated. False otherwise. An ipdv
     * list won't be generated if there are less than two non-lost packets or \e packetsList 
     * and \e delaysList have different sizes.
     */
    bool generateIPDVList(QList<double>& packetsList, 
                          QList<double>& delaysList,
                          QList<double>& ipdvList);

    void addSeries(NMGTestFileData& data,
                   QString& seriesId,
                   QString& seriesText,
                   const QList<double>& xData,
                   const QList<double>& yData,
                   QString& chartTitleText,
                   QString& chartXAxisText,
                   QString& chartYAxisText);
    void initializeSeriesState(NMGChartSeries& series);
    void updateSeriesZValue();
    
    /** 
     * \brief Suggests the min and max values for all series based on \e series values and 
     * assuming that \series is the only series in the scene. It mades some corrections to 
     * the default series values if series contains extreme cases in its values.
     */
    void suggestGlobalSeriesXYValues(const NMGChartSeries& series, 
                                     double& minX, double& minY, 
                                     double& maxX, double& maxY);
    void updateGlobalSeriesXYValues(const double& minX, const double& minY,
                                    const double& maxX, const double& maxY);
    
    // management methods for chart type changes
    bool rebuildSceneWithNewChartType();
    bool rebuildSeriesVertexList(NMGTestFileData& data,
                                 QList<double>& xData, 
                                 QList<double>& yData,
                                 int id = 0);
    void rebuildGlobalSeriesXYValues();
    
    // items management methods
    void setSceneSpaceForChartItems();
    void setScenePositionForChartItems();
    int getMinimumNeededWidth() const;
    int getMinimumNeededHeight() const;    
    void setItemsVisible(bool visible = TRUE);
    
    // ordering methods
    void rebuildSeriesDueToReorder();
       
    int currentSceneWidth;
    int currentSceneHeight;
    
    // Used only to ensure proportional painting with more than one series.
    double seriesMaxXValue;
    double seriesMaxYValue;
    double seriesMinXValue;
    double seriesMinYValue;
    
    // members to support zoom funcionality
    bool zoomed;
    double initialMaxXValue;
    double initialMaxYValue;
    double initialMinXValue;
    double initialMinYValue;
    
    NMGChartRoot* root;
    NMGChartLabel* titleLabel;
    NMGChartLabel* xAxisLabel;
    NMGChartLabel* yAxisLabel;
    NMGChartXAxis* xAxis;
    NMGChartYAxis* yAxis;
    NMGChartWall* chartWall;
    NMGChartLegend* legend;
    QList<NMGChartSeries*> seriesList;
    QList<QString> seriesOrder; /**< Saves series order by its internal identifier. */
    
    bool areItemsVisible;
    bool areBaseChartItemsNotifiedOutside; /**< To notify other components like object inspector.*/
    ChartType buildingPattern; /**< Determines how chart elements must be built. */
    /** 
     * \brief Saves the current test file type take into account for the building of elements 
     * in the scene.
     */
    SupportedType type;
    QPointer<NMGTestLoaderHandler> loaderHandler;
};

#endif
