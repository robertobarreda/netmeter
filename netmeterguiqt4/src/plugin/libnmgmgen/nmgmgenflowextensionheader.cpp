/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
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
#include "nmgmgenflowextensionheader.h"

/**
 * Constructor of the class
 * @param parent 
 * @return 
 */
NMGMGenFlowExtensionHeader::NMGMGenFlowExtensionHeader(QTreeWidgetItem * parent)
 : NMGMGenFlowListItem(parent)
{
	optionStartTime = 0;
	optionStopTime = 1000;
	type = BindingUpdate;
	optionalParameter = "";
}


/**
 * Destructor of the class
 * @return 
 */
NMGMGenFlowExtensionHeader::~NMGMGenFlowExtensionHeader()
{
}

/**
 * Returns the type of the flow list item
 * @return 
 */
int NMGMGenFlowExtensionHeader::getFlowItemType()
{
	return FLOW_LIST_ITEM_EXTENSION_HEADER;
}

/**
 * Returns the start time
 * @return 
 */
int NMGMGenFlowExtensionHeader::getOptionStartTime()
{
	return optionStartTime;
}


/**
 * Setst the start time
 * @param startTime 
 */
void NMGMGenFlowExtensionHeader::setOptionStartTime(int startTime)
{
	optionStartTime = startTime;
	
}

/**
 * Returns the stop time
 * @return 
 */
int NMGMGenFlowExtensionHeader::getOptionStopTime()
{
	return optionStopTime;
}

/**
 * Sets the stop time
 * @param stopTime 
 */
void NMGMGenFlowExtensionHeader::setOptionStopTime(int stopTime)
{
	optionStopTime = stopTime;
}

/**
 * Returns the extension header type
 * @return 
 */
NMGMGenFlowExtensionHeaderType NMGMGenFlowExtensionHeader::getExtensionHeaderType()
{
	return type;
}

/**
 * Sets the extension header type
 * @param t 
 */
void NMGMGenFlowExtensionHeader::setExtensionHeaderType(NMGMGenFlowExtensionHeaderType t)
{
	type = t;
}

/**
 * Returns a string associated to the extension header type
 * @return 
 */
QString NMGMGenFlowExtensionHeader::getExtensionHeaderTypeAsString()
{

	switch(type)
	{
		case BindingUpdate: return STR_BINDING_UPDATE;
		case BindingAck: return STR_BINDING_ACK;
		case BindingRequest: return STR_BINDING_REQUEST;
		case RoutingHeader: return STR_ROUTING_HEADER;
		case HomeAddress: return STR_HOME_ADDRESS;
		case RouterAlert: return STR_ROUTER_ALERT;
		case JumboPayload: return STR_JUMBO_PAYLOAD;
		default: return "";
	}
}
    
/**
 * Sets the extension header type from a string
 * @param s 
 */
void NMGMGenFlowExtensionHeader::setExtensionHeaderTypeFromString(QString s)
{
	if(s==STR_BINDING_UPDATE) type = BindingUpdate;
	else if(s==STR_BINDING_ACK) type = BindingAck;
	else if(s==STR_BINDING_REQUEST) type = BindingRequest;
	else if(s==STR_ROUTING_HEADER) type = RoutingHeader;
	else if(s==STR_HOME_ADDRESS) type = HomeAddress;
	else if(s==STR_ROUTER_ALERT) type = RouterAlert;
	else if(s==STR_JUMBO_PAYLOAD) type= JumboPayload;
	else type = BindingUpdate; /* Binding Update As default */
}

/**
 * Returns the optional parameter if needed
 * @return 
 */
QString NMGMGenFlowExtensionHeader::getOptionalParameter()
{
	return optionalParameter;
}

/**
 * Sets the optional parameter
 * @param param 
 */
void NMGMGenFlowExtensionHeader::setOptionalParameter(QString param)
{
	optionalParameter = param;
}

