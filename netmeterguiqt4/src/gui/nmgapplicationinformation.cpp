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
#include "nmgapplicationinformation.h"

#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QMainWindow>

#include <iostream>
using namespace std;

NMGApplicationInformation * NMGApplicationInformation::info = 0;

/**
 * Constructor of the class NMGApplicationInformation (private method, to acces use getGApplicationInformation)
 * @return A instance of NMGApplicationInformation
 */
NMGApplicationInformation::NMGApplicationInformation()
{
	appName = APP_NAME;
	appVersion = APP_VERSION;
	serverName = "";
	serverVersion = "";
	pluginInfo = 0;
	configurationPath = "";
	pathValidated = false;
	mainWindow = 0;
}

/**
 * Destructor of te class NMGApplicationInformation
 */
NMGApplicationInformation::~NMGApplicationInformation()
{}

void NMGApplicationInformation::createConfigurationDirectory()
{
	QString dirPath = QDir::homePath() + "/" + HOME_DIRECTORY_PATH;
	QDir dir ( dirPath );
	if ( not dir.exists() )
	{
		if ( not dir.mkdir ( dirPath ) )
		{
			cerr << "Error while creating the directory " << qPrintable ( dirPath ) << endl;
			dirPath = "/tmp/.netmeter";
			QDir dirTmp ( dirPath );
			if ( not dirTmp.exists() ) dirTmp.mkdir ( dirPath );
			cerr << "Using " << qPrintable ( dirPath ) << " instead." << endl;
		}
	}
	getApplicationInformation()->setConfigurationPath ( dirPath );
}

/**
 * Returns the unique instance of the class NMGApplicationInformation
 * @return The instance of NMGApplicationInformation
 */
NMGApplicationInformation * NMGApplicationInformation::getApplicationInformation()
{
	if ( !info ) info = new NMGApplicationInformation();
	return info;
}

/**
 * Returns the name of the application
 * @return Name of the application
 */
QString NMGApplicationInformation::getAppName()
{
	return appName;
}

/**
 * Returns the version of the application
 * @return Version of the application
 */
QString NMGApplicationInformation::getAppVersion()
{
	return appVersion;
}

/**
 * Returns the name of the server connected to
 * @return Name of the server connected to ("" if not connected)
 */
QString NMGApplicationInformation::getServerName()
{
	return serverName;
}

/**
 * Sets the server name connected to
 * @param s The server's name connected to
 */
void NMGApplicationInformation::setServerName ( QString s )
{
	serverName = s;
}

/**
 * Returns the server version connected to
 * @return Version of the server connected to
 */
QString NMGApplicationInformation::getServerVersion()
{
	return serverVersion;
}

/**
 * Sets the server version connected to
 * @param s The server0s version connected to
 */
void NMGApplicationInformation::setServerVersion ( QString s )
{
	serverVersion = s;
}

/**
 * Returns a map with all the information of the modules which are in the server
 * @return QMap<QString,NMGPluginInformation> with info of every plugin of the server
 */
QMapPluginInformation * NMGApplicationInformation::getPluginInformation()
{
	return pluginInfo;
}

/**
 * Sets the map with the plugin information of the server
 * @param map QMap<QString,NMGPluginInformation> with info of every plugin of the server
 */
void NMGApplicationInformation::setPluginInformation ( QMapPluginInformation *map )
{
	pluginInfo = map;
}

/**
 * Returns the path for the XML widget configuration files
 * @return The path for the XML widget configuration files
 */
QString NMGApplicationInformation::getConfigurationPath()
{
	/*	int i = 0;
		while(!pathValidated and i++<5)
		{
			QFile file(configurationPath + NMGUI_CONFIGURATION_FILE);
			if(file.open(IO_ReadOnly)) pathValidated = true;
			else
			{
				QString dir = QFileDialog::getExistingDirectory(".",0,0,"Choose configuration directory");
				if(dir) configurationPath = dir;
			}
		}*/
	return configurationPath;
}

/**
 * Returns the path for the XML widget configuration files
 * @return The path for the XML widget configuration files
 */
QString NMGApplicationInformation::getXMLPath()
{
	return XMLPath;
}

/**
 * Returns the path for the XML widget configuration files
 * @return The path for the XML widget configuration files
 */
QString NMGApplicationInformation::getResultPath()
{
	return resultPath;
}

/**
 * Sets the path for the XML widget configuration files
 * @param s New path for the XML widget configuration files
 */
void NMGApplicationInformation::setConfigurationPath ( QString s )
{
	configurationPath = s;
}

/**
 * Sets the path for the XML widget configuration files
 * @param s New path for the XML widget configuration files
 */
void NMGApplicationInformation::setXMLPath ( QString s )
{
	XMLPath = s;
}

/**
 * Sets the path for the XML widget configuration files
 * @param s New path for the XML widget configuration files
 */
void NMGApplicationInformation::setResultPath ( QString s )
{
	resultPath = s;
}

/**
 * Returns the main window of the application
 * @return
 */
NMGMainWindow * NMGApplicationInformation::getMainWindow()
{
	return mainWindow;
}

/**
 * Sets the main window of the application
 * @param
 */
void NMGApplicationInformation::setMainWindow ( NMGMainWindow * w )
{
	mainWindow = w;
}
