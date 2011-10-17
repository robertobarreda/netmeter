/*
 *   $Id: nmsnooper.cpp,v 1.2 2006-03-27 10:53:13 pcapella Exp $
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
#include "nmsnooper.h"
#include <nmxml.h>
#include <unistd.h>

NMSnooper::NMSnooper()
 : NMModuleInterface()
{
}


NMSnooper::~NMSnooper()
{
}

void NMSnooper::initModule()
{
	m_sec = 0;
	m_isrunning = false;
}

void NMSnooper::removeModule()
{
}

void NMSnooper::start(void *data)
{
	if ( m_isrunning) return;

	NMString *nms_xml = (NMString *) data;
	if(nms_xml->Length()) configure(data);

	m_isrunning = true;
	while ( m_isrunning ) {
		status( 0);
		sleep( m_sec);
	}

	NMXML xmlok;
	xmlok.newTree("ok");
	//Send ok....
	*this << xmlok.toString();
	xmlok.freeTree();
	exit();
}

void NMSnooper::stop(void *data)
{
	if ( m_isrunning)
		m_isrunning = false;
}

void NMSnooper::status(void *data)
{
	NMString message = NMParent::listMemoryObjects();
	if ( m_lastmessage != message ) {
		NMStatus stat;
		stat.setMessage( message );
		m_lastmessage = message;
		*this << stat;
	}
}

void NMSnooper::configure(void *data)
{
	NMString *nms_xml = (NMString *) data;
	NMXML nmxml_data;

	nmxml_data.loadTree(nms_xml->c_str());

	NMString sec = nmxml_data.getValue("/*/interval");
	if(!sec.Length()) {
		*this << NMError(NMError::user, "No protocol key");
		nmxml_data.freeTree();
		return;
	}
	m_sec = sec.toInt();
}

