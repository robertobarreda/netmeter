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
#ifndef NMGMGENFLOWMANAGER_H
#define NMGMGENFLOWMANAGER_H

#include <QMap>
#include <QString>

#include <nmgrandomgenerator.h>

class NMGMGenFlow;

typedef QMap<QString, NMGMGenFlow *> NMGMGenFlowManagerMap; /* key-> port of the flow */
typedef NMGMGenFlowManagerMap::iterator NMGMGenFlowManagerMapIterator;

#define MAX_RAND_PORT_INTERVAL 3000 
#define MINIMUM_PORT_NUMBER 5000

/**
@author Ernest Nieto
*/
class NMGMGenFlowManager
{
public:
    NMGMGenFlowManager();
    ~NMGMGenFlowManager();
    
    bool isPortAvailable(int port);
    NMGMGenFlow * getFlowByPort(int port);
    NMGMGenFlow * getFlowById(int id);
    void addFlow(NMGMGenFlow * flow);
    void removeFlow(int port);
    int getNextFreePort();

private:
    NMGMGenFlowManagerMap flowMap;
    int lastFlowPort;
    NMGRandomGenerator random;
};

#endif
