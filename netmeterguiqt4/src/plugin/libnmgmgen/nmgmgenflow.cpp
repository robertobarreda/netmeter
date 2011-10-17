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
#include "nmgmgenflow.h"

#include <iostream>
using namespace std;

#include "nmgmgenflowmanager.h"

/**
 * Constructor of the class
 * @return 
 */
NMGMGenFlow::NMGMGenFlow(NMGMGenFlowManager * m, QTreeWidget * parent)
: NMGMGenFlowListItem(parent)
{
	id = 0;
	manager = m;

	name = DEFAULT_FLOW_NAME;
	start = DEFAULT_FLOW_START;
	duration = DEFAULT_FLOW_DURATION;
	port = manager->getNextFreePort();
	rate = DEFAULT_FLOW_RATE;
	packetSize = DEFAULT_FLOW_PACKET;
	pattern = DEFAULT_FLOW_PATTERN;
	typeOfService = -1;
	trafficClass = -1;
	burst = false;
	burstIntervalTxt = DEFAULT_BURST_INTERVAL;
	burstInterval = 0;
	burstDurationTxt = DEFAULT_BURST_DURATION;
	burstDuration = 0;

	manager->addFlow(this);
	updateListItemText();
}

/**
 * Destructor of the class
 * @return 
 */
NMGMGenFlow::~NMGMGenFlow()
{
}

/**
 * Returns the id of the flow
 * @return 
 */
int NMGMGenFlow::getId()
{
	return id;
}

/**
 * Sets the id of the flow
 * @param newId 
 */
void NMGMGenFlow::setId(int newId)
{
	id = newId;
	updateListItemText();
}

/**
 * Returns the name of the class
 * @return 
 */
QString NMGMGenFlow::getName()
{
	return name;
}

/**
 * Sets the name of the class
 * @param s 
 */
void NMGMGenFlow::setName(QString s)
{
	name = s;
	//updateListItemText();
}

/**
 * Returns the start time of the flow
 * @return 
 */
int NMGMGenFlow::getStartTime()
{
	return start;
}

/**
 * Sets the start time of the flow
 * @param startTime 
 */
void NMGMGenFlow::setStartTime(int startTime)
{
	start = startTime;
	//updateListItemText();
}

/**
 * Returns the stop time of the flow
 * @return 
 */
int NMGMGenFlow::getDuration()
{
	return duration;
}

/**
 * Sets the stop time of the flow
 * @param stopTime 
 */
void NMGMGenFlow::setDuration(int newDuration)
{
	duration = newDuration;
	//updateListItemText();
}

/**
 * Return the port of the flow
 * @return 
 */
int NMGMGenFlow::getPort()
{
	return port;
}

/**
 * Sets the port of the flow
 * @param port 
 */
void NMGMGenFlow::setPort(int p)
{
	if(p!=port)
	{
		manager->removeFlow(port);
		port = p;
		manager->addFlow(this);
		//updateListItemText();
	}
	else port = p;
}

/**
 * Returns the pattern of the flow
 * @return 
 */
QString NMGMGenFlow::getPattern()
{
	return pattern.toUpper();
}

/**
 * Sets the pattern of the flow
 * @param p 
 */
void NMGMGenFlow::setPattern(QString p)
{
	pattern = p.toUpper();
	//updateListItemText();
}

/**
 * Returns the rate of the flow
 * @return 
 */
int NMGMGenFlow::getRate()
{
	return rate;
}

/**
 * Sets the rate of the flow
 * @param r 
 */
void NMGMGenFlow::setRate(int r)
{
	rate = r;
	//updateListItemText();
}

/**
 * Returns the packet size of the flow
 * @return 
 */
int NMGMGenFlow::getPacketSize()
{
	return packetSize;
}

/**
 * Sets the packet size of the flow
 * @param size 
 */
void NMGMGenFlow::setPacketSize(int size)
{
	packetSize = size;
	//updateListItemText();
}

/**
 * Returns the type of service of the flow (from IPv4)
 * @return if -1 its not used
 */
int NMGMGenFlow::getTypeOfService()
{
	return typeOfService;
}

/**
 * Sets the TOS of the flow
 */
void NMGMGenFlow::setTypeOfService(int tos)
{
	typeOfService = tos;
	if(tos>=0) trafficClass = -1;
	//updateListItemText();
}

