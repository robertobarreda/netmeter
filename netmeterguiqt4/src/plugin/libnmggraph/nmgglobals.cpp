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
#include "nmgglobals.h"
#include <cmath>

namespace Globals
{
  QString convertToString(ChartType type, bool extended)
  {
    QString conversion;
    switch(type)
    {
      case OWDOT:
        if(extended) conversion = QString(OWDOT_EXTENDED);
        else conversion = QString(OWDOT_ABBREV);
        break;
          
      case OWDOP:
        if(extended) conversion = QString(OWDOP_EXTENDED);
        else conversion = QString(OWDOP_ABBREV);
        break;
        
      case OWDD:
        if(extended) conversion = QString(OWDD_EXTENDED);
        else conversion = QString(OWDD_ABBREV);
        break;
  
      case IPDVOT:
        if(extended) conversion = QString(IPDVOT_EXTENDED);
        else conversion = QString(IPDVOT_ABBREV);
        break;
          
      case IPDVOP:
        if(extended) conversion = QString(IPDVOP_EXTENDED);
        else conversion = QString(IPDVOP_ABBREV);
        break;
  
      case IPDVD:
        if(extended) conversion = QString(IPDVD_EXTENDED);
        else conversion = QString(IPDVD_ABBREV);
        break;
          
      case BTC:
        if(extended) conversion = QString(BTC_EXTENDED);
        else conversion = QString(BTC_ABBREV);
        break;
    }
    return conversion;
  }
  
  int getSignificantDecimalDigits(double num)
  {
    int result = (int)floor(log10(num));
    if (result >= 0) return 0;
    else return qAbs(result);
  }
}
