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
#include "nmmutex.h"
#include <string.h>

/**
\fn NMMutex::NMMutex()
\brief Constructs a new mutex. The mutex is created in an unlocked state.
*/
NMMutex::NMMutex()
{
	int ret = pthread_mutex_init (&mutex, NULL);
	if (ret)
		cout << "Warning: Mutex init failure: " <<  strerror(ret) << endl;
}


NMMutex::~NMMutex()
{
	int ret = pthread_mutex_destroy(&mutex);
	if (ret)
		cout << "Warning: Mutex destroy failure: " <<  strerror(ret) << endl;
}

/**
\fn NMMutex::lock()
\brief Attempt to lock the mutex. If another thread has locked the mutex then this call will block until that thread has unlocked it.
*/
void NMMutex::lock()
{
	int ret = pthread_mutex_lock(&mutex);
	if (ret)
		cout << "Warning: Mutex lock failure: " <<  strerror(ret) << endl;

}

/**
\fn NMMutex::unlock()
\brief Unlocks the mutex. Attempting to unlock a mutex in a different thread to the one that locked it results in an error. Unlocking a mutex that is not locked results in undefined behaviour.
*/
void NMMutex::unlock()
{
	int ret = pthread_mutex_unlock(&mutex);
	if (ret)
		cout << "Warning: Mutex unlock failure: " <<  strerror(ret) << endl;
}


