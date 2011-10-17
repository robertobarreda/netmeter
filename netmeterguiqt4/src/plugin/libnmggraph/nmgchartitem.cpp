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
#include "nmgchartitem.h"
   
// **************** //
// * NMGChartItem * //
// **************** //
NMGChartItem::NMGChartItem(QGraphicsItem* parent)
  : QGraphicsItem(parent)
{
  currentWidthIntoScene = 0;
  currentHeightIntoScene = 0;
  currentPaddingMargin = 0;
  spaceAvailableIntoSceneChanged = FALSE;
  
  neededMinimumHeightIntoScene = 0;
  neededMinimumWidthIntoScene = 0;
  spaceNeededIntoSceneChanged = FALSE;
}

void NMGChartItem::setCurrentWidthIntoScene(const int& width)
{ 
  if (currentWidthIntoScene != width) 
  {
    prepareGeometryChange();
    currentWidthIntoScene = width;
    spaceAvailableIntoSceneChanged = TRUE;
  }
}

void NMGChartItem::setCurrentHeightIntoScene(const int& height)
{
  if (currentHeightIntoScene != height) 
  {
    prepareGeometryChange();
    currentHeightIntoScene = height;
    spaceAvailableIntoSceneChanged = TRUE;
  }
}

void NMGChartItem::setCurrentWidthHeightIntoScene(const int& width, const int& height)
{
  if (currentWidthIntoScene != width || currentHeightIntoScene != height) 
  {
    prepareGeometryChange();
    currentWidthIntoScene = width;
    currentHeightIntoScene = height;
    spaceAvailableIntoSceneChanged = TRUE;
  }
}

bool NMGChartItem::hasEnoughSpace()
{
  if(spaceNeededIntoSceneChanged)
  {
    neededMinimumWidthIntoScene = getSuggestedMinimumWidthIntoScene();
    neededMinimumHeightIntoScene = getSuggestedMinimumHeightIntoScene();
    spaceNeededIntoSceneChanged = FALSE;
  }
  return (neededMinimumHeightIntoScene <= currentHeightIntoScene &&
          neededMinimumWidthIntoScene <= currentWidthIntoScene);
}

// **************** //
// * NMGChartRoot * //
// **************** //

NMGChartRoot::NMGChartRoot(QGraphicsItem* parent)
  : NMGChartItem(parent)
{
  currentPaddingMargin = 15;
}

QRectF NMGChartRoot::boundingRect() const 
{ 
  return QRectF(); 
}

void NMGChartRoot::paint(QPainter* painter, 
                         const QStyleOptionGraphicsItem* option, 
                         QWidget* widget) 
{
  Q_UNUSED(painter);
  Q_UNUSED(option);
  Q_UNUSED(widget);
}

// ***************** //
// * NMGChartLabel * //
// ***************** //

NMGChartLabel::NMGChartLabel(NMGChartLabel::LabelType type, QGraphicsItem* parent)
  : NMGChartItem(parent), type(type)
{
  currentPaddingMargin = 5;

  currentFont = QFont(); // Initializated with the application's default font.
  currentFont.setPointSize(12);
  currentFont.setStyleHint(QFont::SansSerif);
  if(type == NMGChartLabel::TITLE) 
    currentText = QString::fromLocal8Bit("Title label text");
  else if(type == NMGChartLabel::AXIS_X) 
    currentText = QString::fromLocal8Bit("X-axis label text");
  else if(type == NMGChartLabel::AXIS_Y) 
    currentText = QString::fromLocal8Bit("Y-axis label text");
  baseColor = Qt::black;
}

QRectF NMGChartLabel::boundingRect() const
{
  if(currentWidthIntoScene <= 0 || currentHeightIntoScene <= 0) return QRectF();
  else return QRectF(0, 0, currentWidthIntoScene, currentHeightIntoScene);
}

void NMGChartLabel::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option, 
                          QWidget* widget)
{
  if(hasEnoughSpace())
  {
    painter->setFont(currentFont);
    QPen pen;
    pen.setColor(QColor(baseColor));
    pen.setWidth(1);
    painter->setPen(pen);
    
    QFontMetrics fm(currentFont);
    QRect boundingForText = fm.boundingRect(currentText);
    
    if(type == NMGChartLabel::AXIS_Y) 
    {
      painter->save();
    
      painter->translate(0,currentHeightIntoScene);
      painter->rotate(-90);
      painter->drawText((currentHeightIntoScene - boundingForText.width())/2, 
                        (currentWidthIntoScene - boundingForText.height())/2 + fm.ascent(), 
                        currentText);
      painter->restore();
    }
    else
    {
      painter->drawText((currentWidthIntoScene - boundingForText.width())/2, 
                        (currentHeightIntoScene - boundingForText.height())/2 + fm.ascent(), 
                        currentText);
    }
  }
  else emit signalGiveMeMoreSpace();
}

int NMGChartLabel::getSuggestedMinimumWidthIntoScene() const
{
  int minWidth;
  QFontMetrics fm(currentFont);
  if(type == NMGChartLabel::AXIS_Y)
    minWidth = currentPaddingMargin + fm.height() + currentPaddingMargin;
  else
    minWidth = currentPaddingMargin + fm.boundingRect(currentText).width() + currentPaddingMargin;
  return minWidth; 
}

int NMGChartLabel::getSuggestedMinimumHeightIntoScene() const
{
  int minHeight;
  QFontMetrics fm(currentFont);
  if(type == NMGChartLabel::AXIS_Y)
    minHeight = currentPaddingMargin + fm.boundingRect(currentText).width() + currentPaddingMargin;
  else
    minHeight = currentPaddingMargin + fm.height() + currentPaddingMargin;
  return minHeight;
}

void NMGChartLabel::setText(QString text)
{ 
  currentText = text;
  spaceNeededIntoSceneChanged = TRUE;
  update();
}

void NMGChartLabel::setBaseColor(QColor color)
{ 
  if(baseColor != color)
  {
    baseColor = color;
    update(); //spaceNeededIntoSceneChanged isn't used because there're not size changes
  }
}

void NMGChartLabel::setFontFamily(QString family)
{
  if(currentFont.family() != family)
  {
    currentFont.setFamily(family);
    spaceNeededIntoSceneChanged = TRUE;
    update();
  }
}

