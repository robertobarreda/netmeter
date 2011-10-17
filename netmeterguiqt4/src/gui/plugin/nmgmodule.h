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
#ifndef NMGMODULE_H
#define NMGMODULE_H

#include <nmmoduleinterface.h>

#include "nmgwidgetfactory.h"
#include "nmgisender.h"
#include "nmgimodulestationmanager.h"
#include "nmgimodulecommunication.h"
#include "nmgmodulepreferencesmanager.h"
#include "nmgmodtabwidgetmanager.h"
#include <nmgstation.h>
#include <nmgconnection.h>

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QToolBar>

#define XML_WIDGET_CONFIGURATION_FILE_EXTENSION ".wconf.xml"

#define NMMODULE_CORENAME(x) extern "C"{ \
char * getModuleCoreName(){ return x;}}

#define NMMODULE_GUINAME(x) extern "C"{ \
char * getModuleGUIName(){ return x;}}

#define NMMODULE_MODULEICON(x) extern "C"{ \
char * getModuleIconName(){ return x;}}

#define NMMODULE_TOOLTIP(x) extern "C"{ \
char * getModuleToolTip(){ return x;}}

class NMGModule : public NMModuleInterface, public NMGISender, public NMGIModuleStationManager, public NMGIModuleCommunication
{
	public:
		NMGModule() :tabWidgetManager ( this ), preferencesManager ( this ) { }
		virtual ~NMGModule() {}
		void initModule();
		void removeModule();
		virtual void initModuleGUI();
		virtual void removeModuleGUI();
		bool sendGUICommand ( NMGCommandType commandType, const QString & data = "", const QString & receiver = "" );
		bool sendCommand ( NMGCommandType commandType, const QString & data = "" );
		void dataReceivedParent ( QString s );
		virtual void dataReceived ( QString s );
		virtual void errorReceived ( int errorType, QString errorMessage );
		virtual void statusReceived ( QString s );
		virtual void commandReceived ( QString moduleName, NMGCommandType command, const QString & data = "" );
		virtual void moduleExecutionResults ( QString moduleName, QString data );
		virtual bool setExecutionData ( QString data );
		QString request ( QString moduleName, QString data );
		void endRequest ( QString id );
		void endGUIRequest ( QString data );
		QString subscribe ( QString moduleNameSubscriptor, QString data );
		void endSubscribe ( QString id );
		QString executeModule ( QString moduleName, QString data );
		bool existsModule ( QString moduleName );
		virtual QMenu * getPopupMenu();
		virtual QToolBar * getToolBar();
		virtual void notifyNewStation ( NMGStation * );
		virtual void start ( void *data );
		virtual void stop ( void *data );
		virtual void status ( void *data );
		virtual void configure ( void *data );
		QString getName();
		QString getModuleName();
		QString getModuleCoreName();
		QString getModuleGUIName();
		QString getModuleCapabilities();
		QString getModuleVersion();
		QString getXMLWidgetConfigurationFile();
		virtual QWidget * getWidget();
		QString getTestId();
		void setTestId ( QString newTestId );

		/* Functions for the module station manager interface */
		void addFieldToFieldsManager ( QString fieldId, QString fieldToShow, QString defaultValue="" );
		QString getModuleStationKey();
		NMGModuleFieldsManager & getFieldsManager();
		bool existsStation ( QString hostAddressOrAlias );
		NMGStation * getStation ( QString hostAddressOrAlias );
		void addStation ( NMGStation * );
		void removeStation ( QString hostAddressOrAlias );
		NMGHostInformationComboWidget * createHostComboWidget ( QWidget *parent, const char *name=0 );
		NMGModTabWidgetManager  & getTabWidgetManager();
		NMGModulePreferencesManager & getPreferencesManager();

	protected:
		QString moduleName;
		QString moduleCoreMatching;
		QString moduleGUIName;
		QString moduleCapabilities;
		QString moduleVersion;
		NMGWidgetFactory factory;
		QWidget * widget;
		NMGModTabWidgetManager tabWidgetManager;
		NMGModulePreferencesManager preferencesManager;

	private:
		QString currentTestId;
};

#endif
