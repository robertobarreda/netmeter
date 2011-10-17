/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
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
#ifndef NMPING_H
#define NMPING_H

#include <nmmoduleinterface.h>
#include <nmmoduleinterface.h>
#include <nmmodulemanager.h>
#include <nmeventmanager.h>
#include <nmhost.h>
#include <nmdns.h>
#include <nmexternallocal.h>
#include <nmexternalremote.h>
#include <nmsocket.h>
#include <nmxml.h>

#define EXEC ping

#define HOST_UP 0
#define HOST_DOWN 1
#define HOST_ERROR 2

/**
@author Pau Capella
@brief Ping plugin
*/
class NMPing : public NMModuleInterface
{
	
	//Executable
	NMExternal *nme_ping;
	
	//Guardem el hosts... TODO Aquest valors no son necessaris ja que s'ho pot guarda la Gui
	NMHost source;
	NMHost destination;
	//Guardem el resultat del ping
	int result;
	
public:
	NMPing();
	~NMPing();
	void initModule();
	void removeModule();
	void start(void *data);
	void stop(void *data);
	void status(void *data);
	void configure(void *data);

	void processRecived();
	void processExit();
};



NMMODULE_CLASS(NMPing)
NMMODULE_NAME((char *)"nmping")
NMMODULE_CAPABILITIES((char *)"test")
NMMODULE_VERSION((char *)"0.2")
#endif

