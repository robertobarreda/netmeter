/*
 *   $Id: nmtaskstatus.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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

#include "nmtaskstatus.h"
#include "nmtaskmanager.h"


/*--------------- NMTaskStatusStartTime -----------------*/

NMTaskStatusStartTime::NMTaskStatusStartTime( const char *name, NMParent *parent)
 : NMTaskStatus( name, parent)
{
	m_timer = 0;
}


NMTaskStatusStartTime::~NMTaskStatusStartTime()
{
	if( m_timer)
		delete m_timer;
}

/*! 
    \fn NMTaskStatusStartTime::process( NMTask *task)
    \brief Wait a time for update the status task form startime to clean
    \param task Current task
*/
void NMTaskStatusStartTime::process( NMTask *task)
{
	if ( m_timer) return;

	m_timer = new NMTimer( this);
	if ( !m_timer->start( task->startTime())) {
		cerr << "NMTaskStatusStartTimer process failed: I can not start NMTimer" << endl;
		//TODO Hauriem d'avisar a la Gui?
		return;
	}
	m_timer->timeout.setValue( task);
	m_timer->timeout.connect( this, &NMTaskStatusStartTime::startTask);
	
}

void NMTaskStatusStartTime::startTask()
{
	NMTaskStatusClean *clean = new NMTaskStatusClean("Clean", this->parent());
	//Canviem l'estat actual de la tasca
	NMTask *task = static_cast<NMTask *> ( m_timer->timeout.value());
	task->setStatus( clean);
	//Encuem la tasca! 
	NMTaskManager::self()->addTask( task);
	//No podem alliberar aquests objecte, ja s'encargarà el NMParent :)
}

/*--------------- NMTaskStatusClean -----------------*/

NMTaskStatusClean::NMTaskStatusClean( const char *name, NMParent *parent)
 : NMTaskStatus(name, parent)
{
}

/*! 
    \fn NMTaskStatusClean::process( NMTask *task)
    \brief Throw the task to execute and update the status task form clean to dirty
    \param task Current task
*/
void NMTaskStatusClean::process( NMTask *task)
{
	if (task->isRunning() ) {
		NMString msgerr = " Task " + NMString::number(task->testId()) + " are corrupted in plugin " + task->plugin() +". Why thread is running?";
 		cout << NMError(NMError::fatal, msgerr);
		delete task;
		return;
	}
	//Executem el thread
	task->start();
	//Creem la taskdirty
	NMTaskStatusDirty *taskdirty = new NMTaskStatusDirty("dirty", this->parent());
	//Canviem l'estat actual de la tasca
	task->setStatus(taskdirty);
	//Encuem la tasca! 
	NMTaskManager::self()->addTask(task);
	//Ens borrem!.
	delete this;
}

/*--------------- NMTaskStatusDirty -----------------*/

NMTaskStatusDirty::NMTaskStatusDirty(const char *name, NMParent *parent)
 : NMTaskStatus(name, parent)
{
}

/*! 
    \fn NMTaskStatusDirty::process( NMTask *task) 
    \brief Verify if task is finished and update the status task from dirty to completed 
    \param task Current task
*/
void NMTaskStatusDirty::process( NMTask *task) 
{
	if (task->isFinished()) { //Hem acabat l'execució de la tasca, passem a un estat completed
		//Creem la tasca completed
		NMTaskStatusCompleted *taskcompleted = new NMTaskStatusCompleted("completed", this->parent());
		//Canviem l'estat actual de la tasca
		task->setStatus(taskcompleted);
		//Encuem la tasca! 
		NMTaskManager::self()->addTask(task);
		//Ens borrem!.
		delete this;
 	}

 	else if(!task->isFinished() && !task->isRunning()) {
		NMString msgerr = "Task " + NMString::number(task->testId()) + " are corrupted in plugin " + task->plugin() +". Why thread is NOT running?";
 		cout << NMError(NMError::fatal, msgerr);
		delete task;
		return;
	}
	else 	//thread()->isRunning && !thread()->isFinished
		NMTaskManager::self()->addTask(task);
}

/*--------------- NMTaskStatusCompleted -----------------*/

NMTaskStatusCompleted::NMTaskStatusCompleted( const char *name, NMParent *parent)
 : NMTaskStatus(name, parent)
{
}

/*! 
    \fn NMTaskStatusCompleted::update( NMTask *task)
    \brief Delete the current task
    \param task Current task
*/
void NMTaskStatusCompleted::process( NMTask *task)
{
	delete task;
}