/**
 * Returns the traffic class  of the flow (from IPV6)
 * @return if -1 its not used
 */
int NMGMGenFlow::getTrafficClass()
{
	return trafficClass;
}

/**
 * Sets the traffic class of the flow
 * @param newTrafficClass 
 */
void NMGMGenFlow::setTrafficClass(int newTrafficClass)
{
	trafficClass = newTrafficClass;
	if(trafficClass>=0) typeOfService = -1;
	//updateListItemText();
}

/**
 * Gets Average Interval value of Burst Pattern
 */
int NMGMGenFlow::getBurstInterval()
{
	return burstInterval;
}

/**
 * Gets Average Interval type of Burst Pattern
 */
QString NMGMGenFlow::getBurstIntervalTxt()
{
	return burstIntervalTxt;
}

/**
 * Sets Average Interval type and value and activates burst pattern
 * @param newBurstIntervalTxt 
 * @param newBurstInterval
 */
void NMGMGenFlow::setBurstInterval(QString newBurstIntervalTxt, int newBurstInterval)
{
	burstIntervalTxt = newBurstIntervalTxt;
	burstInterval = newBurstInterval;
}

/**
 * Gets Average Duration value of Burst Pattern
 */
int NMGMGenFlow::getBurstDuration()
{
	return burstDuration;
}

/**
 * Gets Average Duration type of Burst Pattern
 */
QString NMGMGenFlow::getBurstDurationTxt()
{
	return burstDurationTxt;
}

/**
 * Sets Average Duration type and value and activates burst pattern
 * @param newBurstDurationTxt
 * @param newBurstDuration
 */
void NMGMGenFlow::setBurstDuration(QString newBurstDurationTxt, int newBurstDuration)
{
	burstDurationTxt = newBurstDurationTxt;
	burstDuration = newBurstDuration;
}

/**
 * Sets state of Burts pattern
 * @param state
 */
void NMGMGenFlow::setBurst(bool state)
{
	burst = state;
	//updateListItemText();
}

/**
 * Returns state of Burts pattern
 * @return false if Burts pattern is unset
 */
bool NMGMGenFlow::isActiveBurst()
{
	return burst;
}

/**
 * Sets the atttributes of the flow
 * @param flowName 
 * @param start 
 * @param duration
 * @param port 
 * @param pattern 
 * @param rate 
 * @param packetSize 
 */
void NMGMGenFlow::setAttributes(QString flowName, int start, int duration, int port, 
	QString pattern, int rate, int packetSize)
{
	setName(flowName);
	setStartTime(start);
	setDuration(duration);
	setPort(port);
	setPattern(pattern.toUpper());
	setRate(rate);
	setPacketSize(packetSize);
}

/**
 * Updates the text shown for the list item
 * @return 
 */
void NMGMGenFlow::updateListItemText()
{
	QString flowId = QString("%1").arg(getId());
	if(flowId.length() == 1) flowId = "0" + flowId;
	if(!flowId.isEmpty())	setText(0,"Flow " + flowId);
}

/**
 * Returns the list view item corresponding to the flow
 * @return 
 */
QTreeWidgetItem * NMGMGenFlow::getListViewItem()
{
	return this;
}

/**
 * Returns the flow manager
 * @return 
 */
NMGMGenFlowManager * NMGMGenFlow::getFlowManager()
{
	return manager;
}

/**
 * Adds the xml node of the flow to the XML document at the element root
 * @param doc XML Document
 * @param root Node where the flow will be inserted
 */