void NMGChartLabel::setFontSize(int size)
{
  if(size != currentFont.pointSize())
  {
    currentFont.setPointSize(size);
    spaceNeededIntoSceneChanged = TRUE;
    update();    
  }
}

// **************** //
// * NMGChartWall * //
// **************** //

NMGChartWall::NMGChartWall(QGraphicsItem* parent)
  : NMGChartItem(parent), 
    lastSeriesColor(0), 
    numXReferenceLines(11), 
    numYReferenceLines(6),
    areXLinesVisible(TRUE),
    areYLinesVisible(TRUE)
{}

QRectF NMGChartWall::boundingRect() const
{
  if(currentWidthIntoScene <= 0 || currentHeightIntoScene <= 0) return QRectF();
  else return QRectF(0, 0, currentWidthIntoScene, currentHeightIntoScene);
}

void NMGChartWall::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option, 
                         QWidget* widget)
{
  QPen pen;
  pen.setColor(Qt::lightGray);
  pen.setWidth(1);
  painter->setPen(pen);
  
  // lines in vertical direction
  if(areXLinesVisible)
  {
    double xInterval = (double)currentWidthIntoScene/(double)(numXReferenceLines-1);
    for(double i = 1 ; i<numXReferenceLines; i++)
    {
      painter->drawLine(QLineF(i*xInterval, 0, i*xInterval, currentHeightIntoScene));
    }
  }
  
  // lines in horitzonal direction
  if(areYLinesVisible)
  {
    double yInterval = (double)currentHeightIntoScene/(double)(numYReferenceLines-1);
    for(double j = 0 ; j<numYReferenceLines-1; j++)
    {
      painter->drawLine(QLineF(0, j*yInterval, currentWidthIntoScene, j*yInterval));
    }
  }
}

QColor NMGChartWall::getNextSeriesBaseColor()
{
  QColor color;
  
  switch((lastSeriesColor++)%12)
  {
    case 0:
      color = QColor(255,0,0);
      break;
    case 1:
      color = QColor(0,255,0);
      break;
    case 2:
      color = QColor(0,0,255);
      break;
    case 3:
      color = QColor(255,255,0);
      break;
    case 4:
      color = QColor(255,0,255);
      break;
    case 5:
      color = QColor(0,255,255);
      break;
    case 6:
      color = QColor(255,127,0);
      break;
    case 7:
      color = QColor(127,255,0);
      break;
    case 8:
      color = QColor(127,0,255);
      break;
    case 9:
      color = QColor(255,0,127);
      break;
    case 10:
      color = QColor(0,255,127);
      break;
    case 11:
      color = QColor(0,127,255);
      break;
  }
  return color;
}

void NMGChartWall::setNumReferenceLines(int xAxis, int yAxis)
{
  bool changed = FALSE; // used to avoid unnecessary updates
  if(xAxis!=numXReferenceLines && xAxis >= 2) 
  {
    numXReferenceLines = xAxis;
    changed = TRUE;
  }
  if(yAxis!=numYReferenceLines && yAxis >= 2)
  {
    numYReferenceLines = yAxis;
    changed = TRUE;
  }
  if(changed) update(); //spaceNeededIntoSceneChanged isn't used because there're not size changes
}

void NMGChartWall::setXReferenceLinesVisibility(bool areVisible)
{
  if(areXLinesVisible != areVisible)
  {
    areXLinesVisible = areVisible;
    update();
  }
}

void NMGChartWall::setYReferenceLinesVisibility(bool areVisible)
{
  if(areYLinesVisible != areVisible)
  {
    areYLinesVisible = areVisible;
    update();
  }
}

void NMGChartWall::deleteChildSeries(QGraphicsItem* child)
{
  QVariant variant;
  qVariantSetValue<QGraphicsItem*>(variant, child);
  QGraphicsItem::itemChange(ItemChildRemovedChange, variant);
  delete child;
}

// ****************** //
// * NMGChartSeries * //
// ****************** //
NMGChartSeries::NMGChartSeries(QString id, QGraphicsItem* parent)
  : NMGChartItem(parent)
{
  currentText = "Default series name";
  this->id = id;
  minValues.x = origMinValues.x = 0.0;
  minValues.y = origMinValues.y = 0.0;
  maxValues.x = origMaxValues.x = 0.0;
  maxValues.y = origMaxValues.y = 0.0;
  baseColor = Qt::black;
  isAveraged = FALSE;
  isAccumulated = FALSE;
  isClipped = FALSE;
  representation = LINE_TYPE;
  pointSize = 2;
  verticalScaleFactor = 1.0;
  horizontalScaleFactor = 1.0;
  rebuildScaleFactors = FALSE;
}

QRectF NMGChartSeries::boundingRect() const
{
  if(currentWidthIntoScene <= 0 || currentHeightIntoScene <= 0) return QRectF();
  else return QRectF(0, 0, currentWidthIntoScene, currentHeightIntoScene);
}

void NMGChartSeries::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option, 
                           QWidget* widget)
{
  if(!vertexList.isEmpty())
  { 
    if(spaceAvailableIntoSceneChanged || rebuildScaleFactors)
    {
      calculateScaleFactors();
      if(spaceAvailableIntoSceneChanged) spaceAvailableIntoSceneChanged = FALSE;
      if(rebuildScaleFactors) rebuildScaleFactors = FALSE;
    }
    
    QPen pen;
    pen.setColor(baseColor);
    pen.setWidth(1);
    painter->setPen(pen);
  
    if(isClipped)
    {
      painter->setClipRect(0, 0, currentWidthIntoScene + pen.width(), currentHeightIntoScene);
    }
    painter->save();
    painter->translate(0,currentHeightIntoScene);
        
    if(representation == LINE_TYPE) paintLine(painter, option, widget);
    else if(representation == AREA_TYPE) paintArea(painter, option, widget);
    else if(representation == POINTS_TYPE) paintPoints(painter, option, widget);
    else if(representation == BARS_TYPE) paintBars(painter, option, widget);
    
    painter->restore();
  }
}

