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

#ifndef NMGSIMULATOR_H
#define NMGSIMULATOR_H

#include <nmgmodule.h>
#include <QMenu>
#include "nmgsimulatorwidget.h"

#define MOD_CLASS		NMGSimulator
#define MOD_NAME		"NMGSimulator"
#define MOD_CAPABILITIES	"Network Setup"
#define MOD_VERSION		"0.1"
#define CORE_MATCHING		"nmsimulator"
#define GUI_NAME		"Network Emulator"

/**
@author Ernest Nieto
*/
class NMGSimulator : public NMGModule
{
	public:

		NMGSimulator();
		~NMGSimulator();
		void dataReceived ( QString s );
		bool setExecutionData ( QString data );

		QWidget * getWidget();
		QToolBar * getToolBar();
		QMenu * getPopupMenu();
		
	private:
		NMGSimulatorWidget * widget;
};

#define FIELDS2 ((char ***) {{"App1","App 1 Path"},{"App2","App 2 Path"},0})
#define FIELDS 0


NMMODULE_CLASS ( MOD_CLASS )
NMMODULE_NAME ( ( char * ) MOD_NAME )
NMMODULE_CAPABILITIES ( ( char * ) MOD_CAPABILITIES )
NMMODULE_VERSION ( ( char * ) MOD_VERSION )
NMMODULE_CORENAME ( ( char * ) CORE_MATCHING )
NMMODULE_GUINAME ( ( char * ) GUI_NAME )
NMMODULE_TOOLTIP ( ( char * ) "Network Emulator module." )
NMMODULE_MODULEICON( ( char * ) "network2.png")

#endif
