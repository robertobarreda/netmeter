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
#include "nmgmeasurelistwidget.h"
#include "nmgaggregationorientedresultdata.h"
#include "nmgfloworientedresultdata.h"
#include "nmgglobals.h"
#include <nmgmetadatatags.h>
#include <QMap>

#define NO_DATA_AVAILABLE "No data available yet"

NMGMeasureListWidget::NMGMeasureListWidget(QWidget* parent)
  : NMGGroupTreeWidget(parent)
{
  isHeaderSet = FALSE;
  setHeaderLabel(tr(NO_DATA_AVAILABLE));
  connect(this, SIGNAL(itemMoved(const QString&)), this, SLOT(slotItemMoved(const QString&)));
  connect(this, SIGNAL(itemRemoved(const QString&)), this, SLOT(slotItemDeleted()));
}

void NMGMeasureListWidget::addNewTest(const QString& fileName, NMGTestLoaderHandler& loader)
{
  try
  {
    NMGTestFileData& data = loader.getTestFileData(fileName);
    if(!isHeaderSet)
    {
      addHeaderInformation(data.getCurrentSupportedType());
      isHeaderSet = TRUE;
    }
    if(data.getCurrentSupportedType() == Globals::AGGREGATION)
      addAggregationFileInformation(data,fileName);
    else
      addPacketsFileInformation(data,fileName);
  }
  catch(NMGGraphicsError e) { e.raiseError(); }
}

void NMGMeasureListWidget::addHeaderInformation(Globals::SupportedType type)
{
  if(type == Globals::AGGREGATION)
  { 
    QStringList header;
    header << tr("Test"); // 0
    header << tr("Name"); // 1
    header << tr("Source"); // 2
    header << tr("Destination"); // 3
    header << tr("Duration"); // 4
    header << tr("Throughput\n(sender-receiver)"); // 5
    header << tr("Protocol"); // 6
    header << tr("SSS"); // 7
    header << tr("RSS"); // 8
    header << tr("SMS"); // 9

    setHeaderLabels(header);
    hideColumn(1);
    hideColumn(6);
    hideColumn(7);
    hideColumn(8);
    hideColumn(9);
  }
  else if(type == Globals::PACKETS)
  { // packets
    QStringList header;
    header << tr("Test/Flow"); // 0
    header << tr("Name"); // 1
    header << tr("Source\n(IP/port)"); // 2
    header << tr("Destination\n(IP/port)"); // 3
    header << tr("Duration"); // 4
    header << tr("Packet rate\n(packets/sec)"); // 5
    header << tr("Packet size\n(bytes)"); // 6
    header << tr("Pattern"); // 7

    setHeaderLabels(header);
    hideColumn(1);
    hideColumn(7);
  }
}

void NMGMeasureListWidget::addAggregationFileInformation(NMGTestFileData& data, 
    const QString& fileName)
{
  QString filteredFile = fileName.mid(fileName.lastIndexOf("/")+1);
  QString testName = data.getMetadata()->getTestName();
  
  NMGAggregationOrientedResultData* result;
  result = (NMGAggregationOrientedResultData*)data.getResultData();
  if(result)
  {
    QStringList row;
    // Test and name
    row << filteredFile << testName;
    
    // Source
    row << (result->getSourceAddress().isEmpty() ? tr(NOT_AVAILABLE) : result->getSourceAddress());
    
    // Destination
    row << (result->getDestinationAddress().isEmpty() ? tr(NOT_AVAILABLE) :
            result->getDestinationAddress());
    
    // Duration
    row << (result->getElapsedTimeInSeconds() < 0 ? tr(NOT_AVAILABLE) :
            simplifyTime(result->getElapsedTimeInSeconds(), NMGMeasureListWidget::SECS));
    
    // Throughput
    QString tput;
    if(result->getSendingThroughputInKBitsPerSecond() < 0) tput.append(NOT_AVAILABLE);
    else
    {
      tput.append(simplifyDecimalSize(result->getSendingThroughputInKBitsPerSecond()*KILOFACTOR, 
                  NMGMeasureListWidget::BITS));
      tput.append("/sec");
    }
    tput.append(" - ");
    if(result->getReceivingThroughputInKBitsPerSecond() < 0) tput.append(NOT_AVAILABLE);
    else
    {
      tput.append(simplifyDecimalSize(result->getReceivingThroughputInKBitsPerSecond()*KILOFACTOR,
                  NMGMeasureListWidget::BITS));
      tput.append("/sec");
    }
    row << tput;
    
    // Protocol
    row << (result->getTypeOfProtocol().isEmpty() ? tr(NOT_AVAILABLE) : 
            result->getTypeOfProtocol());
    
    // SSS
    row << (result->getSendingSocketSizeInBytes() < 0 ? tr(NOT_AVAILABLE) :
            tr("%1 bytes").arg(QString::number(result->getSendingSocketSizeInBytes())));
    
    // RSS
    row << (result->getReceivingSocketSizeInBytes() < 0 ? tr(NOT_AVAILABLE) :
            tr("%1 bytes").arg(QString::number(result->getReceivingSocketSizeInBytes())));
    
    // SMS
    row << (result->getSendingMessagesSizeInBytes() < 0 ? tr(NOT_AVAILABLE) :
            tr("%1 bytes").arg(QString::number(result->getSendingMessagesSizeInBytes())));
    
    addTest(fileName, row);
  }
}

