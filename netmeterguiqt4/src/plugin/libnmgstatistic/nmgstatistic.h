/***************************************************************************
  *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#ifndef NMGSTATISTIC_H
#define NMGSTATISTIC_H

#include "nmgstatisticwidget.h"
#include "nmgstatisticremotewidget.h"

#include <nmgmodule.h>

#define MOD_CLASS		NMGStatistic
#define MOD_NAME		"NMGStatistic"
#define MOD_CAPABILITIES	"Analysis"
#define MOD_VERSION		"0.1"
#define GUI_NAME		"Statistic"
#define MOD_ICON		"statistic.png"
#define MOD_TOOLTIP		"Module for getting statistics from test results to study the one way delay and throughput"

/*! \brief NMGStatistic undertakes responsability for communicating Statistic Analysis plugin with
 * the other parts of Netmeter GUI. */
class NMGStatistic : public NMGModule
{
	public:
		/*! \brief Default constructor. */
		NMGStatistic();
	
		/*! \brief Default destructor. */
		~NMGStatistic();
	
		/*! \brief Called before loading the plugin. */
		void initModuleGUI();
	
		/*! \brief Called before unloading the plugin. */
		void removeModuleGUI();
	
		/*! \brief Returns the main graphical widget associated to the module. */
		QWidget * getWidget();
	
		/*! \brief Returns the popup menu associated to the module.
		  * \return QMenu pointer to the plugin's menu */
		QMenu * getPopupMenu();
	
		/*! \brief Returns the toolbar associated to the module.
		  * \return QToolBar pointer to the plugin's toolbar */
		QToolBar * getToolBar();
	
		/*! \brief Responds a remote call execution (SUBSCRIPTION or REQUEST).
		 *  \param data request configuration in xml format
		 * \return TRUE if xml request is correctly formated */
		bool setExecutionData ( QString data );
		
		/*! \brief Command received from a subscripted module
		* \param moduleName Name of the module subscripted
		* \param command Commnad executed by the module
		* \param data xml message from the subscriptor */
		void commandReceived ( QString moduleName, NMGCommandType command, const QString & data = "" );

	private:
		void createWidget();
		NMGStatisticWidget * widget;
		NMGStatisticRemoteWidget * remoteWidget;
};

NMMODULE_CLASS ( MOD_CLASS )
NMMODULE_NAME ( ( char * ) MOD_NAME )
NMMODULE_CAPABILITIES ( ( char * ) MOD_CAPABILITIES )
NMMODULE_VERSION ( ( char * ) MOD_VERSION )
NMMODULE_GUINAME ( ( char * ) GUI_NAME )
NMMODULE_MODULEICON ( ( char * ) MOD_ICON )
NMMODULE_TOOLTIP ( ( char * ) MOD_TOOLTIP )

#endif
