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
#include "nmggraphpacketorienteddataparser.h"

NMGGraphPacketOrientedDataParser::NMGGraphPacketOrientedDataParser(NMGFlowOrientedResultData* data)
  :NMGPacketOrientedDataParser()
{
  this->data = data;
  currentFlow = 0;
  currentFlowId = -1;
  flowStartTime.clear();
}

NMGGraphPacketOrientedDataParser::~NMGGraphPacketOrientedDataParser()
{}

void NMGGraphPacketOrientedDataParser::doProcess() 
{
  if(currentFlowId != flowId)
  {
    // a new flow is created because any flow has been processed yet or
    // because a new one has to be processed
    currentFlow = new NMGFlowData();
    saveFlowInformation();
    currentFlowId = flowId;
    if(flowStartTime.isEmpty()) flowStartTime = txTime;
    saveSequenceInformation();
  }
  else
  {
    // a new sequence packet in the same flow has been read
    saveSequenceInformation();
  }
}

void NMGGraphPacketOrientedDataParser::doWhenFlowFinished() 
{
  currentFlowId = -1;
  if(data->contains(currentFlow->getFlowId()))
  {
    delete currentFlow;
    currentFlow = 0;
    raiseError(QObject::tr("There are two flows with the same identifier"));
  }
  else
  {
    currentFlow->fillLostSequences();
    data->addFlowData(currentFlow->getFlowId(), currentFlow);
    currentFlow = 0;
  }
}

void NMGGraphPacketOrientedDataParser::saveFlowInformation()
{
  currentFlow->setFlowId(flowId);
  currentFlow->setSourceAddress(sourceAddress);
  currentFlow->setSourcePort(sourcePort);
  currentFlow->setDestinationAddress(destinationAddress);
  currentFlow->setDestinationPort(destinationPort);
}

void NMGGraphPacketOrientedDataParser::saveSequenceInformation()
{
  double timestamp = timeDifference(flowStartTime, txTime);
  double delay = timeDifference(txTime,rxTime);

  if (delay < 0.0) currentFlow->setDataInvalidity(TRUE);    
  currentFlow->addPacketInfoToList(seqId, timestamp, delay);
}

double NMGGraphPacketOrientedDataParser::timeDifference(const QString& start, const QString& end)
{
  int hoursStart,minsStart,secsStart,microsecsStart;
  int hoursEnd,minsEnd,secsEnd,microsecsEnd;
  int totalSecsStart, totalSecsEnd;
  long long microsecsDiff;
  double result;
  
  sscanf(start.toAscii().data(),"%d:%d:%d.%d",&hoursStart,&minsStart,&secsStart,&microsecsStart);
  sscanf(end.toAscii().data(),"%d:%d:%d.%d",&hoursEnd,&minsEnd,&secsEnd,&microsecsEnd);

  totalSecsStart = (hoursStart*3600)+(minsStart*60)+secsStart;
  totalSecsEnd = (hoursEnd*3600)+(minsEnd*60)+secsEnd;
  
  // operations are calculated in microseconds because of float precision problems
  microsecsDiff = ((long long)totalSecsEnd-(long long)totalSecsStart)*1000000 + 
      ((long long)microsecsEnd-(long long)microsecsStart);
  // but always showing miliseconds
  result = (double)microsecsDiff / 1000.0;
  return result;
}


