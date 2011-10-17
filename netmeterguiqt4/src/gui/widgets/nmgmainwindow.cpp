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
#include "nmgmainwindow.h"

#include <nmgapplicationinformation.h>
#include <nmgmodulemanager.h>

#include <nmghostinformationmanager.h>
#include <nmgpreferencesmanager.h>
#include <nmgimagemanager.h>
#include <statusbarprogresswidget.h>
#include <progressdialog.h>
#include <messagebar.h>
#include <popupmessage.h>

#include <QApplication>
#include <QCloseEvent>
#include <QVBoxLayout>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @param parent QT's parent parameter
 * @param name QT's name parameter
 * @return
 */
NMGMainWindow::NMGMainWindow( QWidget *parent, const char *name )
		: QMainWindow( parent )
{
	NMGApplicationInformation::getApplicationInformation()->setMainWindow( this );

	if ( !name ) setObjectName( "NMGMainWindow" );
	else setObjectName( name );

	setWindowTitle( "NetMeter" );

	applicationStatus = STATUS_APPLICATION_STARTED;
	menuIdentifier = 0;
	toolBarModule = 0;
	previousMenu = 0;

	createWidgets();
	toolBar = new NMGMainWindowToolBar( this );
	addToolBar( toolBar );
	menuBar = new NMGMainWindowMenuBar( this );
	setMenuBar( menuBar );
	connectWidgets();
}

/**
 * Destructor of the class
 * @return
 */
NMGMainWindow::~NMGMainWindow()
{}

/**
 * Executed when the windows is going to close
 * @param e
 */
void NMGMainWindow::closeEvent( QCloseEvent *e )
{
	NMGPreferencesManager::self()->addValue( MAIN_WINDOW_SIZE_LAST_WIDTH, QString( "%1" ).arg( size().width() ) );
	NMGPreferencesManager::self()->addValue( MAIN_WINDOW_SIZE_LAST_HEIGHT, QString( "%1" ).arg( size().height() ) );
	quitApplication();
	e->accept();
}

/**
 * Creates the widgets of the main window
 */
void NMGMainWindow::createWidgets()
{
	QWidget * widget = new QWidget( this );
	widget->setObjectName( "layoutMainWindow" );
	QVBoxLayout * layout = new QVBoxLayout;
	layout->setMargin( 11 );
	layout->setSpacing( 6 );
	widget->setLayout( layout );

	QSplitter * splitter = new QSplitter( widget );
	splitter->setObjectName( "splitterMainWindow" );
	splitter->setOrientation( Qt::Horizontal );
	layout->addWidget( splitter );

	moduleToolBox = new NMGModuleToolBox( widget, "NMGModuleToolBox" );
	tabWidget = new NMGMainTabWidget( widget, "NMGMainTabWidget" );
	splitter->addWidget( moduleToolBox );
	splitter->addWidget( tabWidget );

	setCentralWidget( widget );

	//Progress bar for tests
	ProgressDialog * progressDialog = new ProgressDialog( statusBar(), this );
	progressDialog->raise();
	progressDialog->hide();

	StatusbarProgressWidget * m_progressbar = new StatusbarProgressWidget( progressDialog, this );
	m_progressbar->show();

	//Popup Message in statusBar
	PopupMessage * popupmessage = new PopupMessage( statusBar(), this, 5000 );
	popupmessage->raise();
	popupmessage->hide();

	MessageBar * messageBar = new MessageBar( popupmessage, this );
	messageBar->show();

	//Add here every widget that should be placed in the status bar
	statusBar()->addWidget( messageBar );
	statusBar()->addPermanentWidget( m_progressbar );

	/* resize the window */
	int sizeWidth, sizeHeight;
	QString widthString, heightString;
	widthString = NMGPreferencesManager::self()->getValue( MAIN_WINDOW_SIZE_LAST_WIDTH );
	heightString = NMGPreferencesManager::self()->getValue( MAIN_WINDOW_SIZE_LAST_HEIGHT );
	bool ok;
	sizeWidth = widthString.toInt( &ok );
	if ( not ok ) sizeWidth = MAIN_WINDOW_SIZE_DEFAULT_WIDTH;
	sizeHeight = heightString.toInt( &ok );
	if ( not ok ) sizeHeight = MAIN_WINDOW_SIZE_DEFAULT_HEIGHT;
	resize( QSize( sizeWidth, sizeHeight ).expandedTo( minimumSizeHint() ) );
// 	clearWState ( WState_Polished );
}

