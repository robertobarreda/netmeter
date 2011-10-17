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
#include "nmgtoolbar.h"
#include <nmgmoduleutils.h>
#include <QIcon>
#include <QApplication>

NMGToolBar::NMGToolBar(QWidget* parent, const QString& title)
  :QToolBar(title, parent)
{
  initialMinX = initialMaxX = initialMinY = initialMaxY = 0.0;
  previousMinXZoom = previousMaxXZoom = previousMinYZoom = previousMaxYZoom = 0.0;
  currentXRefVal = currentYRefVal = 2;
  zoomed = FALSE;
  createDefaultActions();
}

NMGToolBar::~NMGToolBar() 
{
  if(addTests) delete addTests;
  if(deleteSelectedMeasures) delete deleteSelectedMeasures;
  if(deleteAllMeasures) delete deleteAllMeasures;
  if(exportTo) delete exportTo;
  if(basicActions) delete basicActions;
  
  if(extendedSeparator) delete extendedSeparator;
  if(chartTypesCombo) delete chartTypesCombo;
  if(selectChartType) delete selectChartType;
  
  if(minXZoomLabel) delete minXZoomLabel;
  if(minXZoomLabelAction) delete minXZoomLabelAction;
  if(minXZoomEdit) delete minXZoomEdit;
  if(minXZoomEditAction) delete minXZoomEditAction;
  if(xZoom) delete xZoom;
  if(maxXZoomEdit) delete maxXZoomEdit;
  if(maxXZoomEditAction) delete maxXZoomEditAction;
  if(maxXZoomLabel) delete maxXZoomLabel;
  if(maxXZoomLabelAction) delete maxXZoomLabelAction;
    
  if(minYZoomLabel) delete minYZoomLabel;
  if(minYZoomLabelAction) delete minYZoomLabelAction;
  if(minYZoomEdit) delete minYZoomEdit;
  if(minYZoomEditAction) delete minYZoomEditAction;
  if(yZoom) delete yZoom;
  if(maxYZoomEdit) delete maxYZoomEdit;
  if(maxYZoomEditAction) delete maxYZoomEditAction;
  if(maxYZoomLabel) delete maxYZoomLabel;  
  if(maxYZoomLabelAction) delete maxYZoomLabelAction;
    
  if(updateZoom) delete updateZoom;
  if(restoreZoom) delete restoreZoom;  
  if(extendedActions) delete extendedActions;
}

