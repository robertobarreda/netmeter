/*
 *   $Id: nmtrace.h,v 1.6 2006-03-21 15:43:17 pcapella Exp $
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
#ifndef NMTRACE_H
#define NMTRACE_H

#define NOT_CONFIGURED 0

#include <nmmoduleinterface.h>
#include <nmexternallocal.h>
#include <nmexternalremote.h>
#include <list>

class TraceItem
{
	public:
		NMString address;
		NMString time;
};

/**
	@author Pau Capella <pau.capella@ac.upc.edu>
	@brief Traceroute plugin
*/
class NMTrace : public NMModuleInterface
{
	NMExternal *nme_traceroute;

	list<TraceItem> listhosts;

public:
	NMTrace();
	~NMTrace();

	void initModule();
	void removeModule();
	void start(void *data);
	void stop(void *data);
	void status(void *data);
	void configure(void *data);

private:
	void processRecived();
	void processExit();
	void sendStatus( TraceItem item );
};

NMMODULE_CLASS(NMTrace)
NMMODULE_NAME((char *)"nmtrace")
NMMODULE_CAPABILITIES((char *)"test")
NMMODULE_VERSION((char *)"0.1")

#endif
