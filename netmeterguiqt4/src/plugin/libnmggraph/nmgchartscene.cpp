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
#include "nmgchartscene.h"
#include "nmgtestfiledata.h"
#include "nmgfloworientedresultdata.h"
#include "nmgaggregationorientedresultdata.h"
#include "nmggraphicserror.h"

NMGChartScene::NMGChartScene(QObject* parent)
  :QGraphicsScene(parent), currentSceneWidth(-1), currentSceneHeight(-1), zoomed(FALSE)
{ 
  root = new NMGChartRoot();
  root->setVisible(FALSE);

  chartWall = new NMGChartWall();

  xAxis = new NMGChartXAxis();
  yAxis = new NMGChartYAxis();
  
  titleLabel = new NMGChartLabel(NMGChartLabel::TITLE);
  xAxisLabel = new NMGChartLabel(NMGChartLabel::AXIS_X);
  yAxisLabel = new NMGChartLabel(NMGChartLabel::AXIS_Y);
  
  legend = new NMGChartLegend();
  
  setItemsVisible(FALSE);
  
  root->setPos(0,0);
  addItem(root);
  chartWall->setZValue(1);
  chartWall->setParentItem(root);
  
  titleLabel->setZValue(2);
  titleLabel->setParentItem(root);
  xAxisLabel->setZValue(2);
  xAxisLabel->setParentItem(root);
  yAxisLabel->setZValue(2);
  yAxisLabel->setParentItem(root);

  xAxis->setZValue(3);
  xAxis->setParentItem(root);
  yAxis->setZValue(4);
  yAxis->setParentItem(root);
  
  legend->setZValue(0);
  legend->setParentItem(chartWall);

  createConnections();
  areBaseChartItemsNotifiedOutside = FALSE;
}