void NMGChartSeries::paintLine(QPainter* painter,
                               const QStyleOptionGraphicsItem* option, 
                               QWidget* widget)
{
  QPainterPath path;
  double closingYpixel = ((minValues.y < 0.0 && 0.0 < maxValues.y) ? yWindowToViewport(0.0) :
                         ((minValues.y >= 0.0) ? yWindowToViewport(minValues.y) : 
                         yWindowToViewport(maxValues.y)));

  QList<Vertex>::const_iterator it = vertexList.constBegin();
  path.moveTo(xWindowToViewport(it->x), -closingYpixel);

  if(isAveraged || isAccumulated)
  {
    int currentXPixel;
    do
    {
      currentXPixel = (int)xWindowToViewport(it->x);
      double accumX = 0.0;
      double accumY = 0.0;
      int numElemsAvrg = 0;
      while(it != vertexList.constEnd() && (int)xWindowToViewport(it->x) == currentXPixel)
      {
        accumX += it->x;
        accumY += it->y;
        numElemsAvrg++;
        it++;
      }
      if(numElemsAvrg>0)
      {
        double newX = accumX/numElemsAvrg;
        double newY = accumY/numElemsAvrg;
        if(isAveraged)
        {
          newX = accumX/numElemsAvrg;
          newY = accumY/numElemsAvrg;
        }
        else
        { // isAccumulated
          newX = accumX/numElemsAvrg;
          newY = accumY;
        }
        path.lineTo(xWindowToViewport(newX),-yWindowToViewport(newY));
      }
    }
    while(it != vertexList.constEnd());    
  }
  else
  { // not average nor accumulated
    do
    {
      path.lineTo(xWindowToViewport(it->x),-yWindowToViewport(it->y));
      it++;
    }
    while(it != vertexList.constEnd());
  }
  path.lineTo(xWindowToViewport(vertexList.last().x), -closingYpixel);
  path.closeSubpath();
  painter->drawPath(path);
}

void NMGChartSeries::paintArea(QPainter* painter,
                               const QStyleOptionGraphicsItem* option, 
                               QWidget* widget)
{
  int maxValuesInPartialPath = 1000;
  QColor hsv = baseColor.toHsv();
  hsv.setHsv(hsv.hue(), (int)(hsv.saturation()*0.25), hsv.value());

  if(isAveraged || isAccumulated)
  {
    painter->setBrush(hsv);
    paintLine(painter, option, widget); 
  }
  else
  { // not average nor accumulated
        
    /* NOTE A specific implementation is needed in this case because Graphics View doesn't
    * support correctly huge amount of points (X11 crashes). The design has been based in 
    * partial paths that are painted with brush (to paint the area), and later they are 
    * added to the complete path (to paint the boundary line).*/
    
    painter->setPen(Qt::NoPen);
    painter->setBrush(hsv);

    QPainterPath completedPath;
    double closingYpixel = ((minValues.y < 0.0 && 0.0 < maxValues.y) ? yWindowToViewport(0.0) :
                           ((minValues.y >= 0.0) ? yWindowToViewport(minValues.y) : 
                           yWindowToViewport(maxValues.y)));

    QList<Vertex>::const_iterator it = vertexList.constBegin();
    do
    {
      QPainterPath partialPath;
      QPainterPath partialPathModif;
      partialPath.moveTo(xWindowToViewport(it->x), -yWindowToViewport(it->y));
      
      for(int i = 1; i <= maxValuesInPartialPath && it != vertexList.constEnd(); i++, it++)
      {
        partialPath.lineTo(xWindowToViewport(it->x),-yWindowToViewport(it->y));
      }
      if(completedPath.isEmpty()) 
      {
        partialPathModif = partialPath;
        partialPathModif.lineTo(partialPathModif.currentPosition().rx(), -closingYpixel);
        partialPathModif.lineTo(xWindowToViewport(vertexList.first().x), -closingYpixel);
        
        completedPath = partialPath;
      }
      else
      {
        partialPathModif = partialPath;
        
        partialPathModif.lineTo(partialPath.currentPosition().rx(), -closingYpixel);
        partialPathModif.lineTo(completedPath.currentPosition().rx(), -closingYpixel);
        partialPathModif.lineTo(completedPath.currentPosition().rx(), 
                                completedPath.currentPosition().ry());
        
        completedPath.connectPath(partialPath);
      }
      partialPathModif.closeSubpath();
      painter->drawPath(partialPathModif);
    }
    while(it != vertexList.constEnd());
    
    painter->setPen(Qt::SolidLine);
    painter->setPen(baseColor);
    painter->setBrush(Qt::NoBrush);
    
    completedPath.lineTo(xWindowToViewport(vertexList.last().x), -closingYpixel);
    completedPath.lineTo(xWindowToViewport(vertexList.first().x), -closingYpixel);
    completedPath.closeSubpath();
    painter->drawPath(completedPath);
  }
}

void NMGChartSeries::paintPoints(QPainter* painter,
                                 const QStyleOptionGraphicsItem* option, 
                                 QWidget* widget)
{
  QPen pen = painter->pen();
  pen.setWidth(pointSize);
  painter->setPen(pen);
  QList<Vertex>::const_iterator it = vertexList.constBegin();
  
  if(isAveraged || isAccumulated)
  {
    int currentXPixel;
    do
    {
      currentXPixel = (int)xWindowToViewport(it->x);
      double accumX = 0.0;
      double accumY = 0.0;
      int numElemsAvrg = 0;
      while(it != vertexList.constEnd() && (int)xWindowToViewport(it->x) == currentXPixel)
      {
        accumX += it->x;
        accumY += it->y;
        numElemsAvrg++;
        it++;
      }
      if(numElemsAvrg>0)
      {
        double newX;
        double newY;
        if(isAveraged)
        {
          newX = accumX/numElemsAvrg;
          newY = accumY/numElemsAvrg;
        }
        else
        { // isAccumulated
          newX = accumX/numElemsAvrg;
          newY = accumY;
        }
        painter->drawPoint((int)xWindowToViewport(newX), -(int)yWindowToViewport(newY));
      }
    }
    while(it != vertexList.constEnd());    
  }
  else
  { // not average nor accumulated
    do
    {
      painter->drawPoint((int)xWindowToViewport(it->x), -(int)yWindowToViewport(it->y));
      it++;
    }
    while(it != vertexList.constEnd());
  }
}

