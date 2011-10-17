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
#include "nmghostinformationwidget.h"
#include "ui_nmghostinformationmainwidget.h"

#include <nmghostinformationmanager.h>
#include <nmgmodulefieldsmanager.h>

#include <QGroupBox>
#include <QComboBox>
#include <QToolBox>
#include <QLineEdit>
#include <QLayout>
#include <QRadioButton>
#include <QObject>
#include <QSpinBox>
#include <QGridLayout>
#include <QList>

#include <iostream>
using namespace std;

/**
 *  Constructs a NMGHostInformation as a child of 'parent', with the
 *  name 'name' and widget flags set to 'fl'.
 * @param parent
 * @param name
 * @param fl
 * @return
 */
NMGHostInformationWidget::NMGHostInformationWidget ( QWidget *parent, const char *name, Qt::WindowFlags fl )
		: QWidget ( parent, fl )
{
	if ( !name )
		setObjectName ( "NMGHostInformation" );
	else
		setObjectName ( name );

	setWindowTitle ( "Host" );

	QHBoxLayout * layout = new QHBoxLayout;
	layout->setMargin ( 0 );
	layout->setSpacing ( 0 );
	layout->setObjectName ( "QHBoxLayoutMainWindow" );
	setLayout ( layout );

	w = new QWidget();
	mainWidget = new Ui::NMGHostInformationMainWidget();
	mainWidget->setupUi ( w );

	// Hide delete button
	mainWidget->btnDeleteStation->setVisible ( false );

	//remove the first item created with the Qt Desginer
	mainWidget->toolBox->removeItem ( 0 );
        mainWidget->toolBox->setBackgroundRole ( QPalette::Window );
	layout->addWidget ( w );

	showAllModuleTabs = false; // Do not show all tabs until its set on porpouse.
        currentModule = QString();
	createModuleTabs();
	connect ( this, SIGNAL ( widgetHidden() ),
	          NMGHostInformationManager::self(), SLOT ( hostWidgetHidden() ) );
	connect ( mainWidget->buttonOk, SIGNAL ( clicked() ),
	          this, SLOT ( buttonOkClicked() ) );
	connect ( mainWidget->buttonCancel, SIGNAL ( clicked() ),
	          this, SLOT ( buttonCancelClicked() ) );
	connect ( mainWidget->btnDeleteStation, SIGNAL ( clicked () ),
	          this, SLOT ( buttonDeleteClicked () ) );
	connect ( mainWidget->comboAdress, SIGNAL ( activated ( const QString& ) ),
	          this, SLOT ( comboAddressAliasActivated ( const QString& ) ) );
	connect ( mainWidget->comboAlias, SIGNAL ( activated ( const QString& ) ),
	          this, SLOT ( comboAddressAliasActivated ( const QString& ) ) );
}

/**
 *  Destroys the object and frees any allocated resources
 * @return
 */
NMGHostInformationWidget::~NMGHostInformationWidget()
{
	// no need to delete child widgets, Qt does it all for us
}

/**
 * Creates the pages of the toolbar for the registered modules
 */
void NMGHostInformationWidget::createModuleTabs()
{
	QVBoxLayout * modLayout;
	NMGHostLineEditFieldWidget * lineEdit;
	QWidget * page;

	QStringList list = NMGHostInformationManager::self()->getFieldsManager().getModuleNames();
	foreach ( QString name, list )
	{
		page = new QWidget ( mainWidget->toolBox );
		page->setObjectName ( name );
                modLayout = new QVBoxLayout;
		page->setLayout ( modLayout );

		NMGFieldsInfoList l = NMGHostInformationManager::self()->getFieldsManager().getModuleFieldsList ( name )->getList();
		for ( QList<NMGModuleFieldsInfo *>::iterator itMod = l.begin(); itMod != l.end(); ++itMod )
		{
			lineEdit = new NMGHostLineEditFieldWidget ( ( *itMod )->fieldName, ( *itMod )->fieldNameToShow,
				( *itMod )->fieldDefaultValue, page );
			modLayout->addWidget ( lineEdit );
		}
		//QSpacerItem * vSpacer = new QSpacerItem ( 21, 550, QSizePolicy::Expanding, QSizePolicy::Expanding );
		//modLayout->addItem ( vSpacer );

		mainWidget->toolBox->addItem ( page, name );
	}
}


