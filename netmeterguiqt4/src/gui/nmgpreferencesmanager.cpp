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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nmgpreferencesmanager.h"
#include "nmgimagemanager.h"
#include <nmgpreferencesmodulewidget.h>
#include <nmghostinformationmanager.h>
#include <ui_nmgpreferencesbasewidget.h>

#include <QFileDialog>
#include <QTabWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QRadioButton>
#include <QMessageBox>
#include <QString>
#include <QDomDocument>
#include <QDomElement>

#include <iostream>
using namespace std;

NMGPreferencesManager * NMGPreferencesManager::instance = 0;

/**
 * Constructor of the class
 * @param parent Qt's parent parameter
 * @param name Qt's name parameter
 * @return
 */
NMGPreferencesManager::NMGPreferencesManager ( QObject *parent, const char *name )
		: QObject ( parent ), w()
{
	setObjectName ( name );
	mapLoadedFromDisk = false;
	widget = new Ui::NMGPreferencesBaseWidget();
	widget->setupUi ( &w );
	connect ( widget->btnSelectPlugins, SIGNAL ( clicked() ), this, SLOT ( showModulesToLoadWidget() ) );
	connect ( widget->btnManageStations, SIGNAL ( clicked() ), this, SLOT ( showStationManagerWidget() ) );
	connect ( widget->buttonOk, SIGNAL ( clicked() ), this, SLOT ( updateDiskFromWidget() ) );
	connect ( widget->buttonCancel, SIGNAL ( clicked() ), this, SLOT ( updateWidgetsFromValues() ) );
	connect ( widget->btnConfDirectoryPath, SIGNAL ( clicked() ), this, SLOT ( selectConfigurationDirectoryClicked() ) );
	connect ( widget->btnImagesDirectoryPath, SIGNAL ( clicked() ), this, SLOT ( selectImageDirectoryClicked() ) );
	connect ( widget->btnLogFilePath, SIGNAL ( clicked() ), this, SLOT ( selectLogFilePath() ) );
	connect ( widget->btnResultDirectoryPath, SIGNAL ( clicked() ), this, SLOT ( selectResultDirectoryClicked() ) );
}

/**
 * Loads the preferences from disk
 */
void NMGPreferencesManager::loadPreferencesFromDisk()
{
	preferencesFile = NMGConfigurationFileManager::self()->getXMLConfigurationFile ( PREFERENCES_FILE_NAME );

	if ( preferencesFile )
	{
		QDomElement root = preferencesFile->getDomDocument().documentElement();
		if ( !root.isNull() and root.tagName() != TAG_PREFERENCES )
			cerr << "Preferences file with wrong tag(" << qPrintable ( root.tagName() ) << ")." << endl;
		else if ( !root.isNull() )
		{
			for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
			{
				if ( node.isElement() and node.toElement().tagName() == TAG_PREFERENCE )
				{
					QString key = node.toElement().attribute ( ATTR_PREF_KEY );
					QString value = node.toElement().attribute ( ATTR_PREF_VALUE );
					addValue ( key, value );
				}
				else cerr << "Invalid preferences element." << endl;
			}
		}
	}
	mapLoadedFromDisk = true;
	updateWidgetsFromValues();
}

/**
 * Destructor of the class
 * @return
 */
NMGPreferencesManager::~NMGPreferencesManager()
{
	QDomDocument doc;
	QDomElement root = doc.createElement ( TAG_PREFERENCES );
	doc.appendChild ( root );
	for ( NMGPreferencesManagerMapIterator it = preferencesMap.begin(); it != preferencesMap.end(); ++it )
	{
		QDomElement elem = doc.createElement ( TAG_PREFERENCE );
		root.appendChild ( elem );
		elem.setAttribute ( ATTR_PREF_KEY, it.key() );
		elem.setAttribute ( ATTR_PREF_VALUE, it.value() );
	}

	if ( preferencesFile )
	{
		preferencesFile->getDomDocument().setContent ( doc.toString() );
		preferencesFile->writeToDisk();
	}
}

/**
 * Returns the unique instance of the class
 * @return
 */
