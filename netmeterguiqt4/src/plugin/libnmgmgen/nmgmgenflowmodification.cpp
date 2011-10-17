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

#include "nmgmgenflowmodification.h"
#include "nmgmgenflow.h"

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return 
 */
NMGMGenFlowModification::NMGMGenFlowModification(QTreeWidgetItem * parent)
  : NMGMGenFlowListItem(parent)
{
	start = DEFAULT_MOD_START;
	rate = DEFAULT_MOD_RATE;
	packetSize = DEFAULT_MOD_PACKET;
	pattern = DEFAULT_MOD_PATTERN;
	
	trafficClass = -1;
	typeOfService = -1;
	burst = false;
	burstIntervalTxt = DEFAULT_BURST_INTERVAL;
	burstInterval = 0;
	burstDurationTxt = DEFAULT_BURST_DURATION;
	burstDuration = 0;
	
	QString num = QString("%1").arg(parent->childCount());
	if(num.length()==1) num = "0" + num;
	setText(0,"Mod "+ num);
	
	NMGMGenFlowListItem * flowParent = (NMGMGenFlowListItem *)parent;
	NMGMGenFlowModification * lastMod = 0;
	
	if(flowParent and flowParent->getFlowItemType() == FLOW_LIST_ITEM_FLOW)
		lastMod = ((NMGMGenFlow*) flowParent)->getLastFlowModification();
	if(lastMod==this) 
		lastMod = 0;

	if(lastMod) copyParametersFromModification(lastMod);
	else if(flowParent)
	{
		if(flowParent->getFlowItemType()==FLOW_LIST_ITEM_FLOW)
			copyParametersFromFlow((NMGMGenFlow*)flowParent);
		else cerr << "Parent of the mod is not a flow" << endl;
	}
	else cerr << "The modification doesn't have parent!" << endl;

}

/**
 * Copies the paremeters of the mod instance to the modification
 * @param mod 
 */
void NMGMGenFlowModification::copyParametersFromModification(NMGMGenFlowModification * mod)
{
	if(!mod) return;
	start = mod->getStartTime()+1;
	rate = mod->getRate();
	packetSize = mod->getPacketSize();
	pattern = mod->getPattern().toUpper();
	setTrafficClass(mod->getTrafficClass());
	setTypeOfService(mod->getTypeOfService());
	getRSVP().setParameters(mod->getRSVP());
}

/**
 * Copies the paremeters of the flow instance to the modification
 * @param flow 
 */
void NMGMGenFlowModification::copyParametersFromFlow(NMGMGenFlow * flow)
{	//first Modification
	if(!flow) return;
	start = 1;
	rate = flow->getRate();
	packetSize = flow->getPacketSize();
	pattern = flow->getPattern().toUpper();
	setTrafficClass(flow->getTrafficClass());
	setTypeOfService(flow->getTypeOfService());
	getRSVP().setParameters(flow->getRSVP());
}

/**
 * Destructor of the class
 * @return 
 */
NMGMGenFlowModification::~NMGMGenFlowModification()
{
}

/**
 * Returns the start time of the flow
 * @return 
 */
int NMGMGenFlowModification::getStartTime()
{
	return start;
}

/**
 * Sets the start time of the flow
 * @param startTime 
 */
void NMGMGenFlowModification::setStartTime(int startTime)
{
	start = startTime;
}

/**
 * Returns the pattern of the flow
 * @return 
 */
QString NMGMGenFlowModification::getPattern()
{
	return pattern.toUpper();
}

/**
 * Sets the pattern of the flow
 * @param p 
 */
void NMGMGenFlowModification::setPattern(QString p)
{
	pattern = p.toUpper();
}

/**
 * Returns the rate of the flow
 * @return 
 */
int NMGMGenFlowModification::getRate()
{
	return rate;
}

/**
 * Sets the rate of the flow
 * @param r 
 */
void NMGMGenFlowModification::setRate(int r)
{
	rate = r;
}

/**
 * Returns the packet size of the flow
 * @return 
 */
int NMGMGenFlowModification::getPacketSize()
{
	return packetSize;
}

/**
 * Sets the packet size of the flow
 * @param size 
 */
void NMGMGenFlowModification::setPacketSize(int size)
{
	packetSize = size;
}

/**
 * Gets Average Interval value of Burst Pattern
 */
int NMGMGenFlowModification::getBurstInterval()
{
	return burstInterval;
}

/**
 * Gets Average Interval type of Burst Pattern
 */
QString NMGMGenFlowModification::getBurstIntervalTxt()
{
	return burstIntervalTxt;
}

/**
 * Sets Average Interval type and value and activates burst pattern
 * @param newBurstIntervalTxt 
 * @param newBurstInterval
 */
void NMGMGenFlowModification::setBurstInterval(QString newBurstIntervalTxt, int newBurstInterval)
{
	burstIntervalTxt = newBurstIntervalTxt;
	burstInterval = newBurstInterval;
}

/**
 * Gets Average Duration value of Burst Pattern
 */
int NMGMGenFlowModification::getBurstDuration()
{
	return burstDuration;
}

/**
 * Gets Average Duration type of Burst Pattern
 */