/**
 * Adds a host field
 * @param module
 * @param field
 * @param fieldName
 */
void NMGHostInformationWidget::addHostField ( QString module, QString fieldName, QString fieldNameToShow, QString defaultValue )
{
	QVBoxLayout * modLayout;
	QWidget * page;
	int i = indexModule ( module );
	if ( i < 0 )
	{
		page = new QWidget ( mainWidget->toolBox );
		page->setObjectName ( module );
		modLayout = new QVBoxLayout;
		page->setLayout ( modLayout );

		mainWidget->toolBox->addItem ( page, module );

		//QSpacerItem * vSpacer = new QSpacerItem ( 21, 550, QSizePolicy::Expanding, QSizePolicy::Expanding );
		//modLayout->addItem ( vSpacer );
	}
	else
	{
		page = mainWidget->toolBox->widget ( i );
		modLayout = ( QVBoxLayout * ) page->layout();
	}

	if ( getLineEdit ( fieldName ) ) return; //the QEditLine exists

	NMGHostLineEditFieldWidget * lineEdit = new NMGHostLineEditFieldWidget ( fieldName, fieldNameToShow, defaultValue, page );
	modLayout->insertWidget (modLayout->count(), lineEdit );
}

/**
 *
 * @param focus
 */
void NMGHostInformationWidget::setWidgetFocus ( NMGHostInfoBaseWidgetFocus focus )
{
	if ( focus == NMGHostInfoTestAddress ) mainWidget->comboAdress->setFocus();
	else if ( focus == NMGHostInfoTestAlias ) mainWidget->comboAlias->setFocus();
	else if ( focus == NMGHostInfoTestProtocol ) mainWidget->testProtocolIPv4->setFocus();
	else if ( focus == NMGHostInfoManagementAddress ) mainWidget->txtManagementAdress->setFocus();
	else if ( focus == NMGHostInfoManagementUsername ) mainWidget->txtManagementUsername->setFocus();
	else if ( focus == NMGHostInfoManagementPassword ) mainWidget->txtManagementPassword->setFocus();
	else if ( focus == NMGHostInfoManagementProtocol ) mainWidget->managementProtocolIPv4->setFocus();
	else if ( focus == NMGHostInfoManagementAddressPort ) mainWidget->spinBoxManagementAddressPort->setFocus();
	else cerr << "Unknown base widget to get the focus at Host information Widget." << endl;
}

/**
 * Sets which field has the focus
 * @param field Field id
 */
void NMGHostInformationWidget::setFieldFocus ( const QString & field )
{
	if ( field.isNull() ) return;
	QLineEdit * edit = getLineEdit ( field );
	if ( !edit )
	{
		cerr << "There is no input widget associated to the field " << qPrintable ( field ) << "." << endl;
		return;
	}
	else edit->setFocus();
}

/**
 * Returns the QLineEdit associated to the name
 * @param name Name of the widget
 * @return QLineEdit or 0 if it doesn't exist
 */
QLineEdit * NMGHostInformationWidget::getLineEdit ( QString name )
{
	QLineEdit * object = w->findChild<QLineEdit *> ( name );
	if ( !object ) return 0;
	else return object;
}

/**
 * Slot executed when the OK button is clicked
 */
