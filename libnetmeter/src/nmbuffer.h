/***************************************************************************
                          nmbuffer.h  -  description
                             -------------------
    begin                : dt feb 15 2005
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

#ifndef NMBUFFER_H
#define NMBUFFER_H

#include "nmparent.h"
#include "nmstring.h"

#define INITIAL_SIZE 500

#define BYTE	char *

/**
  *\brief Provide operations for memory management of raw file buffers. 
  *@author Pau Capella
*/
  
class NMBuffer: public NMParent 
{
	void *buffer;
protected:
	/*Tamany total del buffer zona*/
	long int buffersize;
	/*Posicio en el buffer per escriure*/
	long int offset;
	
public: 
	enum NMFileMode {ReadOnly, WriteOnly, ReadWrite};

	NMBuffer(const char *name=0, NMParent *parent=0);
	~NMBuffer();
	int grow(int increment);
	int alloc(int size);
	int search(const void *sequence, int size);
	int length() {return offset;}
	void clear();

	

protected:
	int writeB(const void *buffer, int size);
	int readB(void *buffer, int size);
	

};

#endif