void NMGMeasureListWidget::addPacketsFileInformation(NMGTestFileData& data, 
    const QString& fileName)
{
  QString filteredFile = fileName.mid(fileName.lastIndexOf("/")+1);
  QString testName = data.getMetadata()->getTestName();
  
  NMGFlowOrientedResultData* result;
  result = (NMGFlowOrientedResultData*)data.getResultData();
  if(result)
  {
    // Test
    addTest(fileName, QStringList() << filteredFile << testName);
    
    QMap<int,NMGFlowData*>::const_iterator it;
    
    for (it = result->constBegin(); it != result->constEnd(); it++) 
    { // we must read flows info
      
      NMGFlowData* flow = it.value();
      
      // we must read extra information located into metadata
      readFlowMetadataInfo(flow, data.getMetadata()->getUncommonElement(TAG_CONF_MGEN));
      
      QStringList flowRow;
      
      // Flow id
      flowRow << QString::number(flow->getFlowId());
      
      // Flow name
      flowRow << flow->getFlowName();
      
      // Source
      QString infoIP = (flow->getSourceAddress().isEmpty() ? tr(NOT_AVAILABLE) : 
                        flow->getSourceAddress());
      infoIP.append("/").append(flow->getSourcePort() < 0 ? tr(NOT_AVAILABLE) :
                                QString::number(flow->getSourcePort()));
      flowRow << infoIP;
      
      // Destination
      infoIP = (flow->getDestinationAddress().isEmpty() ? tr(NOT_AVAILABLE) :
                flow->getDestinationAddress());
      infoIP.append("/").append(flow->getDestinationPort() < 0 ?
                                tr(NOT_AVAILABLE) :
                                QString::number(flow->getDestinationPort()));
      flowRow << infoIP;
      
      // Duration
      flowRow << (flow->getElapsedTimeInMiliSeconds() < 0 ? tr(NOT_AVAILABLE) :
                  simplifyTime(flow->getElapsedTimeInMiliSeconds(), 
                               NMGMeasureListWidget::MILISECS));
      
      // Packet Rate
      flowRow << (flow->getPacketRate() < 0 ? tr(NOT_AVAILABLE) :
                  QString::number(flow->getPacketRate()));
      
      // Packet Size
      flowRow << (flow->getPacketSize() < 0 ? tr(NOT_AVAILABLE) :
                  QString::number(flow->getPacketSize()));
      
      // Pattern
      flowRow << (flow->getPattern().isEmpty() ? tr(NOT_AVAILABLE) : flow->getPattern());
     
      addFlow(QString::number(flow->getFlowId()), flowRow);
    }
  }
}

void NMGMeasureListWidget::readFlowMetadataInfo(NMGFlowData* flow, QString metatags)
{
  if(!metatags.isEmpty())
  {
    QXmlStreamReader xml (metatags);
    int flowStart, flowEnd;

    while (!xml.atEnd())
    {
      xml.readNext();
      if(xml.isStartElement())
      {
        if(xml.name() == TAG_ON)
        {
          if(xml.attributes().value(ATTR_FLOW_ID).toString().toInt() == flow->getFlowId())
          { // we can populate data in flow with metadata attributes
            flowStart = xml.attributes().value(ATTR_TIME).toString().toInt();
            
            if(!xml.attributes().value(ATTR_NAME).isEmpty())
              flow->setFlowName(xml.attributes().value(ATTR_NAME).toString());
            
            if(!xml.attributes().value(ATTR_RATE).isEmpty())
              flow->setPacketRate(xml.attributes().value(ATTR_RATE).toString().toInt());
            
            if(!xml.attributes().value(ATTR_SIZE).isEmpty())
              flow->setPacketSize(xml.attributes().value(ATTR_SIZE).toString().toInt());
            
            if(!xml.attributes().value(ATTR_PATTERN).isEmpty())
              flow->setPattern(xml.attributes().value(ATTR_PATTERN).toString());
          }
        }
        else if(xml.name() == TAG_OFF)
        {
          if(xml.attributes().value(ATTR_FLOW_ID).toString().toInt() == flow->getFlowId())
          {
            flowEnd = xml.attributes().value(ATTR_TIME).toString().toInt();
            int duration = flowEnd-flowStart;
            if(duration > 0)
              flow->setElapsedTimeInMiliSeconds(duration);
          }
        }
      }
    }
    // Errors in xml shouldn't exist, so errors treatment is skipped
  }
}