void NMGToolBar::createDefaultActions()
{
  // Basic actions
  basicActions = new QActionGroup(this);
  
  // Add tests
  addTests = new QAction(QIcon(*NMGModuleUtils::getPixmap("fileopen.png")), 
                               tr("Open tests"),
                               basicActions);
  connect(addTests, SIGNAL(triggered()), this, SIGNAL(signalAddTests()));
  addTests->setShortcut(Qt::CTRL + Qt::Key_O);
  addTests->setStatusTip(tr("Opens and loads new test files to be processed by the plugin (Ctrl+O)"));
  addTests->setToolTip(tr("Open test files"));
  addAction(addTests);
  
  // Delete selected measures
  deleteSelectedMeasures = new QAction(QIcon(*NMGModuleUtils::getPixmap("remove-red.png")), 
                                             tr("Delete selected measures"),
                                             basicActions);
  connect(deleteSelectedMeasures, SIGNAL(triggered()), 
          this, SIGNAL(signalDeleteSelectedMeasures()));
  deleteSelectedMeasures->setShortcut(Qt::CTRL + Qt::Key_D);
  deleteSelectedMeasures->setStatusTip(tr("Deletes selected measures from the list of measures (Ctrl+D)"));
  deleteSelectedMeasures->setToolTip(tr("Delete selected measures"));
  addAction(deleteSelectedMeasures);
  
  // Delete all measures
  deleteAllMeasures = new QAction(QIcon(*NMGModuleUtils::getPixmap("edittrash.png")), 
                                        tr("Delete all measures"),
                                        basicActions);
  connect(deleteAllMeasures, SIGNAL(triggered()), this, SIGNAL(signalDeleteAllMeasures()));
  deleteAllMeasures->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_D);
  deleteAllMeasures->setStatusTip(tr("Deletes all measures from the list of measures (Ctrl+Shift+D)"));
  deleteAllMeasures->setToolTip(tr("Delete all measures"));
  addAction(deleteAllMeasures);
  
  // Export chart
  exportTo = new QAction(QIcon(*NMGModuleUtils::getPixmap("export.png")), 
                               tr("Export to"),
                               basicActions);
  connect(exportTo, SIGNAL(triggered()), this, SIGNAL(signalExportTo()));
  exportTo->setShortcut(Qt::CTRL + Qt::Key_E);
  exportTo->setStatusTip(tr("Exports graphic plot to any of the supported formats (Ctrl+E)"));
  exportTo->setToolTip(tr("Export to ..."));
  addAction(exportTo);
  
  // Extended actions
  extendedActions = new QActionGroup(this);
  extendedActions->setVisible(FALSE);
  
  // Extended separator
  extendedSeparator = addSeparator();
  extendedActions->addAction(extendedSeparator);
      
  // Chart type selector
  chartTypesCombo = new QComboBox(this);
  selectChartType = addWidget(chartTypesCombo);
  extendedActions->addAction(selectChartType);
  selectChartType->setVisible(FALSE);
  chartTypesCombo->setStatusTip(tr("Changes the chart type beetween the available ones"));
  chartTypesCombo->setToolTip(tr("Choose a chart type"));
  chartTypesCombo->setEnabled(TRUE);
  connect(chartTypesCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChartTypeChanged(int)));

  // Zoom actions
  minXZoomLabel = new QLabel(tr("   min"), this);
  minXZoomLabelAction = addWidget(minXZoomLabel);
  extendedActions->addAction(minXZoomLabelAction);
  minXZoomLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  minXZoomLabel->setEnabled(TRUE);
  
  minXZoomEdit = new QDoubleSpinBox(this);
  minXZoomEditAction = addWidget(minXZoomEdit);
  extendedActions->addAction(minXZoomEditAction);
  minXZoomEdit->setFrame(FALSE);
  minXZoomEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
  minXZoomEdit->setMinimum(0.0);
  minXZoomEdit->setMaximum(0.0);
  minXZoomEdit->setEnabled(TRUE);
  
  xZoom = new QAction(QIcon(*NMGModuleUtils::getPixmap("xaxis.png")), 
                      tr("x-axis"),
                      extendedActions);
  connect(xZoom, SIGNAL(triggered()), this, SLOT(slotFitToX()));
  xZoom->setShortcut(Qt::CTRL + Qt::Key_X);
  xZoom->setStatusTip(tr("Fits zoom to X (Ctrl+X)"));
  xZoom->setToolTip(tr("Fits to X"));
  addAction(xZoom);
  
  maxXZoomEdit = new QDoubleSpinBox(this);
  maxXZoomEditAction = addWidget(maxXZoomEdit);
  extendedActions->addAction(maxXZoomEditAction);
  maxXZoomEdit->setFrame(FALSE);
  maxXZoomEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
  maxXZoomEdit->setMinimum(0.0);
  maxXZoomEdit->setMaximum(0.0);
  maxXZoomEdit->setEnabled(TRUE);

  maxXZoomLabel = new QLabel(tr("max   "), this);
  maxXZoomLabelAction = addWidget(maxXZoomLabel);
  extendedActions->addAction(maxXZoomLabelAction);
  maxXZoomLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  maxXZoomLabel->setEnabled(TRUE);
  
  minYZoomLabel = new QLabel(tr("   min"), this);
  minYZoomLabelAction = addWidget(minYZoomLabel);
  extendedActions->addAction(minYZoomLabelAction);
  minYZoomLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  minYZoomLabel->setEnabled(TRUE);
  
  minYZoomEdit = new QDoubleSpinBox(this);
  minYZoomEditAction = addWidget(minYZoomEdit);
  extendedActions->addAction(minYZoomEditAction);
  minYZoomEdit->setFrame(FALSE);
  minYZoomEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
  minYZoomEdit->setMinimum(0.0);
  minYZoomEdit->setMaximum(0.0);
  minYZoomEdit->setEnabled(TRUE);
  
  yZoom = new QAction(QIcon(*NMGModuleUtils::getPixmap("yaxis.png")), 
                      tr("y-axis"),
                      extendedActions);
  connect(yZoom, SIGNAL(triggered()), this, SLOT(slotFitToY()));
  yZoom->setShortcut(Qt::CTRL + Qt::Key_Y);
  yZoom->setStatusTip(tr("Fits zoom to Y (Ctrl+Y)"));
  yZoom->setToolTip(tr("Fits to Y"));
  addAction(yZoom);
  
  maxYZoomEdit = new QDoubleSpinBox(this);
  maxYZoomEditAction = addWidget(maxYZoomEdit);
  extendedActions->addAction(maxYZoomEditAction);
  maxYZoomEdit->setFrame(FALSE);
  maxYZoomEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
  maxYZoomEdit->setMinimum(0.0);
  maxYZoomEdit->setMaximum(0.0);
  maxYZoomEdit->setEnabled(TRUE);

  maxYZoomLabel = new QLabel(tr("max   "), this);
  maxYZoomLabelAction = addWidget(maxYZoomLabel);
  extendedActions->addAction(maxYZoomLabelAction);
  maxYZoomLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  maxYZoomLabel->setEnabled(TRUE);
      
  updateZoom = new QAction(QIcon(*NMGModuleUtils::getPixmap("exec_zoom.png")), 
                           tr("Update zoom"),
                           extendedActions);
  connect(updateZoom, SIGNAL(triggered()), this, SLOT(slotUpdateZoom()));
  updateZoom->setShortcut(Qt::CTRL + Qt::Key_U);
  updateZoom->setStatusTip(tr("Zooms the graphic plot to the entered values (Ctrl+U)"));
  updateZoom->setToolTip(tr("Update zoom"));
  addAction(updateZoom);
  
  restoreZoom = new QAction(QIcon(*NMGModuleUtils::getPixmap("restore_zoom.png")), 
                            tr("Restore zoom"),
                            extendedActions);
  connect(restoreZoom, SIGNAL(triggered()), this, SLOT(slotRestoreZoom()));
  restoreZoom->setShortcut(Qt::CTRL + Qt::Key_R);
  restoreZoom->setStatusTip(tr("Restores zoom values to the initial ones (Ctrl+R)"));
  restoreZoom->setToolTip(tr("Restore zoom"));
  addAction(restoreZoom);
}

