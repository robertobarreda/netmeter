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
#ifndef NMGMGENFLOWEXTENSIONHEADER_H
#define NMGMGENFLOWEXTENSIONHEADER_H

#include "nmgmgenflowlistitem.h"

typedef enum{BindingUpdate,BindingAck,BindingRequest,RoutingHeader,HomeAddress,RouterAlert,JumboPayload} NMGMGenFlowExtensionHeaderType;

#define STR_BINDING_UPDATE	"Binding update"
#define STR_BINDING_ACK		"Binding ack"
#define STR_BINDING_REQUEST	"Binding request"
#define STR_ROUTING_HEADER	"Routing header"
#define STR_HOME_ADDRESS	"Home address"
#define STR_ROUTER_ALERT	"Router alert"
#define STR_JUMBO_PAYLOAD	"Jumbo payload"


/**
@author Ernest Nieto
*/
class NMGMGenFlowExtensionHeader : public NMGMGenFlowListItem
{
public:
    
    NMGMGenFlowExtensionHeader(QTreeWidgetItem *parent);
    ~NMGMGenFlowExtensionHeader();
    
    int getFlowItemType();
    int getOptionStartTime();
    void setOptionStartTime(int startTime);
    int getOptionStopTime();
    void setOptionStopTime(int stopTime);
    NMGMGenFlowExtensionHeaderType getExtensionHeaderType();
    QString getExtensionHeaderTypeAsString();
    void setExtensionHeaderType(NMGMGenFlowExtensionHeaderType t);
    void setExtensionHeaderTypeFromString(QString s);
    QString getOptionalParameter();
    void setOptionalParameter(QString param);
    
    private:
    int optionStartTime;
    int optionStopTime;
    NMGMGenFlowExtensionHeaderType type;
    QString optionalParameter;
};

#endif