void NMGChartScene::createConnections()
{
  connect(root, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  connect(titleLabel, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  connect(xAxisLabel, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  connect(yAxisLabel, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  connect(xAxis, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  connect(yAxis, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  connect(chartWall, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  connect(legend, SIGNAL(signalGiveMeMoreSpace()), this, SLOT(slotGiveMoreSpaceToItems()));
  
  connect(this, SIGNAL(signalSeriesTextChanged(const QString&,const QString&)),
          legend, SLOT(slotChangeEntryText(const QString&,const QString&)));
  connect(this, SIGNAL(signalSeriesTextChanged(const QString&,const QString&)),
          xAxis, SLOT(slotChangeLabelText(const QString&,const QString&)));
  connect(this, SIGNAL(signalSeriesColorChanged(const QString&,const QColor&)),
          legend, SLOT(slotChangeEntryColor(const QString&,const QColor&)));
  connect(this, SIGNAL(signalSeriesVisibilityChanged(const QString&, bool)),
          legend, SLOT(slotChangeEntryVisibility(const QString&, bool)));
}

void NMGChartScene::addNewTest(const QString& fileName, NMGTestLoaderHandler& loader)
{
  try
  {
    if(!loaderHandler) loaderHandler = &loader;
    NMGTestFileData& testData = loader.getTestFileData(fileName);
    if(seriesList.isEmpty())
    {
      switch(testData.getCurrentSupportedType())
      {
        case AGGREGATION:
          type = AGGREGATION;
          buildingPattern = BTC;
          break;
          
        case PACKETS:
          type = PACKETS;
          buildingPattern = OWDOT;
          break;
      }
      emit signalChartTypeChanged(type, buildingPattern);
    }
    if(zoomed) emit signalZoomRestoringNeeded();
    addNewTest(fileName, testData);
  }
  catch(NMGGraphicsError e) { e.raiseError(); }
  
  if(!areBaseChartItemsNotifiedOutside)
  { 
    /* NOTE Next signal can't be done in constructor because it notifies to externals
     * components that need to be created to be effectively notified. This has been 
     * solved with a boolean that has its effect only once because its logical place is 
     * in constructor. */
    QList<NMGChartItem*> newItems;
    newItems << titleLabel << xAxisLabel << yAxisLabel << xAxis << yAxis << chartWall << legend;
    emit signalAddedNewChartItems(newItems);
    areBaseChartItemsNotifiedOutside = TRUE;
  }
  if(!areItemsVisible) setItemsVisible(TRUE);
}

void NMGChartScene::addNewTest(const QString& fileName, NMGTestFileData& data)
{
  QString chartTitleText;
  QString chartXAxisText;
  QString chartYAxisText;
  prepareContextualElements(chartTitleText, chartXAxisText, chartYAxisText, 
                            data.getMetadata()->getTestName());
  bool error = FALSE;
  for(int num = 0; num < numberOfSeries(data); num++)
  {
    QString seriesId;
    QString seriesText;
    QList<double> xData;
    QList<double> yData;
    if(!prepareSeriesData(num, fileName, data, seriesId, seriesText, xData, yData)) error = TRUE;
    addSeries(data, seriesId, seriesText, xData, yData, 
              chartTitleText, chartXAxisText, chartYAxisText);
  }
  if(error)
  {
    ChartType previousPattern = buildingPattern;
    switch(type)
    {
      case PACKETS:
        buildingPattern = OWDOT;
        break;
        
      case AGGREGATION:
        buildingPattern = BTC;
        break;
    }
    NMGGraphicsError e(tr("Data processing error"), 
                       tr("File %1 ").arg(fileName) + 
                       tr("contains data that cannot be calculated under ") +
                       convertToString(previousPattern, TRUE) + tr(".\n") +
                       tr("Graphical analysis will be set automatically to ") +
                       convertToString(buildingPattern, TRUE) + tr("."), 
                       NMGGraphicsError::WARNING);
    e.raiseError();

    rebuildSceneWithNewChartType();
    emit signalChartTypeChanged(type, buildingPattern);
  }
}

void NMGChartScene::prepareContextualElements(QString& chartTitleText, 
                                              QString& chartXAxisText, 
                                              QString& chartYAxisText,
                                              QString lastTestName)
{
  // it is assumed that loaderHandler is defined
  switch(buildingPattern)
  {
    case OWDOT:
      if(loaderHandler->numberOfCurrentLoadedTests() > 1) chartTitleText = tr(OWDOT_EXTENDED);
      else
      {
        chartTitleText = lastTestName;
        if(chartTitleText.isEmpty()) chartTitleText = tr(OWDOT_EXTENDED);
        else chartTitleText = QString("%1 - %2").arg(chartTitleText).arg(tr(OWDOT_ABBREV));
      }
      chartXAxisText = tr(TIME_FROM_START);
      chartYAxisText = tr(DELAY);
      break;
        
    case OWDOP:
      if(loaderHandler->numberOfCurrentLoadedTests() > 1) chartTitleText = tr(OWDOP_EXTENDED);
      else
      {
        chartTitleText = lastTestName;
        if(chartTitleText.isEmpty()) chartTitleText = tr(OWDOP_EXTENDED);
        else chartTitleText = QString("%1 - %2").arg(chartTitleText).arg(tr(OWDOP_ABBREV));
      }
      chartXAxisText = tr(PACKET_SEQUENCE);
      chartYAxisText = tr(DELAY);
      break;
        
    case OWDD:
      if(loaderHandler->numberOfCurrentLoadedTests() > 1) chartTitleText = tr(OWDD_EXTENDED);
      else
      {
        chartTitleText = lastTestName;
        if(chartTitleText.isEmpty()) chartTitleText = tr(OWDD_EXTENDED);
        else chartTitleText = QString("%1 - %2").arg(chartTitleText).arg(tr(OWDD_ABBREV));
      }
      chartXAxisText = tr(DELAY);
      chartYAxisText = tr(PERCENT_PACKETS);
      break;
        
    case IPDVOT:
      if(loaderHandler->numberOfCurrentLoadedTests() > 1) chartTitleText = tr(IPDVOT_EXTENDED);
      else
      {
        chartTitleText = lastTestName;
        if(chartTitleText.isEmpty()) chartTitleText = tr(IPDVOT_EXTENDED);
        else chartTitleText = QString("%1 - %2").arg(chartTitleText).arg(tr(IPDVOT_ABBREV));
      }
      chartXAxisText = tr(TIME_FROM_START);
      chartYAxisText = tr(IPDV_DELAY);
      break;
        
    case IPDVOP:
      if(loaderHandler->numberOfCurrentLoadedTests() > 1) chartTitleText = tr(IPDVOP_EXTENDED);
      else
      {
        chartTitleText = lastTestName;
        if(chartTitleText.isEmpty()) chartTitleText = tr(IPDVOP_EXTENDED);
        else chartTitleText = QString("%1 - %2").arg(chartTitleText).arg(tr(IPDVOP_ABBREV));
      }
      chartXAxisText = tr(PACKET_SEQUENCE);
      chartYAxisText = tr(IPDV_DELAY);
      break;
        
    case IPDVD:
      if(loaderHandler->numberOfCurrentLoadedTests() > 1) chartTitleText = tr(IPDVD_EXTENDED);
      else
      {
        chartTitleText = lastTestName;
        if(chartTitleText.isEmpty()) 
          chartTitleText = tr(IPDVD_EXTENDED);
        else chartTitleText = QString("%1 - %2").arg(chartTitleText).arg(tr(IPDVD_ABBREV));
      }
      chartXAxisText = tr(IPDV_DELAY);
      chartYAxisText = tr(PERCENT_PACKETS);
      break;
        
    case BTC:
      if(loaderHandler->numberOfCurrentLoadedTests() > 1) chartTitleText = tr(BTC_EXTENDED);
      else
      {
        chartTitleText = lastTestName;
        if(chartTitleText.isEmpty()) chartTitleText = tr(BTC_EXTENDED);
        else chartTitleText = QString("%1 - %2").arg(chartTitleText).arg(tr(BTC_ABBREV));
      }
      chartXAxisText = tr(TESTS);
      chartYAxisText = tr(RECEIVER_TPUT);
      break;
  }
}

bool NMGChartScene::prepareSeriesData(int seriesNumber,
                                      const QString& fileName,
                                      NMGTestFileData& data, 
                                      QString& id, 
                                      QString& text, 
                                      QList<double>& xData, 
                                      QList<double>& yData)
{
  bool error = FALSE;
  switch(type)
  {
    case PACKETS:
    {
      
      NMGFlowOrientedResultData* result = (NMGFlowOrientedResultData*)data.getResultData();
      QMap<int, NMGFlowData*>::const_iterator it = result->constBegin();
      for(int iterNum = 0; iterNum < seriesNumber; iterNum++, it++) {}
      NMGFlowData* flow = it.value();
      
      id = QString("%1%2%3").arg(fileName).arg(INTERNAL_SEPARATOR).arg(flow->getFlowId());
      text = QString("%1").arg(flow->getFlowName());
      if(text.isEmpty()) text = tr("Default series text");

      switch(buildingPattern)
      {
        case OWDOT:
          copyList(xData, flow->beginTimestamp(), flow->endTimestamp());
          copyList(yData, flow->beginDelay(), flow->endDelay());
          break;
        case OWDOP:
          copyList(xData, flow->beginSequenceIdentifier(), flow->endSequenceIdentifier());
          copyList(yData, flow->beginDelay(), flow->endDelay());
          break;
        case OWDD:
        {
          QList<double> delaysList;
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          generateDistributionPattern(delaysList, xData, yData);
          break;
        }
        case IPDVOT:
        {
          QList<double> delaysList;
          copyList(xData, flow->beginTimestamp(), flow->endTimestamp());
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          if(!generateIPDVList(xData,delaysList,yData))
          {
            error = TRUE;
            xData.clear(); yData.clear();
          }
          break;
        }
        case IPDVOP:
        {
          QList<double> delaysList;
          copyList(xData, flow->beginSequenceIdentifier(), flow->endSequenceIdentifier());
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          if(!generateIPDVList(xData, delaysList, yData))
          {
            error = TRUE;
            xData.clear(); yData.clear();
          }
          break;
        }
        case IPDVD:
        {
          QList<double> delaysList;
          QList<double> ipdvList;
          QList<double> rubbish;
          copyList(rubbish, flow->beginTimestamp(), flow->endTimestamp());
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          if(!generateIPDVList(rubbish, delaysList, ipdvList))
          {
            error = TRUE;
            xData.clear(); yData.clear();
          }
          else
          {
            rubbish.clear();
            generateDistributionPattern(ipdvList,xData,yData);
          }
          break;
        }
        default:
          break;     
      }
      break;
    }
    case AGGREGATION:
      switch(buildingPattern)
      {
        case BTC:
        {
          NMGAggregationOrientedResultData* result;
          result = (NMGAggregationOrientedResultData*)data.getResultData();
  
          id = QString("%1").arg(fileName);
          text = QString("%1").arg(data.getMetadata()->getTestName());
          if(text.isEmpty()) text = tr("Default series text");    
          xData << seriesList.size()+1; // x axis is forced to reuse chart series paint method
          yData << result->getReceivingThroughputInKBitsPerSecond();
          break;
        }
        default:
          break;
      }
      break;
  }
  return !error;
}

int NMGChartScene::numberOfSeries(NMGTestFileData& data)
{
  int num = 0;
  switch(type)
  {
    case AGGREGATION:
      num = 1;
      break;
    case PACKETS:
      num = ((NMGFlowOrientedResultData*)data.getResultData())->size();
      break;
  }
  return num;
}

void NMGChartScene::copyList(QList<double>& destList, 
                             QList<double>::const_iterator sourceListBegin,
                             QList<double>::const_iterator sourceListEnd)
{
  QList<double>::const_iterator it;
  for(it = sourceListBegin; it != sourceListEnd; it++)
  {
    destList.append(*it);
  }
}

void NMGChartScene::copyList(QList<double>& destList, 
                             QList<int>::const_iterator sourceListBegin,
                             QList<int>::const_iterator sourceListEnd)
{
  QList<int>::const_iterator it;
  for(it = sourceListBegin; it != sourceListEnd; it++)
  {
    destList.append((double)*it);
  }
}

void NMGChartScene::generateDistributionPattern(const QList<double>& srcList, 
                                                QList<double>& valueList, 
                                                QList<double>& percentList)
{
  if(!srcList.isEmpty())
  {
    valueList.clear();
    percentList.clear();
    // bigger the number of decimals is, more precision is obtained 
    // (specially when values disperion is small)
    int numOfDecimals = 4; 
    QList<double>::const_iterator it;
    int idx = -1;
    for(it = srcList.constBegin(); it != srcList.constEnd(); it++)
    {
      double truncatedValue = QString::number(*it, 'g', numOfDecimals).toDouble();
      if((idx = valueList.indexOf(truncatedValue)) != -1) 
        percentList.replace(idx, percentList.at(idx)+1.0);
      else
      {
        double value = truncatedValue;
        double percent = 1.0;
        // ordered insertion by value
        int i = valueList.size();
        while(i>0 && valueList.at(i-1) > value) { --i; }
        valueList.insert(i, value);
        percentList.insert(i, percent);
      }
    }
    
    // filtering lost packets
    if((idx = valueList.indexOf(QString::number(0.0, 'g', numOfDecimals).toDouble())) != -1)
    {
      valueList.removeAt(idx); 
      percentList.removeAt(idx);
    }
    
    // transform into %
    double factor = 100.0/(double)srcList.size();
    for(int i=0; i<percentList.size(); i++) percentList.replace(i, percentList.at(i)*factor);
  }
}

bool NMGChartScene::generateIPDVList(QList<double>& packetsList, 
                                     QList<double>& delaysList,
                                     QList<double>& ipdvList)
{
  bool error = FALSE;
  if(packetsList.size() == delaysList.size())
  {
    // first: lost packets (delay == 0) are removed. Contiguous elements to the lost packet 
    // are also removed if it's possible because ipdv is calculated between non lost contiguous
    // packets.
    int size = delaysList.size();
    int i = 0;
    while(i < size)
    {
      if(delaysList.at(i) == 0.0)
      {
        int next = i+1;
        if(next < size)
        {
          delaysList.removeAt(next);
          packetsList.removeAt(next);
        }
        
        delaysList.removeAt(i);
        packetsList.removeAt(i);
        
        int prev = i-1;
        if(prev >= 0)
        {
          delaysList.removeAt(prev);
          packetsList.removeAt(prev);
          i--;
        }         
        size = delaysList.size();
      }
      else i++;
    }
    
    // second: ipdv lists are calculated
    if(delaysList.size() > 1)
    {
      // ipdv list will contains only size()-1 ipdv values so, they has to be assigned to
      // last size()-1 packets, and consequently the first one is removed (they also could
      // be assigned to the first size()-1 packets)
      packetsList.removeAt(0);
          
      double previous = delaysList.at(0);
      double current = delaysList.at(0);
      int i = 1;
      while(i < delaysList.size())
      {
        // operations are calculated in microseconds because of double precision problems
        current = delaysList.at(i);
        int ipdv = (int)(current*1000) - (int)(previous*1000);
        ipdvList.append((double)ipdv/1000.0);
        // advance to the next delay
        previous = current;
        i++;
      }
    }
    else error = TRUE;
  }
  else error = TRUE;
  return !error;
}

void NMGChartScene::addSeries(NMGTestFileData& data,
                              QString& seriesId,
                              QString& seriesText,
                              const QList<double>& xData,
                              const QList<double>& yData,
                              QString& chartTitleText,
                              QString& chartXAxisText,
                              QString& chartYAxisText)
{
  // before adding new series
  if(!chartTitleText.isEmpty()) titleLabel->setText(chartTitleText);
  if(!chartXAxisText.isEmpty()) xAxisLabel->setText(chartXAxisText);
  if(!chartYAxisText.isEmpty()) yAxisLabel->setText(chartYAxisText);

  switch(type)
  {
    case AGGREGATION:
      xAxis->setDataType(NMGChartAxis::TEXT);
      xAxis->addLabel(seriesId, seriesText);
      yAxis->setDataType(NMGChartAxis::NUMBER);
      yAxis->setNumReferenceValues(6);
      chartWall->setNumReferenceLines(2,6);
      break;
      
    case PACKETS:
      xAxis->setDataType(NMGChartAxis::NUMBER);
      xAxis->setNumReferenceValues(11);
      yAxis->setDataType(NMGChartAxis::NUMBER);
      yAxis->setNumReferenceValues(6);
      chartWall->setNumReferenceLines(11,6);
      break;
  }
  
  // adding new series
  NMGChartSeries* series = new NMGChartSeries(seriesId);
  connect(series, SIGNAL(signalTextChanged(const QString&, const QString&)),
          this, SIGNAL(signalSeriesTextChanged(const QString&, const QString&)));
  connect(series, SIGNAL(signalColorChanged(const QString&, const QColor&)),
          this, SIGNAL(signalSeriesColorChanged(const QString&, const QColor&)));
  connect(series, SIGNAL(signalVisibilityChanged(const QString&, bool)),
          this, SIGNAL(signalSeriesVisibilityChanged(const QString&, bool)));
  series->setText(seriesText);
  series->setBaseColor(chartWall->getNextSeriesBaseColor());
  series->setVertexList(xData, yData);
  initializeSeriesState(*series);
  
  seriesList.append(series);
  seriesOrder.append(seriesId);
  updateSeriesZValue();

  // after adding new series
  double minX, minY, maxX, maxY;
  suggestGlobalSeriesXYValues(*series, minX, minY, maxX, maxY);
  updateGlobalSeriesXYValues(minX, minY, maxX, maxY);
  
  switch(type)
  {
    case AGGREGATION:
      yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
      emit signalAxisRangesChanged(0.0, 0.0, xAxis->getNumReferenceValues(),
                                   seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                   buildingPattern);
      break;
      
    case PACKETS:
      xAxis->setAxisScale(seriesMinXValue, seriesMaxXValue);
      yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
      emit signalAxisRangesChanged(seriesMinXValue, seriesMaxXValue, xAxis->getNumReferenceValues(), 
                                   seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                   buildingPattern);
      break;
  }
  legend->slotAddNewEntry(series->getId(), series->getText(), series->getBaseColor());
    
  series->setCurrentWidthHeightIntoScene(chartWall->getCurrentWidthIntoScene(), 
                                         chartWall->getCurrentHeightIntoScene());
  series->setPos(0,0);
  series->setParentItem(chartWall);
  emit signalAddedNewChartItem(series);
}

void NMGChartScene::initializeSeriesState(NMGChartSeries& series)
{
  switch(buildingPattern)
  {
    case OWDOT:
    case OWDOP:
      series.setAverageState(TRUE);
      series.setAccumulateState(FALSE);
      series.setClippingState(FALSE);
      series.setSeriesType(LINE_TYPE);
      break;
      
    case OWDD:
      series.setAverageState(FALSE);
      series.setAccumulateState(TRUE);
      series.setClippingState(TRUE);
      series.setSeriesType(BARS_TYPE);
      break;
      
    case IPDVOT:
    case IPDVOP:
      series.setAverageState(FALSE);
      series.setAccumulateState(FALSE);
      series.setClippingState(FALSE);
      series.setSeriesType(POINTS_TYPE);
      break;
      
    case IPDVD:
      series.setAverageState(FALSE);
      series.setAccumulateState(TRUE);
      series.setClippingState(TRUE);
      series.setSeriesType(BARS_TYPE);
      break;
      
    case BTC:
      series.setAverageState(FALSE);
      series.setAccumulateState(FALSE);
      series.setClippingState(FALSE);
      series.setSeriesType(LINE_TYPE);
      break;
  }
}

void NMGChartScene::updateSeriesZValue()
{
  QList<NMGChartSeries*>::iterator it;
  int zValue;
  for(it = seriesList.begin(), zValue = -1; 
      it != seriesList.end(); 
      it++, zValue--)
  {
    (*it)->setZValue(zValue);
  }
}

void NMGChartScene::suggestGlobalSeriesXYValues(const NMGChartSeries& series, 
                                                double& minX, double& minY, 
                                                double& maxX, double& maxY)
{
  switch(type)
  {
    case AGGREGATION:
      
      minX = 0; maxX = series.getMaxXValue()+1;
      minY = 0;
      
      if(series.getMinYValue() == series.getMaxYValue()) maxY = series.getMaxYValue()+1;
      else maxY = series.getMaxYValue();
      break;
    
    case PACKETS:    
      // corrections here will rarely occur.
      
      minX = series.getMinXValue();
      
      if(series.getMinXValue() == series.getMaxXValue()) maxX = series.getMaxXValue()+1;
      else maxX = series.getMaxXValue();
      
      minY = series.getMinYValue();
      if(series.getMinYValue() == series.getMaxYValue()) maxY = series.getMaxYValue()+1;
      else maxY = series.getMaxYValue();
      break;
      
    default: 
      minX = series.getMinXValue(); maxX = series.getMaxXValue();
      minY = series.getMinYValue(); maxY = series.getMaxYValue();
  }
}

void NMGChartScene::updateGlobalSeriesXYValues(const double& minX, const double& minY,
                                               const double& maxX, const double& maxY)
{
  /* NOTE this allows series, when updated, to be shown proportional 
   * to the rest of loaded series. */

  if(seriesList.size() <= 1)
  {
    seriesMinXValue = minX;
    seriesMinYValue = minY;
    seriesMaxXValue = maxX;
    seriesMaxYValue = maxY;
  }
  else
  {
    seriesMinXValue = qMin(seriesMinXValue, minX);
    seriesMinYValue = qMin(seriesMinYValue, minY);
    seriesMaxXValue = qMax(seriesMaxXValue, maxX);
    seriesMaxYValue = qMax(seriesMaxYValue, maxY);
  }
  QList<NMGChartSeries*>::iterator it;
  for(it = seriesList.begin(); it != seriesList.end(); it++)
  {
    (*it)->setMinValues(seriesMinXValue, seriesMinYValue);
    (*it)->setMaxValues(seriesMaxXValue, seriesMaxYValue);
  }
}

void NMGChartScene::setChartType(ChartType chartType)
{
  bool canBeSet = TRUE;
  if(seriesList.isEmpty() || chartType == buildingPattern) canBeSet = FALSE;
  else
  {
    switch (type)
    {
      case PACKETS:
      {
        if(chartType != OWDOT && chartType != OWDOP && chartType != OWDD && 
           chartType != IPDVOT && chartType != IPDVOP && chartType != IPDVD)
          canBeSet = FALSE;
        break;
      }
      case AGGREGATION:
      {
        // Aggregation type has not derivates at this moment, so this method has not effect
        if(chartType != BTC) canBeSet = FALSE;
        break;
      }
    }
  }  
  if(canBeSet) 
  {
    if(zoomed) emit signalZoomRestoringNeeded();
    ChartType previousPattern = buildingPattern;
    buildingPattern = chartType;
    if(!rebuildSceneWithNewChartType())
    {
      NMGGraphicsError e(tr("Data processing error"), 
                         tr("Some series cannot be calculated under ") + 
                         convertToString(buildingPattern, TRUE) + tr(".\n") +
                         tr("Graphical analysis will be restored automatically to ") +
                         convertToString(previousPattern, TRUE) + tr("."), 
                         NMGGraphicsError::WARNING);
      e.raiseError();
      buildingPattern = previousPattern;
      rebuildSceneWithNewChartType();
      emit signalChartTypeChanged(type, buildingPattern);
    }
  }
}

bool NMGChartScene::rebuildSceneWithNewChartType()
{
  bool error = FALSE;
  QString beforeSeparator;
  int afterSeparator = 0;
  QString lastTestName;

  QList<NMGChartSeries*>::iterator it;
  
  // rebuilding series data
  for(it = seriesList.begin(); it != seriesList.end() && !error; it++)
  {    
    // NOTE because INTERNAL_SEPARATOR appears only once in getId(), 
    // identifiers can have only one or two items
    
    QStringList identifiers = (*it)->getId().split(INTERNAL_SEPARATOR, QString::SkipEmptyParts);
    beforeSeparator = identifiers.at(0); // filePath
    if(identifiers.size() == 2) afterSeparator = identifiers.at(1).toInt(); // flow id
    
    // NOTE loaderHandler should be always valid because rebuild is only done if at least 
    // one test file has been previously added.
    try
    {
      NMGTestFileData& testData = loaderHandler->getTestFileData(beforeSeparator);
  
      QList<double> xData, yData;
      if(!rebuildSeriesVertexList(testData, xData, yData, afterSeparator)) error = TRUE;
      else
      {
        (*it)->setVertexList(xData, yData);
        lastTestName = testData.getMetadata()->getTestName();
      }
    }
    catch(NMGGraphicsError e) { e.raiseError(); }
  }
  
  if(!error)
  {
    rebuildGlobalSeriesXYValues();
        
    // updating items graphically
    for(it = seriesList.begin(); it != seriesList.end() && !error; it++) initializeSeriesState(*(*it));
  
    QString chartTitleText;
    QString chartXAxisText;
    QString chartYAxisText;
    prepareContextualElements(chartTitleText, chartXAxisText, chartYAxisText, lastTestName);
    
    titleLabel->setText(chartTitleText);
    xAxisLabel->setText(chartXAxisText);
    yAxisLabel->setText(chartYAxisText);
      
    switch(type)
    {
      case AGGREGATION:
        yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
        emit signalAxisRangesChanged(0.0, 0.0, xAxis->getNumReferenceValues(),
                                     seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                     buildingPattern);
        break;
      
      case PACKETS:
        xAxis->setAxisScale(seriesMinXValue, seriesMaxXValue);
        yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
        emit signalAxisRangesChanged(seriesMinXValue, seriesMaxXValue, xAxis->getNumReferenceValues(),
                                     seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                     buildingPattern);
        break;
    }
  }
  return !error;
}

bool NMGChartScene::rebuildSeriesVertexList(NMGTestFileData& data,
                                            QList<double>& xData, 
                                            QList<double>& yData,
                                            int id)
{
  bool error = FALSE;
  switch(type)
  {
    case PACKETS:
    {
      NMGFlowOrientedResultData* result = (NMGFlowOrientedResultData*)data.getResultData();
      QMap<int, NMGFlowData*>::const_iterator it = result->find(id);
      NMGFlowData* flow = it.value();

      switch(buildingPattern)
      {
        case OWDOT:
          copyList(xData, flow->beginTimestamp(), flow->endTimestamp());
          copyList(yData, flow->beginDelay(), flow->endDelay());
          break;
        case OWDOP:
          copyList(xData, flow->beginSequenceIdentifier(), flow->endSequenceIdentifier());
          copyList(yData, flow->beginDelay(), flow->endDelay());
          break;
        case OWDD:
        {
          QList<double> delaysList;
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          generateDistributionPattern(delaysList, xData, yData);
          break;
        }       
        case IPDVOT:
        {
          QList<double> delaysList;
          copyList(xData, flow->beginTimestamp(), flow->endTimestamp());
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          if(!generateIPDVList(xData,delaysList,yData))
          {
            error = TRUE;
            xData.clear(); yData.clear();
          }
          break;
        }   
        case IPDVOP:
        {
          QList<double> delaysList;
          copyList(xData, flow->beginSequenceIdentifier(), flow->endSequenceIdentifier());
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          if(!generateIPDVList(xData, delaysList, yData))
          {
            error = TRUE;
            xData.clear(); yData.clear();
          }
          break;
        }
        case IPDVD:
        {
          QList<double> delaysList;
          QList<double> ipdvList;
          QList<double> rubbish;
          copyList(rubbish, flow->beginTimestamp(), flow->endTimestamp());
          copyList(delaysList, flow->beginDelay(), flow->endDelay());
          if(!generateIPDVList(rubbish, delaysList, ipdvList))
          {
            error = TRUE;
            xData.clear(); yData.clear();
          }
          else
          {
            rubbish.clear();
            generateDistributionPattern(ipdvList,xData,yData);
          }
          break;
        }   
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
  return !error;
}

void NMGChartScene::rebuildGlobalSeriesXYValues()
{ 
  if(!seriesList.isEmpty())
  {
    QList<NMGChartSeries*>::iterator it;
    it = seriesList.begin();
    suggestGlobalSeriesXYValues(*(*it), 
                                seriesMinXValue, seriesMinYValue,
                                seriesMaxXValue, seriesMaxYValue);
    it++;
    while(it != seriesList.end())
    {
      double minX, minY, maxX, maxY;
      suggestGlobalSeriesXYValues(*(*it), minX, minY, maxX, maxY);
      
      seriesMinXValue = qMin(seriesMinXValue, minX);
      seriesMinYValue = qMin(seriesMinYValue, minY);
      seriesMaxXValue = qMax(seriesMaxXValue, maxX);
      seriesMaxYValue = qMax(seriesMaxYValue, maxY);
      it++;
    }
    
    for(it = seriesList.begin(); it != seriesList.end(); it++)
    {
      (*it)->setMinValues(seriesMinXValue, seriesMinYValue);
      (*it)->setMaxValues(seriesMaxXValue, seriesMaxYValue);
    }
  }  
}

void NMGChartScene::setItemsVisible(bool visible)
{
  chartWall->setVisible(visible);
  // series and legend are implicity set to visible because they are chartWall children

  xAxis->setVisible(visible);
  yAxis->setVisible(visible);
  
  titleLabel->setVisible(visible);
  xAxisLabel->setVisible(visible);
  yAxisLabel->setVisible(visible);
  
  areItemsVisible = visible;
}

// ********* //
// * SLOTS * //
// ********* //

void NMGChartScene::slotResizeScene(const int& width, const int& height)
{
  if(getMinimumNeededWidth() < width) currentSceneWidth = width;
  else currentSceneWidth = getMinimumNeededWidth();
  if(getMinimumNeededHeight() < height) currentSceneHeight = height;
  else currentSceneHeight = getMinimumNeededHeight();
  
  setSceneRect(0, 0, currentSceneWidth, currentSceneHeight);
  if(!items().isEmpty())
  {
    setSceneSpaceForChartItems();
    setScenePositionForChartItems();
  }
}

void NMGChartScene::slotGiveMoreSpaceToItems()
{
  slotResizeScene(currentSceneWidth, currentSceneHeight);
}

void NMGChartScene::setSceneSpaceForChartItems()
{
  // TITLE LABEL
  titleLabel->setCurrentWidthHeightIntoScene(currentSceneWidth, 
                                             titleLabel->getSuggestedMinimumHeightIntoScene()); 
  
  // X AXIS LABEL
  xAxisLabel->setCurrentWidthHeightIntoScene(currentSceneWidth -
                                             yAxisLabel->getSuggestedMinimumWidthIntoScene() -
                                             yAxis->getSuggestedMinimumWidthIntoScene() -
                                             root->getCurrentPaddingMargin(), 
                                             xAxisLabel->getSuggestedMinimumHeightIntoScene());
  
  // Y AXIS LABEL
  yAxisLabel->setCurrentWidthHeightIntoScene(yAxisLabel->getSuggestedMinimumWidthIntoScene(),
                                             currentSceneHeight -
                                             titleLabel->getSuggestedMinimumHeightIntoScene() - 
                                             xAxisLabel->getSuggestedMinimumHeightIntoScene() - 
                                             xAxis->getSuggestedMinimumHeightIntoScene());
  
  // X AXIS
  xAxis->setCurrentWidthHeightIntoScene(currentSceneWidth -
                                        yAxisLabel->getSuggestedMinimumWidthIntoScene() -
                                        yAxis->getSuggestedMinimumWidthIntoScene() -
                                        root->getCurrentPaddingMargin(), 
                                        xAxis->getSuggestedMinimumHeightIntoScene());

  // Y AXIS
  yAxis->setCurrentWidthHeightIntoScene(yAxis->getSuggestedMinimumWidthIntoScene(),
                                        currentSceneHeight -
                                        titleLabel->getSuggestedMinimumHeightIntoScene() - 
                                        xAxisLabel->getSuggestedMinimumHeightIntoScene() - 
                                        xAxis->getSuggestedMinimumHeightIntoScene());
  
  // CHART WALL
  chartWall->setCurrentWidthHeightIntoScene(currentSceneWidth - 
                                            yAxisLabel->getCurrentWidthIntoScene() -
                                            yAxis->getCurrentWidthIntoScene() - 
                                            root->getCurrentPaddingMargin(),
                                            currentSceneHeight -
                                            titleLabel->getCurrentHeightIntoScene() - 
                                            xAxisLabel->getCurrentHeightIntoScene() - 
                                            xAxis->getCurrentHeightIntoScene());
  
  // LEGEND
  legend->setCurrentWidthHeightIntoScene(chartWall->getCurrentWidthIntoScene(), 
                                         chartWall->getCurrentHeightIntoScene());
  
  // SERIES
  QList<NMGChartSeries*>::iterator it;
  for(it = seriesList.begin(); it != seriesList.end(); it++)
  {
    (*it)->setCurrentWidthHeightIntoScene(chartWall->getCurrentWidthIntoScene(), 
                                          chartWall->getCurrentHeightIntoScene());
  }
}

void NMGChartScene::setScenePositionForChartItems()
{
  int x, y;
  // TITLE LABEL
  x = 0;
  y = 0;
  titleLabel->setPos(x,y);
  
  // X AXIS LABEL
  x = yAxisLabel->getCurrentWidthIntoScene() + yAxis->getCurrentWidthIntoScene();
  y = currentSceneHeight - xAxisLabel->getCurrentHeightIntoScene();
  xAxisLabel->setPos(x,y);
  
  // Y AXIS LABEL
  x = 0;
  y = titleLabel->getCurrentHeightIntoScene();
  yAxisLabel->setPos(x,y);
  
  // X AXIS
  x = yAxisLabel->getCurrentWidthIntoScene() + yAxis->getCurrentWidthIntoScene();
  y = currentSceneHeight - xAxisLabel->getCurrentHeightIntoScene() - 
      xAxis->getCurrentHeightIntoScene();
  xAxis->setPos(x,y);
  
  // Y AXIS
  x = yAxisLabel->getCurrentWidthIntoScene();
  y = titleLabel->getCurrentHeightIntoScene();
  yAxis->setPos(x,y);

  // CHART WALL
  x = yAxisLabel->getCurrentWidthIntoScene() + yAxis->getCurrentWidthIntoScene();
  y = titleLabel->getCurrentHeightIntoScene();
  chartWall->setPos(x,y);
  
  // LEGEND
  x = y = 0; // position is in terms of parent, that it is chart wall
  legend->setPos(x,y);
  
  // SERIES
  x = y = 0; // position is in terms of parent, that it is chart wall
  
  QList<NMGChartSeries*>::iterator it;
  for(it = seriesList.begin(); it != seriesList.end(); it++)
  {
    (*it)->setPos(x,y);
  }
}

int NMGChartScene::getMinimumNeededWidth() const
{
  int min = qMax(xAxisLabel->getSuggestedMinimumWidthIntoScene(),
                 xAxis->getSuggestedMinimumWidthIntoScene());
  min = qMax(min, legend->getSuggestedMinimumWidthIntoScene());
  min += yAxisLabel->getSuggestedMinimumWidthIntoScene();
  min += yAxis->getSuggestedMinimumWidthIntoScene();
  min += root->getCurrentPaddingMargin();
  min = qMax(min, titleLabel->getSuggestedMinimumWidthIntoScene());
  return min;
}

int NMGChartScene::getMinimumNeededHeight() const
{
  int min = qMax(yAxisLabel->getSuggestedMinimumHeightIntoScene(),
                 yAxis->getSuggestedMinimumHeightIntoScene());
  min = qMax(min, legend->getSuggestedMinimumHeightIntoScene());
  min += titleLabel->getSuggestedMinimumHeightIntoScene();
  min += xAxisLabel->getSuggestedMinimumHeightIntoScene();
  min += xAxis->getSuggestedMinimumHeightIntoScene();
  return min;
}

void NMGChartScene::slotDeleteSeries(const QString& id)
{
  if(seriesOrder.contains(id))
  { // id is a valid series identifier
    if(zoomed) emit signalZoomRestoringNeeded();
    // we must notify at the beginning, before delete series data (it could be necessary 
    // for others to do the deletion operations.
    emit signalSeriesDeleted(id);
    
    // firstly structures are deleted
    int idx = seriesOrder.indexOf(id);
    seriesOrder.removeAt(idx);
    seriesList.removeAt(idx);
        
    foreach (QGraphicsItem *child, chartWall->QGraphicsItem::children()) 
    {
      NMGChartSeries* series = dynamic_cast<NMGChartSeries*>(child);
      if(series)
      {
        if(series->getId() == id) chartWall->deleteChildSeries(child);
      }
    }
    
    // secondly values are updated
    updateSeriesZValue();
    rebuildSeriesDueToReorder();
    QList<NMGChartSeries*>::iterator it;
    for(it = seriesList.begin(); it != seriesList.end(); it++)
    {
      (*it)->setMinValues((*it)->getOriginalMinXValue(), (*it)->getOriginalMinYValue());
      (*it)->setMaxValues((*it)->getOriginalMaxXValue(), (*it)->getOriginalMaxYValue());
    }
    rebuildGlobalSeriesXYValues();
    legend->slotDeleteEntry(id);
    switch(type)
    {
      case AGGREGATION:
        xAxis->removeLabel(id);
        yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
        emit signalAxisRangesChanged(0.0, 0.0, xAxis->getNumReferenceValues(),
                                     seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                     buildingPattern);
        break;
      
      case PACKETS:
        xAxis->setAxisScale(seriesMinXValue, seriesMaxXValue);
        yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
        emit signalAxisRangesChanged(seriesMinXValue, seriesMaxXValue, xAxis->getNumReferenceValues(), 
                                     seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                     buildingPattern);
        break;
    }
    
    // thirdly series are repainted if any
    if(seriesList.isEmpty())
    {
      areItemsVisible = FALSE;
      setItemsVisible(areItemsVisible);
      emit signalEmptyScene();
    }
    else
    {
      for(it = seriesList.begin(); it != seriesList.end(); it++) (*it)->update();
    }
  }
}

void NMGChartScene::slotOrderSeries(const QString& seriesId, int newOrder)
{
  int oldOrder = seriesOrder.indexOf(seriesId);
  if(oldOrder != -1 && (newOrder >= 0 && newOrder < seriesList.size()))
  {  
    if(zoomed) emit signalZoomRestoringNeeded();
    // firstly we make the reorder for all necessary chart items in scene
    seriesList.move(oldOrder, newOrder);
    seriesOrder.move(oldOrder, newOrder);
    updateSeriesZValue();
    legend->slotChangeEntryOrder(seriesId, newOrder);
    xAxis->slotChangeLabelOrder(seriesId, newOrder);
    
    // secondly we rebuild series data if it is necessary due to the reorder
    rebuildSeriesDueToReorder();
    rebuildGlobalSeriesXYValues();

    // thirdly we repaint all necessary chart items to show the reorder effect
    QList<NMGChartSeries*>::iterator it;
    for(it = seriesList.begin(); it != seriesList.end(); it++) (*it)->update();
  }
}

void NMGChartScene::rebuildSeriesDueToReorder()
{
  switch(type)
  {
    case AGGREGATION:
      switch(buildingPattern)
      {
        case BTC:
        {
          // NOTE loaderHandler should be always valid because reorder is only done if at least 
          // one test file has been previously added. Furthermore, because of type, seriesId 
          // only contains the file path (see prepareSeriesData method for detailed information)
          try
          {
            for(int i = 0; i < seriesOrder.size(); i++)
            {
              NMGTestFileData& data = loaderHandler->getTestFileData(seriesOrder.at(i));
              NMGAggregationOrientedResultData* result;
              result = (NMGAggregationOrientedResultData*)data.getResultData();
      
              QList<double> xData, yData;
              xData << i+1; // xData is forced to reuse chart series paint method
              yData << result->getReceivingThroughputInKBitsPerSecond();
              seriesList.at(i)->setVertexList(xData, yData);
            }
          }
          catch(NMGGraphicsError e) { e.raiseError(); }
          break;
        }
        default:
          break;
      }
      break;
    case PACKETS:
        // nothing to do
      break;
  }
}

void NMGChartScene::slotZoom(const double& minX, const double& maxX, 
                             const double& minY, const double& maxY)
{
  QList<NMGChartSeries*>::iterator it;
  
  // saving initial values the first time a zoom operation is done
  if(!zoomed)
  {
    zoomed = TRUE;
    initialMinXValue = seriesMinXValue;
    initialMaxXValue = seriesMaxXValue;
    initialMinYValue = seriesMinYValue;
    initialMaxYValue = seriesMaxYValue;
  }
  // updating clipping state
  for(it = seriesList.begin(); it != seriesList.end(); it++) (*it)->setClippingState(TRUE);
  
  // changing series and axis according to the new zoom values 
  switch(type)
  {
    case AGGREGATION:
      seriesMinYValue = minY;
      seriesMaxYValue = maxY;
      for(it = seriesList.begin(); it != seriesList.end(); it++)
      {
        (*it)->setMinValues(initialMinXValue, seriesMinYValue);
        (*it)->setMaxValues(initialMaxXValue, seriesMaxYValue);
        (*it)->update();
      }
      yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
      emit signalAxisRangesChanged(0.0, 0.0, xAxis->getNumReferenceValues(),
                                   seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                   buildingPattern);
      break;
      
    case PACKETS:
      seriesMinXValue = minX;
      seriesMaxXValue = maxX;
      seriesMinYValue = minY;
      seriesMaxYValue = maxY;
      for(it = seriesList.begin(); it != seriesList.end(); it++)
      {
        (*it)->setMinValues(seriesMinXValue, seriesMinYValue);
        (*it)->setMaxValues(seriesMaxXValue, seriesMaxYValue);
        (*it)->update();
      }
      xAxis->setAxisScale(seriesMinXValue, seriesMaxXValue);
      yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
      emit signalAxisRangesChanged(seriesMinXValue, seriesMaxXValue, xAxis->getNumReferenceValues(), 
                                   seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                   buildingPattern);
      break;
  }
}

void NMGChartScene::slotRestoreZoom()
{
  QList<NMGChartSeries*>::iterator it;
  
  if(zoomed)
  {
    zoomed = FALSE;
    seriesMinXValue = initialMinXValue;
    seriesMaxXValue = initialMaxXValue;
    seriesMinYValue = initialMinYValue;
    seriesMaxYValue = initialMaxYValue;

    // changing series and axis according to the initial zoom values 
    switch(type)
    {
      case AGGREGATION:
        for(it = seriesList.begin(); it != seriesList.end(); it++)
        {
          (*it)->setMinValues(initialMinXValue, seriesMinYValue);
          (*it)->setMaxValues(initialMaxXValue, seriesMaxYValue);
          (*it)->update();
        }
        yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
        emit signalAxisRangesChanged(0.0, 0.0, xAxis->getNumReferenceValues(),
                                     seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                     buildingPattern);
        break;
        
      case PACKETS:
        for(it = seriesList.begin(); it != seriesList.end(); it++)
        {
          (*it)->setMinValues(seriesMinXValue, seriesMinYValue);
          (*it)->setMaxValues(seriesMaxXValue, seriesMaxYValue);
          (*it)->update();
        }
        xAxis->setAxisScale(seriesMinXValue, seriesMaxXValue);
        yAxis->setAxisScale(seriesMinYValue, seriesMaxYValue);
        emit signalAxisRangesChanged(seriesMinXValue, seriesMaxXValue, xAxis->getNumReferenceValues(), 
                                     seriesMinYValue, seriesMaxYValue, yAxis->getNumReferenceValues(),
                                     buildingPattern);
        break;
    }
  }
  
  // updating clipping state
  switch(buildingPattern)
  {
    case OWDD:
    case IPDVD:
      for(it = seriesList.begin(); it != seriesList.end(); it++) (*it)->setClippingState(TRUE);
      break;
    default:
      for(it = seriesList.begin(); it != seriesList.end(); it++) (*it)->setClippingState(FALSE);
      break;
  }
}

#include "nmgchartscene.moc"
