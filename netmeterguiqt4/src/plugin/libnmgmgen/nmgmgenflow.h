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

#ifndef NMGMGENFLOW_H
#define NMGMGENFLOW_H

#include <QTreeWidget>
#include <QDomDocument>
#include <QMap>
#include <QXmlStreamReader>

#include "nmgmgenflowmodification.h"
#include "nmgmgenflowlistitem.h"
#include "nmgmgenflowrsvp.h"
#include "nmgxmlflowsortedlist.h"
#include "nmgmgenflowmanager.h"
#include "nmgmgenxmltags.h"

#define DEFAULT_FLOW_NAME	"Flow"
#define DEFAULT_FLOW_START	0
#define DEFAULT_FLOW_DURATION	1000
#define DEFAULT_FLOW_PORT	5000
#define DEFAULT_FLOW_RATE	100
#define DEFAULT_FLOW_PACKET	100
#define DEFAULT_FLOW_PATTERN	"PERIODIC"
#define DEFAULT_BURST_INTERVAL	"Regular"
#define DEFAULT_BURST_DURATION	"Fixed"


class NMGMGenFlow : public NMGMGenFlowListItem 
{
public:
    NMGMGenFlow(NMGMGenFlowManager * m, QTreeWidget * parent);
    ~NMGMGenFlow();
    
    int getId();
    void setId(int newId);
    QString getName();
    void setName(QString s);
    int getStartTime();
    void setStartTime(int startTime);
    int getDuration();
    void setDuration(int d);
    int getPort();
    void setPort(int port);
    QString getPattern();
    void setPattern(QString p);
    int getRate();
    void setRate(int r);
    int getPacketSize();
    void setPacketSize(int size);
    int getTypeOfService();
    void setTypeOfService(int tos);
    int getTrafficClass();
    void setTrafficClass(int newTrafficClass);
    int getBurstInterval();
    QString getBurstIntervalTxt();
    void setBurstInterval(QString newBurstIntervalTxt, int newBurstInterval);
    int getBurstDuration();
    QString getBurstDurationTxt();
    void setBurstDuration(QString newBurstDurationTxt, int newBurstDuration);
    void setBurst(bool state);
    bool isActiveBurst();
    void setAttributes(QString flowName, int start, int stop, int port, 
	QString pattern, int rate, int packetSize);
    void updateListItemText();
    NMGMGenFlowManager * getFlowManager();
    QTreeWidgetItem * getListViewItem();
    void addFlowToXMLNode(NMGXMLFlowSortedList & sortedList, QDomDocument& doc, NMGQDomSortElement& root, QString address, int id);
    void setParametersFromOnNode(QXmlStreamReader & xml);
    void addFlowRSVPDestinationToXMLNode(NMGXMLFlowSortedList & sortedList, QDomDocument& doc, NMGQDomSortElement& root, QString address);
    int getFlowItemType();
    NMGMGenFlowRSVP & getRSVP();
    NMGMGenFlowModification * getLastFlowModification();
    
private:
//    FlowModificationList modificationList;
    NMGMGenFlowRSVP rsvpParam;
    int id;
    QString name;
    int start;
    int duration;
    int port;
    QString pattern;
    int rate;
    int packetSize;
    NMGMGenFlowManager * manager;
    
    /* IPv4 Fields */
    int typeOfService;
    
    /* IPv6 Fields */
    int trafficClass;

    /* Burst Pattern */
    bool burst;
    QString burstIntervalTxt;
    int burstInterval;
    QString burstDurationTxt;
    int burstDuration;
};

#endif
