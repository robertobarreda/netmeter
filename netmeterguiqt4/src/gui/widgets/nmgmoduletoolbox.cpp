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
#include "nmgmoduletoolbox.h"

#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QFileDialog>
#include <QIcon>
#include <QObject>
#include <QList>
#include <QPalette>

#define LABEL_SUFFIX	"Label"
#define BUTTON_SUFFIX	"Button"

#define LABEL_CLASS	"QLabel"
#define BUTTON_CLASS	"QToolButton"

#define IS_WIDGET_OF_MODULE(w)	(QString(w->metaObject()->className())==BUTTON_CLASS or QString(w->metaObject()->className())==LABEL_CLASS)

#define GREY_LEVEL_COLOR	242

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGModuleToolBox::NMGModuleToolBox( QWidget *parent, const char *name )
		: QToolBox( parent )
{
	setObjectName( name ),
	idMenuCreateInstance = menu.addAction( "&New instance", this, SLOT( menuCreateInstance() ) );
	idMenuUnloadModule = menu.addAction( "&Unload module", this, SLOT( menuUnloadModule() ) );
	menu.addSeparator();
	menu.addAction( "&Load new module", this, SLOT( menuLoadModule() ) );

	/** Here is defined the space the plug-in's tool box will take **/

	QSizePolicy sizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );
	sizePolicy.setHorizontalStretch( 0 );
	sizePolicy.setVerticalStretch( 0 );
	sizePolicy.setHeightForWidth( this->sizePolicy().hasHeightForWidth() );
	setSizePolicy( sizePolicy );

	/** Maximum size of the plugin's toolbar **/
	setMaximumWidth( 150 );
	setFrameShape( QToolBox::NoFrame );
	setCurrentIndex( 0 );

	loadExistingModules();
}

/**
 * Destructor of the class
 * @return
 */
NMGModuleToolBox::~NMGModuleToolBox()
{}

/**
 * Handles the mouse events
 * @param e
 */
void NMGModuleToolBox::mousePressEvent( QMouseEvent * e )
{
	if ( e->button() == Qt::RightButton )
	{
		QWidget * w = childAt( e->x(), e->y() );
		if ( IS_WIDGET_OF_MODULE( w ) )
		{
			widgetRightClicked = w;
			idMenuCreateInstance->setEnabled( true );
			idMenuUnloadModule->setEnabled( true );
		}
		else
		{
			idMenuCreateInstance->setEnabled( false );
			idMenuUnloadModule->setEnabled( false );
		}
		menu.popup( e->globalPos() );
	}
}

/**
 * Loads the existing modules in the NMGModuleManager
 */
void NMGModuleToolBox::loadExistingModules()
{
	QList<NMModule *> l = NMGModuleManager::self()->getModuleList();
	for ( QList<NMModule *>::iterator it = l.begin(); it != l.end(); ++it )
		addModule( getModuleName( *it ), ( *it )->getCapabilities().c_str() );
}

/**
 * Adds a module to the toolBox
 * @param moduleName Name of the module
 * @param capabilities Capabilities of the module
 */
void NMGModuleToolBox::addModule( QString moduleName, QString capabilities )
{
	capabilities = transformCapabilities( capabilities );
	NMGModuleToolBoxMapIterator it = capabilitiesMap.find( capabilities );
	QWidget * widget;
	QVBoxLayout * layout;
	QSpacerItem * spacer;
	if ( it == capabilitiesMap.end() )
	{
		widget = new QWidget( this );
		widget->setObjectName( capabilities + "Widget" );
		QPalette palette;
		palette.setColor( widget->backgroundRole(), QColor( GREY_LEVEL_COLOR, GREY_LEVEL_COLOR, GREY_LEVEL_COLOR ) );
		widget->setPalette( palette );

		layout = new QVBoxLayout( widget );
		layout->setMargin( 11 );
		layout->setSpacing( 6 );
		layout->setObjectName( capabilities + "Layout" );

		addItem( widget, capabilities );
		capabilitiesMap.insert( capabilities, widget );

		spacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
		layout->addItem( spacer );
		spacerMap.insert( capabilities, spacer );
	}
	else
	{
		widget = *it;
		layout = ( QVBoxLayout * ) widget->layout();
		spacer = *spacerMap.find( capabilities );
	}
	addModuleWidgets( widget, layout, spacer, moduleName );
}

