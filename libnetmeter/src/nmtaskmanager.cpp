/*
 *   $Id: nmtaskmanager.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#include "nmtaskmanager.h"

NMTaskManager *NMTaskManager::nmtaskmanager = 0;

/**
  \fn NMTaskManager::self() 
  \brief Return an instance of NMTaskManager
*/
NMTaskManager *NMTaskManager::self() 
{
	if(nmtaskmanager)
		return nmtaskmanager;
	return nmtaskmanager = new NMTaskManager("taskmanager",0); 
}


NMTaskManager::NMTaskManager(const char *name, NMParent *nmparent)
 : NMParent(name,nmparent),
exit(false),
contTasks(0)
{
}


NMTaskManager::~NMTaskManager()
{
}

/**
  \fn NMTaskManager::eventLoop()
  \brief Begin the main event loop.
*/
void NMTaskManager::runLoop()
{	
	if(exit) return;
	cout << "TaskLoop runnning" << '\n';
	while(!exit) {
		dispatchTasks();
		usleep(500);
	}
}

/**
  \fn NMTaskManager::exitLoop()
  \brief Stop the event loop.
*/
void NMTaskManager::exitLoop()
{
	exit = true;
}


/**
  \fn NMEventManager::eventDispatcher()
  \brief Dispath events.
*/
void NMTaskManager::dispatchTasks()
{	
	NMTask *task;
	mutex.lock();
	l_tasks=l_pendents;
	l_pendents.clear(); 
	mutex.unlock();
	while (l_tasks.size()) {
		//Treim el event de la cua.
		task = l_tasks.front();
		l_tasks.pop_front();
		processTask(task);
	}
	
}


/**
  \fn NMTaskManager::addNewTask (const NMTask *task)
  \brief Add a new task.
  \param Task
  \return Unique task handle.
*/
int NMTaskManager::addNewTask (NMTask *task)
{
	task->setTaskId( contTasks++);
	return addTask( task);
}

/**
  \fn NMTaskManager::addTask (int taskid, NMString pluginname, NMTaskType tasktype, NMString xmlparam)
  \brief Add a new task.
  \param task  task to add
  \return Task handler
*/
int NMTaskManager::addTask (NMTask *task)
{	
	mutex.lock();
	l_pendents.push_back(task);
	mutex.unlock();
	
	return task->taskId();
}

/**
  \fn NMTask::processTask(NMTask task) 
  \brief Handle one task.
  \param task Task to excute.
*/
void NMTaskManager::processTask(NMTask *task)
{
	task->process();
}


//#endif