NMGPreferencesManager * NMGPreferencesManager::self()
{
	if ( !instance )
	{
		instance = new NMGPreferencesManager();
		instance->loadPreferencesFromDisk();
	}
	return instance;
}

/**
 * Shows the preferences widget (showing the tab general)
 */
void NMGPreferencesManager::showGeneralPreferencesWidget()
{
	updateWidgetsFromValues();
	widget->tabWidget->setCurrentIndex ( TAB_GENERAL_PREFERENCES );
	w.show();
}

/**
 * Shows the preferences widget (showing the tab modules)
 */
void NMGPreferencesManager::showModulePreferencesWidget()
{
	updateWidgetsFromValues();
	widget->tabWidget->setCurrentIndex ( TAB_MODULE_PREFERENCES );
	w.show();
}

/**
 * Shows the station's manager widget (showing the tab stations)
 */
void NMGPreferencesManager::showConnectionPreferencesWidget()
{
	updateWidgetsFromValues();
	widget->tabWidget->setCurrentIndex ( TAB_CONNECTION_PREFERENCES );
	w.show();
}

/**
 * Shows the preferences widget (showing the tab connection)
 */
void NMGPreferencesManager::showStationsPreferencesWidget()
{
	updateWidgetsFromValues();
	widget->tabWidget->setCurrentIndex ( TAB_STATIONS_PREFERENCES );
	w.show();
}

/**
 * Adds a value to the preferences manager
 * @param key
 * @param value
 */
void NMGPreferencesManager::addValue ( QString key, QString value )
{
	preferencesMap.insert ( key, value );
	updateWidgetsFromAddedValue ( key, value );
}

/**
 * Returns a value form the preferences manager
 * @param key Key of the value
 * @return The value or "" if it doens't exist
 */
QString NMGPreferencesManager::getValue ( QString key )
{
	NMGPreferencesManagerMapIterator it = preferencesMap.find ( key );
	if ( it == preferencesMap.end() ) return "";
	return *it;
}

/**
 * Removes a value from the preferences manager
 * @param key Key of the value
 */
void NMGPreferencesManager::removeValue ( QString key )
{
	preferencesMap.remove ( key );
}

/**
 * Returns true if exists a value
 * @param key
 * @return
 */
bool NMGPreferencesManager::existsValue ( QString key )
{
	NMGPreferencesManagerMapIterator it = preferencesMap.find ( key );
	if ( it == preferencesMap.end() ) return false;
	return true;
}

/**
 * Returns the XML directory, if it's not defined the default one is chosen
 * @return Directory of the XML files
 */
QString NMGPreferencesManager::getXMLDirectory()
{
	QString dir;
	if ( !mapLoadedFromDisk )
		dir = NMGApplicationInformation::getApplicationInformation()->getXMLPath();
	else
		dir = getValue ( KEY_XML_DIR );
	if ( dir.isNull() || dir.isEmpty() )
		dir = NMGApplicationInformation::getApplicationInformation()->getXMLPath();

	if ( !mapLoadedFromDisk )
		setXMLDirectory ( dir );
	else if ( dir.isEmpty() )
	{
		dir = QString ( XML_DIR );
		if ( !dir.isNull() )
			setXMLDirectory ( dir );
		else dir = "";
	}
	NMGApplicationInformation::getApplicationInformation()->setXMLPath ( dir );
	return dir;
}

/**
 * Returns the results directory path, if it's not defined, a file dialog appears to ask to the user
 * @return
 */
QString NMGPreferencesManager::getConfigurationDirectory()
{
	QString dir;
	if ( !mapLoadedFromDisk ) dir = NMGApplicationInformation::getApplicationInformation()->getConfigurationPath();
	else dir = getValue ( KEY_CONF_DIR );
	if ( dir.isNull() or dir.isEmpty() ) dir = NMGApplicationInformation::getApplicationInformation()->getConfigurationPath();

	if ( !mapLoadedFromDisk ) setConfigurationDirectory ( dir );
	else if ( dir.isEmpty() )
	{
		dir = QFileDialog::getExistingDirectory ( 0, tr ( "Choose configuration directory" ), ".", QFileDialog::ShowDirsOnly );
		if ( !dir.isNull() ) setConfigurationDirectory ( dir );
		else dir = "";
	}
	NMGApplicationInformation::getApplicationInformation()->setConfigurationPath ( dir );
	return dir;
}

