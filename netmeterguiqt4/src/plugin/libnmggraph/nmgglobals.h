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
#ifndef NMGGLOBALS_H
#define NMGGLOBALS_H

#include <QString>

// this internal separator must be the same as the value defined on SEPARATOR into
// <nmggrouptreewidget.h> to avoid unexpected results. This is done like this because
// SEPARATOR is not visible for all classes in the plugin.
#define INTERNAL_SEPARATOR "#"

// scene labelling

#define OWDOT_EXTENDED    "One Way Delay over Time"
#define OWDOP_EXTENDED    "One Way Delay over Packets"
#define OWDD_EXTENDED     "One Way Delay Distribution"
#define IPDVOT_EXTENDED   "Inter Packet Delay Variation over Time"
#define IPDVOP_EXTENDED   "Inter Packet Delay Variation over Packets"
#define IPDVD_EXTENDED    "Inter Packet Delay Variation Distribution"
#define BTC_EXTENDED      "Bulk Transport Capacity"

#define OWDOT_ABBREV      "OWDoT"
#define OWDOP_ABBREV      "OWDoP"
#define OWDD_ABBREV       "OWDD"
#define IPDVOT_ABBREV     "IPDVoT"
#define IPDVOP_ABBREV     "IPDVoP"
#define IPDVD_ABBREV      "IPDVD"
#define BTC_ABBREV        "BTC"

#define TIME_FROM_START   "Time from start (msec)"
#define DELAY             "Delay (msec)"
#define IPDV_DELAY        "IPDV (msec)"
#define PACKET_SEQUENCE   "Packet sequence number"
#define PERCENT_PACKETS   "% of packets"
#define RECEIVER_TPUT     "Recv. Throughput (KBits/s)"
#define TESTS             "Tests"

// test reader thread

#define TYPE_PACKETS    "PACKETS"
#define TYPE_TPUT       "THROUGHPUT"

// measure list components

#define KILOFACTOR      1000
#define MEGAFACTOR      1000000
#define NOT_AVAILABLE   "N/A"

// series representation

#define LINE_TYPE       "LINE"
#define AREA_TYPE       "AREA"
#define POINTS_TYPE     "POINTS"
#define BARS_TYPE       "BARS"

// legend positions

#define NORTH         "North"
#define NORTHEAST     "Northeast"
#define EAST          "East"
#define SOUTHEAST     "Southeast"
#define SOUTH         "South"
#define SOUTHWEST     "Southwest"
#define WEST          "West"
#define NORTHWEST     "Northwest"
#define CENTER        "Center"

namespace Globals
{ 
  // test file data, test reader thread , chart scene and measure list component
  
  /** \brief This enum describes the supported test file types by the graphic analysis plugin. */
  enum SupportedType 
  {
    PACKETS,
    AGGREGATION
  };
  
  // chart scene
  
  /** 
   * \brief This enum describes the chart types available for \e SupportedType, used to
   * paint data in different ways into NMGPlotWidget. 
   * 
   * The supported chart types at the moment are: OWDOT, OWDOP, OWDD, IPDVOT, IPDVOP, 
   * IPDVD and BTC. Not all types can be used in a specific test file, because they may
   * have not sense in that test. For example, BTC is only available when the data to be
   * painted belongs to a test file of AGGREGATION type.
   */
  enum ChartType
  {
    OWDOT,   /**< One Way Delay Over Time. */
    OWDOP,   /**< One Way Delay Over Packets. */
    OWDD,    /**< One Way Delay Distribution. */
    IPDVOT,  /**< Inter Packet Delay Variation Over Time. */
    IPDVOP,  /**< Inter Packet Delay Variation Over Packets. */
    IPDVD,   /**< Inter Packet Delay Variation Distribution. */
    BTC      /**< Bulk Transport Capacity. */
  };
  
  /** 
   * \brief Maps \e type enumeration into the corresponding string.
   * \param type Enumeration to convert.
   * \param extended True for a complete string; False for an abbreviated one
   */
  QString convertToString(ChartType type, bool extended = FALSE);
  
  /** 
   * \brief Returns the most significat decimals for the entry number.
   * \param num Entry number.
   */
  int getSignificantDecimalDigits(double num);

};

#endif
