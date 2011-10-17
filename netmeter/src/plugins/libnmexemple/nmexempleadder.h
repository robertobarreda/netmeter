/*
 *   $Id: nmexempleadder.h,v 1.2 2007-02-18 23:59:29 rserral Exp $
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
#ifndef NMEXEMPLEADDER_H
#define NMEXEMPLEADDER_H


#include <nmmoduleinterface.h>
#include <nmxml.h>


/**
@author Pau Capella
*/
class NMExempleAdder : public NMModuleInterface
{
public:
    NMExempleAdder();

    ~NMExempleAdder();

	void initModule();
	void removeModule();
	void start(void *data);
	void stop(void *data);
	void status(void *data);
	void configure(void *data);

	void processRecived();
	void processExit();
};

NMMODULE_CLASS(NMExempleAdder)
NMMODULE_NAME((char *)"nmexempleadder")
NMMODULE_CAPABILITIES((char *)"test")
NMMODULE_VERSION((char *)"0.1")

#endif