void NMGHostInformationWidget::buttonOkClicked()
{
	currentStation = new NMGStation();

	currentStation->getTestHost().setAddress ( getTestHostAddress() );
	currentStation->getTestHost().setAlias ( getTestHostAlias() );
	currentStation->getTestHost().setHostIPV4 ( getTestIsIPv4() );
	if ( !getManagementHostAddress().trimmed().isEmpty() )
		currentStation->getManagementHost().setAddress ( getManagementHostAddress() );
	else currentStation->getManagementHost().setAddress ( getTestHostAddress() );
	currentStation->getManagementHost().setAlias ( getManagementHostAlias() );
	currentStation->getManagementHost().setHostIPV4 ( getManagementIsIPv4() );
	currentStation->setManagementUsername ( getManagementUsername() );
	currentStation->setManagementPassword ( getManagementPassword() );
	currentStation->setManagementAddressPort ( getManagementAddressPort() );

	for ( int i  = 0; i < mainWidget->toolBox->count(); ++i )
	{
		QString module = mainWidget->toolBox->itemText ( i );
		QWidget * widget = mainWidget->toolBox->widget ( i );

		QList<QLineEdit *> l = widget->findChildren<QLineEdit *>();

		for ( QList<QLineEdit *>::iterator it = l.begin(); it != l.end(); ++it )
		{
			QString fieldName = ( *it )->objectName();
			currentStation->getStationFieldsManager().addField ( module, fieldName, ( *it )->text() );
		}
	}
	hide();
}

/**
 * Slot executed whan the Cancel button is clicked
 */
void NMGHostInformationWidget::buttonCancelClicked()
{
	hide();
}

/**
 * Slot executed whan the Delete button is clicked
 */
void NMGHostInformationWidget::buttonDeleteClicked()
{
	QString addressIdStation = getTestHostAddress();
	QString aliasIdStation = getTestHostAlias();
	if ( !addressIdStation.isEmpty() || !aliasIdStation.isEmpty() )
	{
		if ( !addressIdStation.isEmpty() )
			mainWidget->comboAdress->removeItem(mainWidget->comboAdress->currentIndex ());
		if ( !aliasIdStation.isEmpty() )
			mainWidget->comboAlias->removeItem(mainWidget->comboAlias->currentIndex ());

		NMGHostInformationManager::self()->removeStation ( addressIdStation.isEmpty()?aliasIdStation:addressIdStation );
		updateWidgetWithStation();
	}
}

/**
 * Method executed whan the widget is shown
 * @param
 */
void NMGHostInformationWidget::showEvent ( QShowEvent * )
{
	currentStation = 0;
	updateCurrentModuleView();
}

/**
 * Method executed when the widget is hidden
 * @param
 */
void NMGHostInformationWidget::hideEvent ( QHideEvent * )
{
	emit widgetHidden();
}

/**
 * Returns the station associted to the information introduced in the widget (0 if the action was cancelled)
 * @return
 */
NMGStation * NMGHostInformationWidget::getStation()
{
	return currentStation;
}

/**
 * Returns the index of the page of the ToolBox with the indicated name
 * @param name Name of the module
 * @return The index or -1 if it doesn't exist such page
 */
int NMGHostInformationWidget::indexModule ( QString name )
{
	for ( int i  = 0; i < mainWidget->toolBox->count(); ++i )
		if ( mainWidget->toolBox->itemText ( i ) == name ) return i;
	return -1;
}

/**
 * Returns the current module
 * @return
 */
QString NMGHostInformationWidget::getCurrentModule()
{
	return currentModule;
}

/**
 * Sets the current module to the indicated module
 * @param mod
 */
void NMGHostInformationWidget::setCurrentModule ( QString mod )
{
	currentModule = mod;
}

/**
 * Updates the toolBox showing the page with the module indicated in currentModule, if currentModule is "", all modules are shown
 */
void NMGHostInformationWidget::updateCurrentModuleView()
{
	bool found = false;
	if ( showAllModuleTabs )
	{
		/* show everything */
		for ( int i  = 0; i < mainWidget->toolBox->count(); ++i )
			mainWidget->toolBox->setItemEnabled( i, true );
		if ( mainWidget->toolBox->count() > 0 ) mainWidget->toolBox->setCurrentWidget( mainWidget->toolBox->widget( 0 ) );
                else mainWidget->toolBox->hide();
        	mainWidget->btnDeleteStation->setVisible ( true ); // Show delete button for management

                showAllModuleTabs = false; // do not show the next time unless that it's set again
		return;
	}

	mainWidget->btnDeleteStation->setVisible ( false ); // Hide delete button for other uses.
	if ( currentModule.isEmpty() )
	{
		mainWidget->toolBox->hide();
	}
	else
	{
		for ( int i  = 0; i < mainWidget->toolBox->count(); ++i )
		{
			if ( mainWidget->toolBox->itemText ( i ) == currentModule )
			{
				//enable it
				mainWidget->toolBox->setItemEnabled ( i, true );
				mainWidget->toolBox->setCurrentWidget ( mainWidget->toolBox->widget ( i ) );
				found = true;
			}
			else
				//dissable it
				mainWidget->toolBox->setItemEnabled ( i, false );
		}
		if ( !found )
		{
			setCurrentModule ( "" );
			updateCurrentModuleView();
		}
		else
		{
			mainWidget->toolBox->show();
		}
	}
}