void NMGToolBar::setXAxisParametersVisibility(bool visible)
{
  minXZoomLabelAction->setVisible(visible);
  minXZoomEditAction->setVisible(visible);
  xZoom->setVisible(visible);
  maxXZoomEditAction->setVisible(visible);
  maxXZoomLabelAction->setVisible(visible);
}

int NMGToolBar::getSignificantDecimalsForInterval(const double& min, 
                                                  const double& max, 
                                                  const int& refValues) const
{
  Q_ASSERT_X((refValues-1)>0, "getSignificantDecimalsForInterval", "division by zero in interval");
  double interval = (max-min)/(refValues-1);
  return Globals::getSignificantDecimalDigits(interval);
}

// ********* //
// * SLOTS * //
// ********* //

void NMGToolBar::slotChangeChartType(SupportedType supported, ChartType type)
{
  chartTypesCombo->clear();
  switch(supported)
  {
    case PACKETS:
    {
      chartTypesCombo->addItem(tr(OWDOT_ABBREV), QVariant(OWDOT));
      chartTypesCombo->setItemData(0, QVariant(tr(OWDOT_EXTENDED)), Qt::ToolTipRole);
      
      chartTypesCombo->addItem(tr(OWDOP_ABBREV), QVariant(OWDOP));
      chartTypesCombo->setItemData(1, QVariant(tr(OWDOP_EXTENDED)), Qt::ToolTipRole);
        
      chartTypesCombo->addItem(tr(OWDD_ABBREV), QVariant(OWDD));
      chartTypesCombo->setItemData(2, QVariant(tr(OWDD_EXTENDED)), Qt::ToolTipRole);
      
      chartTypesCombo->addItem(tr(IPDVOT_ABBREV), QVariant(IPDVOT));
      chartTypesCombo->setItemData(3, QVariant(tr(IPDVOT_EXTENDED)), Qt::ToolTipRole);
      
      chartTypesCombo->addItem(tr(IPDVOP_ABBREV), QVariant(IPDVOP));
      chartTypesCombo->setItemData(4, QVariant(tr(IPDVOP_EXTENDED)), Qt::ToolTipRole);
      
      chartTypesCombo->addItem(tr(IPDVD_ABBREV), QVariant(IPDVD));
      chartTypesCombo->setItemData(5, QVariant(tr(IPDVD_EXTENDED)), Qt::ToolTipRole);
      
      int idx = chartTypesCombo->findText(convertToString(type));
      if(idx != -1) chartTypesCombo->setCurrentIndex(idx);
      if(!selectChartType->isVisible()) selectChartType->setVisible(TRUE);
      setXAxisParametersVisibility(TRUE);
      break;
    } 
    case AGGREGATION:
      if(selectChartType->isVisible()) selectChartType->setVisible(FALSE);
      setXAxisParametersVisibility(FALSE);
      break;
  }
  extendedActions->setVisible(TRUE);
}

