/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#ifndef NMGNETPERF_H
#define NMGNETPERF_H

#include <nmgmodule.h>

#include "nmgnetperfwidget.h"

#define MOD_CLASS		NMGNetperf
#define MOD_NAME		"NMGNetperf"
#define MOD_CAPABILITIES	"test"
#define MOD_VERSION		"0.1"
#define CORE_MATCHING		"NETPERF"
#define GUI_NAME		"Throughput"


class NMGNetperf : public NMGModule
{
	public:
		NMGNetperf();
		/**
		* Destructor of the class
		* @return
		*/
		inline ~NMGNetperf() { if ( widget->isTestRunning() ) sendCommand ( Stop, "" ); }

		inline void initModuleGUI() {}
		inline void removeModuleGUI() {}

		QWidget * getWidget();
		QMenu * getPopupMenu();
		QToolBar * getToolBar();

		void errorReceived ( int errorType, QString errorMessage );
		void statusReceived ( QString data );
		void dataReceived ( QString data );

	private:
		inline void createWidget()
		{ if ( !widget ) widget = new NMGNetperfWidget ( this, this, this, &getTabWidgetManager(), &getPreferencesManager() ); }

	private:
		NMGNetperfWidget * widget;
};

NMMODULE_CLASS ( MOD_CLASS )
NMMODULE_NAME ( ( char * ) MOD_NAME )
NMMODULE_CAPABILITIES ( ( char * ) MOD_CAPABILITIES )
NMMODULE_VERSION ( ( char * ) MOD_VERSION )
NMMODULE_CORENAME ( ( char * ) CORE_MATCHING )
NMMODULE_GUINAME ( ( char * ) GUI_NAME )
NMMODULE_MODULEICON ( ( char * ) "netperf.png" )
NMMODULE_TOOLTIP ( ( char * ) "Module for the NETPERF utility to study the one way delay" )

#endif
