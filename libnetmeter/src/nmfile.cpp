/***************************************************************************
                          nmfile.cpp  -  description
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

#include "nmfile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/**
\fn NMFile::NMFile(const NMString &nms_name, NMParent *parent)
\brief NMFile construct with set a file nms_name. 
*/
NMFile::NMFile(const NMString &nms_name, NMParent *parent)
: NMBuffer(nms_name.c_str(), parent)
{
	fd = -1;
	autocommit = false;
	filename = nms_name;
}

NMFile::~NMFile()
{
	close();	
}

/**
 \fn setFileName (const NMString &nms_name)
 \brief Sets the name of the file to name.
 \param nms_name Filename
*/
void NMFile::setFileName (const NMString &nms_name)
{
	if(fd != -1) {
		cerr << "NMFile open failed: File descriptor is in used, close first." << '\n';
		return;
	}
	filename = nms_name;
}
/**
 \fn NMFile::getFileName () const;
 \brief Returns the name set by setName().
*/
NMString NMFile::fileName () const
{
	return filename;
}

/** 
 \brief Open operation with an exsisting file descriptor.
 \param fd File descrition to open.
 \param mode Permissions of the file.
 \return On success, fd is returned. On failure, a non-zero is returned.
 */
int NMFile::open (int fd, NMFileMode mode)
{
	if(fd != -1) {
		cerr << "NMFile open failed: File descriptor is in used, close first." << '\n';
		return -1;
	}
	this->fd = fd;
	this->mode = mode;
	return this->fd;
}

/** 
 \brief Open operation with an exsisting file descriptor
 \param mode Permissions of the file.
 \return On success, fd is returned. On failure, a non-zero is returned.
*/
int NMFile::open (NMFileMode mode)
{
	if (!filename.Length())  {
		cerr << "NMFile open failed: No name to open." << '\n';
		return -1;
	}

	if(fd != -1) {
		cerr << "NMFile open failed: File descriptor is in used, close first." << '\n';
		return -1;
	} 
	
	this->mode = mode;
	
	if(mode == ReadOnly)
		fd = ::open(filename.c_str(), O_RDONLY);
	else if(mode == WriteOnly)
		fd = ::open(filename.c_str(), O_WRONLY);
	else
		fd = ::open(filename.c_str(), O_RDWR);
	if (fd < 0) {
		cerr << "NMFile open failed: " << strerror(errno) << '\n';
		return -1;
	}
	return fd; 
		
		
}

/**
  \fn NMFile::close()
  \brief Close the current file.
  \return On succes, 0 is returned. On failure, a non-zero error code is returned.
*/
void NMFile::close()
{
	if(length()) commit();
	if(fd == -1) return;
	filename = "";
	::close(fd);
	fd = -1;
}

/**
  \fn NMFile::create(NMString nms_name)
  \brief Create a empty file called nms_mame and open it.
  \param nms_name Name of the new file.
  \return On success, fd is returned. On failure, a non-zero is returned.
*/
int NMFile::create(NMString nms_name)
{
	
	if(fd != -1) {
		cerr << "NMFile createFile failed: File descriptor is in used, close first." << '\n';
		return -1;
	}
	this->filename = nms_name;
	this->mode = WriteOnly;
	
	fd = creat(nms_name.c_str(), 0644);
	if (fd < 0) {
		cerr << "NMFile createFile failed: " << strerror(errno) << '\n';
		return -1;
	}
	return fd;	
}

/**
  \brief Returns true if this file exists; otherwise returns false.
*/
bool NMFile::exists() const
{
	if(!access(filename.c_str(), F_OK)) return true;
	return false;
}

/**
  \brief Removes the file
  \return Returns true if successful; otherwise returns false
*/
bool NMFile::remove()
{
	if(::remove(filename.c_str()) < 0) return false;
	return true;
}

/**
  \brief Write the information to disk. The changes will only be commited if autocommit is set or if the commit call is issued.
  \param buffer Information to write.
  \param size Information size
  \return On success, the number of bytes write is returned.
*/
int NMFile::write (const void * buffer, int size)
{
	int rc;
	if(fd < 0) {
		cerr << "NMFile write failed: Create or open a file first." << '\n';
		return -1;
	}
	if(mode == ReadOnly) {
		cerr << "NMFile write failed: You don't have permission for write this file." << '\n';
		return -1;
	}
	rc = writeB (buffer, size);
	if(autocommit)
		commit(); 
	return rc;
}

/**
\brief Read the information from disk.
\param buffer Information into to buffer.
\param size Information size.
\return On success, the number of bytes read is returned and the file position is advanced by this number.
*/
int NMFile::read(void *buffer, int size)
{
	int rc;
	if(fd < 0) {
		cerr << " NMFile read failed: Create or open a file first" << '\n';
		return -1;
	}
	if(mode == WriteOnly) {
		cerr << "NMFile read failed: You don't have permission for write this file" << '\n';
		return -1;
	}
	
	rc = ::read(fd, buffer, size);
	return rc;
}


/**
  \brief Commits all the changes in the buffer to disk.
  \return The  number of bytes written are returned or <0 if error
*/
int NMFile::commit()
{
	char buf[32768];
	int rc = 0;
	if(fd < 0) {
		cerr << " NMFile commit failed: Create or open a file first" << '\n';
		return -1;
	}
	memset(buf,'\0',32768);
	if( length() > 0) {
		readB(buf,length());
		rc = ::write(fd, buf, strlen(buf));
		if(rc < 0) {
			cerr << "NMFile commit failed: " << strerror(errno) << '\n';
			return rc;
		}	
	}
	if(length() > 0) commit();
	clear();
	return rc;
}

/**
  \brief Activate or desactivate the autocommit.
  \param b_stat True or false;
*/
void NMFile::autoCommit(bool b_stat)
{
	autocommit = b_stat;
}

/**
  \brief Return the current position of file.
  \return Current position
*/
int NMFile::tell() const
{
	if(fd < 0) {
		cerr << " NMFile tell failed: Create or open a file first" << '\n';
		return -1;
	}
	return lseek(fd, 0, SEEK_CUR);

}

/**
  \brief Reposition read/write file offset.
  \param from Directive to use: SEEK_SET(0), SEEK_CUR(1) or SEEK_END(2).
  \param offset File offset.
  \return The resulting offset location as measured in bytes from the beginning  of  the  file.
*/
int NMFile::seek (int from, int offset)
{
	int rc;
	if(fd < 0) {
		cerr << "NMFile seek failed: Create or open a file first" << '\n';
		return -1;
	}
	if(from < 0 || from > 2) {
		cerr << "NMFile seek failed: I don't undestarnd this directive" << '\n';
		return -1;
	}
	rc = lseek(fd, offset, from);
	return rc;
}

/**
  \fn NMFile::exists (const NMString &filename)
  \brief Returns true if this file exists; otherwise returns false.
  \param filename File to check.
*/
bool NMFile::exists (const NMString &filename)
{
	if(!access(filename.c_str(), F_OK)) return true;
	return false;
}

/**
  \fn NMFile::remove (const NMString &filename)
  \brief Removes the file
  \param filename File to remove.
  \return Returns true if successful; otherwise returns false.
*/
bool NMFile::remove (const NMString &filename)
{
	if(::remove(filename.c_str()) < 0) return false;
	return true;
}