void NMGToolBar::slotEmptyPlot()
{
  chartTypesCombo->clear();
  if(selectChartType->isVisible()) selectChartType->setVisible(FALSE);
  extendedActions->setVisible(FALSE);
}

void NMGToolBar::slotChangeAxisRanges(const double& currentMinX, const double& currentMaxX, int xNumRefValues,
                                      const double& currentMinY, const double& currentMaxY, int yNumRefValues,
                                      ChartType type)
{
  int decimalDigits;
  
  switch(type)
  {
    case OWDOP:
    case IPDVOP:
      minXZoomEdit->setDecimals(0);
      maxXZoomEdit->setDecimals(0);
      decimalDigits = getSignificantDecimalsForInterval(currentMinY, currentMaxY, yNumRefValues);
      minYZoomEdit->setDecimals(decimalDigits);
      maxYZoomEdit->setDecimals(decimalDigits);
      if(!zoomed)
      {
        minXZoomEdit->setMinimum(currentMinX);
        minXZoomEdit->setMaximum(currentMaxX);
        maxXZoomEdit->setMinimum(currentMinX);
        maxXZoomEdit->setMaximum(currentMaxX);
        minYZoomEdit->setMinimum(currentMinY);
        minYZoomEdit->setMaximum(currentMaxY);
        maxYZoomEdit->setMinimum(currentMinY);
        maxYZoomEdit->setMaximum(currentMaxY);
      }
      break;
      
    case BTC:
      decimalDigits = getSignificantDecimalsForInterval(currentMinY, currentMaxY, yNumRefValues);
      minYZoomEdit->setDecimals(decimalDigits);
      maxYZoomEdit->setDecimals(decimalDigits);
      if(!zoomed)
      {
        minYZoomEdit->setMinimum(currentMinY);
        minYZoomEdit->setMaximum(currentMaxY);
        maxYZoomEdit->setMinimum(currentMinY);
        maxYZoomEdit->setMaximum(currentMaxY);
      }
      break;
      
    case OWDOT:
    case IPDVOT:
      decimalDigits = getSignificantDecimalsForInterval(currentMinX, currentMaxX, xNumRefValues);
      minXZoomEdit->setDecimals(decimalDigits);
      maxXZoomEdit->setDecimals(decimalDigits);
      
      decimalDigits = getSignificantDecimalsForInterval(currentMinY, currentMaxY, yNumRefValues);
      minYZoomEdit->setDecimals(decimalDigits);
      maxYZoomEdit->setDecimals(decimalDigits);
      if(!zoomed)
      {
        minXZoomEdit->setMinimum(currentMinX);
        minXZoomEdit->setMaximum(currentMaxX);
        maxXZoomEdit->setMinimum(currentMinX);
        maxXZoomEdit->setMaximum(currentMaxX);
        minYZoomEdit->setMinimum(currentMinY);
        minYZoomEdit->setMaximum(currentMaxY);
        maxYZoomEdit->setMinimum(currentMinY);
        maxYZoomEdit->setMaximum(currentMaxY);
      }
      break;
      
    case OWDD:
    case IPDVD:
      decimalDigits = getSignificantDecimalsForInterval(currentMinX, currentMaxX, xNumRefValues);
      minXZoomEdit->setDecimals(decimalDigits);
      maxXZoomEdit->setDecimals(decimalDigits);
      
      decimalDigits = getSignificantDecimalsForInterval(currentMinY, currentMaxY, yNumRefValues);
      minYZoomEdit->setDecimals(decimalDigits);
      maxYZoomEdit->setDecimals(decimalDigits);
      if(!zoomed)
      {
        minXZoomEdit->setMinimum(currentMinX);
        minXZoomEdit->setMaximum(currentMaxX);
        maxXZoomEdit->setMinimum(currentMinX);
        maxXZoomEdit->setMaximum(currentMaxX);
        minYZoomEdit->setMinimum(0.0);
        minYZoomEdit->setMaximum(100.0);
        maxYZoomEdit->setMinimum(0.0);
        maxYZoomEdit->setMaximum(100.0);
      }
      break;
  }
  minXZoomEdit->setValue(currentMinX);
  maxXZoomEdit->setValue(currentMaxX);
  minYZoomEdit->setValue(currentMinY);
  maxYZoomEdit->setValue(currentMaxY);
  
  currentXRefVal = xNumRefValues;
  currentYRefVal = yNumRefValues;
  
  if(!zoomed)
  {
    initialMinX = minXZoomEdit->value();
    initialMaxX = maxXZoomEdit->value();
    initialMinY = minYZoomEdit->value();
    initialMaxY = maxYZoomEdit->value();
    
    previousMinXZoom = initialMinX;
    previousMaxXZoom = initialMaxX;
    previousMinYZoom = initialMinY;
    previousMaxYZoom = initialMaxY;
    
    xZoom->setToolTip(tr("Fits to X (%1,%2)").arg(initialMinX).arg(initialMaxX));
    yZoom->setToolTip(tr("Fits to Y (%1,%2)").arg(initialMinY).arg(initialMaxY));
  }
  
  minXZoomEdit->updateGeometry();
  maxXZoomEdit->updateGeometry();
  minYZoomEdit->updateGeometry();
  maxYZoomEdit->updateGeometry();
}

