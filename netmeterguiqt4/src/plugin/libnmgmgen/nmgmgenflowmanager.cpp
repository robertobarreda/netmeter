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

#include "nmgmgenflowmanager.h"
#include "nmgmgenflow.h"

/**
 * Contructor of the class
 * @return 
 */
NMGMGenFlowManager::NMGMGenFlowManager()
{
	
	lastFlowPort = ((int) (((double) random.random())*MAX_RAND_PORT_INTERVAL/random.getMaxValue()))+MINIMUM_PORT_NUMBER-1;
}


/**
 * Destructor of the class
 * @return 
 */
NMGMGenFlowManager::~NMGMGenFlowManager()
{
}


/**
 * Returns true if the port indicated is available
 * @param port Port number
 * @return 
 */
bool NMGMGenFlowManager::isPortAvailable(int port)
{
	NMGMGenFlowManagerMapIterator it = flowMap.find(QString("%1").arg(port));
	if(it==flowMap.end()) return true;
	return false;
}

/**
 * Returns the flow identified by the indicated port
 * @param port Port number
 * @return 
 */
NMGMGenFlow * NMGMGenFlowManager::getFlowByPort(int port)
{
	NMGMGenFlowManagerMapIterator it = flowMap.find(QString("%1").arg(port));
	if(it==flowMap.end()) return 0;
	return *it;
}

/**
 * Returns the flow identified by the id
 * @param id Identifier of the flow (flow_id)
 * @return THe flow or 0
 */
NMGMGenFlow * NMGMGenFlowManager::getFlowById(int id)
{
	for(NMGMGenFlowManagerMapIterator it = flowMap.begin(); it!=flowMap.end(); ++it)
	{
		if(it.value()->getId()==id) 
			return it.value();
	}

	return 0;
}

/**
 * Adds a flow to the manager
 * @param flow 
 */
void NMGMGenFlowManager::addFlow(NMGMGenFlow * flow)
{
	QString port = QString("%1").arg(flow->getPort());
	flowMap.insert(port,flow);
	lastFlowPort = flow->getPort();
}

/**
 * Removes the flow identified by port
 * @param port Port number
 */
void NMGMGenFlowManager::removeFlow(int port)
{
	flowMap.remove(QString("%1").arg(port));
}

/**
 * Returns a free port. It's the next free port of the last added flow
 * @return 
 */
int NMGMGenFlowManager::getNextFreePort()
{
	int port;
	for(port = lastFlowPort + 1; not isPortAvailable(port); ++port);
	return port;
}
