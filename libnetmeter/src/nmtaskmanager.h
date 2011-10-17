/*
 *   $Id: nmtaskmanager.h,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
 *   Copyright (c) 2005 Pau Capella
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
#ifndef NMTASKMANAGER_H
#define NMTASKMANAGER_H

#include <list>
#include <iterator>

#include <nmparent.h>
#include <nmmutex.h>
#include <nmstring.h>
#include <nmtask.h>

/**
@author Pau Capella
*/
class NMTaskManager : public NMParent
{

	static NMTaskManager* nmtaskmanager;
	
	/*Dins d'aquesta llista tenim tants les tasques que s'han d'executar */
	list<NMTask *> l_tasks;
	/*Dins d'aquesta llista tenim les tasques pendents.*/
	list<NMTask *> l_pendents;

	bool exit;
	int contTasks;

	NMMutex mutex;

	NMTaskManager(const char *name, NMParent *nmparent);

public:
	static NMTaskManager *self();	
	~NMTaskManager();

	void runLoop();
	void exitLoop();

	int addNewTask(NMTask *task);
	int addTask(NMTask *task);

private:

	void dispatchTasks();
	void processTask(NMTask *task);


};

#endif