/**
 * Slot executed whan the address combo is activated
 * @param string
 */
void NMGHostInformationWidget::comboAddressAliasActivated ( const QString & string )
{
	NMGStation * station = NMGHostInformationManager::self()->getStation ( string );
	if ( !station ) cerr << "There is no station with address " << qPrintable ( string ) << endl;
	else updateWidgetWithStation ( station );
}

/**
 * Updates the widget fields with the values of the station
 * @param station
 */
void NMGHostInformationWidget::updateWidgetWithStation ( NMGStation * station )
{
	if ( !station )
	{
		setTestHostAddress ( "" );
		setTestHostAlias ( "" );
		setTestIsIPv4 ( true );
		setManagementHostAddress ( "" );
		setManagementHostAlias ( "" );
		setManagementIsIPv4 ( true );
		setManagementPassword ( "" );
		setManagementUsername ( "" );
		setManagementAddressPort ( 22 );
		updateWidgetWithStationFields();
	}
	else
	{
		setTestHostAddress ( station->getTestHost().getAddress() );
		setTestHostAlias ( station->getTestHost().getAlias() );
		setTestIsIPv4 ( station->getTestHost().isHostIPv4() );
		setManagementHostAddress ( station->getManagementHost().getAddress() );
		setManagementHostAlias ( station->getManagementHost().getAlias() );
		setManagementIsIPv4 ( station->getManagementHost().isHostIPv4() );
		setManagementPassword ( station->getManagementPassword() );
		setManagementUsername ( station->getManagementUsername() );
		setManagementAddressPort ( station->getManagementAddressPort() );
		updateWidgetWithStationFields ( station );
	}
}

/**
 * Updates the fields of the widget with the information at m
 * @param m
 */
void NMGHostInformationWidget::updateWidgetWithStationFields ( NMGStation * station )
{
	for ( int i  = 0; i < mainWidget->toolBox->count(); ++i )
	{
		QString module = mainWidget->toolBox->itemText ( i );
		QWidget * widget = mainWidget->toolBox->widget ( i );

		QList<QLineEdit *> l = widget->findChildren<QLineEdit *>();

		for ( QList<QLineEdit *>::iterator it = l.begin(); it != l.end(); ++it )
		{
			QString fieldName = ( *it )->objectName();
			if ( station ) ( *it )->setText ( ( station->getStationFieldsManager() ).getField ( module, fieldName ) );
			else ( *it )->setText ( NMGHostInformationManager::self()->getFieldsManager().getDefaultValue ( module, fieldName ) );
		}
	}
}

/**
 * Adds the host address to the combo box with test host addresses
 * @param hostAddress Address of the test host
 */
void NMGHostInformationWidget::addTestHostAddressToList ( QString hostAddress )
{
	mainWidget->comboAdress->insertItem ( mainWidget->comboAdress->count(), hostAddress );
}

/**
 * Adds the host alias to the combo box with test host aliases
 * @param hostalias Alias of the test host
 */
void NMGHostInformationWidget::addTestHostAliasToList ( QString hostAlias )
{
	mainWidget->comboAlias->insertItem ( mainWidget->comboAlias->count(), hostAlias );
}

/**
 * Returns the test host address
 * @return
 */
QString NMGHostInformationWidget::getTestHostAddress()
{
	return mainWidget->comboAdress->currentText();
}

/**
 * Returns the test host alias
 * @return
 */
QString NMGHostInformationWidget::getTestHostAlias()
{
	return mainWidget->comboAlias->currentText();
}

/**
 * Returns if the test host protocol is IPv4
 * @return
 */