/**
 * Returns the configuration directory, if it's not defined, a file dialog appears to ask to the user
 * @return
 */
QString NMGPreferencesManager::getResultDirectory()
{
	QString dir;
	if ( !mapLoadedFromDisk ) dir = NMGApplicationInformation::getApplicationInformation()->getResultPath();
	else dir = getValue ( KEY_RESULT_DIR );
	if ( dir.isNull() or dir.isEmpty() ) dir = NMGApplicationInformation::getApplicationInformation()->getResultPath();

	if ( !mapLoadedFromDisk ) setResultDirectory ( dir );
	else if ( dir.isEmpty() )
	{
		dir = QFileDialog::getExistingDirectory ( &w, tr ( "Select the results directory" ), ".", QFileDialog::ShowDirsOnly );
		if ( !dir.isNull() ) setResultDirectory ( dir );
		else dir = "";
	}
	NMGApplicationInformation::getApplicationInformation()->setResultPath ( dir );
	return dir;
}

/**
 * Sets the configuration directory
 * @param dir
 */
void NMGPreferencesManager::setConfigurationDirectory ( QString dir )
{
	widget->txtConfDirectoryPath->setText ( dir );
	NMGApplicationInformation::getApplicationInformation()->setConfigurationPath ( dir );
	addValue ( KEY_CONF_DIR, dir );
}

/**
 * Sets the XML directory
 * @param dir
 */
void NMGPreferencesManager::setXMLDirectory ( QString dir )
{
	//widget->txtConfDirectoryPath->setText(dir);
	NMGApplicationInformation::getApplicationInformation()->setXMLPath ( dir );
	addValue ( KEY_XML_DIR, dir );
}

/**
 * Sets the Result directory
 * @param dir
 */
void NMGPreferencesManager::setResultDirectory ( QString dir )
{
	widget->txtResultDirectoryPath->setText ( dir );
	NMGApplicationInformation::getApplicationInformation()->setResultPath ( dir );
	createResultDirectory ( dir );
}

/**
 * Returns the netmeter address
 * @return address
 */
QString NMGPreferencesManager::getNetMeterAddress()
{
	QString address = getValue ( NETMETER_SERVERADDRESS );
	return address.isEmpty()?LOCALHOST:address;
}

/**
 * Sets the netmeter address
 * @param address
 */
void NMGPreferencesManager::setNetMeterAddress ( QString address )
{
	widget->txtNetMeterAddress->setText ( address );
	addValue ( NETMETER_SERVERADDRESS, address );
}

/**
 * Returns the netmeter port
 * @return
 */
int NMGPreferencesManager::getNetMeterPort()
{
	QString port = getValue ( NETMETER_SERVERPORT );
	if ( port.isEmpty() ) return -1;
	else return port.toInt();
}

/**
 * Sets the netmeter port
 * @param port
 */
void NMGPreferencesManager::setNetMeterPort ( int port )
{
	widget->spinBoxNetMeterPort->setValue ( port );
	addValue ( NETMETER_SERVERPORT, QString::number ( port ) );
}

/**
 * updates the widgets from the values
 */
void NMGPreferencesManager::updateWidgetsFromValues()
{
	QString address = getValue ( NETMETER_SERVERADDRESS );
        if ( !address.isEmpty() ) widget->txtNetMeterAddress->setText ( address );

	QString port = getValue ( NETMETER_SERVERPORT );
	if ( !port.isEmpty() ) widget->spinBoxNetMeterPort->setValue ( port.toInt() );

	if ( getValue ( ID_LOAD_ALL_MODULES ) == ID_LOAD_ALL_MODULES_FALSE )
		widget->btnSelectPluginsToLoad->setChecked ( true );
}

/**
 * Saves to disk the values of the widget
 */
