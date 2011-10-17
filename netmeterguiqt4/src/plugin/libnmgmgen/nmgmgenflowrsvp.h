/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
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
#ifndef NMGMGENFLOWRSVP_H
#define NMGMGENFLOWRSVP_H

#include "nmgmgenxmltags.h"

#include <qstring.h>
#include <qdom.h>

typedef enum {ResvWildcard, ResvFixedFilter, ResvSharedExplicit} NMGRSVPResvStyle;

#define SHORT_RESV_WILDCARD		"WF"
#define SHORT_RESV_FIXEDFILTER		"FF"
#define SHORT_RESV_SHAREDEXPLICIT	"SE"

/**
@author Ernest Nieto
*/
class NMGMGenFlowRSVP
{
public:
    NMGMGenFlowRSVP();

    ~NMGMGenFlowRSVP();
    
    void setParameters(const NMGMGenFlowRSVP & source);
    
    bool isActive();
    
    void setActive(bool isActive);
    
    NMGRSVPResvStyle getResvStyle();
    
    QString getResvStyleAsShortString();
    
    void setResvStyle(NMGRSVPResvStyle newStyle);
    
    void setResvStyleAsShortString(QString shortStringStyle);
    
    QString getSourceParam();
    
    void setSourceParam(QString newSourceParam);
    
    QString getDestParam();
    
    void setDestParam(QString newDestParam);
    
    void addRSVPToXMLNode(QDomDocument & doc,QDomElement & on);
    
private:
    bool active;
    NMGRSVPResvStyle style;
    QString sourceParam;
    QString destParam;
};

#endif
