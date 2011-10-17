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
#include "nmgpreferencesmodulewidget.h"

#include <nmgmodulemanager.h>
#include <nmgpreferencesmanager.h>

#include <QPushButton>
#include <QToolBox>
#include <QWidget>
#include <QCheckBox>
#include <QLayout>
#include <QMap>
#include <QObject>
#include <QList>

#define PRE_MOD		"["
#define POST_MOD	"]"

#include <iostream>
using namespace std;


/**
 * Contructor  of the class
 * @param parent
 * @param name
 * @param fl
 * @return
 */
NMGPreferencesModuleWidget::NMGPreferencesModuleWidget( QWidget *parent, const char *name, Qt::WindowFlags fl )
		: QDialog( parent, fl )
{
	if ( !name ) setObjectName( "NMGPreferencesModuleBaseWidget" );
	else setObjectName( name );
	setSizeGripEnabled( TRUE );
	preferencesModuleBaseWidgetLayout = new QVBoxLayout( this );
	preferencesModuleBaseWidgetLayout->setMargin( 11 );   //11 is default margin
	preferencesModuleBaseWidgetLayout->setSpacing( 6 );
	preferencesModuleBaseWidgetLayout->setObjectName( "NMGPreferencesModuleBaseWidgetLayout" );

	setWindowTitle( "Select modules to load" );

	toolBox1 = new QToolBox( this );
	toolBox1->setObjectName( "toolBox1" );
	toolBox1->setCurrentIndex( 0 );

	loadExistingModules();
	preferencesModuleBaseWidgetLayout->addWidget( toolBox1 );

	layout1 = new QHBoxLayout( 0 );
	layout1->setMargin( 0 );
	layout1->setSpacing( 6 );
	layout1->setObjectName( "layout1" );

	horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout1->addItem( horizontal_Spacing2 );

	buttonOk = new QPushButton( this );
	buttonOk->setObjectName( "buttonOk" );
	buttonOk->setAutoDefault( TRUE );
	buttonOk->setDefault( TRUE );
	buttonOk->setText( "&OK" ) ;
// 	buttonOk->setShortcut ( QKeySequence ( QString::null ) );
	layout1->addWidget( buttonOk );

	buttonCancel = new QPushButton( this );
	buttonCancel->setObjectName( "buttonCancel" );
	buttonCancel->setAutoDefault( TRUE );
	buttonCancel->setText( "&Cancel" ) ;
// 	buttonCancel->setShortcut ( QKeySequence ( QString::null ) ) ;
	layout1->addWidget( buttonCancel );
	preferencesModuleBaseWidgetLayout->addLayout( layout1 );

	resize( QSize( 230, 295 ).expandedTo( minimumSizeHint() ) );
	ensurePolished();  //clearWState ( WState_Polished );

	// signals and slots connections
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/**
 * Destroyer of the class
 * @return
 */
NMGPreferencesModuleWidget::~NMGPreferencesModuleWidget()
{}

/**
 * Loads the existing modules and creates the widgets (checkboxes) for them
 */
void NMGPreferencesModuleWidget::loadExistingModules()
{
	QString moduleList = NMGPreferencesManager::self()->getValue( ID_MODULES_TO_LOAD );
	QString loadAllModules = NMGPreferencesManager::self()->getValue( ID_LOAD_ALL_MODULES );

	QMap<QString, QVBoxLayout*> mapWidget; //key capability

	QList<NMModule *> l = NMGModuleManager::self()->getModuleList();
	for ( QList<NMModule *>::iterator it = l.begin(); it != l.end(); ++it )
	{
		QString modCap, modName;
		NMGModuleParent * mod = ( NMGModuleParent * ) * it;
		modCap = transformCapabilities( mod->getCapabilities().c_str() );
		modName = mod->getPluginNameGUI().c_str();

		QWidget * page;
		QVBoxLayout* pageLayout;
		QMap<QString, QVBoxLayout*>::iterator it = mapWidget.find( modCap );
		if ( it == mapWidget.end() )
		{
			page = new QWidget( toolBox1 );
			page->setObjectName( "page" + modCap );

			//page->setBackgroundMode ( QWidget::PaletteBackground );
			//page->setBackgroundRole( QPalette::Window );

			pageLayout = new QVBoxLayout( page );
			pageLayout->setMargin( 11 );  //11 is default margin
			pageLayout->setSpacing( 6 );
			pageLayout->setObjectName( "page1Layout" );
			toolBox1->addItem( page, modCap );
			mapWidget.insert( modCap, pageLayout );
		}
		else pageLayout = *it;

		QCheckBox * checkBox = new QCheckBox( pageLayout->parentWidget() );
		checkBox->setObjectName( modName );
		checkBox->setText( modName );
		if ( loadAllModules == ID_LOAD_ALL_MODULES_TRUE or moduleList.isEmpty() or moduleList.contains( PRE_MOD + modName + POST_MOD ) )
			checkBox->setChecked( true );
		pageLayout->addWidget( checkBox );
	}
}

/**
 * Slot executed whn the user clicks the ok button
 */
void NMGPreferencesModuleWidget::accept()
{
	QString moduleList = "";
	hide();

	QList<QCheckBox *> l = topLevelWidget()->findChildren<QCheckBox *>();

	for ( QList<QCheckBox *>::iterator it = l.begin(); it != l.end(); ++it )
	{
		QString moduleName = ( *it )->objectName();
		if (( *it )->isChecked() )
		{
			if ( moduleList.isEmpty() ) moduleList = PRE_MOD + moduleName + POST_MOD;
			else moduleList += ( QString( "," ) + QString( PRE_MOD ) + moduleName + QString( POST_MOD ) );
		}
	}

	if ( moduleList.isEmpty() )
	{
		NMGPreferencesManager::self()->removeValue( ID_MODULES_TO_LOAD );
		NMGPreferencesManager::self()->addValue( ID_LOAD_ALL_MODULES, ID_LOAD_ALL_MODULES_TRUE );
	}
	else
	{
		NMGPreferencesManager::self()->addValue( ID_MODULES_TO_LOAD, moduleList );
		NMGPreferencesManager::self()->addValue( ID_LOAD_ALL_MODULES, ID_LOAD_ALL_MODULES_FALSE );
	}

	//delete this;
}

/**
 * Slot executed when the user clicks the cancel button
 */
void NMGPreferencesModuleWidget::reject()
{
	hide();
	//delete this;
}

/**
 * Transform the string capabilites to be shown correctly to the user
 * @param capabilities
 * @return
 */
QString NMGPreferencesModuleWidget::transformCapabilities( QString capabilities )
{
	return capabilities.toLower().replace( 0, 1, capabilities.left( 1 ).toUpper() );
}

#include "nmgpreferencesmodulewidget.moc"