/**
 * Connects the widgets
 */
void NMGMainWindow::connectWidgets()
{
	connect( moduleToolBox, SIGNAL( moduleActivated( QString ) ), this, SLOT( showModule( QString ) ) );
	connect( moduleToolBox, SIGNAL( moduleActivated( QString ) ), NMGHostInformationManager::self(),
	         SLOT( setCurrentWidgetModule( QString ) ) );
	connect( moduleToolBox, SIGNAL( createInstaceModule( const QString & ) ), this, SLOT( createNewInstanceModule( const QString & ) ) );
	connect( moduleToolBox, SIGNAL( unloadModule( QString ) ), NMGModuleManager::self(), SLOT( unloadExistingModule( QString ) ) );
	connect( moduleToolBox, SIGNAL( loadNewModule( QString ) ), NMGModuleManager::self(), SLOT( loadNewModule( QString ) ) );
	connect( NMGModuleManager::self(), SIGNAL( newModuleAdded( QString, QString ) ), moduleToolBox, SLOT( addModule( QString, QString ) ) );
	connect( NMGModuleManager::self(), SIGNAL( newModuleAdded( QString, QString ) ), menuBar, SLOT( addNewModuleAddedToMenu( QString, QString ) ) );
	connect( NMGModuleManager::self(), SIGNAL( finishedGUIRequest( QString, QString, QString ) ), this, SLOT( showTask( QString, QString, QString ) ) );

	connect( tabWidget, SIGNAL( closeCurrentTab() ), this, SLOT( closeCurrentTab() ) );
	connect( tabWidget, SIGNAL( closeAllTabs() ), this, SLOT( closeAllTabs() ) );
	connect( tabWidget, SIGNAL( closeOtherTabs() ), this, SLOT( closeOtherTabs() ) );

	connect( tabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( currentTabChanged() ) );
	connect( NMGConnection::getConnection(), SIGNAL( finishedTask( QString, QString, QString ) ), this,
	         SLOT( showTask( QString, QString, QString ) ) );
	connect( NMGConnection::getConnection(), SIGNAL( connectionStatusChanged( NMGStatus ) ), this,
	         SLOT( connectionStatusChanged( NMGStatus ) ) );

}

/**
 * Slot executed when the user selects a module
 * @param s Module selected
 */
void NMGMainWindow::showModule( QString name )
{
	if ( NMGModuleManager::self()->existsModule( name ) )
	{
		for ( int i = tabWidget->count(); i >= 0; --i )
		{
			if ( name == tabWidget->tabText( i ) )
			{
				tabWidget->setCurrentIndex( i );
				NMGModule * mod =
				    NMGModuleManager::self()->getInstanceManager().getModuleByWidget( tabWidget->getCurrentPageWidget() );
				showCurrentToolbarAndMenu( name, mod );
				return;
			}
		}

		createNewInstanceModule( name );
	}
	else cerr << "The module " << qPrintable( name ) << " isn't in the module manager." << endl;
}

/**
 * Shows the tool bar and menu of the current module and hides the previous items
 * @param m
 */