/**
 * Adds the widgets for the new moduleName module
 * @param widget Widget
 * @param  layout Layout of the widget
 * @param moduleName Module name
 */
void NMGModuleToolBox::addModuleWidgets( QWidget * widget, QVBoxLayout * layout, QSpacerItem * spacer, QString moduleName )
{
	layout->removeItem( spacer );

	NMGModuleToolBoxItem * item = new NMGModuleToolBoxItem( widget, moduleName, widget->palette().color( widget->backgroundRole() ) );
	connect( item, SIGNAL( moduleItemActivated( QString ) ), this, SIGNAL( moduleActivated( QString ) ) );
	layout->addWidget( item );
	item->show();

	layout->addItem( spacer );
}

/**
 * Slot executed when the user wants to create a new instance of the selected item
 */
void NMGModuleToolBox::menuCreateInstance()
{
	QString moduleName = getModuleNameOfRightClickedWidget();
	if ( !moduleName.isEmpty() ) emit createInstaceModule( moduleName );
}

/**
 * Slot executed whan the user wants to unload the selected module
 */
void NMGModuleToolBox::menuUnloadModule()
{
	QString moduleName = getModuleNameOfRightClickedWidget();
	if ( !moduleName.isEmpty() )
	{
		emit unloadModule( moduleName );

		QLabel * label = findChild<QLabel *>( moduleName + LABEL_SUFFIX );

		if ( !label ) cerr << "Error: there is no label with name " << qPrintable( moduleName ) << endl;
		else
		{
			QWidget * widget = ( QWidget* ) label->parent()->parent();
			delete label->parent();
			if ( widget->children().count() == 1 ) // only the spacer
			{
				int index = indexOf( widget );
				capabilitiesMap.remove( itemText( index ) );
				spacerMap.remove( itemText( index ) );
				removeItem( index );
				delete widget;
			}
		}
	}
	else cout << "There is no module to unload (NMGModuleToolBox::menuUnloadModule)" << endl;
}

/**
 * Returns the name of the module of the widget clicked (right click)
 * @return
 */
QString NMGModuleToolBox::getModuleNameOfRightClickedWidget()
{
	if ( !widgetRightClicked ) return "";
	if ( QString( widgetRightClicked->metaObject()->className() ) == LABEL_CLASS )
	{
		QLabel * label = ( QLabel * ) widgetRightClicked;
		QString labelName( label->objectName() );
		labelName = labelName.left( labelName.length() - QString( LABEL_SUFFIX ).length() );
		return labelName;

	}
	else if ( QString( widgetRightClicked->metaObject()->className() ) == BUTTON_CLASS )
	{
		QPushButton * button = ( QPushButton * ) widgetRightClicked;
		QString buttonName( button->objectName() );
		buttonName = buttonName.left( buttonName.length() - QString( BUTTON_SUFFIX ).length() );
		return buttonName;
	}
	cerr << "Unknown widget at NMGModuleToolBox::getModuleNameOfRightClickedWidget " << endl;
	cerr << widgetRightClicked->metaObject()->className() << "." << endl;
	return "";
}


/**
 * Slot executed when the user wants to load a new module
 */
void NMGModuleToolBox::menuLoadModule()
{
	QString file = QFileDialog::getOpenFileName( 0, tr( "Select an existing module" ),
	               QString(), tr( "Modules (*.so);;Libtool library (*.la);;All files (*.*)" ) );

	if ( !file.isNull() and !file.isEmpty() ) emit loadNewModule( file );
}

/**
 * Returns the name associated to the module (first the gui, after the core and after the name)
 * @param m
 * @return
 */
QString NMGModuleToolBox::getModuleName( NMModule * m )
{
	NMGModuleParent * mod = ( NMGModuleParent * ) m;
	QString nameGUI( mod->getPluginNameGUI().c_str() );
	if ( !nameGUI.isEmpty() ) return nameGUI;
	QString nameCore( mod->getPluginNameCore().c_str() );
	if ( !nameCore.isEmpty() ) return nameCore;
	QString name( mod->getPluginName().c_str() );
	if ( !name.isEmpty() ) return name;
	return "null";
}

/**
 * Transform the string capabilites to be shown correctly to the user
 * @param capabilities
 * @return
 */
QString NMGModuleToolBox::transformCapabilities( QString capabilities )
{
	return capabilities.toLower().replace( 0, 1, capabilities.left( 1 ).toUpper() );
}

#include "nmgmoduletoolbox.moc"