void NMGChartSeries::paintBars(QPainter* painter, 
                               const QStyleOptionGraphicsItem* option, 
                               QWidget* widget)
{
  QPen pen = painter->pen();
  pen.setWidth(1);
  painter->setPen(pen);
  
  double closingYpixel = ((minValues.y < 0.0 && 0.0 < maxValues.y) ? yWindowToViewport(0.0) :
                         ((minValues.y >= 0.0) ? yWindowToViewport(minValues.y) : 
                         yWindowToViewport(maxValues.y)));
  QList<Vertex>::const_iterator it = vertexList.constBegin();
  
  if(isAveraged || isAccumulated)
  {
    int currentXPixel;
    do
    {
      currentXPixel = (int)xWindowToViewport(it->x);
      double accumX = 0.0;
      double accumY = 0.0;
      int numElemsAvrg = 0;
      while(it != vertexList.constEnd() && (int)xWindowToViewport(it->x) == currentXPixel)
      {
        accumX += it->x;
        accumY += it->y;
        numElemsAvrg++;
        it++;
      }
      if(numElemsAvrg>0)
      {
        double newX;
        double newY;
        if(isAveraged)
        {
          newX = xWindowToViewport(accumX/numElemsAvrg);
          newY = yWindowToViewport(accumY/numElemsAvrg);
        }
        else
        { // isAccumulated
          newX = xWindowToViewport(accumX/numElemsAvrg);
          newY = yWindowToViewport(accumY);
        }
        painter->drawLine((int)newX, -(int)closingYpixel, (int)newX, -(int)newY);
      }
    }
    while(it != vertexList.constEnd());    
  }
  else
  { // not average nor accumulated
    do
    {
      int x = (int)xWindowToViewport(it->x);
      painter->drawLine(x, -(int)closingYpixel, x, -(int)yWindowToViewport(it->y));
      it++;
    }
    while(it != vertexList.constEnd());
  }
}

void NMGChartSeries::setVisible(bool visible)
{
  if(isVisible() != visible)
  {
    emit signalVisibilityChanged(id, visible);
    QGraphicsItem::setVisible(visible);
  }
}

void NMGChartSeries::setText(QString text)
{ 
  if(currentText != text)
  {
    currentText = text;
    // update is not necessary because currentText is not a visible element but must be notified
    // to other potencial visible elements
    emit signalTextChanged(id, currentText);
  }
}

void NMGChartSeries::setBaseColor(QColor color)
{ 
  if(baseColor != color)
  {
    baseColor = color;
    update(); //spaceNeededIntoSceneChanged isn't used because there're not size changes
    emit signalColorChanged(id, color);
  }
}

void NMGChartSeries::setAverageState(bool averaged)
{
  if(isAveraged != averaged)
  {
    isAveraged = averaged;
    if(isAveraged && isAccumulated) isAccumulated = FALSE;
    update();
  }
}

void NMGChartSeries::setAccumulateState(bool accumulated)
{
  if(isAccumulated != accumulated)
  {
    isAccumulated = accumulated;
    if(isAccumulated && isAveraged) isAveraged = FALSE;
    update();
  }
}

void NMGChartSeries::setClippingState(bool clipped)
{
  if(isClipped != clipped)
  {
    isClipped = clipped;
    update();
  }
}

void NMGChartSeries::setSeriesType(QString type)
{
  if(type == LINE_TYPE || type == AREA_TYPE || type == POINTS_TYPE || type == BARS_TYPE)
  {
    representation = type.toUpper();
    update();
  }
}

void NMGChartSeries::setPointSize(int size)
{
  if(pointSize != size && size>0)
  {
    pointSize = size;
    if(representation == POINTS_TYPE) update();
  }
}

// NOTE: for perfomance issues never do update in next method
void NMGChartSeries::setVertexList(const QList<double>& xData, const QList<double>& yData)
{	
  if (!xData.isEmpty() && !yData.isEmpty() && xData.size()==yData.size())
  {
    if (!vertexList.isEmpty()) vertexList.clear(); // remove previous list
  
    QList<double>::const_iterator itx = xData.constBegin();
    QList<double>::const_iterator ity = yData.constBegin();
    
    minValues.x = origMinValues.x = *itx;
    maxValues.x = origMaxValues.x = *itx;
    minValues.y = origMinValues.y = *ity;
    maxValues.y = origMaxValues.y = *ity;
    do
    {
      vertexList.append(Vertex(*itx,*ity));
                  
      minValues.x = origMinValues.x = qMin(minValues.x,*itx);
      minValues.y = origMinValues.y = qMin(minValues.y,*ity);
      maxValues.x = origMaxValues.x = qMax(maxValues.x,*itx);
      maxValues.y = origMaxValues.y = qMax(maxValues.y,*ity);
      
      itx++;
      ity++;
    }
    while(itx != xData.constEnd() && ity != yData.constEnd());
    rebuildScaleFactors = TRUE;
  }
}

// NOTE: for perfomance issues never do update in next method
void NMGChartSeries::setMaxValues(const double& x, const double& y) 
{ 
  maxValues.x = x;
  maxValues.y = y;
  rebuildScaleFactors = TRUE;
}

// NOTE: for perfomance issues never do update in next method
void NMGChartSeries::setMinValues(const double& x, const double& y)
{
  minValues.x = x;
  minValues.y = y;
  rebuildScaleFactors = TRUE;
}

double NMGChartSeries::xWindowToViewport(const double& xcoord)
{
  /* NOTE conversion is done following the next rule
  * x viewport := (viewport width / window width)* x window -
  *               ((viewport width / window width)* x window coordinate system origin - 
  *                                                 x viewport coordinate system origin)
  * ------- simplifying
  * x viewport := (viewport width / window width)* x window -
  *               ((viewport width / window width)* x window coordinate system origin)
  * ------- simplifying
  * x viewport := (viewport width / window width)*(x window - x window coordinate system origin)
  */
  return horizontalScaleFactor*(xcoord-minValues.x);
}

double NMGChartSeries::yWindowToViewport(const double& ycoord)
{
  /* NOTE conversion is done following the next rule
  * y viewport := (viewport height / window height)* y window -
  *               ((viewport height / window height)* y window coordinate system origin - 
  *                                                   y viewport coordinate system origin)
  * ------- simplifying
  * y viewport := (viewport height / window height)* y window -
  *               ((viewport height / window height)* y window coordinate system origin)
  * ------- simplifying
  * y viewport := (viewport height / window height)*(y window - y window coordinate system origin)
  */  
  return verticalScaleFactor*(ycoord-minValues.y);
}

void NMGChartSeries::calculateScaleFactors()
{
  Q_ASSERT_X((maxValues.y-minValues.y) != 0, "calculateScaleFactors", "division by zero in y");
  Q_ASSERT_X((maxValues.x-minValues.x) != 0, "calculateScaleFactors", "division by zero in x");
  verticalScaleFactor = currentHeightIntoScene/(maxValues.y-minValues.y);
  horizontalScaleFactor = currentWidthIntoScene/(maxValues.x-minValues.x);
}

