/***************************************************************************
 *   Copyright (C) 2006 by David Lacort                                    *
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#include "nmggraphics.h"

NMGGraphics::NMGGraphics()
{
  moduleName = MOD_NAME;
  moduleCapabilities = MOD_CAPABILITIES;
  moduleVersion = MOD_VERSION;
  moduleCoreMatching = CORE_MATCHING;
  moduleGUIName = GUI_NAME;
  widget = 0;
}

NMGGraphics::~NMGGraphics()
{ 
  /* NOTE
   * The widget should be deleted to free memory but due to a problem with netmeterGUI
   * it can't be done when it should be, it means now (if done an obscure segmentation 
   * fault occurs).
   * The consequences are that when a new instance of the plugin is closed in its 
   * corresponding tab in the user interface, all structures used by that instance of
   * the plugin remain into memory until the application is closed and the system removes
   * safely all the structures. It is not a problem for the plugin, that works correctly
   * instead of that, but it has to be under consideration when the plugin uses a 
   * considerable amount of memory to work (as this plugin does).
   * 
   * if(widget) delete widget;
   */
}

void NMGGraphics::initModuleGUI() {}

void NMGGraphics::removeModuleGUI() {}

void NMGGraphics::createWidget()
{
  if(!widget) widget = new NMGGraphicsWidget(0,"graphic widget",&getPreferencesManager());
}

QWidget * NMGGraphics::getWidget()
{
  createWidget();
  return widget;
}

QMenu * NMGGraphics::getPopupMenu()
{
  createWidget();
  return 0;
}   

QToolBar * NMGGraphics::getToolBar()
{
  createWidget();
  return widget->getToolBar();
}

bool NMGGraphics::setExecutionData( QString data )
{
	cout << "Data Recieved: " << qPrintable( data ) << endl;
	if ( data.contains("subscribe", Qt::CaseInsensitive) ) // Attend a SUBSCRIBE command
	{
		/*! \par Received Message Format for a SUBSCRIPTION. It's always empty.
		 * Waits until arrives a START command.
		 * <data>	
		 *  	<subscribe /> --> IT SHOULD BE ALWAYS AN EMPTY TAG. INFORMATION IS SENT 
                 *                        BY sender->sendCommand ( Start, "<data><test>" + fileResultPath + "</test></data>" );
		 * </data>
		*/
		return true;
	}
        return false;
}

void NMGGraphics::commandReceived ( QString moduleName, NMGCommandType command, const QString & data )
{
	cout << "NMGGraphics::commandReceived (" << qPrintable ( NMGUtil::getCommandTypeName(command) );
        cout << ") from " << qPrintable ( moduleName ) << endl;

	if ( command == Start ) widget->loadData( data );
        else cerr << "[ERROR] NMGGraphics Command not supported" << endl;
}
