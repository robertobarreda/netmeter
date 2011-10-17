/*
 *   $Id: nmtaskclean.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#include "nmtaskclean.h"
#include "nmtaskdirty.h"
#include "nmtaskmanager.h"

NMTaskClean::NMTaskClean( const char *name, NMParent *parent)
 : NMTaskStatus(name, parent)
{
}


NMTaskClean::~NMTaskClean()
{
}

/*! 
    \fn NMTaskClean::update( NMTask *task)
    \brief Throw the task to execute and update the status task form clean to dirty
    \param task Current task
*/
void NMTaskClean::process( NMTask *task)
{
	
	
	if (task->isRunning() ) {
		NMString msgerr = "Task ";
		msgerr = msgerr + NMString::number(task->testId()) + " are corrupted in plugin " + task->plugin() +". Why thread is running?";
 		cout << NMError(NMError::fatal, msgerr);
 		cout << NMError(NMError::fatal, msgerr);
		delete task;
		return;
	}
	//Executem el thread
	task->start();
	//Creem la taskdirty
	NMTaskDirty *taskdirty = new NMTaskDirty("dirty", this->parent());
	//Canviem l'estat actual de la tasca
	task->setStatus(taskdirty);
	//Encuem la tasca! 
	NMTaskManager::self()->addTask(task);
	//Ens borrem!.
	delete this;

}
