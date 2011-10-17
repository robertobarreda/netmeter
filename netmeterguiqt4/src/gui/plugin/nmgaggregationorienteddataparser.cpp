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
#include "nmgaggregationorienteddataparser.h"
#include "nmgaggregationorientedtags.h"

NMGAggregationOrientedDataParser::NMGAggregationOrientedDataParser()
  : rss(-1), 
    sss(-1), 
    sms(-1), 
    time(-1.0),
    tputSent(-1.0),
    tputRecv(-1.0),
    okMsgSent(-1),
    koMsgSent(-1),
    okMsgRecv(-1)
{
  isResultSectionRead = FALSE;
  isResultSectionEmpty = TRUE;	
  sourceAddress.clear();
  destinationAddress.clear();
  typeOfProtocol.clear();
  
  stopParsing = FALSE;
  completedPercent = 0;
  status = "Parser initialized";
}

bool NMGAggregationOrientedDataParser::read(QIODevice* device)
{
  setDevice(device);
  completedPercent = 1;
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

void NMGAggregationOrientedDataParser::readResult()
{
  Q_ASSERT(isStartElement() && name() == TAG_RESULT);

  while (!atEnd() && !isResultSectionRead && !stopParsing) 
  {
    readNext();
    if (isEndElement())
    {
      if(isResultSectionEmpty) 
        raiseError(QObject::tr("No data found in this file."));
      else
      {
        isResultSectionRead = TRUE;
        completedPercent = 100;
        status = "Data read";
      }
      break;
    }
    if (isStartElement())
    {
      isResultSectionEmpty = FALSE;
      if (name() == TAG_TCP) readTCP();
      else if (name() == TAG_UDP) readUDP();
    }
  }
}

void NMGAggregationOrientedDataParser::readTCP()
{
  Q_ASSERT(isStartElement() && name() == TAG_TCP);
  
  completedPercent = 25;
  status = "Reading data";
  typeOfProtocol = TAG_TCP;
  
  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) 
    {
      doProcess();
      sourceAddress.clear();
      destinationAddress.clear();
      rss = -1;
      sss = -1;
      sms = -1;
      time = -1.0;
      tputSent = -1.0;
      tputRecv = -1.0;
      okMsgSent = -1;
      koMsgSent = -1;
      okMsgRecv = -1;
      typeOfProtocol.clear();
      
      completedPercent = 75;
      status = "Reading data";
      break;
    }
    
    if (isStartElement()) 
    {
      if (name() == TAG_SOURCE) readSourceAddress();
      else if (name() == TAG_DESTINATION) readDestinationAddress();
      else if (name() == TAG_RSS) readRSS();
      else if (name() == TAG_SSS) readSSS();
      else if (name() == TAG_SMS) readSMS();
      else if (name() == TAG_TIME) readTime();
      else if (name() == TAG_TPUT) readRecvThroughput();
      else readUnknownElement();      
    }
  }
}

void NMGAggregationOrientedDataParser::readUDP()
{
  Q_ASSERT(isStartElement() && name() == TAG_UDP);
  
  completedPercent = 25;
  status = "Reading data";
  typeOfProtocol = TAG_UDP;
  
  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) 
    {
      doProcess();
      sourceAddress.clear();
      destinationAddress.clear();
      rss = -1;
      sss = -1;
      sms = -1;
      time = -1.0;
      tputSent = -1.0;
      tputRecv = -1.0;
      okMsgSent = -1;
      koMsgSent = -1;
      okMsgRecv = -1;
      typeOfProtocol.clear();
      
      completedPercent = 75;
      status = "Reading data";
      break;
    }
    
    if (isStartElement()) 
    {
      if (name() == TAG_SOURCE) readSourceAddress();
      else if (name() == TAG_DESTINATION) readDestinationAddress();
      else if (name() == TAG_RSS) readRSS();
      else if (name() == TAG_SSS) readSSS();
      else if (name() == TAG_SMS) readSMS();
      else if (name() == TAG_TIME) readTime();
      else if (name() == TAG_STPUT) readSentThroughput();
      else if (name() == TAG_RTPUT) readRecvThroughput();
      else if (name() == TAG_SMO) readSentMsgOK();
      else if (name() == TAG_SMK) readSentMsgKO();
      else if (name() == TAG_RMO) readRecvMsgOK();
      else readUnknownElement();      
    }
  }
}

