/*
 *   $Id: nmexemple.cpp,v 1.1 2006-02-27 23:12:11 enieto Exp $
 *
 *   Copyright (c) 2006 Pau Capella
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "nmexemple.h"

NMExemple::NMExemple()
{
}


NMExemple::~NMExemple()
{
}

void NMExemple::initModule()
{

}

void NMExemple::removeModule()
{

}

void NMExemple::start(void *data)
{
	NMString *nms_xml = (NMString *) data;
	NMXML nmxml_data;
	cout << "DATA:"<< nms_xml->c_str() << endl;
	NMXML xmlPing;
	NMString nms_pingresult;

	
	nmxml_data.loadTree(nms_xml->c_str());
	
	NMString value = nmxml_data.getValue("/nmexemple");
		
	xmlPing.newTree("data_received");
	xmlPing.addData("here", value);
	
	nms_pingresult = xmlPing.toString();
	xmlPing.freeTree();
	*this << nms_pingresult;
	exit();
}

void NMExemple::stop(void *data)
{

}

void NMExemple::status(void *data)
{

}

void NMExemple::configure(void *data)
{

}

void NMExemple::processRecived()
{

}

void NMExemple::processExit()
{

}
