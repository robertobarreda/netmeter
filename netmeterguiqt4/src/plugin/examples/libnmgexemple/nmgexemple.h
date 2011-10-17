/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
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
#ifndef NMGEXEMPLE_H
#define NMGEXEMPLE_H

#include "nmgexemplewidget.h"
#include <nmgmodule.h>

#include <QMenu>
#include <QString>

#define MOD_CLASS		NMGExemple
#define MOD_NAME		"NMGExemple"
#define MOD_CAPABILITIES	"example"
#define MOD_VERSION		"0.1"
#define CORE_MATCHING		"nmexemple"
#define GUI_NAME		"Exemple"

/**
@author Ernest Nieto
*/
class NMGExemple : public NMGModule
{
	public:
		NMGExemple();
		~NMGExemple();
		void initModuleGUI();
		void removeModuleGUI();
		void notifyNewStation( NMGStation * );
		void dataReceived( QString data );
		QWidget * getWidget();
		QMenu * getPopupMenu();

	private:
		NMGExempleWidget * widget;
};

#define FIELDS2 ((char ***) {{"App1","App 1 Path"},{"App2","App 2 Path"},0})
#define FIELDS 0

NMMODULE_CLASS( MOD_CLASS )
NMMODULE_NAME(( char * ) MOD_NAME )
NMMODULE_CAPABILITIES(( char * ) MOD_CAPABILITIES )
NMMODULE_VERSION(( char * ) MOD_VERSION )
NMMODULE_CORENAME(( char * )CORE_MATCHING )
NMMODULE_GUINAME(( char * )GUI_NAME )
NMMODULE_TOOLTIP(( char * ) "Simple example module.\nSends a message to the server and the server replie the same." )

#endif