bool NMGHostInformationWidget::getTestIsIPv4()
{
	return mainWidget->testProtocolIPv4->isChecked();
}

/**
 * Returns the management host address
 * @return
 */
QString NMGHostInformationWidget::getManagementHostAddress()
{
	return mainWidget->txtManagementAdress->text();
}

/**
 * Returns the management host alias
 * @return
 */
QString NMGHostInformationWidget::getManagementHostAlias()
{
	return "";
}

/**
 * Returns the management address port
 * @return
 */
int NMGHostInformationWidget::getManagementAddressPort()
{
	return mainWidget->spinBoxManagementAddressPort->value();
}

/**
 * Sets the management address port
 * @param port
 */
void NMGHostInformationWidget::setManagementAddressPort ( int port )
{
	mainWidget->spinBoxManagementAddressPort->setValue ( port );
}

/**
 * Returns if the management host protocol is IPV4
 * @return
 */
bool NMGHostInformationWidget::getManagementIsIPv4()
{
	if ( mainWidget->managementProtocolIPv4->isChecked() ) return true;
	return false;
}

/**
 * Returns the management host username
 * @return
 */
QString NMGHostInformationWidget::getManagementUsername()
{
	return mainWidget->txtManagementUsername->text();
}

/**
 * Returns the management host password
 * @return
 */
QString NMGHostInformationWidget::getManagementPassword()
{
	return mainWidget->txtManagementPassword->text();
}

/**
 * Sets the test host addres of the widget
 * @param testAddr
 */
void NMGHostInformationWidget::setTestHostAddress ( QString testAddr )
{
	mainWidget->comboAdress->setEditText ( testAddr );
}

/**
 * Sets the test host alias of the widget
 * @param testAlias
 */
void NMGHostInformationWidget::setTestHostAlias ( QString testAlias )
{
	mainWidget->comboAlias->setEditText ( testAlias );
}

/**
 * Sets if the test host is IPv4
 * @param testIPv4
 */
void NMGHostInformationWidget::setTestIsIPv4 ( bool testIPv4 )
{
	if ( testIPv4 )
	{
		mainWidget->testProtocolIPv6->setChecked ( false );
		mainWidget->testProtocolIPv4->setChecked ( true );
	}
	else
	{
		mainWidget->testProtocolIPv4->setChecked ( false );
		mainWidget->testProtocolIPv6->setChecked ( true );
	}
}

/**
 * Sets the management host address
 * @param managAddr
 */
void NMGHostInformationWidget::setManagementHostAddress ( QString managAddr )
{
	mainWidget->txtManagementAdress->setText ( managAddr );
}

/**
 * Sets the management host alias (not used!)
 * @param
 */
void NMGHostInformationWidget::setManagementHostAlias ( QString )
{
	/* NOT USED! */
}

/**
 * Sets if the management host is IPv4
 * @param managIPv4
 */
void NMGHostInformationWidget::setManagementIsIPv4 ( bool managIPv4 )
{
	if ( managIPv4 )
	{
		mainWidget->managementProtocolIPv4->setChecked ( true );
		mainWidget->managementProtocolIPv6->setChecked ( false );
	}
	else
	{
		mainWidget->managementProtocolIPv4->setChecked ( false );
		mainWidget->managementProtocolIPv6->setChecked ( true );
	}
}

/**
 * Sets the management username
 * @param user
 */
void NMGHostInformationWidget::setManagementUsername ( QString user )
{
	mainWidget->txtManagementUsername->setText ( user );
}

/**
 * Sets the management password
 * @param pass
 */
void NMGHostInformationWidget::setManagementPassword ( QString pass )
{
	mainWidget->txtManagementPassword->setText ( pass );
}

/**
 * Sets to Return and Escape keys Accept and Cancel actions
 */
void NMGHostInformationWidget::keyReleaseEvent ( QKeyEvent *e )
{
	switch ( e->key() )
	{
		case Qt::Key_Return:
			buttonOkClicked();
			break;
		case Qt::Key_Escape:
			buttonCancelClicked();
			break;
	}
}

#include "nmghostinformationwidget.moc"
