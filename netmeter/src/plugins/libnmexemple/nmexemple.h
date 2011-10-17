/*
 *   $Id: nmexemple.h,v 1.2 2007-05-27 11:19:34 rserral Exp $
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
#ifndef NMEXEMPLE_H
#define NMEXEMPLE_H

#include <nmmoduleinterface.h>
#include <nmxml.h>


/**
@author Ernest Nieto
*/
class NMExemple : public NMModuleInterface
{
public:
    NMExemple();

    ~NMExemple();

	void initModule();
	void removeModule();
	void start(void *data);
	void stop(void *data);
	void status(void *data);
	void configure(void *data);

	void processRecived();
	void processExit();
};
NMMODULE_CLASS(NMExemple)
NMMODULE_NAME((char *)"nmexemple")
NMMODULE_CAPABILITIES((char *)"test")
NMMODULE_VERSION((char *)"0.1")

#endif