QString NMGMeasureListWidget::simplifyTime(double time, 
                                          NMGMeasureListWidget::TimeUnits startUnit)
{
  QString simplification;
  switch (startUnit)
  {
    case NMGMeasureListWidget::MILISECS:
      if(time >= 1000) simplification = simplifyTime(time/1000, NMGMeasureListWidget::SECS);
      else simplification = QString(tr("%1 msecs")).arg(time,0,'f',0);
      break;
    case NMGMeasureListWidget::SECS:
      if(time >= 60) simplification = simplifyTime(time/60, NMGMeasureListWidget::MINS);
      else simplification = QString(tr("%1 secs")).arg(time,0,'f',2);
      break;
    case NMGMeasureListWidget::MINS:
      if(time >= 60) simplification = simplifyTime(time/60, NMGMeasureListWidget::HOURS);
      else simplification = QString(tr("%1 mins")).arg(time,0,'f',2);
      break;
    case NMGMeasureListWidget::HOURS:
      simplification = QString(tr("%1 hours")).arg(time,0,'f',2);
      break;
  }
  return simplification;
}

QString NMGMeasureListWidget::simplifyBinarySize(double size, 
                                                 NMGMeasureListWidget::BinarySizeUnits startUnit)
{
  QString simplification;
  switch (startUnit)
  {
    case NMGMeasureListWidget::BYTES:
      if(size >= 1024) 
        simplification = simplifyBinarySize(size/1024, NMGMeasureListWidget::KBYTES);
      else simplification = QString(tr("%1 bytes")).arg(size,0,'f',0);
      break;
    case NMGMeasureListWidget::KBYTES:
      if(size >= 1024) 
        simplification = simplifyBinarySize(size/1024, NMGMeasureListWidget::MBYTES);
      else simplification = QString(tr("%1 KB")).arg(size,0,'f',2);
      break;
    case NMGMeasureListWidget::MBYTES:
      if(size >= 1024) 
        simplification = simplifyBinarySize(size/1024, NMGMeasureListWidget::GBYTES);
      else simplification = QString(tr("%1 MB")).arg(size,0,'f',2);
      break;
    case NMGMeasureListWidget::GBYTES:
      simplification = QString(tr("%1 GB")).arg(size,0,'f',2);
      break;
  }
  return simplification;
}

QString NMGMeasureListWidget::simplifyDecimalSize(double size,
                                                  NMGMeasureListWidget::DecimalSizeUnits startUnit)
{
  QString simplification;
  switch (startUnit)
  {
    case NMGMeasureListWidget::BITS:
      if(size >= 1000) 
        simplification = simplifyDecimalSize(size/1000, NMGMeasureListWidget::KBITS);
      else simplification = QString(tr("%1 bits")).arg(size,0,'f',0);
      break;
    case NMGMeasureListWidget::KBITS:
      if(size >= 1000) 
        simplification = simplifyDecimalSize(size/1000, NMGMeasureListWidget::MBITS);
      else simplification = QString(tr("%1 Kbits")).arg(size,0,'f',2);
      break;
    case NMGMeasureListWidget::MBITS:
      if(size >= 1000) 
        simplification = simplifyDecimalSize(size/1000, NMGMeasureListWidget::GBITS);
      else simplification = QString(tr("%1 Mbits")).arg(size,0,'f',2);
      break;
    case NMGMeasureListWidget::GBITS:
      simplification = QString(tr("%1 Gbits")).arg(size,0,'f',2);
      break;
  }
  return simplification;
}

bool NMGMeasureListWidget::isEmpty()
{
  return (topLevelItemCount() == 0);
}

// ********* //
// * SLOTS * //
// ********* //

void NMGMeasureListWidget::slotChangeMeasureListItemText(const QString& id, const QString& text)
{
  if(NMGGroupTreeWidgetItem* item = findItemById(id)) item->setText(1, text);
}

void NMGMeasureListWidget::slotChangeMeasureListItemColor(const QString& id, const QColor& color)
{
  if(NMGGroupTreeWidgetItem* item = findItemById(id))
  {
    QPixmap decoration = QPixmap(12,12);
    decoration.fill(color);
    item->setData(0, Qt::DecorationRole, qVariantFromValue(decoration));
  }
}

