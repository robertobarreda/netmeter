/*
 *   $Id: nmtaskdirty.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#include "nmtaskdirty.h"
#include "nmtaskmanager.h"
#include "nmtaskcompleted.h"
#include "nmerror.h"

NMTaskDirty::NMTaskDirty(const char *name, NMParent *parent)
 : NMTaskStatus(name, parent)
{
}


NMTaskDirty::~NMTaskDirty()
{
}

/*! 
    \fn NMTaskDirty::update( NMTask *task) 
    \brief Verify if task is finished and update the status task from dirty to completed 
    \param task Current task
*/
void NMTaskDirty::process( NMTask *task) 
{
	
	if (task->isFinished()) { //Hem acabat l'execució de la tasca, passem a un estat completed
		//Creem la tasca completed
		NMTaskCompleted *taskcompleted = new NMTaskCompleted("completed", this->parent());
		//Canviem l'estat actual de la tasca
		task->setStatus(taskcompleted);
		//Encuem la tasca! 
		NMTaskManager::self()->addTask(task);
		//Ens borrem!.
		delete this;
 	}

 	else if(!task->isFinished() && !task->isRunning()) {
		NMString msgerr = "Task ";
		msgerr = msgerr + NMString::number(task->testId()) + " are corrupted in plugin " + task->plugin() +". Why thread is NOT running?";
 		cout << NMError(NMError::fatal, msgerr);
		delete task;
		return;
	}
	else 	//thread()->isRunning && !thread()->isFinished
		NMTaskManager::self()->addTask(task);
	
}