QString NMGMGenFlowModification::getBurstDurationTxt()
{
	return burstDurationTxt;
}

/**
 * Sets Average Duration type and value and activates burst pattern
 * @param newBurstDurationTxt
 * @param newBurstDuration
 */
void NMGMGenFlowModification::setBurstDuration(QString newBurstDurationTxt, int newBurstDuration)
{
	burstDurationTxt = newBurstDurationTxt;
	burstDuration = newBurstDuration;
}

/**
 * Sets state of Burts pattern
 * @param state
 */
void NMGMGenFlowModification::setBurst(bool state)
{
	burst = state;
// 	updateListItemText();
}

/**
 * Returns state of Burts pattern
 * @return false if Burts pattern is unset
 */
bool NMGMGenFlowModification::isActiveBurst()
{
	return burst;
}

/**
 * Sets the attributes of the flow
 * @param start 
 */
void NMGMGenFlowModification::setAttributes(int start, QString pattern, int rate, int packetSize)
{
	setStartTime(start);
	setPattern(pattern.toUpper());
	setRate(rate);
	setPacketSize(packetSize);
}

/**
 * Returns the list view item of the modification
 * @return 
 */
QTreeWidgetItem * NMGMGenFlowModification::getListViewItem()
{
	return this;
}

/**
 * Adds the modification flow to the xml node
 * @param doc XML Document
 * @param root Node to append
 * @param address Address of the flow
 * @param id Identifier of the flow
 */
void NMGMGenFlowModification::addModificationFlowToXMLNode(NMGXMLFlowSortedList & sortedList, QDomDocument& doc, NMGQDomSortElement & /*root*/, QString address,int id)
{
	NMGQDomSortElement *mod = new NMGQDomSortElement(doc.createElement(TAG_MOD));
	sortedList.inSort(mod);
	
	mod->setAttribute(ATTR_RATE,getRate());
	mod->setAttribute(ATTR_PATTERN,getPattern().toUpper());
	mod->setAttribute(ATTR_TIME,((NMGMGenFlowModification *) getListViewItem()->parent())->getStartTime()+getStartTime());
	mod->setAttribute(ATTR_DEST,address);
	mod->setAttribute(ATTR_SIZE,getPacketSize());
	mod->setAttribute(ATTR_FLOW_ID,id);

	rsvp.addRSVPToXMLNode(doc,*mod);

	if(isActiveBurst())
	{
		NMGQDomSortElement burstNode = doc.createElement(TAG_BURST);
		burstNode.setAttribute(ATTR_INTERVAL_TXT,getBurstIntervalTxt().toUpper());
		burstNode.setAttribute(ATTR_INTERVAL,QString("%1").arg(getBurstInterval()));
		burstNode.setAttribute(ATTR_DURATION_TXT,getBurstDurationTxt().toUpper());
		burstNode.setAttribute(ATTR_DURATION,QString("%1").arg(getBurstDuration()));
		mod->appendChild(burstNode);
	}

	if(getTypeOfService()>=0)
	{
		NMGQDomSortElement tosNode = doc.createElement(TAG_TOS);
		QDomText tosText = doc.createTextNode(QString("%1").arg(getTypeOfService()));
		tosNode.appendChild(tosText);
		mod->appendChild(tosNode);
	}
	
	if(getTrafficClass()>=0)
	{
		NMGQDomSortElement trafficClassNode = doc.createElement(TAG_TRAFFIC_CLASS);
		QDomText trafficClassText = doc.createTextNode(QString("%1").arg(getTrafficClass()));
		trafficClassNode.appendChild(trafficClassText);
		mod->appendChild(trafficClassNode);
	}
}

/**
 * Sets the parameters of the modification
 * @param xml xmlreader with the tags mod
 */
void NMGMGenFlowModification::setParametersFromModNode(QXmlStreamReader & xml)
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
				cerr << "Unknown Tag at NMGMGenFlowModification::setParametersFromModNode -->" << qPrintable(xml.name().toString()) << endl;
			}
		}
		else if (xml.isEndElement() and xml.name() == TAG_MOD)
		{
			/* If we're done we return */
			return;
		}
		xml.readNext();
	}
}

/**
 * Returns the type of the flow item
 * @return 
 */
int NMGMGenFlowModification::getFlowItemType()
{
	return FLOW_LIST_ITEM_MODIFICATION;
}

/**
 * Returns the type of service
 * @return 
 */
int NMGMGenFlowModification::getTypeOfService()
{
	return typeOfService;
}

/**
 * Returns the traffic class
 * @return 
 */
int NMGMGenFlowModification::getTrafficClass()
{
	return trafficClass;
}

/**
 * Returns the rsvp parameter
 * @return 
 */
NMGMGenFlowRSVP & NMGMGenFlowModification::getRSVP()
{
	return rsvp;
}

/**
 * Sets the type of service
 * @param newTypeOfService 
 */
void NMGMGenFlowModification::setTypeOfService(int newTypeOfService)
{
	typeOfService = newTypeOfService;
}

/**
 * Sets the traffic class
 * @param newTrafficClass 
 */
void NMGMGenFlowModification::setTrafficClass(int newTrafficClass)
{
	trafficClass = newTrafficClass;
}
