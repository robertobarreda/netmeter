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
#ifndef NMEXTERNALLOCAL_H
#define NMEXTERNALLOCAL_H

#include "nmexternal.h"
#include "nmbuffer.h"
#include <unistd.h>
#include <errno.h>
#include <signal.h>

/**
@author Pau Capella
@brief Permit to run local programs.
*/
/**
<b> General usage and features </b>\n
This class allows run child processes without having
to worry about UNIX signal handling issues and zombie process reaping.
When the child process is invoked, child and the parent process run concurrently.
A small usage example:\n
\code
    NMExternalLocal proc("myproc",NULL);
    proc << "my_executable";
    proc << "These" << "are" << "the" << "command" << "line" << "args";
    proc->run();
 \endcode
<b>Communication with the child process:</b>\n
NMExternalLocal supports communication with the child process through
stdin and stdout.
The class have a NMSocket public member (stdInOut).See NMSocket class help for methodes information.
*/

class NMExternalLocal : public NMExternal
{

public:
	NMExternalLocal(const char *name = 0, bool listener = true, NMParent *parent = 0);
	~NMExternalLocal();

	virtual NMExternal::Error run();
	//bool kill();
};

#endif
