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
#ifndef NMENTITY_H
#define NMENTITY_H

#include<nmthread.h>

#define eventId int

/**
@author Pau Capella
@brief Abstract class for events.
*/
class NMEntity {
	
public:

     enum NMEntityClass{ event=0, task };

     enum NMEventStatus {
	clean, //Primer cop a la cua.
	dirty, //L'event s'esta tractant.
	timeout, //El temps per l'event ha acabat...
	completed
     };

     NMEntity();
     NMEntity( eventId i_id, NMEntityClass nmec_class);

    virtual ~NMEntity();

     void setId(eventId i_id);
     void setEntityClass(NMEntityClass nmec_class);
     void setStatus(NMEventStatus i_status);

     eventId getId();
     int getEntityClass();
     int getStatus();

private:
	eventId id;
	NMEntityClass entityclass;
	NMEventStatus status;
};

#endif
