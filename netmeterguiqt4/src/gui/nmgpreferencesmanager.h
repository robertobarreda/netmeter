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
#ifndef NMGPREFERENCESMANAGER_H
#define NMGPREFERENCESMANAGER_H

#include "nmgconfigurationfilemanager.h"
#include "nmgapplicationinformation.h"

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QMap>

#define PREFERENCES_FILE_NAME		"preferences.nmgui.xml"

#define TAG_PREFERENCES			"preferences"
#define TAG_PREFERENCE				"preference"
#define ATTR_PREF_KEY				"key"
#define ATTR_PREF_VALUE			"value"
#define ID_LOAD_ALL_MODULES		"LoadAllModules"
#define ID_LOAD_ALL_MODULES_TRUE	"True"
#define ID_LOAD_ALL_MODULES_FALSE	"False"
#define ID_MODULES_TO_LOAD		"ModulesToLoad"

#define NMG_IMAGES_PATH			"NMGImagesPath"

#define KEY_CONF_DIR			"ConfigurationDirectory"
#define KEY_XML_DIR			"XmlPluginDirectory"
#define KEY_RESULT_DIR  		"ResultDirectory"

#define NETMETER_SERVERPORT		"NetmeterServerPort"
#define NETMETER_SERVERADDRESS		"NetmeterServerAddress"
#define LOCALHOST                       "127.0.0.1"

#define TAB_GENERAL_PREFERENCES		0
#define TAB_MODULE_PREFERENCES		1
#define TAB_CONNECTION_PREFERENCES	2
#define TAB_STATIONS_PREFERENCES	3

namespace Ui {
    class NMGPreferencesBaseWidget;
} // namespace Ui

typedef QMap<QString,QString> NMGPreferencesManagerMap;
typedef NMGPreferencesManagerMap::iterator NMGPreferencesManagerMapIterator;

class NMGPreferencesManager : public QObject
{
		Q_OBJECT
	private:
		NMGPreferencesManager ( QObject *parent = 0, const char *name = 0 );

	public:
		~NMGPreferencesManager();
		static NMGPreferencesManager * self();
		void addValue ( QString key, QString value );
		QString getValue ( QString key );
		void removeValue ( QString key );
		bool existsValue ( QString key );
		QString getConfigurationDirectory();
		QString getResultDirectory();
		QString getXMLDirectory();
		QString getNetMeterAddress();
		int getNetMeterPort();

	public slots:
		void showGeneralPreferencesWidget();
		void showModulePreferencesWidget();
		void showConnectionPreferencesWidget();
                void showStationsPreferencesWidget();
		void setNetMeterAddress ( QString address );
		void setNetMeterPort ( int port );
		void setConfigurationDirectory ( QString dir );
		void setXMLDirectory ( QString dir );
		void setResultDirectory ( QString dir );
		void selectConfigurationDirectoryClicked();
		void selectImageDirectoryClicked();
		void selectResultDirectoryClicked();
		void selectLogFilePath();

	private slots:
		void updateWidgetsFromValues();
		void updateDiskFromWidget();
		void showModulesToLoadWidget();
		void showStationManagerWidget();

	private:
		void loadPreferencesFromDisk();
		void updateWidgetsFromAddedValue ( QString key, QString value );
		void createResultDirectory ( QString dirPath );

		QDialog w;
		Ui::NMGPreferencesBaseWidget * widget;
		static NMGPreferencesManager * instance;
		NMGPreferencesManagerMap preferencesMap;
		NMGXMLConfigurationFile * preferencesFile;
		bool mapLoadedFromDisk;
};

#endif