void NMGMGenFlow::addFlowToXMLNode(NMGXMLFlowSortedList & sortedList, QDomDocument& doc, NMGQDomSortElement& root, QString address, int id)
{
	NMGQDomSortElement *on = new NMGQDomSortElement(doc.createElement(TAG_ON));
	sortedList.inSort(on);

	address = address + "/"+QString("%1").arg(getPort());
	on->setAttribute(ATTR_NAME,getName());
	on->setAttribute(ATTR_RATE,getRate());
	on->setAttribute(ATTR_PATTERN,getPattern().toUpper());
	on->setAttribute(ATTR_TIME,getStartTime());
	on->setAttribute(ATTR_DEST,address);
	on->setAttribute(ATTR_SIZE,getPacketSize());
	on->setAttribute(ATTR_FLOW_ID,id);
	
	rsvpParam.addRSVPToXMLNode(doc,*on);

	if(isActiveBurst())
	{
		NMGQDomSortElement burstNode = doc.createElement(TAG_BURST);
		burstNode.setAttribute(ATTR_INTERVAL_TXT,getBurstIntervalTxt().toUpper());
		burstNode.setAttribute(ATTR_INTERVAL,QString("%1").arg(getBurstInterval()));
		burstNode.setAttribute(ATTR_DURATION_TXT,getBurstDurationTxt().toUpper());
		burstNode.setAttribute(ATTR_DURATION,QString("%1").arg(getBurstDuration()));
		on->appendChild(burstNode);
	}

	if(getTypeOfService()>=0)
	{
		NMGQDomSortElement tosNode = doc.createElement(TAG_TOS);
		QDomText tosText = doc.createTextNode(QString("%1").arg(getTypeOfService()));
		tosNode.appendChild(tosText);
		on->appendChild(tosNode);
	}
	
	if(getTrafficClass()>=0)
	{
		NMGQDomSortElement trafficClassNode = doc.createElement(TAG_TRAFFIC_CLASS);
		QDomText trafficClassText = doc.createTextNode(QString("%1").arg(getTrafficClass()));
		trafficClassNode.appendChild(trafficClassText);
		on->appendChild(trafficClassNode);
	}

	NMGMGenFlow *flow = (NMGMGenFlow *)getListViewItem();
	int fills = flow->childCount();

	if(fills > 0)
	{
		for(int j=0; j<fills; j++)
		{
			NMGMGenFlowListItem * listItem = (NMGMGenFlowListItem *)flow->child(j);
			if(listItem->getFlowItemType()==FLOW_LIST_ITEM_MODIFICATION)
			{
				NMGMGenFlowModification * mod = (NMGMGenFlowModification *)flow->child(j);
				mod->addModificationFlowToXMLNode(sortedList,doc,root,address,id);
			}
		}
	}

	NMGQDomSortElement * off = new NMGQDomSortElement(doc.createElement(TAG_OFF));
	sortedList.inSort(off);
	off->setAttribute(ATTR_TIME, getStartTime()+getDuration());
	off->setAttribute(ATTR_FLOW_ID, id);
}

/**
 * Sets the parameter of the flow from a xml node (tag on)
 * @param elem Element with on tags
 */
void NMGMGenFlow::setParametersFromOnNode(QXmlStreamReader & xml)
{
	while(!xml.atEnd())
	{
		if (xml.isStartElement())
		{
			if (xml.name() == TAG_RSVP)
			{
				getRSVP().setActive(true);
				getRSVP().setSourceParam(xml.attributes().value(ATTR_SPEC).toString());
			}
			else if(xml.name() == TAG_BURST)
			{
				int intervalint = xml.attributes().value(ATTR_INTERVAL).toString().toInt();
				QString intervaltxt = xml.attributes().value(ATTR_INTERVAL_TXT).toString();
				int durationint = xml.attributes().value(ATTR_DURATION).toString().toInt();
				QString durationtxt = xml.attributes().value(ATTR_DURATION_TXT).toString();

				setBurstInterval(intervaltxt, intervalint);
				setBurstDuration(durationtxt, durationint);
				setBurst(true);
			}
			else if(xml.name() == TAG_TOS)
			{
				setTypeOfService(xml.readElementText().toInt());
			}
			else if(xml.name() == TAG_TRAFFIC_CLASS)
			{
				setTrafficClass(xml.readElementText().toInt());
			}
			else
			{
				cerr << "Unknown Tag at NMGMGenFlow::setParametersFromOnNode -->" << qPrintable(xml.name().toString()) << endl;
			}
		}
		else if (xml.isEndElement() and xml.name() == TAG_ON)
		{
			/* If we're done we return */
			return;
		}
		xml.readNext();
	}
}
/*	for(QDomNode node = elem.firstChild(); not node.isNull(); node = node.nextSibling())
	{
		if(node.isElement())
		{
			NMGQDomSortElement child = node.toElement();
			if(child.tagName()==TAG_RSVP)
			{
				getRSVP().setActive(true);
				getRSVP().setSourceParam(child.attribute(ATTR_SPEC));
			}
			else if(child.tagName()==TAG_BURST)
			{
				setBurstInterval(child.attribute(ATTR_INTERVAL_TXT),
							child.attribute(ATTR_INTERVAL).toInt());
				setBurstDuration(child.attribute(ATTR_DURATION_TXT),
							child.attribute(ATTR_DURATION).toInt());
				setBurst(true);
			}
			else if(child.tagName()==TAG_TOS) setTypeOfService(child.text().toInt());
			else if(child.tagName()==TAG_TRAFFIC_CLASS) setTrafficClass(child.text().toInt());
			else cerr<<"Unknown tag for the on node of a flow ("<< qPrintable(child.tagName()) << ")." << endl;
		}
	}
*/