void NMGAggregationOrientedDataParser::readSourceAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_SOURCE);
  sourceAddress = readElementText();
}

void NMGAggregationOrientedDataParser::readDestinationAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_DESTINATION);
  destinationAddress = readElementText();
}

void NMGAggregationOrientedDataParser::readRSS()
{
  Q_ASSERT(isStartElement() && name() == TAG_RSS);
  QString rss = readElementText();
  if(rss.isEmpty()) this->rss = -1;
  else this->rss = rss.toInt();
}

void NMGAggregationOrientedDataParser::readSSS()
{
  Q_ASSERT(isStartElement() && name() == TAG_SSS);
  QString sss = readElementText();
  if(sss.isEmpty()) this->sss = -1;
  else this->sss = sss.toInt();
}

void NMGAggregationOrientedDataParser::readSMS()
{
  Q_ASSERT(isStartElement() && name() == TAG_SMS);
  QString sms = readElementText();
  if(sms.isEmpty()) this->sms = -1;
  else this->sms = sms.toInt();
}

void NMGAggregationOrientedDataParser::readTime()
{
  Q_ASSERT(isStartElement() && name() == TAG_TIME);
  QString time = readElementText();
  if(time.isEmpty()) this->time = -1.0;
  else this->time = time.toDouble();
}

void NMGAggregationOrientedDataParser::readSentThroughput()
{
  Q_ASSERT(isStartElement() && name() == TAG_STPUT);
  QString tput = readElementText();
  if(tput.isEmpty()) tputSent = -1.0;
  else tputSent = tput.toDouble();
}

void NMGAggregationOrientedDataParser::readRecvThroughput()
{
  Q_ASSERT(isStartElement() && (name() == TAG_RTPUT || name() == TAG_TPUT));
  QString tput = readElementText();
  if(tput.isEmpty()) tputRecv = -1.0;
  else tputRecv = tput.toDouble();
}

void NMGAggregationOrientedDataParser::readSentMsgOK()
{
  Q_ASSERT(isStartElement() && name() == TAG_SMO);
  QString msg = readElementText();
  if(msg.isEmpty()) okMsgSent = -1;
  else okMsgSent = msg.toInt(); 
}

void NMGAggregationOrientedDataParser::readSentMsgKO()
{
  Q_ASSERT(isStartElement() && name() == TAG_SMK);
  QString msg = readElementText();
  if(msg.isEmpty()) koMsgSent = -1;
  else koMsgSent = msg.toInt();
}

void NMGAggregationOrientedDataParser::readRecvMsgOK()
{
  Q_ASSERT(isStartElement() && name() == TAG_RMO);
  QString msg = readElementText();
  if(msg.isEmpty()) okMsgRecv = -1;
  else okMsgRecv = msg.toInt();
}

void NMGAggregationOrientedDataParser::readUnknownElement()
{
  Q_ASSERT(isStartElement());

  while (!atEnd() && !stopParsing) 
  {
    readNext();
    if (isEndElement()) break;
    if (isStartElement()) readUnknownElement();
  }
}

void NMGAggregationOrientedDataParser::stop()
{
  stopParsing = TRUE;
  raiseError(QObject::tr("Parser cancelled by user"));
}

int NMGAggregationOrientedDataParser::getCompletedPercent() const
{
  return completedPercent;
}

QString NMGAggregationOrientedDataParser::getStatus() const
{
  return status;
}
