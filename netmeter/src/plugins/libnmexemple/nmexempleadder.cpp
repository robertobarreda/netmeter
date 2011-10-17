/*
 *   $Id: nmexempleadder.cpp,v 1.1 2006-03-09 15:03:40 enieto Exp $
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
#include "nmexempleadder.h"

NMExempleAdder::NMExempleAdder()
{
}


NMExempleAdder::~NMExempleAdder()
{
}


void NMExempleAdder::initModule()
{

}

void NMExempleAdder::removeModule()
{

}

void NMExempleAdder::start(void *data)
{
	NMString *nms_xml = (NMString *) data;
	NMXML nmxml_data;
	cout << "DATA:"<< nms_xml->c_str() << endl;

	
	nmxml_data.loadTree(nms_xml->c_str());
	
		
	
	NMString value1 = nmxml_data.getValue("/*/value1");
	if(!value1.Length()) {
		*this << NMError(NMError::user, "No value 1");
		nmxml_data.freeTree();
		return;
	}
	

	NMString value2 = nmxml_data.getValue("/*/value2");
	if(!value2.Length()) {
		*this << NMError(NMError::user, "No value 2");
		nmxml_data.freeTree();
		return;
	}

	int addValue = value1.toInt() + value2.toInt();
	NMString value =NMString::number(addValue);

	NMXML xmlAdd;
	xmlAdd.newTree("data");
	xmlAdd.addData("add", value);
	
	NMString addResult = xmlAdd.toString();
	xmlAdd.freeTree();
	*this << addResult;
	exit();
}

void NMExempleAdder::stop(void *data)
{

}

void NMExempleAdder::status(void *data)
{

}

void NMExempleAdder::configure(void *data)
{

}

void NMExempleAdder::processRecived()
{

}

void NMExempleAdder::processExit()
{

}