void NMGToolBar::slotUpdateZoom()
{
  bool error = FALSE;
  QString errors;
  if(minXZoomEditAction->isVisible() && maxXZoomEditAction->isVisible())
  {
    if(minXZoomEdit->value() < minXZoomEdit->minimum())
    { // it should never occur with a QDoubleSpinBox
      error = TRUE;
      errors.append(tr("x-axis minimum value is smaller than the legal minimum %1\n").arg(minXZoomEdit->minimum()));
    }
    if(maxXZoomEdit->value() > maxXZoomEdit->maximum())
    { // it should never occur with a QDoubleSpinBox
      error = TRUE;
      errors.append(tr("x-axis maximum value is greater than the legal maximum %1\n").arg(maxXZoomEdit->maximum()));
    }
    if(minXZoomEdit->value() >= maxXZoomEdit->value())
    {
      error = TRUE;
      errors.append(tr("x-axis contains an invalid or empty interval [") + 
                    QString::number(minXZoomEdit->value()) + tr(" - ") + 
                    QString::number(maxXZoomEdit->value()) + tr("]\n"));
    }
  }
  if(minYZoomEditAction->isVisible() && maxYZoomEditAction->isVisible())
  {
    if(minYZoomEdit->value() < minYZoomEdit->minimum())
    { // it should never occur with a QDoubleSpinBox
      error = TRUE;
      errors.append(tr("y-axis minimum value is smaller than the legal minimum %1\n").arg(minYZoomEdit->minimum()));
    }
    if(maxYZoomEdit->value() > maxYZoomEdit->maximum())
    { // it should never occur with a QDoubleSpinBox
      error = TRUE;
      errors.append(tr("y-axis maximum value is greater than the legal maximum %1\n").arg(maxYZoomEdit->maximum()));
    }
    if(minYZoomEdit->value() >= maxYZoomEdit->value())
    {
      error = TRUE;
      errors.append(tr("y-axis contains an invalid or empty interval [") +
                    QString::number(minYZoomEdit->value()) + tr(" - ") + 
                    QString::number(maxYZoomEdit->value()) + tr("]\n"));
    }
  }
  if(error)
  {
    NMGGraphicsError e(tr("Wrong entry values"), 
                       errors + tr("\nValues will be restored to the previous ones"), 
                       NMGGraphicsError::WARNING);
    e.raiseError();
    minXZoomEdit->setValue(previousMinXZoom);
    maxXZoomEdit->setValue(previousMaxXZoom);
    minYZoomEdit->setValue(previousMinYZoom);
    maxYZoomEdit->setValue(previousMaxYZoom);
  }
  else
  {
    previousMinXZoom = minXZoomEdit->value();
    previousMaxXZoom = maxXZoomEdit->value();
    previousMinYZoom = minYZoomEdit->value();
    previousMaxYZoom = maxYZoomEdit->value();
    
    if(minXZoomEdit->value() == initialMinX && maxXZoomEdit->value() == initialMaxX &&
       minYZoomEdit->value() == initialMinY && maxYZoomEdit->value() == initialMaxY)
    {
      if(zoomed) slotRestoreZoom();
    }
    else 
    {
      zoomed = TRUE;
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      emit signalZoom(minXZoomEdit->value(), maxXZoomEdit->value(),
                      minYZoomEdit->value(), maxYZoomEdit->value());
      QApplication::restoreOverrideCursor();
    }
  }
}

