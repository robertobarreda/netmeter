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
#include "nmgpacketorienteddataparser.h"
#include "nmgpacketorientedtags.h"

NMGPacketOrientedDataParser::NMGPacketOrientedDataParser()
  : flowId(-1), 
    sourcePort(-1), 
    destinationPort(-1), 
    seqId(-1), 
    packetSize(-1)
{
  isResultSectionRead = FALSE;
  isResultSectionEmpty = TRUE;
  sourceAddress.clear();
  destinationAddress.clear();
  fileSize = -1;
  stopParsing = FALSE;
  status = "Parser initialized";
}

bool NMGPacketOrientedDataParser::isDataRead() const
{
  return isResultSectionRead;
}

bool NMGPacketOrientedDataParser::read(QIODevice* device)
{
  setDevice(device);
  fileSize = device->size();
  status = "Reading data";

  while (!atEnd() && !isResultSectionRead && !stopParsing) 
  {
    readNext();
  
    if (isStartElement()) 
    {
      if (name() == TAG_RESULT) readResult();
    }
  }
  if(!hasError()) {
    if (atEnd() && !isResultSectionRead)
      raiseError(QObject::tr("The expected result section was not found."));
  }
  return !error();
}

void NMGPacketOrientedDataParser::readResult()
{
  Q_ASSERT(isStartElement() && name() == TAG_RESULT);

  while (!atEnd() && !isResultSectionRead && !stopParsing) 
  {
    readNext();
    if(isEndElement())
    {
      if(isResultSectionEmpty) 
        raiseError(QObject::tr("No data found in this file."));
      else
      {
        isResultSectionRead = TRUE;
        status = "Data read";
      }
      break;
    }
    if (isStartElement())
    {
      isResultSectionEmpty = FALSE;      
      if (name() == TAG_FLOWS) readFlows();
    }
  }
}

void NMGPacketOrientedDataParser::readFlows()
{
  Q_ASSERT(isStartElement() && name() == TAG_FLOWS);

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) break;
    
    if (isStartElement())
      if (name() == TAG_FLOW) readFlow();
  }
}

void NMGPacketOrientedDataParser::readFlow()
{
  Q_ASSERT(isStartElement() && name() == TAG_FLOW);
  
  flowId = (attributes().value(ATTR_ID)).toString().toInt();

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) 
    {
      doWhenFlowFinished();
      flowId = -1;
      sourceAddress.clear();
      sourcePort = -1;
      destinationAddress.clear();
      destinationPort = -1;
      break;
    }
    
    if (isStartElement()) 
    {
      if (name() == TAG_SOURCE) readFlowSourceInformation();
      else if (name() == TAG_DESTINATION) readFlowDestinationInformation();
      else if (name() == TAG_SEQ) readSequenceInFlow(); 
      else readUnknownElement();      
    }
  }
}

void NMGPacketOrientedDataParser::readFlowSourceInformation()
{
  Q_ASSERT(isStartElement() && name() == TAG_SOURCE);

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) break;

    if (isStartElement()) 
    {
      if (name() == TAG_ADDRESS) readSourceAddress();
      else if (name() == TAG_PORT) readSourcePort();
      else readUnknownElement();
    }
  }
}

void NMGPacketOrientedDataParser::readSourceAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_ADDRESS);
  sourceAddress = readElementText();
}

void NMGPacketOrientedDataParser::readSourcePort()
{
  Q_ASSERT(isStartElement() && name() == TAG_PORT);
  QString port = readElementText();
  if(port.isEmpty()) sourcePort = -1;
  else sourcePort = port.toInt();
}

void NMGPacketOrientedDataParser::readFlowDestinationInformation()
{
  Q_ASSERT(isStartElement() && name() == TAG_DESTINATION);

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) break;

    if (isStartElement()) 
    {
      if (name() == TAG_ADDRESS) readDestinationAddress();
      else if (name() == TAG_PORT) readDestinationPort();
      else readUnknownElement();
    }
  }
}

void NMGPacketOrientedDataParser::readDestinationAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_ADDRESS);
  destinationAddress = readElementText();
}

void NMGPacketOrientedDataParser::readDestinationPort()
{
  Q_ASSERT(isStartElement() && name() == TAG_PORT);
  QString port = readElementText();
  if(port.isEmpty()) destinationPort = -1;
  else destinationPort = port.toInt();
}

void NMGPacketOrientedDataParser::readSequenceInFlow()
{
  Q_ASSERT(isStartElement() && name() == TAG_SEQ);
  
  QString aux = (attributes().value(ATTR_ID)).toString();
  if(!aux.isEmpty()) seqId = aux.toInt();

  while (!atEnd() && !stopParsing)
  {
    readNext();

    if (isEndElement())
    {
      if (seqId!=-1 && !txTime.isEmpty() && !rxTime.isEmpty())
      {
        doProcess();
        seqId = -1;
        txTime.clear();
        rxTime.clear();
        packetSize = -1;
      }
      else
      {
        raiseError(QObject::tr("The XML file has an incomplete sequence element"));
      }      
      break;
    }
    
    if (isStartElement()) 
    {
      if (name() == TAG_TX) readSequenceTxTime();
      else if (name() == TAG_RX) readSequenceRxTime();
      else if (name() == TAG_SIZE) readSequencePacketSize();
      else readUnknownElement();      
    }
  }
}

void NMGPacketOrientedDataParser::readSequenceTxTime()
{
  Q_ASSERT(isStartElement() && name() == TAG_TX);
  txTime = readElementText();
}

void NMGPacketOrientedDataParser::readSequenceRxTime()
{
  Q_ASSERT(isStartElement() && name() == TAG_RX);
  rxTime = readElementText();
}

void NMGPacketOrientedDataParser::readSequencePacketSize()
{
  Q_ASSERT(isStartElement() && name() == TAG_SIZE);
  QString packet = readElementText();
  if(packet.isEmpty()) packetSize = -1;
  else packetSize = packet.toInt();
}

void NMGPacketOrientedDataParser::readUnknownElement()
{
  Q_ASSERT(isStartElement());

  while (!atEnd() && !stopParsing) 
  {
    readNext();
    if (isEndElement()) break;
    if (isStartElement()) readUnknownElement();
  }
}

void NMGPacketOrientedDataParser::stop()
{
  stopParsing = TRUE;
  raiseError(QObject::tr("Parser cancelled by user"));
}

int NMGPacketOrientedDataParser::getCompletedPercent() const
{
  if(fileSize>0) return (characterOffset()*100)/fileSize;
  else return 0;
}

QString NMGPacketOrientedDataParser::getStatus() const
{
  return status;
}
