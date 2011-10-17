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
#ifndef NMINTERPRET_H
#define NMINTERPRET_H

#include <list>
#include "nmparent.h"
#include "nmxml.h"
#include "nminterface.h"
#include "nmtaskmanager.h"
#include "nmerror.h"
#include "nmstatus.h"

/**
@author Pau Capella
@brief This class processes all messages recieved from gui and generates a response for this.
*/
class NMInterface;

class NMInterpret
{

public:
	 NMInterpret();
	~NMInterpret();

	void translate(char *xmlData);
	NMString attach(char *xmlbuffer);
	NMString pendent();

	int generateResult (const int& id, const NMString &pluginame, const NMString& data);
	int generateResult (const int& id, const NMString& data);
	int generatePendent (const NMString &pluginame, const NMString &data);
	int generateError (const int& id, NMError error);
	int generateStatus( const int &m_testid, NMStatus status);
	
	bool isPendent();

private:

	int generateACK (const int& id);
	int generateNACK (const int& id);

	NMString generateAttach();

public:
	
	static NMString client;
	static NMString version;
	
private:

	static list<NMString> *pendingqueue;
};

#endif
