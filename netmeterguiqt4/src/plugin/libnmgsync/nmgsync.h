/***************************************************************************
 *   Copyright (C) 2005 by Joan Sala                                       *
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

#ifndef NMGSYNC_H
#define NMGSYNC_H

#include <nmgmodule.h>
#include "nmgsyncwidget.h"
#include "nmgsyncremote.h"

#include <QMenu>
#include <QMessageBox>

#define MOD_CLASS		NMGSync
#define MOD_NAME		"NMGSync"
#define MOD_CAPABILITIES	"test"
#define MOD_VERSION		"0.1"
#define CORE_MATCHING		"nmsync"
#define GUI_NAME		"Sync"

/**
@author Joan Sala
*/
class NMGSync : public NMGModule
{

public:
    	NMGSync();
    	~NMGSync();

	// NMGModule
	
	void initModuleGUI();
    	void removeModuleGUI();
    	void notifyNewStation(NMGStation *);
    	void statusReceived(QString data);
    	void errorReceived(int errorType, QString errorMessage);
    	void dataReceived(QString data);

	// NMGModuleComunication

	bool setExecutionData(QString data);
	void commandReceived(QString moduleName, NMGCommandType command, const QString &);

    	QWidget * getWidget();
    	QMenu * getPopupMenu();

private:
	NMGSyncWidget * widget;
	NMGSyncRemote * remote;
	QString conf;
};

NMMODULE_CLASS( MOD_CLASS )
NMMODULE_NAME(( char * ) MOD_NAME )
NMMODULE_CAPABILITIES(( char * ) MOD_CAPABILITIES )
NMMODULE_VERSION(( char * ) MOD_VERSION )
NMMODULE_CORENAME(( char * ) CORE_MATCHING )
NMMODULE_GUINAME(( char * ) GUI_NAME )
NMMODULE_TOOLTIP(( char * ) "Module to study ntptime output in order to evaluate remote sync state" )
NMMODULE_MODULEICON((char *)"sync.png")

#endif
