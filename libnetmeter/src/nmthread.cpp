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
#include "nmthread.h"

NMThread::NMThread()
:m_running(false), m_finished(false)
{
	m_handle = 0;
	m_args = 0;
}


NMThread::~NMThread()
{
	if ( !m_finished && m_running)
		kill();
}

/*!
    \fn NMThread::start()
    \brief Start the execution of thread calling run() in the subclass.
*/
void NMThread::start()
{
	if (m_running) {
		cerr << "START: Thread are runnig!" << '\n';
		return;
	}
	int ret;
	
	m_running = true;
	m_finished = false;
	m_args = this;

	ret = pthread_create(&m_handle, NULL, startThread, m_args);
	if(ret != 0) {
		cout << "CRITICAL ERROR: No thread creation: " << strerror(ret) << endl;
		m_running = false;
		m_finished = false;
		m_args = 0;
		return;
	}
	ret = pthread_detach( m_handle);
	if( ret != 0)
		cout << "WARINIG: Thread is not in detach state: " << strerror(ret) << endl;
}

/**
\fn NMThread::kill()
\brief Terminate the execution of thread.
\return  On success, TRUE is returned. Otherwise FALSE is returned.
*/
bool NMThread::kill()
{
	if ( !m_handle ) return true;
	int ret;
    	ret = pthread_cancel( m_handle );
	if (ret) return false;
	m_finished = false;
	m_running = false;
	return true;
}

/**
\fn NMThread::isFinished()
\return Returns TRUE if the thread is finished; otherwise returns FALSE.
*/
bool NMThread::isFinished() const
{
	return m_finished;
}

/**
\fn NMThread::isRunning()
\return  Returns TRUE if the thread is running; otherwise returns FALSE.
*/
bool NMThread::isRunning() const
{
	return m_running;
}

/**
\fn NMThread::wait()
\brief A thread calling this function will block until the thread associated with this NMThread object has finished execution
\return Returns TRUE if not error ocurred; otherewise return FALSE.
*/
bool NMThread::wait() const
{
	if ( !m_handle) return false;
	return !pthread_join( m_handle, NULL) ? true : false;
}


void *NMThread::startThread( void *args)
{
	NMThread *th = (NMThread *) args;

	//Excuta el thread...
	th->run();

	th->m_finished = true;
	th->m_running = false;

	pthread_exit(NULL);	
}
