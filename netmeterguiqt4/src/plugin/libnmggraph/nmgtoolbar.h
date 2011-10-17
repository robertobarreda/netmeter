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
#ifndef NMGTOOLBAR_H
#define NMGTOOLBAR_H

#include "nmgglobals.h"
#include "nmggraphicserror.h"
#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QAction>
#include <QActionGroup>
using namespace Globals;

/**
 * \brief NMGToolBar is the class responsible for creating the specific plugin toolbar, and 
 * managing it accordingly to external or internal events.
 */
class NMGToolBar : public QToolBar
{
  Q_OBJECT

  public:

    /**
     * \brief Class constructor.
     * \param parent Parent widget for the toolbar. If a parent is passed the toolbar will be 
     * deleted when that parent is destroyed.
     * \param title Toolbar title. The given title identifies the toolbar and it is shown in 
     * the context menu provided by QMainWindow.
     */
    NMGToolBar(QWidget* parent, const QString& title = 0);

    /** \brief Default destructor. */
    ~NMGToolBar();

  signals:

    /** \brief This signal is emitted when an "add tests" action is invoked in the toolbar.*/
    void signalAddTests();
    
    /** 
     * \brief This signal is emitted when a "chart exportation" action is invoked in the 
     * toolbar.
     */
    void signalExportTo();
    
    /** \brief This signal is emitted when a chart type change action is invoked in the toolbar.*/
    void signalChartTypeChanged(ChartType type);
    
    /** 
     * \brief This signal is emitted when the selected measures deletion action is invoked
     * in the toolbar.
     */
    void signalDeleteSelectedMeasures();
    
    /** \brief This signal is emitted when complete deletion action is invoked in the toolbar.*/
    void signalDeleteAllMeasures();
    
    /** 
     * \brief This signal is emitted when zoom parameters changes in such a way that a zoom
     * in the scene is possible.
     * \param currentMinX Minimum value that x-axis has to take once zoom is done.
     * \param currentMaxX Maximum value that x-axis has to take once zoom is done.
     * \param currentMinY Minimum value that y-axis has to take once zoom is done.
     * \param currentMaxY Maximum value that y-axis has to take once zoom is done.
     */
    void signalZoom(const double& currentMinX, const double& currentMaxX,
                    const double& currentMinY, const double& currentMaxY);
    
    /** 
     * \brief This signal is emitted to notify to the scene that it must restore scene zoom to 
     * the initial state.
     */
    void signalRestoreZoom();
    
  public slots:
    
    /** 
     * \brief This slot can be called to change the chart type options available depending
     * on current \e type .
     */
    void slotChangeChartType(SupportedType supported, ChartType type);
    
    /** \brief This slot can be called to update actions that depends on the plot's emptiness. */
    void slotEmptyPlot();
    
    /**
     * \brief This slot can be called to synchronize zoom entry parameters with the graphical
     * axis.
     * \param currentMinX This parameter holds the current minimum value taken by x-axis.
     * \param currentMaxX This parameter holds the current maximum value taken by x-axis.
     * \param xNumRefValues Number of labels that appear in the x-axis.
     * \param currentMinY This parameter holds the current minimum value taken by y-axis.
     * \param currentMaxY This parameter holds the current maximum value taken by y-axis.
     * \param yNumRefValues Number of labels that appear in the y-axis.
     * \param type Chart type used to build scene.
     */
    void slotChangeAxisRanges(const double& currentMinX, const double& currentMaxX, int xNumRefValues,
                              const double& currentMinY, const double& currentMaxY, int yNumRefValues,
                              ChartType type);
    
    /** 
     * \brief This slot can be called to restore zoom.
     * 
     * It can be connected not only internally but also externally.
     */
    void slotRestoreZoom();
    
  private slots:
    
    void slotUpdateZoom();
    void slotFitToX();
    void slotFitToY();
    void slotChartTypeChanged(int index);
    
  private:

    void createDefaultActions();
    void setXAxisParametersVisibility(bool visible);
    /** 
     * \brief Divides the interval defined by min and max in refValues-1 parts and
     * calculates the most significant decimals for one of these parts.
     * \param min Minimum value for the interval.
     * \param max Maximum value for the interval.
     * \param refValues Number of parts for the interval plus one. This value must be at less 2
     */
    int getSignificantDecimalsForInterval(const double& min, 
                                          const double& max, 
                                          const int& refValues) const;

    QActionGroup* basicActions;
    QAction* addTests;
    QAction* deleteSelectedMeasures;
    QAction* deleteAllMeasures;
    QAction* exportTo;
    
    QActionGroup* extendedActions;
    QAction* extendedSeparator;
    
    QComboBox* chartTypesCombo;
    QAction* selectChartType;
    
    // zoom actions
    QLabel* minXZoomLabel;
    QAction* minXZoomLabelAction;
    QDoubleSpinBox* minXZoomEdit;
    QAction* minXZoomEditAction;
    QAction* xZoom;
    QDoubleSpinBox* maxXZoomEdit;
    QAction* maxXZoomEditAction;
    QLabel* maxXZoomLabel;
    QAction* maxXZoomLabelAction;
    
    QLabel* minYZoomLabel;
    QAction* minYZoomLabelAction;
    QDoubleSpinBox* minYZoomEdit;
    QAction* minYZoomEditAction;
    QAction* yZoom;
    QDoubleSpinBox* maxYZoomEdit;
    QAction* maxYZoomEditAction;
    QLabel* maxYZoomLabel;
    QAction* maxYZoomLabelAction;
    
    QAction* updateZoom;
    QAction* restoreZoom;
    
    // non graphical members to support zoom funcionality
    double initialMinX, initialMaxX;
    double initialMinY, initialMaxY;
    double previousMinXZoom, previousMaxXZoom;
    double previousMinYZoom, previousMaxYZoom;
    int currentXRefVal, currentYRefVal;
    bool zoomed;
};

#endif
