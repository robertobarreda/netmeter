/***************************************************************************
                          nmbuffer.cpp  -  description
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
#include <stdlib.h>

#include "nmbuffer.h"
/**
\fn NMBuffer()
\brief Construct of the NMBuffer class.
*/
NMBuffer::NMBuffer(const char *name, NMParent *parent)
:NMParent(name,parent)
{
	buffer = malloc(INITIAL_SIZE);	
	memset(buffer, '\0', INITIAL_SIZE);
	buffersize = INITIAL_SIZE;
	offset = 0;
}
/**
\fn ~NMBuffer()
\brief Destruct of the NMBuffer class.
*/
NMBuffer::~NMBuffer()
{
	free(buffer);
}
/**
  \fn NMBuffer::grow(int increment)
  \brief Increase o reduce the current buffer preserving the contents.
  \param    increment Value to increase o decrease (if "increment" is negative).
  \return   The size of the buffer or < 0 si error.
*/
int NMBuffer::grow(int increment)
{	
	long int li_offsetcopia = offset;
	void *newbuffer=malloc(buffersize);
	if(!newbuffer) {
		cerr << "NMBuffer error: Not enought memory!\n";
		return -1;
	}
	
	if (this->readB(newbuffer,buffersize)<0) {
		cerr << "NMBuffer Error: " << '\n';
		free(newbuffer);
		return -1;
	}
	//cout << "Copiat de buffer a newbuffer!----> " << (BYTE)newbuffer << "\n";
	free(buffer);
	buffer=malloc(buffersize+increment);
	
	if(!buffer) {
		cerr << "NMBuffer error: Not enought memory!\n";
		buffer = newbuffer;
		return -1;
	}
	if(buffersize + increment > buffersize) //El buffer aumenta
		this->writeB(newbuffer, buffersize); 
	else //El buffer disminuex ALERTA: Podem perdre dades!
		this->writeB(newbuffer,buffersize+increment);
	
	free(newbuffer);
	buffersize+=increment;
	offset=li_offsetcopia;
	return buffersize;
}
/** 
  \brief Clear the current buffer and creates a new one with size "size"  
  \param    size Size of the new buffer.
  \return   The size of the new buffer or < 0 si error.
*/
int NMBuffer::alloc(int size)
{
	free(buffer);
	buffer = malloc(size);
	if(!buffer) {
		cerr << "NMBuffer error: Not enought memory!\n";
		return -1;
	}
	memset(buffer, '\0', size);
	buffersize = size;
	offset = 0;
	return size;
}
/**
  \brief Copy size bytes from buffer to memory buffer pointed to offset.
  \param    buffer Bytes to write.
  \param    size Number of bytes to write.
  \return   The number of write bytes or < 0 si error.
*/
int NMBuffer::writeB(const void *buffer, int size)
{
	//Tenim prou espai en el buffer?
	if((offset + size) > buffersize) {
		if((grow((offset+size)-buffersize+1)) == -1) {
			cerr << "NBuffer error: I can not write in buffer" << "\n";
			return -1;
		}
	}
	//Copiem size bytes de buffer a this->buffer
	//Per moure-n's per this->buffer faig un cast a byte.
	memcpy(((BYTE)this->buffer + offset), buffer, size);
	offset += size;
	//cout << "Buffer Write-> tamany del buffer anterior: " << offset-size << " Ara el tamany es: " << offset << '\n'; 
	//memset((BYTE)this->buffer + offset,'\0',1);
	return size; 
}
/**
  \brief Copy size bytes from memory buffer pointed to offset to buffer. This bytes are removed of memory.
  \param    buffer Bytes to read
  \param    size Number of bytes to read
  \return   The number of read bytes or < 0 si error
*/
int NMBuffer::readB(void *buffer, int size)
{	
	memset(buffer,'\0',size);
	if(size > offset) size = offset;
	//Compiem size bytes de this->buffer a buffer
	memcpy(buffer, (BYTE)this->buffer, size);
        //offset=offset-size;
	memmove((BYTE)this->buffer, ((BYTE)this->buffer)+size, offset-size);
	offset -= size;
	memset(((BYTE)this->buffer)+offset,'\0',offset+size);
	//cout << "Buffer Read-> tamany del buffer anterior: " << offset+size << " Ara el tamany es: " << offset << '\n'; 
	
	return size;
}
/**
   \fn int NMBuffer::search(const void *sequence, int size)
   \brief Generic search for not-strings
   \param sequence Sequence to search. Sequence can be anything.
   \param size Number of bytes to search.
   \return Offset of sequence in the buffer or -1 if not found or error.
*/
int NMBuffer::search(const void *sequence, int size)
{
	int pos=0;
	while(pos <= offset - size)
	{
		if(memcmp((BYTE)buffer + pos, (BYTE)sequence, size)==0) 
			return pos;
		pos++;
	}
	return -1;
}


/*!
    \fn void NMBuffer::clear()
    \brief Clean the buffer with "\0" and keep the size
 */
void NMBuffer::clear()
{
    memset(buffer,'\0',buffersize);
    offset = 0;
}
