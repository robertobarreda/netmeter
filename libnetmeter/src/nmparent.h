/***************************************************************************
                          nmparent.h  -  description
                             -------------------
    begin                : ds feb 12 2005
    copyright            : (C) 2005 by Pau Capella
    email                : pcapella@ac.upc.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NMPARENT_H
#define NMPARENT_H

#include <iostream>
#include <assert.h>
#include <list>
#include <errno.h>
#include <string.h>

#include "nmstring.h"
#include "nmmutex.h"

#define true 1
#define false 0

#define MAXNAME 40

#define DEBUG
#define NDEBUG

//using namespace std;

class NMParent;
class NMMutex;
class NMEventManager;

/**
  *@author Pau Capella
  *@brief The base class of NetMeter objects.
*/
/** You can ignore this class. This class manages all the memory, it can destroy a memory object if this is not use.
*/

class NMParent {
 	
	char objname[MAXNAME];
	static list<NMParent *> *objectslist;
	static NMMutex *mutex;
	NMParent *m_parent;

protected:
	 
	bool isevent;
	
	list<NMParent *> events();
public: 

	NMParent( const char *name=0, NMParent *parent=0);
	NMParent( NMParent *parent);

	virtual ~NMParent();
	
	char *name() { return objname; }
	void setName( char *newname ) { strcpy(objname, newname);}

	void setParent( NMParent *parent) { m_parent = parent;}
	NMParent *parent() { return m_parent;}
	
	static NMString listMemoryObjects();

	void eventManagerStart();
};

#endif
