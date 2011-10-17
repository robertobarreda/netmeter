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
#include "nmgmainwindowmenubar.h"

#include "nmgmainwindow.h"
#include "ui_nmgaboutwidget.h"
#include <nmgpreferencesmanager.h>
#include <nmgimagemanager.h>

#include <QObject>
#include <QMap>
#include <QList>
#include <QLabel>

#include  <iostream>
using namespace std;

#define CAPABILITIES_POPUPMENU_SUFFIX	"CapabilityMenu"

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGMainWindowMenuBar::NMGMainWindowMenuBar( NMGMainWindow *parent, const char *name )
		: QMenuBar(( QMainWindow * ) parent )
{
	/* File */
	fileMenu = /*menuBar()->*/addMenu( tr( "&File" ) );

	connectAct = new QAction( tr( "&Connect to server" ), parent );
	connect( connectAct, SIGNAL( triggered() ), parent, SLOT( connectToServer() ) );
	disconnectAct = new QAction( tr( "&Disconnect to server" ), parent );
	disconnectAct->setEnabled( false );
	connect( disconnectAct, SIGNAL( triggered() ), parent, SLOT( disconnectFromServer() ) );
	QAction * quitAct = new QAction( tr( "&Quit" ), parent );
	quitAct->setShortcut( tr( "Ctrl+Q" ) );
	connect( quitAct, SIGNAL( triggered() ), parent, SLOT( quitApplication() ) );

	fileMenu->addAction( connectAct );
	fileMenu->addAction( disconnectAct );
	fileMenu->addAction( quitAct );

	/* Plugins */
	createMenuPlugins( parent );

	/* Settings */
	settingsMenu = /*menuBar()->*/addMenu( tr( "&Settings" ) );

	QAction * generalPrefAct = new QAction( tr( "&General preferences" ), parent );
	connect( generalPrefAct, SIGNAL( triggered() ), NMGPreferencesManager::self(), SLOT( showGeneralPreferencesWidget() ) );
	QAction * pluginPrefAct = new QAction( tr( "&Plugins preferences" ), parent );
	connect( pluginPrefAct, SIGNAL( triggered() ), NMGPreferencesManager::self(), SLOT( showModulePreferencesWidget() ) );
	QAction * connectionPrefAct = new QAction( tr( "&Connection preferences" ), parent );
	connect( connectionPrefAct, SIGNAL( triggered() ), NMGPreferencesManager::self(), SLOT( showConnectionPreferencesWidget() ) );
	QAction * stationsPrefAct = new QAction( tr( "&Stations Manager" ), parent );
	connect( stationsPrefAct, SIGNAL( triggered() ), NMGPreferencesManager::self(), SLOT( showStationsPreferencesWidget() ) );

	settingsMenu->addAction( generalPrefAct );
	settingsMenu->addAction( pluginPrefAct );
        settingsMenu->addAction( connectionPrefAct );
        settingsMenu->addAction( stationsPrefAct );
	
        /* Help */
	aboutMenu = /*menuBar()->*/addMenu( tr( "&Help" ) );

	QAction * aboutAct = new QAction( tr( "&About Netmeter" ), parent );
	connect( aboutAct, SIGNAL( triggered() ), this, SLOT( showAboutWidget() ) );

	aboutMenu->addAction( aboutAct );

	/* Create the About Dialog */
	aboutWidget = new Ui::NMGAboutWidget;
	aboutWidget->setupUi( &w );
	aboutWidget->labelLogo->setPixmap( *NMGImageManager::loadPixmap( "netmeterlogo.png" ) );
}


/**
 * Destroyer of the class
 * @return
 */
NMGMainWindowMenuBar::~NMGMainWindowMenuBar()
{}

/**
 * Sets if the application is connected to the server
 * @param connected
 */
void NMGMainWindowMenuBar::setConnected( bool connected )
{
	if ( connected )
	{
		connectAct->setEnabled( false );
		disconnectAct->setEnabled( true );
	}
	else
	{
		connectAct->setEnabled( true );
		disconnectAct->setEnabled( false );
	}
}

/**
 * Creates the plugins menu adding al the modules loaded
 */
