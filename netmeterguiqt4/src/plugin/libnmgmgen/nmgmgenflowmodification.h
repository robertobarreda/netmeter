/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
 *   Copyright (C) 2007 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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
#ifndef NMGMGENFLOWMODIFICATION_H
#define NMGMGENFLOWMODIFICATION_H

#include "nmgmgenflowlistitem.h"

#include <QString>
#include <QDomDocument>
#include <QXmlStreamReader>

class NMGMGenFlow;

#include "nmgmgenflowrsvp.h"
#include "nmgxmlflowsortedlist.h"

#include "nmgmgenxmltags.h"

#define DEFAULT_MOD_START	1
#define DEFAULT_MOD_RATE	100
#define DEFAULT_MOD_PACKET	100
#define DEFAULT_MOD_PATTERN	"PERIODIC"
#define DEFAULT_BURST_INTERVAL	"Regular"
#define DEFAULT_BURST_DURATION	"Fixed"



class NMGMGenFlowModification : public NMGMGenFlowListItem
{
public:
    NMGMGenFlowModification(QTreeWidgetItem * parent);

    ~NMGMGenFlowModification();

    void copyParametersFromModification(NMGMGenFlowModification * mod);
    void copyParametersFromFlow(NMGMGenFlow * flow);
    int getStartTime();
    void setStartTime(int startTime);
    QString getPattern();
    void setPattern(QString p);
    int getRate();
    void setRate(int r);
    int getPacketSize();
    void setPacketSize(int size);
    int getBurstInterval();
    QString getBurstIntervalTxt();
    void setBurstInterval(QString newBurstIntervalTxt, int newBurstInterval);
    int getBurstDuration();
    QString getBurstDurationTxt();
    void setBurstDuration(QString newBurstDurationTxt, int newBurstDuration);
    void setBurst(bool state);
    bool isActiveBurst();
    void setAttributes(int start, QString pattern, int rate, int packetSize);
    QTreeWidgetItem * getListViewItem();
    void addModificationFlowToXMLNode(NMGXMLFlowSortedList & sortedList, QDomDocument& doc, NMGQDomSortElement & root,QString address,int id);
    void setParametersFromModNode(QXmlStreamReader & xml);
    int getFlowItemType();
    int getTypeOfService();
    int getTrafficClass();
    NMGMGenFlowRSVP & getRSVP();
    void setTypeOfService(int newTypeOfService);
    void setTrafficClass(int trafficClass);

private:
    int start;
    QString pattern;
    int rate;
    int packetSize;
    NMGMGenFlowRSVP rsvp;
    int typeOfService;
    int trafficClass;

    /* Burst Pattern */
    bool burst;
    QString burstIntervalTxt;
    int burstInterval;
    QString burstDurationTxt;
    int burstDuration;
};

#endif
