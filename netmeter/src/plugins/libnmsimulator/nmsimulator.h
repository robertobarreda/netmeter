/***************************************************************************
 *   Copyright (C) 2005 by Rus Andrei Bogdan                               *
 *   bogdanrus2004@yahoo.com                                               *
 *   Copyright (C) 2008 by Joan Sala Calero                                *
 *   jsala@ac.upc.edu                                                      *
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

#ifndef NMSIMULATOR_H
#define NMSIMULATOR_H

#include <iostream>
#include <string.h>
#include <stdio.h>

#include <nmmoduleinterface.h>
#include <nmxml.h>
#include <nmftp.h>
#include <nmparent.h>
#include <nmexternalremote.h>
#include <nmdns.h>
#include <nmstring.h>
#include <nmfile.h>

/**
@author Rus Andrei Bogdan
*/
class NMSimulator : public NMModuleInterface
{
public:
    NMSimulator();

    ~NMSimulator();

	void initModule();
	void removeModule();
	void start(void *data);
	void stop(void *data);
	void status(void *data);
	void configure(void *data);
	void processRecived();
	void processExit();



private:

	NMExternal * simulatorExternal, * proc;
	NMFtp *sftp;
	NMString response, managementProtocol, managementPassword, interfacesName[100], managementUsername, managementAddress;
	NMHost sourceHost;
	NMString simulator_file_1, simulator_file_2, simulator_script_2;
	
	int addressPort, commandNumber, interfacesNumber;
	void readTC(); 
	void getTm1(); 
	void getTm2();
	void gettingDataFromTm1();
	void gettingDataFromTm2();
	int checkTC(char *interface);
	void implementSettings();
};

#endif
