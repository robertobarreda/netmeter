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
#include "nmggraphaggregationorienteddataparser.h"
#include <nmgaggregationorientedtags.h>

NMGGraphAggregationOrientedDataParser::NMGGraphAggregationOrientedDataParser(NMGAggregationOrientedResultData* data)
  :NMGAggregationOrientedDataParser()
{
  this->data = data;
}

NMGGraphAggregationOrientedDataParser::~NMGGraphAggregationOrientedDataParser()
{}

void NMGGraphAggregationOrientedDataParser::doProcess() 
{
  // we save all common data
  data->setSourceAddress(sourceAddress);
  data->setDestinationAddress(destinationAddress);
  data->setReceivingSocketSizeInBytes(rss);
  data->setSendingSocketSizeInBytes(sss);
  data->setSendingMessagesSizeInBytes(sms);
  data->setElapsedTimeInSeconds(time);
  data->setReceivingThroughputInKBitsPerSecond(tputRecv);
  data->setTypeOfProtocol(typeOfProtocol);

  // and now the uncommon data
  if(typeOfProtocol == TAG_UDP)
  {
    data->setSendingThroughputInKBitsPerSecond(tputSent);
    data->setSendingOkMessages(okMsgSent);
    data->setSendingKoMessages(koMsgSent);
    data->setReceivingOkMessages(okMsgRecv);
  }
}