void NMGMainWindow::showCurrentToolbarAndMenu( QString name, NMGModule * m )
{
	/* hide the old stuff */
	if ( menuIdentifier )
	{
		menuBar->removeAction( menuIdentifier );
		menuBar->blockQAccelSignalsOfMenu( previousMenu, true );
	}
	if ( toolBarModule )
	{
		//removeToolBarBreak( toolBarModule ); //Just in case we want to load the plugin's toolbar in separate rows
		removeToolBar( toolBarModule );
		toolBarModule->setVisible( FALSE );
	}

	/* show the new */
	previousMenu = 0;
	QMenu * menu = 0;

	NMGHostInformationManager::self()->setCurrentWidgetModule( name );

	if ( m and ( menu = m->getPopupMenu() ) )
	{
		menu->setTitle( name );
		previousMenu = menu;
		menuBar->blockQAccelSignalsOfMenu( menu, false );
		menuIdentifier = menuBar->addModuleMenu( menu );
	}
	else menuIdentifier = 0;

	if ( m and ( toolBarModule = m->getToolBar() ) )
	{
		toolBarModule->setWindowTitle( m->getModuleGUIName() + "'s ToolBar" );
		toolBarModule->setVisible( TRUE );
		addToolBar( toolBarModule );
		//insertToolBarBreak( toolBarModule ); //Just in case we want to load the plugin's toolbar in separate rows
	}
}


/**
 * Creates a new instance of the module and shows it
 * @param module
 */
void NMGMainWindow::createNewInstanceModule( const QString & module )
{
	if ( NMGModuleManager::self()->existsModule( module ) )
	{
		QString id = NMGModuleManager::self()->createModuleInterface( module );
		NMGModule * m = NMGModuleManager::self()->getModuleInterface( id );

		QWidget * w = m->getWidget();

		if ( !w )
		{
			cerr << "Error getting the widget" << endl;
			return;
		}
		createNewModuleTab( module, w, NMGModuleManager::self()->getModuleParent( m )->getModuleIcon().c_str() );
		showCurrentToolbarAndMenu( module, m );
	}
	else cerr << "The module " << qPrintable( module ) << " isn't in the module manager." << endl;
}

/**
 * Creates a new module tab
 * @param name
 * @param w
 */
void NMGMainWindow::createNewModuleTab( QString name, QWidget * w, QString icon )
{
	tabWidget->addTab( w, name, icon );
}

/**
 * Returns the current NMGModule that is being used in the tabBar
 * @return
 */
NMGModule * NMGMainWindow::getCurrentModule()
{
	return NMGModuleManager::self()->getInstanceManager().getModuleByWidget( tabWidget->getCurrentPageWidget() );
}

/**
 * Creates a new QToolBar with the main window as its parent
 * @return
 */
QToolBar * NMGMainWindow::createNewToolBar()
{
	return new QToolBar( this );
}

/**
 * Creates the widget of the module and it's added to the tabWidget
 * @param mod
 */
void NMGMainWindow::addWidgetFromExistingModule( NMGModule * mod )
{
	if ( !mod ) return;
	QWidget * widget = mod->getWidget();
	if ( !widget )
	{
		cerr << "Error getting the widget" << endl;
		return;
	}
	createNewModuleTab( mod->getModuleGUIName(), widget, NMGModuleManager::self()->getModuleParent( mod )->getModuleIcon().c_str() );
	showCurrentToolbarAndMenu( mod->getModuleGUIName(), mod );
}

/**
 * Unloads all the instances of a module
 * @param module Module to unload all of its instances
 */
void NMGMainWindow::unloadAllInstancesOfModule( QString module )
{
	if ( NMGModuleManager::self()->existsModule( module ) )
	{
		for ( int i = tabWidget->count(); i >= 0; --i )
		{
			if ( module == tabWidget->tabText( i ) )
			{
				NMGModule * mod =
				    NMGModuleManager::self()->getInstanceManager().getModuleByWidget( tabWidget->getPageWidget( i ) );
				if ( mod ) NMGModuleManager::self()->deletePluginInterface( mod );
				tabWidget->deletePage( i );
			}
		}
		menuBar->deleteModuleFromPluginMenu( module );

		NMGModule * next = NMGModuleManager::self()->getInstanceManager().
		                   getModuleByWidget( tabWidget->getCurrentPageWidget() );

		showCurrentToolbarAndMenu( next ? next->getName() : "", next );
	}
	else cerr << "The module " << qPrintable( module ) << " isn't in the module manager." << endl;
}