// **************** //
// * NMGChartAxis * //
// **************** //

NMGChartAxis::NMGChartAxis(QGraphicsItem* parent)
  : NMGChartItem(parent)
{
  currentFont = QFont(); // Initializated with the application's default font.
  currentFont.setPointSize(10);
  currentFont.setStyleHint(QFont::SansSerif);
  currentPaddingMargin = 10;

  location = NMGChartAxis::EXTERNAL;
  type = NMGChartAxis::NUMBER;
  numReferenceValues = 2;
  minLabelValue = 0.0;
  maxLabelValue = 1.0;
  labelsList << QString::number(0) << QString::number(1);
  idsList.clear();
}

void NMGChartAxis::setDataType(const NMGChartAxis::DataType newType)
{
  if(type != newType)
  {
    labelsList.clear();
    idsList.clear();
    if(newType == NMGChartAxis::NUMBER)
    {
      location = NMGChartAxis::EXTERNAL;
      type = NMGChartAxis::NUMBER;
      numReferenceValues = 2;
      minLabelValue = 0.0;
      maxLabelValue = 1.0;
      labelsList << QString::number(0) << QString::number(1);
    }
    if(newType == NMGChartAxis::TEXT)
    { 
      location = NMGChartAxis::INTERNAL;
      type = NMGChartAxis::TEXT;
      numReferenceValues = 0;
    }
  }
}

void NMGChartAxis::setNumReferenceValues(int value)
{
  if(type == NMGChartAxis::NUMBER)
  {
    if(value!=numReferenceValues && value >= 2) 
    {
      numReferenceValues = value;
      recalculateAxisLabels();
      spaceNeededIntoSceneChanged = TRUE;
      update(); 
    }
  }
}

void NMGChartAxis::setAxisScale(double minValue, double maxValue)
{
  if(type == NMGChartAxis::NUMBER)
  {
    bool changed = FALSE; // used to avoid unnecessary calculations
    if(minLabelValue != minValue) 
    {
      minLabelValue = minValue;
      changed = TRUE;
    }
    if(maxLabelValue != maxValue)
    {
      maxLabelValue = maxValue;
      changed = TRUE;
    }
    if(changed)
    {
      recalculateAxisLabels();
      spaceNeededIntoSceneChanged = TRUE;
      update();
    }
  }
}

void NMGChartAxis::addLabel(const QString& id, QString text)
{
  Q_ASSERT_X(!idsList.contains(id), "addLabel", "repeated identifier");
  if(type == NMGChartAxis::TEXT)
  {
    numReferenceValues++;
    if(text.size() > 7)
    {
      QString aux = text.left(7);
      labelsList << aux.leftJustified(10, '.');
    }
    else labelsList << text;
    idsList << id;
    spaceNeededIntoSceneChanged = TRUE;
    update();
  }
}

void NMGChartAxis::removeLabel(const QString& id)
{
  if(type == NMGChartAxis::TEXT && idsList.contains(id))
  {
    int idx = idsList.indexOf(id);
    labelsList.removeAt(idx);
    idsList.removeAt(idx);
    numReferenceValues--;
    spaceNeededIntoSceneChanged = TRUE;
    update();
  }
}

void NMGChartAxis::setFontFamily(QString family)
{
  if(currentFont.family() != family)
  {
    currentFont.setFamily(family);
    spaceNeededIntoSceneChanged = TRUE;
    update();
  }
}

void NMGChartAxis::setFontSize(int size)
{
  if(size != currentFont.pointSize())
  {
    currentFont.setPointSize(size);
    spaceNeededIntoSceneChanged = TRUE;
    update();
  }
}

void NMGChartAxis::recalculateAxisLabels()
{
  labelsList.clear();
  double interval = (maxLabelValue - minLabelValue)/(numReferenceValues-1);
  int decimalDigits = Globals::getSignificantDecimalDigits(interval);
  double acumulation = minLabelValue;
  
  labelsList << QString::number(minLabelValue, 'f', decimalDigits);
  for(int i = 2; i < numReferenceValues; i++)
  {
    acumulation += interval;
    labelsList << QString::number(acumulation, 'f', decimalDigits);
  }
  labelsList << QString::number(maxLabelValue, 'f', decimalDigits);
}

// ********* //
// * SLOTS * //
// ********* //

void NMGChartAxis::slotChangeLabelText(const QString& id, const QString& newText)
{
  if(type == NMGChartAxis::TEXT)
  {
    QString modif(newText);
    if(modif.size() > 7) 
    {
      modif = modif.left(7);
      modif = modif.leftJustified(10, '.');
    }
    int pos = idsList.indexOf(id);
    if(pos != -1)
    {
      labelsList.replace(pos, modif);
      spaceNeededIntoSceneChanged = TRUE;
      update();
    }
  }
}

void NMGChartAxis::slotChangeLabelOrder(const QString& id, int newOrder)
{
  if(type == NMGChartAxis::TEXT)
  {
    int oldOrder = idsList.indexOf(id);
    if(oldOrder != -1 && (newOrder >= 0 && newOrder < idsList.size()))
    {
      idsList.move(oldOrder, newOrder);
      labelsList.move(oldOrder, newOrder);
      update();
    }
  }
}

// ***************** //
// * NMGChartXAxis * //
// ***************** //

NMGChartXAxis::NMGChartXAxis(QGraphicsItem* parent)
  : NMGChartAxis(parent)
{}

QRectF NMGChartXAxis::boundingRect() const
{
  if(currentWidthIntoScene <= 0 || currentHeightIntoScene <= 0) return QRectF();
  else 
  {
    if(location == NMGChartAxis::EXTERNAL)
    {
      double extraWidth = QFontMetricsF(currentFont).height()/2.0; // label is rotated 90º degrees
      return QRectF(-extraWidth, 0, 
                    extraWidth + currentWidthIntoScene + extraWidth, currentHeightIntoScene);
    }
    else // INTERNAL
      return QRectF(0, 0, currentWidthIntoScene, currentHeightIntoScene);
  }
}