void NMGMeasureListWidget::slotDeleteAllItems()
{
  selectAllItems(model(), selectionModel(), QModelIndex());
  removeSelectedItems();
}

void NMGMeasureListWidget::selectAllItems(QAbstractItemModel* model, 
                                          QItemSelectionModel* selectionModel,
                                          const QModelIndex& parent)
{
  if(model->hasChildren(parent))
  {
    if(selectionModel->isSelected(parent))
    {
      // this is done because of the removeSelectedItems() behaviour and the signals emitted,
      // to avoid extra signaling (e.g. extra parent deletion signal done by 
      // removeSelectedItems() due to no more children).
      selectionModel->select(parent, QItemSelectionModel::Deselect);
    }
    QModelIndex topLeft = model->index(0, 0, parent);
    QModelIndex bottomRight = model->index(model->rowCount(parent)-1, 
                                           model->columnCount(parent)-1, parent);
    QItemSelection selection;
    selection.select(topLeft, bottomRight);    
    selectionModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    
    QModelIndexList selectedIndexes = selection.indexes();
    QModelIndexList::const_iterator it;
    for(it = selectedIndexes.constBegin(); it != selectedIndexes.constEnd(); it++)
    {
      selectAllItems(model, selectionModel, *it);
    }
  }
}

void NMGMeasureListWidget::slotItemDeleted()
{
  if(topLevelItemCount() == 0)
  {    
    setColumnCount(1);
    setHeaderLabel(tr(NO_DATA_AVAILABLE));
    isHeaderSet = FALSE;
  }
}

void NMGMeasureListWidget::slotItemMoved(const QString& id)
{
  QList<QString> guessed;
  guessValidAtomicItemsMoved(id, guessed);
  QList<QString>::iterator it;
  for(it = guessed.begin(); it != guessed.end(); it++)
  {
    int position = guessMovementPosition(*it);
    emit signalValidAtomicItemMoved(*it, position);
  }
}

void NMGMeasureListWidget::guessValidAtomicItemsMoved(const QString& id, QList<QString>& guessed)
{
  for (int i = 0; i < topLevelItemCount(); ++i) 
  {
    NMGGroupTreeWidgetItem* item = static_cast<NMGGroupTreeWidgetItem*>(topLevelItem(i));
    guessValidAtomicItemsMovedRecursive(id, guessed, item);
  }
}

void NMGMeasureListWidget::guessValidAtomicItemsMovedRecursive(const QString& id, 
                                                               QList<QString>& guessed,
                                                               NMGGroupTreeWidgetItem* item)
{
  if(item->getId() == id)
  {
    if(item->childCount() == 0)
    {
      if(item->type() == NMGGroupTreeWidgetItem::Test || 
         item->type() == NMGGroupTreeWidgetItem::Flow)
      {
        guessed.append(item->getId());
      }
    }
    else
    { // item not atomic, processing children
      for (int i = 0; i < item->childCount(); ++i) 
      {
        NMGGroupTreeWidgetItem* child = static_cast<NMGGroupTreeWidgetItem*>(item->child(i));
        guessValidAtomicItemsMovedRecursive(child->getId(), guessed, child);
      }
    }
  }
  else
  {
    if(item->childCount() != 0)
    {
      for (int i = 0; i < item->childCount(); ++i) 
      {
        NMGGroupTreeWidgetItem* child = static_cast<NMGGroupTreeWidgetItem*>(item->child(i));
        guessValidAtomicItemsMovedRecursive(child->getId(), guessed, child);
      }
    }
  }
}

int NMGMeasureListWidget::guessMovementPosition(const QString& id)
{
  int position = 0;
  bool found = FALSE;
  for (int i = 0; i < topLevelItemCount() && !found; ++i) 
  {
    NMGGroupTreeWidgetItem* item = static_cast<NMGGroupTreeWidgetItem*>(topLevelItem(i));
    guessMovementPositionRecursive(id, position, found, item);
  }
  return position;
}

void NMGMeasureListWidget::guessMovementPositionRecursive(const QString& id, 
                                                          int& position,
                                                          bool& found, 
                                                          NMGGroupTreeWidgetItem* item)
{
  if(item->getId() != id)
  {
    if(item->childCount() == 0)
    {
      if(item->type() == NMGGroupTreeWidgetItem::Test || 
         item->type() == NMGGroupTreeWidgetItem::Flow)
      {
        position++;
      }
    }
    else
    {
      for (int i = 0; i < item->childCount() && !found; ++i) 
      {
        NMGGroupTreeWidgetItem* child = static_cast<NMGGroupTreeWidgetItem*>(item->child(i));
        guessMovementPositionRecursive(id, position, found, child);
      }
    }
  }
  else found = TRUE;
}

#include "nmgmeasurelistwidget.moc"
