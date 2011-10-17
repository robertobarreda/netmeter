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
#ifndef NMTASK_H
#define NMTASK_H

#include <nmparent.h>
#include <nmthread.h>
#include <nmtaskstatus.h>
#include <nmdatetime.h>


class NMTaskStatus;

/**
@author Pau Capella
@brief Provide information about current NetMeter tasks.
*/

class NMTask: public NMParent, public NMThread {

public:
	NMTask( int testid, NMString plugin, NMString xmlparam, NMDateTime startime, const char *name = 0, NMParent *parent = 0);
	~NMTask();

	int testId();
	int taskId();
	NMString plugin();
	NMString* xmlData();
	NMDateTime startTime();
	NMTaskStatus* status();
	
	void setTaskId( int taskid);
	void setStatus( NMTaskStatus *status);

	void process();

protected:
	virtual void run() = 0;

private:
	int m_testid;
	int m_taskid;
	NMString m_plugin;
	NMString *m_xmlparam;
	NMDateTime m_starttime;
	NMTaskStatus *m_status;
};

#endif
