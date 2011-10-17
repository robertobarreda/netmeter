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

#include "nmgxmlflowsortedlist.h"
#include <iostream>

using namespace std;

/**
 * Contructor of the class NMGQDomSortElement
 * @return 
 */
NMGQDomSortElement::NMGQDomSortElement():QDomElement()
{
}

/**
 * Contructor 2 of the class NMGQDomSortElement
 * @return 
 */
NMGQDomSortElement::NMGQDomSortElement(const QDomElement & x):QDomElement(x)
{
}

/**
 * Destroyer of the class NMGQDomSortElement
 * @return 
 */
NMGQDomSortElement::~NMGQDomSortElement()
{
}

/**
 * Method needed to sort elements within a list 
 * @return: the < item
 */
bool NMGQDomSortElement::operator<(const NMGQDomSortElement & x)
{
	/*
	QDomElement * elem1 = (QDomElement *)x;
	QDomElement * elem2 = (QDomElement *)this;

	int time1 = elem1->attribute(ATTR_TIME).toInt(), 
	int time2 = elem2->attribute(ATTR_TIME).toInt();

	if(time1 < time2) return false;
	else if(time1>time2) return true;
	else
	{	
		if(not elem1->hasAttribute(ATTR_FLOW_ID)) return 0;
		else
		{
			int flowId1 = elem1->attribute(ATTR_FLOW_ID).toInt(), flowId2 = elem2->attribute(ATTR_FLOW_ID).toInt();
			if(flowId1<flowId2) return -1;
			else if(flowId1>flowId2) return 1;
			else
			{ 
				if(elem1->tagName()==TAG_ON) return -1;
				else if(elem2->tagName()==TAG_ON) return 1;
				return 0;
			}
		}
	}
	*/
	// provisional:
	return true;
}

/**
 * Contructor of the class NMGXMLFlowSortedList
 * @return 
 */
NMGXMLFlowSortedList::NMGXMLFlowSortedList():QList<NMGQDomSortElement *>()
{
}

/**
 * Destroyer of the class NMGXMLFlowSortedList
 * @return 
 */
NMGXMLFlowSortedList::~NMGXMLFlowSortedList()
{
}

/**
 * Insert an element to the list keeping it as a sorted list
 * @return 
 */

void NMGXMLFlowSortedList::inSort(NMGQDomSortElement * qdse)
{
	append(qdse);
//	qSort(this->begin(), this->end());
}

/**
 * Order the list
 * @return 
 */
void NMGXMLFlowSortedList::sort()
{
	qSort(this->begin(), this->end());
}

// int QPtrList::compareItems ( QPtrCollection::Item item1, QPtrCollection::Item item2 ) [virtual protected]
// 
// This virtual function compares two list items.
// 
// Returns:
// 
//     * zero if item1 == item2
//     * nonzero if item1 != item2 
// 
// This function returns int rather than bool so that reimplementations can return three values and use it to sort by:
// 
//     * 0 if item1 == item2
//     * > 0 (positive integer) if item1 > item2
//     * < 0 (negative integer) if item1 < item2 
// 
// inSort() requires that compareItems() is implemented as described here.

/**
 * Compares two items (looking the time attribute)
 * @param item1 
 * @param item2 
 * @return 
 */
/*
int NMGXMLFlowSortedList::compareItems ( Q3PtrCollection::Item item1, Q3PtrCollection::Item item2 )
{
	QDomElement * elem1 = (QDomElement *) item1;
	QDomElement * elem2 = (QDomElement *) item2;
	int time1 = elem1->attribute(ATTR_TIME).toInt(), time2 = elem2->attribute(ATTR_TIME).toInt();
	if(time1<time2) return -1;
	else if(time1>time2) return 1;
	else
	{	
		if(not elem1->hasAttribute(ATTR_FLOW_ID)) return 0;
		else
		{
			int flowId1 = elem1->attribute(ATTR_FLOW_ID).toInt(), flowId2 = elem2->attribute(ATTR_FLOW_ID).toInt();
			if(flowId1<flowId2) return -1;
			else if(flowId1>flowId2) return 1;
			else
			{ 
				if(elem1->tagName()==TAG_ON) return -1;
				else if(elem2->tagName()==TAG_ON) return 1;
				return 0;
			}
		}
	}
}
*/