/**
 * Shows the result of the task finished with id
 * @param id Id of the task
 * @param moduleName Name of the module of the task (not used)
 * @param data xml data for the result of the task
 */
void NMGMainWindow::showTask( QString id, QString , QString data )
{
	NMGModule * m = NMGModuleManager::self()->getModuleInterface( id );
	if ( !m ) cerr << "There is no plugin associated to the identifier " << qPrintable( id ) << "." << endl;
	else
		m->dataReceivedParent( data );
}

/**
 * Slot executed whan the current tab changes (updates the current toolbar and menu)
 * @param w
 */
void NMGMainWindow::currentTabChanged()
{
	NMGModule * mod = getCurrentModule();
        if (!mod) return;
	showCurrentToolbarAndMenu( mod->getName(), mod );
}

/**
 * Slot executed when the user wants to close the current tab
 */
void NMGMainWindow::closeCurrentTab()
{
	QWidget * current = tabWidget->getCurrentPageWidget();
	if ( current )
	{
		NMGModule * mod = NMGModuleManager::self()->getInstanceManager().getModuleByWidget( current );
		NMGModuleManager::self()->deletePluginInterface( mod );
		//TODO Remove Widget Module Host Information from NMGHostInformationManager
		tabWidget->deleteCurrentPage();

		NMGModule * next = NMGModuleManager::self()->getInstanceManager().getModuleByWidget( tabWidget->getCurrentPageWidget() );
		showCurrentToolbarAndMenu( next ? next->getName() : "", next );
	}
}

/**
 * Slot executed when the user want to close all the tabs
 */
void NMGMainWindow::closeAllTabs()
{
	while ( tabWidget->count() > 0 ) closeCurrentTab();
}

/**
 * Slot executed when the user want to close all the tabs except the current
 */
void NMGMainWindow::closeOtherTabs()
{
	if ( tabWidget->count() <= 1 ) return;

	int currentPage = tabWidget->currentIndex();
	int totalPages = tabWidget->count();

	for ( int i = totalPages - 1; i > currentPage; i-- )
	{
		tabWidget->setCurrentIndex( i );
		closeCurrentTab();

	}
	for ( int i  = 0; i < currentPage; i++ )
	{
		tabWidget->setCurrentIndex( 0 );
		closeCurrentTab();
	}
}


/**
 * Slot executed whan the status of the connection changes
 * @param status
 */
void NMGMainWindow::connectionStatusChanged( NMGStatus status )
{
	if ( NMGConnection::getConnection()->getConnectionStatus() == Disconnected and
	        applicationStatus == STATUS_APPLICATION_EXIT_SENT )
	{
		close();
		return;
	}
	if ( status == Disconnected )
	{
		menuBar->setConnected( false );
		toolBar->setConnected( false );
	}
	else // Connected,AttachSent,ReadyToRead,WaitingDeattachAck,WaitingExitAck
	{
		menuBar->setConnected( true );
		toolBar->setConnected( true );
	}
}

/**
 * Slot which connects to the server
 */
void NMGMainWindow::connectToServer()
{
	NMGConnection::getConnection()->start();
}

/**
 * Slots which disconnects form the server
 */
void NMGMainWindow::disconnectFromServer()
{
	NMGConnection::getConnection()->deattachServer();
}

/**
 * Slots which closes the application, if the connection is connected, it sends and exit message
 */
void NMGMainWindow::quitApplication()
{
	applicationStatus = STATUS_APPLICATION_EXIT_SENT;
	if ( NMGConnection::getConnection()->getConnectionStatus() != Disconnected )
		NMGConnection::getConnection()->exitServer();
	else close();
}

/**
 * Returns the module tool box
 * @return
 */
NMGModuleToolBox * NMGMainWindow::getModuleToolBox()
{
	return moduleToolBox;
}

/**
 * Returns the tab widget
 * @return
 */
NMGMainTabWidget * NMGMainWindow::getTabWidget()
{
	return tabWidget;
}

#include "nmgmainwindow.moc"
