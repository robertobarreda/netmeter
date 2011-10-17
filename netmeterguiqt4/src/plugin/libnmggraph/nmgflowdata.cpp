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
#include "nmgflowdata.h"

NMGFlowData::NMGFlowData()
  : flowId(-1), sourcePort(-1), destinationPort(-1), 
    time(-1), rate(-1), size(-1), isInvalid(FALSE)
{
  flowName.clear();
  sourceAddress.clear();
  destinationAddress.clear();
  pattern.clear();
  seqIdList.clear();
  timestampList.clear();
  delayList.clear();
}

void NMGFlowData::addPacketInfoToList(const int& seqId, 
                                      const double& timestamp, 
                                      const double& delay)
{
  int i = seqIdList.size();
  while(i>0 && seqIdList.at(i-1) > seqId) { --i; }
  seqIdList.insert(i, seqId);
  timestampList.insert(i, timestamp);
  delayList.insert(i, delay);
}

void NMGFlowData::fillLostSequences()
{
  int n = seqIdList.size();
  int currentSeq, prevSeq;
  double currentTimestamp, prevTimestamp;

  for(int i=1; i < n; ++i)
  {
    currentSeq = seqIdList.at(i);
    currentTimestamp = timestampList.at(i);

    prevSeq = seqIdList.at(i-1);
    prevTimestamp = timestampList.at(i-1);

    double avgTimestamp;
    int diffSeqID;

    diffSeqID = (currentSeq - prevSeq);
    if (diffSeqID > 1)
    { // have to add lost packets
      int seqId;
      double timestamp, delay = 0.0;

      avgTimestamp = (currentTimestamp - prevTimestamp) / diffSeqID;
      // filling with empty packets
      int j = 1;
      do
      {
        seqId = prevSeq + j;
        timestamp = prevTimestamp + (avgTimestamp*j);

        seqIdList.insert(i, seqId);
        timestampList.insert(i, timestamp);
        delayList.insert(i, delay);

        i++; // maintains the index to currentSeq after insertions
        j++; // registers a new lost packet insertion
        n++; // list size is being incremented by new packets
      }
      while(j < diffSeqID);
    }
  }
}