/**
 * Adds the flow RSVP part for the destination of the test
 * @param doc 
 * @param root 
 * @param address 
 */
void NMGMGenFlow::addFlowRSVPDestinationToXMLNode(NMGXMLFlowSortedList & sortedList, QDomDocument& doc, NMGQDomSortElement& /*root*/, QString address)
{
	bool addedRSVP = false;
	if(rsvpParam.isActive())
	{
		addedRSVP = true;
		NMGQDomSortElement *resv = new NMGQDomSortElement(doc.createElement(TAG_RESV));
		sortedList.inSort(resv);
		resv->setAttribute(ATTR_TIME,QString("%1").arg(getStartTime()));
		resv->setAttribute(ATTR_DEST,address+"/"+QString("%1").arg(getPort()));
		resv->setAttribute(ATTR_STYLE,getRSVP().getResvStyleAsShortString());
		resv->setAttribute(ATTR_SPEC,getRSVP().getDestParam());
	}

	NMGMGenFlow *flow = (NMGMGenFlow *)getListViewItem();
	int fills = flow->childCount();

	if(fills > 0)
	{
		for(int j=0; j<fills; j++)
		{
       			NMGMGenFlowListItem * listItem = (NMGMGenFlowListItem *)flow->child(j);
			if(listItem->getFlowItemType()==FLOW_LIST_ITEM_MODIFICATION)
			{
				NMGMGenFlowModification * mod =  (NMGMGenFlowModification *)flow->child(j);
				if(mod->getRSVP().isActive())
				{
					addedRSVP = true;
					NMGQDomSortElement * resv = new NMGQDomSortElement(doc.createElement(TAG_RESV));
					sortedList.inSort(resv);
					resv->setAttribute(ATTR_TIME,QString("%1").arg(getStartTime()+mod->getStartTime()));
					resv->setAttribute(ATTR_DEST,address+"/"+QString("%1").arg(getPort()));
					resv->setAttribute(ATTR_STYLE,mod->getRSVP().getResvStyleAsShortString());
					resv->setAttribute(ATTR_SPEC,mod->getRSVP().getDestParam());
				}
			}
		}
	}
	
	if(addedRSVP)
	{
		NMGQDomSortElement *unresv = new NMGQDomSortElement(doc.createElement(TAG_UNRESV));
		sortedList.inSort(unresv);
		unresv->setAttribute(ATTR_TIME,QString("%1").arg(getStartTime()+getDuration()));
		unresv->setAttribute(ATTR_DEST,address+"/"+QString("%1").arg(getPort()));
	}
}

/**
 * Returns the type of the flow item
 * @return 
 */
int NMGMGenFlow::getFlowItemType()
{
	return FLOW_LIST_ITEM_FLOW;
}

/**
 * Returns the RSVP parameter
 * @return 
 */
NMGMGenFlowRSVP & NMGMGenFlow::getRSVP()
{
	return rsvpParam;
}

/**
 * Returns the modification with greater start time of the flow (or if there isn't anyone)
 * @return 
 */
NMGMGenFlowModification * NMGMGenFlow::getLastFlowModification()
{
	int maxStartTime = -1;
	NMGMGenFlowModification * max = 0;
	QTreeWidgetItem * item = getListViewItem();
	int n = item->childCount();
	int i = 0;

	while (i < n)
	{
		NMGMGenFlowListItem * listItem = (NMGMGenFlowListItem *)item->child(i);

		if(listItem->getFlowItemType()==FLOW_LIST_ITEM_MODIFICATION)
		{
			if(((NMGMGenFlowModification *)listItem)->getStartTime()>maxStartTime)
			{
				max = ((NMGMGenFlowModification *)listItem);
				maxStartTime = max->getStartTime();
			}
		i++;	
		}
	}

	if(maxStartTime<0) return 0;
	else return max;
}

