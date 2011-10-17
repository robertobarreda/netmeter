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
#include "nmghostinformationcombowidget.h"
#include "ui_nmghostinformationcombowidgetbase.h"

#include <nmghostinformationmanager.h>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGHostInformationComboWidget::NMGHostInformationComboWidget( QWidget *parent, const char *name )
		: QWidget( parent )
{
	setObjectName( name );
	QHBoxLayout * layout = new QHBoxLayout( this );

	w = new QWidget();
	mainWidget = new Ui::NMGHostInformationComboWidgetBase();
	mainWidget->setupUi( w );

	layout->addWidget( w );
	connect(( const QObject * ) mainWidget->buttonOpen, SIGNAL( clicked() ),
	        this, SLOT( buttonClicked() ) );
	connect(( const QObject * ) mainWidget->comboHosts, SIGNAL( activated( const QString & ) ),
	        this, SLOT( hostSelected( const QString & ) ) );
	connect( mainWidget->comboHosts, SIGNAL( editTextChanged( const QString & ) ),
	         this, SLOT( slotTextChanged( const QString & ) ) );

	NMGHostInformationManager::self()->addAddressToCombo( this );

	QPalette palette;
	palette.setColor( mainWidget->comboHosts->backgroundRole(), Qt::white );
	mainWidget->comboHosts->setPalette( palette );
	mainWidget->comboHosts->clearEditText();
}

/**
 * Destructor of the class
 * @return
 */
NMGHostInformationComboWidget::~NMGHostInformationComboWidget()
{
	NMGHostInformationManager::self()->comboToBeDeleted( this );
}

/**
 * Returns the address of the station of the combo box
 * @return
 */
QString NMGHostInformationComboWidget::getStationAddressFromCombo()
{
	return mainWidget->comboHosts->currentText();
}

/**
 * Shows the host information widget
 */
void NMGHostInformationComboWidget::showHostInformationWidget()
{
	NMGHostInformationManager::self()->showHostWidget( mainWidget->comboHosts->currentText(), this );
}

/**
 * Shows the host information widget for the indicated host
 * @param host
 */
void NMGHostInformationComboWidget::showHostInformationWidget( const QString & host, const QString & fieldToFocus )
{
	if ( !host.isNull() ) NMGHostInformationManager::self()->showHostWidget( host, this, fieldToFocus );
	else NMGHostInformationManager::self()->showHostWidget( mainWidget->comboHosts->currentText(), this, fieldToFocus );
}

/**
 * Indicates which widget will have the focus
 * @param baseWidget
 */
void NMGHostInformationComboWidget::setHostBaseWidgetFocus( NMGHostInfoComboBaseWidgetFocus baseWidget )
{
	NMGHostInformationManager::self()->setBaseWidgetFocus(( NMGHostInfoBaseWidgetFocus ) baseWidget );
}

/**
 * Returns the station associated to the address at the combo
 * @return Returns the station  or 0 if there is no such station)
 */
NMGStation * NMGHostInformationComboWidget::getStationFromCombo()
{
	return NMGHostInformationManager::self()->getStation( mainWidget->comboHosts->currentText() );
}

/**
 * Adds the specified address to the combo
 * @param address
 */
void NMGHostInformationComboWidget::addAddressToCombo( QString address )
{
	mainWidget->comboHosts->addItem( address );
}

/**
 * Slot executes when the user clicks the button of the widget
 */
void NMGHostInformationComboWidget::buttonClicked()
{
	NMGHostInformationManager::self()->showHostWidget( mainWidget->comboHosts->currentText(), this );
}

/**
 * Slot executed whan a item is selected from the combo box
 * @param string
 */
void NMGHostInformationComboWidget::hostSelected( const QString & string )
{
	if ( !string.isEmpty() ) emit stationSelected( string );
}

/**
 * Sets the address of the combo to ""
 * @param address
 */
void NMGHostInformationComboWidget::clear()
{
	mainWidget->comboHosts->setCurrentIndex(-1);
}

/**
 * The host widget information notifies of the new station selected added
 * @param station New station (can be 0 if cancelled)
 */
void NMGHostInformationComboWidget::notifyStation( NMGStation * station )
{
	if ( station )
		mainWidget->comboHosts->setCurrentIndex( mainWidget->comboHosts->findText(station->getTestHost().getAddress()));
	emit stationSelected( mainWidget->comboHosts->currentText() );
	emit stationChanged( station );
}

/**
 * Sets the address of the combo (if it doesn't exist it's added)
 * @param address
 */
void NMGHostInformationComboWidget::setAddressOfStation( const QString & address )
{
	mainWidget->comboHosts->setCurrentIndex( mainWidget->comboHosts->findText(address) );
}

void NMGHostInformationComboWidget::slotTextChanged( const QString &text )
{
	QPalette palette;

	if ( text.isEmpty() || NMGHostInformationManager::self()->existsStation( text ) )
		palette.setColor( mainWidget->comboHosts->backgroundRole(), Qt::white );
	else
		palette.setColor( mainWidget->comboHosts->backgroundRole(), QColor( 255, 175, 175 ) ); //QColor pink

	mainWidget->comboHosts->setPalette( palette );
}

#include "nmghostinformationcombowidget.moc"