void NMGChartXAxis::paint(QPainter* painter, 
                          const QStyleOptionGraphicsItem* option, 
                          QWidget* widget)
{
  if(hasEnoughSpace())
  {
    painter->setFont(currentFont);
    QPen pen;
    pen.setWidth(1);

    // painting a base rectangle for axis line
    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->setBrush(Qt::white);
    painter->drawRect(0, 0, currentWidthIntoScene, 5);
    painter->setBrush(Qt::NoBrush);
    
    // painting axis line
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    
    double xInterval = 0.0;
    double x = 0.0;
    painter->drawLine(0, 0, currentWidthIntoScene, 0);
    if(location == NMGChartAxis::EXTERNAL)
    {
      xInterval = (double)currentWidthIntoScene/(double)(numReferenceValues-1);
      for(int i = 0; i < numReferenceValues; i++)
      {
        x = i*xInterval;
        painter->drawLine(QLineF(x, 0, x, 5));
      }
    }
    else
    { // INTERNAL
      xInterval = (double)currentWidthIntoScene/(double)(numReferenceValues+1);
      for(int i = 1; i <= numReferenceValues; i++)
      {
        x = i*xInterval;
        painter->drawLine(QLineF(x, 0, x, 5)); 
      }
    }
    
    // painting labels
    painter->save();
    painter->translate(0, currentHeightIntoScene); // starting at bottom side
    painter->rotate(270);
    QFontMetricsF fm(currentFont);
    if(location == NMGChartAxis::EXTERNAL)
    {
      xInterval = (double)currentWidthIntoScene/(double)(numReferenceValues-1);
      // NOTE numReferenceValues-1 is always > 0 because in external mode there are 
      // a minimum of 2 reference values.
      for(int i = 0 ; i < labelsList.size(); i++)
      {
        QRectF boundingForText = fm.boundingRect(labelsList.at(i));
        painter->drawText(QPointF(currentHeightIntoScene - currentPaddingMargin -  
                                  fm.width(labelsList.at(i)), 
                                  i*xInterval + boundingForText.height()/4.0), 
                          labelsList.at(i));
      }
    }
    else
    { // INTERNAL
      if(!labelsList.isEmpty())
      { // there is at least one label to be painted
        xInterval = (double)currentWidthIntoScene/(double)(numReferenceValues+1);
        for(int i = 0; i < labelsList.size(); i++)
        {
          QRectF boundingForText = fm.boundingRect(labelsList.at(i));
          painter->drawText(QPointF(currentHeightIntoScene - currentPaddingMargin -  
                                    fm.width(labelsList.at(i)), 
                                    (i+1)*xInterval + boundingForText.height()/4.0), 
                            labelsList.at(i));
        }
      }
    }
    painter->restore();
  }
  else emit signalGiveMeMoreSpace();
}

int NMGChartXAxis::getSuggestedMinimumWidthIntoScene() const 
{
  int minWidth = 0;
  int labelsListSize = labelsList.size();
  double maxLabelWidth = 0.0; // consider that labels are rotated 90º degrees
  QFontMetricsF fm(currentFont);
  
  for(int i = 0; i < labelsListSize; i++)
  {
    QRectF boundingForText = fm.boundingRect(labelsList.at(i));
    maxLabelWidth = qMax(maxLabelWidth, boundingForText.height());
  }

  if(location == NMGChartAxis::INTERNAL) minWidth = (int)(maxLabelWidth*labelsListSize);
  else
  { // EXTERNAL
    if(labelsListSize>=2) minWidth = (int)(maxLabelWidth*(labelsListSize-1));
    else if(labelsListSize == 1) minWidth = (int)(maxLabelWidth/2);   
  }
  return minWidth;
}

int NMGChartXAxis::getSuggestedMinimumHeightIntoScene() const
{
  int minHeight = 0;
  int maxLabelHeight = 0; // consider that labels are rotated 90º degrees
  QFontMetrics fm(currentFont);
  
  for(int i = 0; i < labelsList.size(); i++)
  {
    QRect boundingForText = fm.boundingRect(labelsList.at(i));
    maxLabelHeight = qMax(maxLabelHeight, boundingForText.width());
  }
  minHeight = currentPaddingMargin + maxLabelHeight + currentPaddingMargin;
  return minHeight;
}

// ***************** //
// * NMGChartYAxis * //
// ***************** //

NMGChartYAxis::NMGChartYAxis(QGraphicsItem* parent)
  : NMGChartAxis(parent)
{}

QRectF NMGChartYAxis::boundingRect() const
{
  if(currentWidthIntoScene <= 0 || currentHeightIntoScene <= 0) return QRectF();
  else 
  {
    if(location == NMGChartAxis::EXTERNAL)
    {
      double extraHeight = QFontMetricsF(currentFont).height()/2.0;
      return QRectF(0, -extraHeight, 
                    currentWidthIntoScene, extraHeight + currentHeightIntoScene + extraHeight);
    }
    else // INTERNAL
      return QRectF(0, 0, currentWidthIntoScene, currentHeightIntoScene);
  }
}

