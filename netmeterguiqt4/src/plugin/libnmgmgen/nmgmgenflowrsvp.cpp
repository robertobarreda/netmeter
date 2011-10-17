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
#include "nmgmgenflowrsvp.h"

/**
 * Contructor of the class
 * @return 
 */
NMGMGenFlowRSVP::NMGMGenFlowRSVP()
{
	active = false;
	style = ResvWildcard;
	sourceParam = "";
	destParam = "";
}


/**
 * Destroyer of the class
 * @return 
 */
NMGMGenFlowRSVP::~NMGMGenFlowRSVP()
{

}

/**
 * Copies the parameters of the source instance
 * @param source 
 */
void NMGMGenFlowRSVP::setParameters(const NMGMGenFlowRSVP & source)
{
	active = source.active;
	style = source.style;
	sourceParam = source.sourceParam;
	destParam = source.destParam;
}

/**
 * Returns true if the rsvp is active
 * @return 
 */
bool NMGMGenFlowRSVP::isActive()
{
	return active;
}

/**
 * Sets if the rsvp is active
 * @param isActive 
 */
void NMGMGenFlowRSVP::setActive(bool isActive)
{
	active = isActive;
}

/**
 * Returns the RESV style
 * @return 
 */
NMGRSVPResvStyle NMGMGenFlowRSVP::getResvStyle()
{
	return style;
}

/**
 * Sets the RESV Style
 * @param newStyle 
 */
void NMGMGenFlowRSVP::setResvStyle(NMGRSVPResvStyle newStyle)
{
	style = newStyle;
}

/**
 * Returns the RESV style as a short string
 * @return 
 */
QString NMGMGenFlowRSVP::getResvStyleAsShortString()
{
	if(style==ResvWildcard) return SHORT_RESV_WILDCARD;
	else if(style==ResvFixedFilter) return SHORT_RESV_FIXEDFILTER;
	else return SHORT_RESV_SHAREDEXPLICIT;
}

/**
 * Sets the RESV style form a short string
 * @param shortStringStyle 
 */
void NMGMGenFlowRSVP::setResvStyleAsShortString(QString shortStringStyle)
{
	if(shortStringStyle==SHORT_RESV_WILDCARD) style=ResvWildcard;
	else if(shortStringStyle==SHORT_RESV_FIXEDFILTER) style=ResvFixedFilter;
	else style = ResvSharedExplicit;
}

/**
 * Returns the source parameter
 * @return 
 */
QString NMGMGenFlowRSVP::getSourceParam()
{
	return sourceParam;
}

/**
 * Sets the source param
 * @param newSourceParam 
 */
void NMGMGenFlowRSVP::setSourceParam(QString newSourceParam)
{
	sourceParam = newSourceParam;
}

/**
 * Returns the destination parameter
 * @return 
 */
QString NMGMGenFlowRSVP::getDestParam()
{
	return destParam;
}

/**
 * Sets the destination parameter
 * @param newSourceParam 
 */
void NMGMGenFlowRSVP::setDestParam(QString newDestParam)
{
	destParam = newDestParam;
}
	

/**
 * Adds the node of the rsvp parameter (if tis active) to the on XML element
 * @param doc XML document
 * @param on XML node with the tags <on></on>
 */
void NMGMGenFlowRSVP::addRSVPToXMLNode(QDomDocument & doc,QDomElement & on)
{
	if(!active) return;
	QDomElement rsvp = doc.createElement(TAG_RSVP);
	rsvp.setAttribute(ATTR_SPEC,getSourceParam());
/*	rsvp.setAttribute(ATTR_RSVP_SOURCE_PARAM,getSourceParam());
	rsvp.setAttribute(ATTR_RSVP_RESV_STYLE,getResvStyleAsShortString());
	rsvp.setAttribute(ATTR_RSVP_DEST_PARAM,getDestParam());*/
	on.appendChild(rsvp);
}    

