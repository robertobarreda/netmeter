/***************************************************************************
 *   Copyright (C) 2005 by Josep Ramon Benet                               *
 *   josep.ramon.benet@upc.edu                                             *
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
#ifndef NMGBATCHTASK_H
#define NMGBATCHTASK_H

#include <nmstring.h>

//#include <qdom.h>

/**
@author Josep Ramon benet
 */
class NMGBatchTask{
	private:
		int id;
		NMString name;
		NMString plugin;
		NMString state;
		NMString *xml;
		
	public:
		NMGBatchTask();
		NMGBatchTask(int nm_id, NMString nm_name, NMString nm_plugin, NMString nm_state, NMString nm_xml);

		~NMGBatchTask();
    	
		NMString getName();
		NMString getPlugin();
		NMString getState();
    	//void setXML();

};

#endif