void NMGChartYAxis::paint(QPainter* painter, 
                          const QStyleOptionGraphicsItem* option, 
                          QWidget* widget)
{
  if(hasEnoughSpace())
  {
    painter->setFont(currentFont);
    QPen pen;
    pen.setWidth(1);
    
    // painting a base rectangle for axis line
    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->setBrush(Qt::white);
    painter->drawRect(currentWidthIntoScene-5, 0, 5, currentHeightIntoScene);
    painter->setBrush(Qt::NoBrush);
    
    // painting axis line
    pen.setColor(Qt::black);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    
    double yInterval = 0.0;
    double y = 0.0;
    painter->drawLine(currentWidthIntoScene, 0, currentWidthIntoScene, currentHeightIntoScene);
    if(location == NMGChartAxis::EXTERNAL)
    {
      yInterval = (double)currentHeightIntoScene/(double)(numReferenceValues-1);
      for(int j = 0; j < numReferenceValues; j++)
      {
        y = j*yInterval;
        painter->drawLine(QLineF(currentWidthIntoScene-5, y, currentWidthIntoScene, y));
      }
    }
    else
    { // INTERNAL
      yInterval = (double)currentHeightIntoScene/(double)(numReferenceValues+1);
      for(int j = 1; j <= numReferenceValues; j++)
      {
        y = j*yInterval;
        painter->drawLine(QLineF(currentWidthIntoScene-5, y, currentWidthIntoScene, y)); 
      }
    }
    
    // painting labels
    painter->save();
    painter->translate(0, currentHeightIntoScene); // starting at bottom side
    QFontMetricsF fm(currentFont);
    if(location == NMGChartAxis::EXTERNAL)
    {
      yInterval = (double)currentHeightIntoScene/(double)(numReferenceValues-1);
      // NOTE numReferenceValues-1 is always > 0 because in external mode there are 
      // a minimum of 2 reference values.
      for(int j = 0 ; j < labelsList.size(); j++)
      {
        QRectF boundingForText = fm.boundingRect(labelsList.at(j));
        painter->drawText(QPointF(currentWidthIntoScene - currentPaddingMargin -  
                                  fm.width(labelsList.at(j)), 
                                  -j*yInterval+boundingForText.height()/4.0), 
                          labelsList.at(j));
      }
    }
    else
    { // INTERNAL
      if(!labelsList.isEmpty())
      { // there is at least one label to be painted
        yInterval = (double)currentHeightIntoScene/(double)(numReferenceValues+1);
        for(int j = 0; j < labelsList.size(); j++)
        {
          QRectF boundingForText = fm.boundingRect(labelsList.at(j));
          painter->drawText(QPointF(currentWidthIntoScene - currentPaddingMargin -  
                                    fm.width(labelsList.at(j)), 
                                    -((j+1)*yInterval) + boundingForText.height()/4.0), 
                            labelsList.at(j));
        }
      }
    }
    painter->restore();
  }
  else emit signalGiveMeMoreSpace();
}

int NMGChartYAxis::getSuggestedMinimumWidthIntoScene() const 
{
  int minWidth = 0;
  int maxLabelWidth = 0;
  QFontMetrics fm(currentFont);
  
  for(int i = 0; i < labelsList.size(); i++)
  {
    QRect boundingForText = fm.boundingRect(labelsList.at(i));
    maxLabelWidth = qMax(maxLabelWidth, boundingForText.width());
  }
  minWidth = currentPaddingMargin + maxLabelWidth + currentPaddingMargin;
  return minWidth;
}

int NMGChartYAxis::getSuggestedMinimumHeightIntoScene() const
{
  int minHeight = 0;
  int labelsListSize = labelsList.size();
  double maxLabelHeight = 0.0;
  QFontMetricsF fm(currentFont);
  
  for(int i = 0; i < labelsListSize; i++)
  {
    QRectF boundingForText = fm.boundingRect(labelsList.at(i));
    maxLabelHeight = qMax(maxLabelHeight, boundingForText.height());
  }

  if(location == NMGChartAxis::EXTERNAL)
  {
    if(labelsListSize>=2) minHeight = (int)(maxLabelHeight*(labelsListSize-1));
    else if(labelsListSize == 1) minHeight = (int)(maxLabelHeight/2);
  }
  else
  { // INTERNAL
    minHeight = (int)(maxLabelHeight*labelsListSize);
  }
  return minHeight;
}

// ****************** //
// * NMGChartLegend * //
// ****************** //

NMGChartLegend::NMGChartLegend(QGraphicsItem* parent)
  : NMGChartItem(parent)
{
  currentPaddingMargin = 5;

  currentPosition = NMGChartLegend::NE;
  currentFont = QFont(); // Initializated with the application's default font.
  currentFont.setPointSize(10);
  currentFont.setStyleHint(QFont::SansSerif);
  
  legendEntries.clear();
  entriesOrder.clear();
  positionMapping.clear();
  positionMapping[NMGChartLegend::N] = NORTH;
  positionMapping[NMGChartLegend::NE] = NORTHEAST;
  positionMapping[NMGChartLegend::E] = EAST;
  positionMapping[NMGChartLegend::SE] = SOUTHEAST;
  positionMapping[NMGChartLegend::S] = SOUTH;
  positionMapping[NMGChartLegend::SW] = SOUTHWEST;
  positionMapping[NMGChartLegend::W] = WEST;
  positionMapping[NMGChartLegend::NW] = NORTHWEST;
  positionMapping[NMGChartLegend::C] = CENTER;
  
  decorationPixels = 8;
  decorationToTextPadding = 5;
  internalPaddingMargin = 3;
  background = Qt::white;
  
  neededMinimumWidthIntoScene = getSuggestedMinimumWidthIntoScene();
  neededMinimumHeightIntoScene = getSuggestedMinimumHeightIntoScene();
}

QRectF NMGChartLegend::boundingRect() const
{
  if(currentWidthIntoScene <= 0 || currentHeightIntoScene <= 0) return QRectF();
  else return QRectF(0, 0, currentWidthIntoScene, currentHeightIntoScene);
}

void NMGChartLegend::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* option, 
                           QWidget* widget)
{
  if(hasEnoughSpace())
  {
    painter->setFont(currentFont);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(background);
    QFontMetrics fm(currentFont);
    
    painter->save();
    translateToCurrentPosition(painter);
    
    // painting legend outline
    painter->drawRect(0,0,
                      neededMinimumWidthIntoScene-(currentPaddingMargin << 1), 
                      neededMinimumHeightIntoScene-(currentPaddingMargin << 1));
    
    // painting legend entries
    
    // first we new to calculate the space available for each visible legend entry
    double yInterval = (double)(neededMinimumHeightIntoScene-(currentPaddingMargin << 1) -
                       (internalPaddingMargin << 1)) / (double)numberOfVisibleEntries();
    // for performance issues we calculate the relative position of a decoration for any entry
    double decorationRelativeYPos = yInterval-decorationPixels;
    if(decorationRelativeYPos > 0) decorationRelativeYPos /= 2;

    QList<QString>::const_iterator it;
    int idx = 1;
    for(it = entriesOrder.constBegin(); it != entriesOrder.constEnd(); it++)
    {
      LegendEntry entry = legendEntries.find(*it).value();
      if(entry.visible)
      {
        // text
        painter->drawText(internalPaddingMargin + decorationPixels + decorationToTextPadding,
                          internalPaddingMargin + (int)(idx*yInterval) - fm.descent(), 
                          entry.text);
        
        // decoration
        painter->setBrush(entry.color);
        painter->drawRect(internalPaddingMargin,
                          (int)(internalPaddingMargin + (idx-1)*yInterval + 
                                decorationRelativeYPos),
                          decorationPixels,
                          decorationPixels);
        idx++;
      }
    }
    painter->restore();
  }
  else emit signalGiveMeMoreSpace();
}

