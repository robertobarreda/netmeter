/***************************************************************************
                          nmfile.h  -  description
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

#ifndef NMFILE_H
#define NMFILE_H

#include "nmparent.h"
#include "nmbuffer.h"
#include "nmstring.h"
#include "nmhost.h"

/**
  @author Pau Capella
  @brief Provides access to files - basic I/O operations, copying, removing,...
  */
/**
NMFiles working with memory buffer, data may not be written to the file at once. Call commit() to make sure that the data is really written. 
*/
class NMFile : public NMBuffer  {

  int fd;
  NMFileMode mode;
  bool autocommit;
  NMString filename;

public: 
	NMFile (const NMString &nms_name, NMParent *parent);
	~NMFile();

	void setFileName (const NMString &nms_name);
	NMString fileName () const;

	int open (NMFileMode mode);
	int open (int fd, NMFileMode mode);
	void close ();
  	int create (NMString nms_name);
	
	bool exists () const;
	bool remove ();

  	int write (const void * buffer, int size);
  	int read (void *buffer, int size );
  	int seek (int from, int offset );
  	int tell () const;
  	void autoCommit (bool b_stat);
  	int commit ();

	//static
	static bool exists (const NMString &filename);
	static bool remove (const NMString &filename);
};

#endif
