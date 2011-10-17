/***************************************************************************
 *   Copyright (C) 2007 by Joan Sala Calero <jsala@ac.upc.edu>             *
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

#ifndef NMGCONVERTER_H
#define NMGCONVERTER_H

#include <nmgmodule.h>
#include <nmstring.h>

#include "nmgconverterwidget.h"
#include "nmgconverterwidgetfromconf.h"

#define MOD_CLASS		NMGConverter
#define MOD_NAME		"NMGConverter"
#define MOD_CAPABILITIES	"Analysis"
#define CORE_MATCHING	  	"Analysis"
#define MOD_VERSION		"0.2"
#define MOD_ICON                "converter.png"
#define MOD_TOOLTIP		"Module to convert files within different formats"
#define GUI_NAME		"Script Runner"


class NMGConverter : public NMGModule
{
	public:
		/** Constructor && Destructor of the class **/
		NMGConverter();
		~NMGConverter();

		void initModuleGUI();
		void removeModuleGUI();

		QWidget * getWidget();
		QMenu * getPopupMenu();
		QToolBar * getToolBar();
		
		bool setExecutionData ( QString data );
		void commandReceived ( QString moduleName, NMGCommandType command, const QString & data = "" );

	private:
		void createWidget();	
		NMGConverterWidget * widget;
};


NMMODULE_CLASS(MOD_CLASS)
NMMODULE_NAME((char *) MOD_NAME)
NMMODULE_CAPABILITIES((char *) MOD_CAPABILITIES)
NMMODULE_VERSION((char *) MOD_VERSION)
NMMODULE_CORENAME((char *)CORE_MATCHING)
NMMODULE_GUINAME((char *)GUI_NAME)
NMMODULE_MODULEICON((char *)MOD_ICON)
NMMODULE_TOOLTIP((char *)MOD_TOOLTIP)

#endif