void NMGPreferencesManager::updateDiskFromWidget()
{
	if ( not widget->btnSelectPluginsToLoad->isChecked() )
	{
		addValue ( ID_LOAD_ALL_MODULES, ID_LOAD_ALL_MODULES_TRUE );
		removeValue ( ID_MODULES_TO_LOAD );
	}
	else addValue ( ID_LOAD_ALL_MODULES, ID_LOAD_ALL_MODULES_FALSE );

	if ( !widget->txtResultDirectoryPath->text().isEmpty() )
		createResultDirectory ( widget->txtResultDirectoryPath->text() );

	addValue ( NETMETER_SERVERPORT, QString::number ( widget->spinBoxNetMeterPort->value() ) );

        QString address = widget->txtNetMeterAddress->text();
	if ( !address.isEmpty() ) addValue ( NETMETER_SERVERADDRESS, address );
        else addValue ( NETMETER_SERVERADDRESS, LOCALHOST );
}

/**
 * Shows the widgets where the user can select which modules want to load
 */
void NMGPreferencesManager::showModulesToLoadWidget()
{
	NMGPreferencesModuleWidget * modulesToLoadWidget = new NMGPreferencesModuleWidget ( &w, "modulesToLoadWidget", Qt::Dialog );
	modulesToLoadWidget->setWindowModality ( Qt::ApplicationModal );
	modulesToLoadWidget->show();
}

/**
 * Shows Station Manager
 */
void NMGPreferencesManager::showStationManagerWidget()
{
	NMGHostInformationManager::self()->loadInformationWidgetForManagement();
}

/**
 * Selects the image directory path
 */
void NMGPreferencesManager::selectConfigurationDirectoryClicked()
{
	QString path = QFileDialog::getExistingDirectory ( &w, tr ( "Select the configuration directory" ), ".", QFileDialog::ShowDirsOnly );
	if ( !path.isEmpty() ) setConfigurationDirectory ( path );
}

/**
 * Selects the image directory path
 */
void NMGPreferencesManager::selectImageDirectoryClicked()
{
	QString path = QFileDialog::getExistingDirectory ( &w, tr ( "Select the images directory" ), ".", QFileDialog::ShowDirsOnly );
	if ( !path.isEmpty() ) addValue ( NMG_IMAGES_PATH, path );
}

/**
 * Selects the result directory path
 */
void NMGPreferencesManager::selectResultDirectoryClicked()
{
	QString path = QFileDialog::getExistingDirectory ( &w, tr ( "Select the results directory" ), ".", QFileDialog::ShowDirsOnly );
	if ( !path.isEmpty() ) widget->txtResultDirectoryPath->setText ( path );
}

/**
 * Selects the log file path
 */
void NMGPreferencesManager::selectLogFilePath()
{
	QString path = QFileDialog::getExistingDirectory ( &w, tr ( "Select the log directory" ), ".", QFileDialog::ShowDirsOnly );
	if ( !path.isEmpty() ) widget->txtLogFilePath->setText ( path );
}

/**
 * Updates the widgets (if exists) of an added value
 * @param key
 * @param value
 */
void NMGPreferencesManager::updateWidgetsFromAddedValue ( QString key, QString value )
{
	if ( key == NMG_IMAGES_PATH ) widget->txtImagesDirectoryPath->setText ( value );
	else if ( key == KEY_RESULT_DIR ) widget->txtResultDirectoryPath->setText ( value );
}

void NMGPreferencesManager::createResultDirectory ( QString dirPath )
{
	QDir dir ( dirPath );
	if ( not dir.exists() )
	{
		if ( not dir.mkdir ( dirPath ) )
		{
			cerr << "Error while creating the directory " << qPrintable ( dirPath ) << endl;

			dirPath = "/tmp/netmeter-resultdir";
			QDir dirTmp ( dirPath );
			if ( not dirTmp.exists() ) dirTmp.mkdir ( dirPath );

			cerr << "Using " << qPrintable ( dirPath ) << " instead." << endl;

			widget->txtResultDirectoryPath->setText ( dirPath );

			QMessageBox::critical ( &w, "Error while creating the directory", "Error while creating the directory.\nUsing " + dirPath + " instead." );
		}
	}
	addValue ( KEY_RESULT_DIR, dirPath );
}

#include "nmgpreferencesmanager.moc"