void NMGMainWindowMenuBar::createMenuPlugins( NMGMainWindow *parent )
{
	pluginsMenu = /*menuBar()->*/addMenu( tr( "&Plugins" ) );

	QAction * loadModuleAct = new QAction( tr( "&Load new module" ), parent );
	connect( loadModuleAct, SIGNAL( triggered() ), parent->getModuleToolBox(), SLOT( menuLoadModule() ) );
	QAction * closeCurrAct = new QAction( tr( "Close &current tab" ), parent );
	closeCurrAct->setShortcut( tr( "Ctrl+W" ) );
	connect( closeCurrAct, SIGNAL( triggered() ), parent, SLOT( closeCurrentTab() ) );
	QAction * closeAllAct = new QAction( tr( "Close &all tabs" ), parent );
	connect( closeAllAct, SIGNAL( triggered() ), parent, SLOT( closeAllTabs() ) );
	QAction * closeOtherAct = new QAction( tr( "Close &other tabs" ), parent );
	connect( closeOtherAct, SIGNAL( triggered() ), parent, SLOT( closeOtherTabs() ) );

	pluginsMenu->addAction( loadModuleAct );
	pluginsMenu->addAction( closeCurrAct );
	pluginsMenu->addAction( closeAllAct );
	pluginsMenu->addAction( closeOtherAct );

	pluginsMenu->addSeparator();

	pluginsSignalMapper = new QSignalMapper( this );
	connect( pluginsSignalMapper, SIGNAL( mapped( const QString & ) ), parent, SLOT( createNewInstanceModule( const QString & ) ) );

	QMap<QString, QMenu *> capabilitiesMap; /* key->capability */

	QList<NMModule *> l = NMGModuleManager::self()->getModuleList();
	for ( QList<NMModule *>::iterator it = l.begin(); it != l.end(); ++it )
	{
		NMGModuleParent * mod = ( NMGModuleParent * ) * it;
		QString moduleName = mod->getPluginNameGUI().c_str();
		QString moduleCapabilities = transformCapabilities( mod->getCapabilities().c_str() );
		QMap<QString, QMenu *>::iterator it = capabilitiesMap.find( moduleCapabilities );
		QMenu * menu;
		if ( it == capabilitiesMap.end() )
		{
			menu = pluginsMenu->addMenu( moduleCapabilities );
			menu->setObjectName( moduleCapabilities + CAPABILITIES_POPUPMENU_SUFFIX );
			capabilitiesMap.insert( moduleCapabilities, menu );
		}
		else menu = *it;

		QAction * pluginAction = new QAction( moduleName, this );
		pluginsSignalMapper->setMapping( pluginAction, moduleName );
		connect( pluginAction, SIGNAL( triggered() ), pluginsSignalMapper, SLOT( map() ) );
		menu->addAction( pluginAction );
	}
}

/**
 * Adds a module to the menu
 * @param moduleName
 * @param capabilities
 */
void NMGMainWindowMenuBar::addNewModuleAddedToMenu( QString moduleName, QString capabilities )
{
	capabilities = transformCapabilities( capabilities );

	QMenu * capabilityMenu = findChild<QMenu *> ( capabilities + CAPABILITIES_POPUPMENU_SUFFIX );
	if ( !capabilityMenu )
	{
		capabilityMenu = pluginsMenu->addMenu( capabilities );
		capabilityMenu->setObjectName( capabilities + CAPABILITIES_POPUPMENU_SUFFIX );
	}

	QAction * pluginAction = new QAction( moduleName, this );
	pluginsSignalMapper->setMapping( pluginAction, moduleName );
	connect( pluginAction, SIGNAL( triggered() ), pluginsSignalMapper, SLOT( map() ) );
	capabilityMenu->addAction( pluginAction );
}


/**
 * Deletes the specified module from the plugin menu
 * @param moduleName
 */
void NMGMainWindowMenuBar::deleteModuleFromPluginMenu( QString moduleName )
{
	QList<QMenu *> pluginMenuList = pluginsMenu->findChildren<QMenu *> ( );

	for ( QList<QMenu *>::iterator itm = pluginMenuList.begin(); itm != pluginMenuList.end(); ++itm )
	{
		QList<QAction *> actionList = ( *itm )->actions();

		for ( QList<QAction *>::iterator ita = actionList.begin(); ita != actionList.end(); ++ita )
		{
			if (( *ita )->text() == moduleName )
			{
				( *itm )->removeAction( * ita );
				if ( ( *itm )->isEmpty() ) delete ( *itm );
				return;
			}
		}
	}
}

/**
 * Block/Unblocks the signals of the Qaccel childs of the menu
 * @param menu Menu to block its QAccel childs
 * @param blockSignals If true the signals are blocked, if false, the signals are unblocked
 */
void NMGMainWindowMenuBar::blockQAccelSignalsOfMenu( QMenu * menu, bool blockSignals )
{
	if ( !menu ) return;
	const QList<QAction *> actionList = menu->findChildren<QAction *> ( );
	for ( QList<QAction *>::const_iterator i = actionList.constBegin(); i != actionList.constEnd(); ++i )
	{
		( *i )->blockSignals( blockSignals );
		( *i )->setEnabled( not blockSignals );
	}
}

/**
 * Transform the string capabilites to be shown correctly to the user
 * @param capabilities
 * @return
 */
QString NMGMainWindowMenuBar::transformCapabilities( QString capabilities )
{
	return capabilities.toLower().replace( 0, 1, capabilities.left( 1 ).toUpper() );
}

/**
 * Display About to Widget
 */
void NMGMainWindowMenuBar::showAboutWidget()
{
	w.show();
}

QAction * NMGMainWindowMenuBar::addModuleMenu( QMenu * moduleMenu )
{
	return	insertMenu( aboutMenu->menuAction(), moduleMenu ) ;
}

#include "nmgmainwindowmenubar.moc"
