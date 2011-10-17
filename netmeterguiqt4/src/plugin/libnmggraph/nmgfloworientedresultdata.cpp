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
#include "nmgfloworientedresultdata.h"

NMGFlowOrientedResultData::NMGFlowOrientedResultData()
  : QMap<int,NMGFlowData*>()
{
  clear();
}

NMGFlowOrientedResultData::~NMGFlowOrientedResultData()
{
  QMap<int,NMGFlowData*>::iterator it;
  for(it = begin(); it != end(); ++it) 
  {
    delete it.value(); // needed because value is a pointer with a custom structure
  }
}

NMGFlowOrientedResultData::NMGFlowOrientedResultData(const NMGFlowOrientedResultData& object)
{
  QMap<int,NMGFlowData*>::const_iterator it;
  for(it = object.constBegin(); it != object.constEnd(); ++it)
  {
    insertMulti(it.key(), new NMGFlowData(*it.value())); // implicit NMGFlowData copy-constructor
  }
}

NMGFlowOrientedResultData& NMGFlowOrientedResultData::operator=(const NMGFlowOrientedResultData& object)
{
  if(&object != this)
  { // avoid auto-copy

    // deleted previous data in the implicit object
    QMap<int,NMGFlowData*>::iterator it;
    for(it = begin(); it != end(); ++it) 
    {
      delete it.value(); // needed because value is a pointer with a custom structure
    }
    clear();

    // copy the new data in the implicit object
    QMap<int,NMGFlowData*>::const_iterator it2;
    for(it2 = object.constBegin(); it2 != object.constEnd(); ++it2)
    {
      insertMulti(it2.key(), new NMGFlowData(*it2.value()));
    }
  }
  return *this;
}

void NMGFlowOrientedResultData::addFlowData(int flowId, NMGFlowData* flow) 
{
  insertMulti(flowId, flow);
}

void NMGFlowOrientedResultData::deleteFlowData(int flowId)
{
  QMap<int,NMGFlowData*>::iterator it = find(flowId);
  if(it != end())
  {
    delete it.value();
    erase(it);
  }
}

bool NMGFlowOrientedResultData::isDataInvalid() const
{
  bool invalid = FALSE;
  QMap<int,NMGFlowData*>::const_iterator it;
  for(it = constBegin(); it != constEnd(); it++)
  {
    invalid = invalid || it.value()->isDataInvalid();
  }
  return invalid;
}
