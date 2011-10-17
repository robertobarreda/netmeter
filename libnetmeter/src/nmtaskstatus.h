/*
 *   $Id: nmtaskstatus.h,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
 *   Copyright (c) 2006 Pau Capella
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef NMTASKSTATUS_H
#define NMTASKSTATUS_H

#include "nmtask.h"
#include "nmtimer.h"

class NMTask;

/**
	@author Pau Capella <pau.capella@ac.upc.edu>
	@brief Abstract class for status tasks
*/
class NMTaskStatus: public NMParent  {

public:

	NMTaskStatus(const char *name = 0, NMParent *parent = 0) : NMParent(name, parent) {}
	virtual ~NMTaskStatus() {}

	virtual void process( NMTask *task) = 0;
};

/*------------------------------------*/

/**
@author Pau Capella <pau.capella@ac.upc.edu>
@brief Manages the task in timeout status
*/

class NMTaskStatusStartTime: public NMTaskStatus {

public:
	NMTaskStatusStartTime( const char *name = 0, NMParent *parent = 0);
	~NMTaskStatusStartTime();

	void process( NMTask *task);

private:
	void startTask();
	NMTimer *m_timer;
};

/*------------------------------------*/

/**
@author Pau Capella <pau.capella@ac.upc.edu>
@brief Manages the task in clean status
*/
class NMTaskStatusClean : public NMTaskStatus
{
public:
	NMTaskStatusClean(const char *name, NMParent *parent);
	~NMTaskStatusClean() {}

	void process( NMTask *task);
};

/*------------------------------------*/

/**
	@author Pau Capella <pau.capella@ac.upc.edu>
	@brief Manages the task in dirty status
*/
class NMTaskStatusDirty : public NMTaskStatus
{
public:
	NMTaskStatusDirty(const char *name, NMParent *parent);
	~NMTaskStatusDirty() {}

	void process( NMTask *task);
};

/*------------------------------------*/

/**
	@author Pau Capella <pau.capella@ac.upc.edu>
	@brief Manages the task in completed status
*/
class NMTaskStatusCompleted : public NMTaskStatus
{
public:
	NMTaskStatusCompleted(const char *name, NMParent *parent);
	~NMTaskStatusCompleted() {}

	void process( NMTask *task);

};
#endif