void NMGChartLegend::translateToCurrentPosition(QPainter* painter)
{
  int x = 0;
  int y = 0;
  int width = neededMinimumWidthIntoScene-(currentPaddingMargin << 1);
  int height = neededMinimumHeightIntoScene-(currentPaddingMargin << 1);
  switch(currentPosition)
  {
    case N:
      x = (currentWidthIntoScene-width) >> 1;
      y = currentPaddingMargin;
      break;
    case NE:
      x = currentWidthIntoScene-neededMinimumWidthIntoScene+currentPaddingMargin;
      y = currentPaddingMargin;
      break;
    case E:
      x = currentWidthIntoScene-neededMinimumWidthIntoScene+currentPaddingMargin;
      y = (currentHeightIntoScene-height) >> 1;
      break;
    case SE:
      x = currentWidthIntoScene-neededMinimumWidthIntoScene+currentPaddingMargin;
      y = currentHeightIntoScene-neededMinimumHeightIntoScene+currentPaddingMargin;
      break;
    case S:
      x = (currentWidthIntoScene-width) >> 1;
      y = currentHeightIntoScene-neededMinimumHeightIntoScene+currentPaddingMargin;
      break;
    case SW:
      x = currentPaddingMargin;
      y = currentHeightIntoScene-neededMinimumHeightIntoScene+currentPaddingMargin;
      break;
    case W:
      x = currentPaddingMargin;
      y = (currentHeightIntoScene-height) >> 1;
      break;
    case NW:
      x = currentPaddingMargin;
      y = currentPaddingMargin;
      break;
    case C:
      x = (currentWidthIntoScene-width) >> 1;
      y = (currentHeightIntoScene-height) >> 1;
      break;
  }
  painter->translate(x,y);
}

int NMGChartLegend::getSuggestedMinimumWidthIntoScene() const
{
  QFontMetrics fm(currentFont);
  int minWidth = currentPaddingMargin + internalPaddingMargin + decorationPixels +
                 decorationToTextPadding;
  
  int textWidth = 0;
  QMap<QString, LegendEntry>::const_iterator it;
  
  for(it = legendEntries.constBegin(); it != legendEntries.constEnd(); it++)
  {
    if(it.value().visible) 
    {
      textWidth = qMax(textWidth, fm.boundingRect(it.value().text).width());
    }
  }
  minWidth += textWidth + internalPaddingMargin + currentPaddingMargin;
  return minWidth;
}

int NMGChartLegend::getSuggestedMinimumHeightIntoScene() const
{
  QFontMetrics fm(currentFont);
  int minHeight = currentPaddingMargin + internalPaddingMargin;
    
  int textHeight = 0;
  QMap<QString, LegendEntry>::const_iterator it;  
  for(it = legendEntries.constBegin(); it != legendEntries.constEnd(); it++)
  {
    if(it.value().visible)
    {
      textHeight = qMax(textHeight, fm.boundingRect(it.value().text).height());
    }
  }
  minHeight += qMax(decorationPixels, textHeight)*numberOfVisibleEntries();
  minHeight += internalPaddingMargin + currentPaddingMargin;
  return minHeight;
}

void NMGChartLegend::setFontFamily(QString family)
{
  if(currentFont.family() != family)
  {
    currentFont.setFamily(family);
    spaceNeededIntoSceneChanged = TRUE;
    update();
  }
}

void NMGChartLegend::setFontSize(int size)
{
  if(currentFont.pointSize() != size)
  {
    currentFont.setPointSize(size);
    spaceNeededIntoSceneChanged = TRUE;
    update();
  }
}

void NMGChartLegend::setPosition(QString position)
{
  if(position == NORTH || position == NORTHEAST || position == EAST ||
     position == SOUTHEAST || position == SOUTH || position == SOUTHWEST ||
     position == WEST || position == NORTHWEST || position == CENTER)
  {
    LegendPosition pos = positionMapping.key(position);
    currentPosition = pos;
    update();
  }
}

int NMGChartLegend::numberOfVisibleEntries() const
{
  int num = 0;
  QMap<QString, LegendEntry>::const_iterator it;  
  for(it = legendEntries.constBegin(); it != legendEntries.constEnd(); it++)
  {
    if(it.value().visible) num++;
  }
  return num;
}

// ********* //
// * SLOTS * //
// ********* //

void NMGChartLegend::slotAddNewEntry(const QString& seriesId, 
                                     const QString& text, 
                                     const QColor& color)
{
  legendEntries.insert(seriesId, LegendEntry(color, text, TRUE));
  entriesOrder.append(seriesId);
  spaceNeededIntoSceneChanged = TRUE;
}

void NMGChartLegend::slotDeleteEntry(const QString& seriesId)
{
  entriesOrder.removeAll(seriesId);
  legendEntries.remove(seriesId);
  spaceNeededIntoSceneChanged = TRUE;
  update();  
}

void NMGChartLegend::slotChangeEntryText(const QString& seriesId, const QString& text)
{
  QMap<QString, LegendEntry>::iterator it;
  if((it = legendEntries.find(seriesId)) != legendEntries.end())
  {
    if(it.value().text != text)
    {
      it.value().text = text;
      spaceNeededIntoSceneChanged = TRUE;
      update();
    }
  }
}

void NMGChartLegend::slotChangeEntryColor(const QString& seriesId, const QColor& color)
{
  QMap<QString, LegendEntry>::iterator it;
  if((it = legendEntries.find(seriesId)) != legendEntries.end())
  {
    if(it.value().color != color)
    {
      it.value().color = color;
      update();
    }
  }  
}

void NMGChartLegend::slotChangeEntryOrder(const QString& seriesId, int newOrder)
{
  int oldOrder = entriesOrder.indexOf(seriesId);
  if(oldOrder != -1 && (newOrder >= 0 && newOrder < entriesOrder.size()))
  {    
    entriesOrder.move(oldOrder, newOrder);
    update();
  }
}

void NMGChartLegend::slotChangeEntryVisibility(const QString& seriesId, bool visible)
{
  QMap<QString, LegendEntry>::iterator it;
  if((it = legendEntries.find(seriesId)) != legendEntries.end())
  {
    if(it.value().visible != visible)
    {
      it.value().visible = visible;
      spaceNeededIntoSceneChanged = TRUE;
      update();
    }
  }
}

#include "nmgchartitem.moc"