void NMGToolBar::slotRestoreZoom()
{
  if(zoomed)
  {
    minXZoomEdit->setValue(initialMinX);
    maxXZoomEdit->setValue(initialMaxX);
    minYZoomEdit->setValue(initialMinY);
    maxYZoomEdit->setValue(initialMaxY);
    zoomed = FALSE;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    emit signalRestoreZoom();
    QApplication::restoreOverrideCursor();
  }
}

void NMGToolBar::slotFitToX()
{
  int decimalDigits = getSignificantDecimalsForInterval(initialMinX, initialMaxX, currentXRefVal);
  minXZoomEdit->setDecimals(decimalDigits);
  maxXZoomEdit->setDecimals(decimalDigits);
  minXZoomEdit->setValue(initialMinX);
  maxXZoomEdit->setValue(initialMaxX);
  slotUpdateZoom();
}

void NMGToolBar::slotFitToY()
{
  int decimalDigits = getSignificantDecimalsForInterval(initialMinY, initialMaxY, currentYRefVal);
  minYZoomEdit->setDecimals(decimalDigits);
  maxYZoomEdit->setDecimals(decimalDigits);
  minYZoomEdit->setValue(initialMinY);
  maxYZoomEdit->setValue(initialMaxY);
  slotUpdateZoom();
}

void NMGToolBar::slotChartTypeChanged(int index)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  
  QVariant option = chartTypesCombo->itemData(index, Qt::UserRole);
  if(option.isValid()) emit signalChartTypeChanged((ChartType)option.toInt());
  
  QApplication::restoreOverrideCursor();
}

#include "nmgtoolbar.moc"
