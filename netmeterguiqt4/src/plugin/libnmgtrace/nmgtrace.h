/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella                                     *
 *   pcapella@ac.upc.edu                                                   *
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

#ifndef NMGTRACE_H
#define NMGTRACE_H

#include <nmgmodule.h>
#include "nmgtracewidget.h"

#define MOD_CLASS		NMGTrace
#define MOD_NAME		"NMGTrace"
#define MOD_CAPABILITIES	"test"
#define MOD_VERSION		"0.1"
#define CORE_MATCHING		"nmtrace"
#define GUI_NAME		"Trace"

class NMGTraceWidget;

/**
@author Pau Capella <pcapella@ac.upc.edu>
*/
class NMGTrace : public NMGModule
{
public:
	NMGTrace();
	~NMGTrace();
	void initModuleGUI();
        void removeModuleGUI();

	QWidget *getWidget();

private:

	void dataReceived(QString data);
	void statusReceived(QString data);
	void errorReceived(int errorType, QString errorMessage);

	NMGTraceWidget *m_tracewidget;
};

NMMODULE_CLASS( MOD_CLASS )
NMMODULE_NAME(( char * ) MOD_NAME )
NMMODULE_CAPABILITIES(( char * ) MOD_CAPABILITIES )
NMMODULE_VERSION(( char * ) MOD_VERSION )
NMMODULE_CORENAME(( char * ) CORE_MATCHING )
NMMODULE_GUINAME(( char * ) GUI_NAME )
NMMODULE_TOOLTIP(( char * ) "Module to study the traceroute output")
NMMODULE_MODULEICON((char *) "trace.png")

#endif
