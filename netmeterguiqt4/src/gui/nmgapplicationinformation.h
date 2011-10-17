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
#ifndef NMGAPPLICATIONINFORMATION_H
#define NMGAPPLICATIONINFORMATION_H

#include <QString>
#include <QMap>

class NMGMainWindow;

struct NMGPluginInformation
{
	QString name;
	QString capabilities;
	QString version;
};

typedef QMap<QString,NMGPluginInformation> QMapPluginInformation;

#define APP_NAME 	"netmetergui"
#define APP_VERSION	"0.1"

#define NMGUI_CONFIGURATION_FILE	"netmetergui.xml"

#define HOME_DIRECTORY_PATH	".netmeter/"

/**
Class wich has information about the application
*/
class NMGApplicationInformation
{
	private:
		NMGApplicationInformation();

	public:
		~NMGApplicationInformation();
		static NMGApplicationInformation * getApplicationInformation();
		QString getAppName();
		QString getAppVersion();
		QString getServerName();
		void setServerName ( QString s );
		QString getServerVersion();
		void setServerVersion ( QString s );
		QMapPluginInformation * getPluginInformation();
		void setPluginInformation ( QMapPluginInformation *map );
		QString getConfigurationPath();
		QString getXMLPath();	
		QString getResultPath();
		void setConfigurationPath ( QString s );
		void setXMLPath ( QString s );
		void setResultPath ( QString s );
		NMGMainWindow * getMainWindow();
		void setMainWindow ( NMGMainWindow * );
		static void createConfigurationDirectory();

	private:
		static NMGApplicationInformation * info;
		NMGMainWindow * mainWindow;
		QString appName;
		QString appVersion;
		QString serverName;
		QString serverVersion;
		QMapPluginInformation * pluginInfo;
		QString configurationPath;
		QString XMLPath;
		QString resultPath;
		bool pathValidated;
};

#endif
