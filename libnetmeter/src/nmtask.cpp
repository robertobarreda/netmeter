/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "nmtask.h"

/**
  \fn NMTask::NMTask(const char *name, int testid, NMString plugin, NMString xmlparam, NMParent *parent)
  \brief Create a new task
*/
NMTask::NMTask(int testid, NMString plugin, NMString xmlparam, NMDateTime starttime, const char *name, NMParent *parent)
: NMParent(name, parent), 
  NMThread(),
  m_testid(testid), m_taskid(0), m_plugin(plugin), m_starttime(starttime)
{
	m_xmlparam = new NMString(xmlparam);
	if( starttime.isNull())
		m_status = new NMTaskStatusClean("clean", this);	
	else
		m_status = new NMTaskStatusStartTime("starttime", this);
}

NMTask::~NMTask()
{
	//taskid = 0;
	//type = NMTask::Undefined;
	delete m_xmlparam;
	delete m_status;
}

/*! 
    \fn NMTask::testId()
    \brief Get an unique identification test 
*/
int NMTask::testId()
{
	return m_testid;
}

/*! 
    \fn NMTask::taskId()
    \brief Get an unique indentification task
*/
int NMTask::taskId()
{
	return m_taskid;
}


/*! 
    \fn NMTask::plugin()
    \brief Return a string with the plugin name associated to the task
*/
NMString NMTask::plugin()
{
	return m_plugin;
}

/*! 
    \fn NMTask::xmlData()
    \brief Return a string with the data for the plugin associated to the task
*/
NMString *NMTask::xmlData()
{
	return m_xmlparam;
}

/*! 
    \fn NMTask::startTime()
    \brief Return the time when task start
*/
NMDateTime NMTask::startTime()
{
	return m_starttime;
}

/*! 
    \fn NMTask::status()
    \brief Return a status class
*/
NMTaskStatus* NMTask::status()
{
	return m_status;
}

/*! 
    \fn NMTask::setTaskId(int taskid)
    \brief Set an unique identification task
    \param i_taskid Task identification
*/
void NMTask::setTaskId( int taskid)
{
	m_taskid = taskid;
}

/*! 
    \fn NMTask::setStatus( NMTaskStatus* status)
    \brief Set object status for task
    \param status object status
*/
void NMTask::setStatus( NMTaskStatus* status)
{
	m_status = status;
}

/*! 
    \fn void NMTask::process()
    \brief Process or check the status task
*/
void NMTask::process()
{
	m_status->process(this);
}

