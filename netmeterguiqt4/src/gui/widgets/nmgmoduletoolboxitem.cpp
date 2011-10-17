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
#include "nmgmoduletoolboxitem.h"

#include <nmgimagemanager.h>

#define DEFAULT_ICON_NAME	"icon.png"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSizePolicy>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGModuleToolBoxItem::NMGModuleToolBoxItem( QWidget *parent, QString modName, const QColor & backgroundColor )
		: QWidget( parent )
{
	setObjectName( modName );
	setMouseTracking( true );

	QPalette palette;
	palette.setColor( backgroundRole(), backgroundColor );
	setPalette( palette );

	moduleName = modName;

	QVBoxLayout * layoutVertical = new QVBoxLayout( this );
	layoutVertical->setMargin( 0 );
	layoutVertical->setSpacing( 0 );
	layoutVertical->setObjectName( moduleName + "LayoutVertical" );

	QHBoxLayout * layoutHoritzontal = new QHBoxLayout( 0 );
	layoutHoritzontal->setMargin( 0 );
	layoutHoritzontal->setSpacing( 0 );
	layoutHoritzontal->setObjectName( moduleName + "LayoutHoritzontal" );

	QSpacerItem * leftSpacer = new QSpacerItem( 1, 21, QSizePolicy::Maximum, QSizePolicy::Minimum );
	layoutHoritzontal->addItem( leftSpacer );

	QString toolTip = (( NMGModuleParent * ) NMGModuleManager::self()->getModule( moduleName ) )->getModuleToolTip().c_str();

	QLabel * pixmapLabel1 = new QLabel( this );
	pixmapLabel1->setMaximumSize( QSize( 64, 64 ) );
	pixmapLabel1->setAlignment( Qt::AlignHCenter );
	pixmapLabel1->setObjectName( moduleName + "Label" );

	if ( !toolTip.isEmpty() )
	{
		pixmapLabel1->setToolTip( toolTip );
		pixmapLabel1->setWhatsThis( toolTip );
	}

	QString iconFile = (( NMGModuleParent * ) NMGModuleManager::self()->getModule( moduleName ) )->getModuleIcon().c_str();
	if ( iconFile.isEmpty() ) iconFile = DEFAULT_ICON_NAME;
	pixmapLabel1->setPixmap( * ( NMGImageManager::loadPixmap( iconFile ) ) );
	pixmapLabel1->setScaledContents( TRUE );
	layoutHoritzontal->addWidget( pixmapLabel1 );
	pixmapLabel1->setMouseTracking( true );

	QSpacerItem * rightSpacer = new QSpacerItem( 1, 21, QSizePolicy::Maximum, QSizePolicy::Minimum );
	layoutHoritzontal->addItem( rightSpacer );
	layoutVertical->addLayout( layoutHoritzontal );


	QLabel * txtModuleLabel = new QLabel( "<p align=\"center\">" + moduleName + "</p>", this );
	txtModuleLabel->setObjectName( moduleName + "Label" );
	if ( !toolTip.isEmpty() )
	{
		txtModuleLabel->setToolTip( toolTip );
		txtModuleLabel->setWhatsThis( toolTip );
	}

	QSizePolicy policy1( QSizePolicy::Preferred, QSizePolicy::Preferred );
	policy1.setHorizontalStretch( 0 );
	policy1.setVerticalStretch( 0 );
	policy1.setHeightForWidth( txtModuleLabel->sizePolicy().hasHeightForWidth() );
	txtModuleLabel->setSizePolicy( policy1 );

	/** Size of the label for the plugin's name at toolbar **/
	txtModuleLabel->adjustSize(); //setMaximumSize( QSize( 100, 150 ) );
	QPalette palettelabel;
	palettelabel.setColor( txtModuleLabel->backgroundRole(), backgroundColor );
	txtModuleLabel->setPalette( palettelabel );
	txtModuleLabel->setAlignment( Qt::AlignVCenter ); //how to 'or' with Qt::TextWordWrap ??
	layoutVertical->addWidget( txtModuleLabel );
}

/**
 * Destructor of the class
 * @return
 */
NMGModuleToolBoxItem::~NMGModuleToolBoxItem()
{}

/**
 * Returns the module name
 * @return
 */
QString NMGModuleToolBoxItem::getModuleName()
{
	return moduleName;
}

/**
 * Emits the activate signal of the module
 */
void NMGModuleToolBoxItem::emitActivated()
{
	emit moduleItemActivated( moduleName );
}

/**
 * Handles the mouse events
 * @param e
 */
void NMGModuleToolBoxItem::mousePressEvent( QMouseEvent * e )
{
	if ( e->button() == Qt::LeftButton ) emitActivated();
	else QWidget::mousePressEvent( e );
}

#include "nmgmoduletoolboxitem.moc"
