/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
 *   Copyright (C) 2007 by Joan Sala                                       *
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

#include "nmgmgen.h"
#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return 
 */
NMGMGen::NMGMGen()
{
	moduleName = MOD_NAME;
	moduleCapabilities = MOD_CAPABILITIES;
	moduleVersion = MOD_VERSION;
	moduleCoreMatching = CORE_MATCHING;
	moduleGUIName = GUI_NAME;
	widget=0;
	
	addFieldToFieldsManager(ID_MGEN_PATH,"MGen Path","/usr/local/bin/mgen");
	addFieldToFieldsManager(ID_DREC_PATH,"Drec Path","/usr/local/bin/drec");
	addFieldToFieldsManager(ID_DECODE_PATH,"Decode Path","/usr/local/bin/decode");
	addFieldToFieldsManager(ID_MGEN4_PATH, "MGen v4 Path","/usr/local/bin/mgen4");
}


/**
 * Destructor of the class
 * @return 
 */
NMGMGen::~NMGMGen()
{
	if(widget->isTestRunning()) sendCommand(Stop,"");
}


void NMGMGen::initModuleGUI()
{
}

void NMGMGen::removeModuleGUI()
{
}

void NMGMGen::createWidget()
{
	if(!widget) widget = new NMGMGenWidget(this,this,this,&flowManager,&getTabWidgetManager(),&getPreferencesManager());
}

/**
 * Returns the widget associated to the module
 * @return 
 */
QWidget * NMGMGen::getWidget()
{
	createWidget();
	return widget;
}
 
/**
 * Returns the popup menu associated to the module
 * @return 
 */
QMenu * NMGMGen::getPopupMenu()
{
	createWidget();
	return widget->getPopupMenu();
}   

/**
 * Returns the toolbar associated to the module
 * @return 
 */
QToolBar * NMGMGen::getToolBar()
{
	createWidget();
	return widget->getToolBar();
}

/**
 * The server has sent a result to a start request
 * @param data Data which was between the tags <data> ...</data> (including the tags)
 */
void NMGMGen::dataReceived(QString data)
{
	//TODO: test if pendent...
	if(!widget) 
		cerr << RED << "[ERROR] NMGMGen::dataReceived with no widget created" << ENDCOLOR << endl;
	else widget->notifyResult(data);
}

/**
 * The server sent a error message to the module
 * @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
 * @param errorMessage Error Message
 */
void NMGMGen::errorReceived(int errorType, QString errorMessage)
{
	#ifdef DEBUG
		cout << "NMGMGen::errorReceived type="<<errorType<< ", message=" << qPrintable(errorMessage) <<"."<<endl;
	#endif
	if(!widget) 
		cerr << RED << "[ERROR] MGEN Error received with no widget: type="<<errorType<<", message="<< qPrintable(errorMessage) << ENDCOLOR << endl;

	else widget->notifyError(errorType, errorMessage);
}

/**
 * The server sent a status reply
 * @param data Data which was between the tags <data> ...</data> (including the tags)
 */
void NMGMGen::statusReceived(QString data)
{
	if(!widget) 
		cerr << RED <<"[ERROR] MGEN Status received with no widget:"<< qPrintable(data) << ENDCOLOR << endl;
	else 
		widget->notifyStatus(data);
}
