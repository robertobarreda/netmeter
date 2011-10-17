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
#ifndef NMGHOSTINFORMATIONWIDGET_H
#define NMGHOSTINFORMATIONWIDGET_H

#include <nmgstation.h>
#include <nmgmoduleutils.h>

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace Ui
{
	class NMGHostInformationMainWidget;
} // namespace Ui


typedef enum {NMGHostInfoTestAddress = 0, NMGHostInfoTestAlias = 1, NMGHostInfoTestProtocol = 2, NMGHostInfoManagementAddress = 3,
              NMGHostInfoManagementUsername = 4, NMGHostInfoManagementPassword = 5, NMGHostInfoManagementProtocol = 6,
              NMGHostInfoManagementAddressPort = 7
             } NMGHostInfoBaseWidgetFocus;


class NMGHostLineEditFieldWidget : public QWidget
{
		Q_OBJECT
	public:
		inline NMGHostLineEditFieldWidget ( QString objectIdName, QString labelText, QString defValue = "", QWidget *parent = 0 )
		{
			layout = new QHBoxLayout ( this );
			layout->setContentsMargins ( 0, 0, 0, 0 );

			label = new QLabel ( labelText, this );
			QSizePolicy sizePolicy ( QSizePolicy::Fixed, QSizePolicy::MinimumExpanding );
			sizePolicy.setHorizontalStretch ( 0 );
			sizePolicy.setVerticalStretch ( 0 );
			sizePolicy.setHeightForWidth ( label->sizePolicy().hasHeightForWidth() );
			label->setSizePolicy ( sizePolicy );
			layout->addWidget ( label );

			lineEdit = new QLineEdit ( defValue , this );
			lineEdit->setObjectName ( objectIdName );
			layout->addWidget ( lineEdit );

			if ( !defValue.isEmpty() )
			{
				defaultValue = defValue;
				defaultButton = new QPushButton ( this );
				defaultButton->setIcon ( QIcon ( *NMGModuleUtils::getPixmap ( "reload.png" ) ) );
				defaultButton->setObjectName ( objectIdName + "_default" );
				defaultButton->setToolTip ( "Restore default value: " + defaultValue );
				layout->addWidget ( defaultButton );
				connect ( defaultButton, SIGNAL ( clicked() ), this, SLOT ( restoreDefaultValue() ) );
			}
		}

		QHBoxLayout *layout;
		QLabel * label;
		QLineEdit * lineEdit;
		QPushButton * defaultButton;
		QString defaultValue;

	private slots:
		inline void restoreDefaultValue () { lineEdit->setText ( defaultValue ); }
};

class NMGHostInformationWidget : public QWidget
{
		Q_OBJECT
	public:
		NMGHostInformationWidget ( QWidget *parent = 0, const char *name = 0, Qt::WindowFlags fl = 0 );
		~NMGHostInformationWidget();
		QString getCurrentModule();
		void setCurrentModule ( QString mod );
		void updateCurrentModuleView ();
		void addTestHostAddressToList ( QString hostAddress );
		void addTestHostAliasToList ( QString hostAlias );
		QString getTestHostAddress();
		QString getTestHostAlias();
		bool getTestIsIPv4();
		QString getManagementHostAddress();
		QString getManagementHostAlias();
		bool getManagementIsIPv4();
		QString getManagementUsername();
		QString getManagementPassword();
		int getManagementAddressPort();
		void setManagementAddressPort ( int port );
		void setTestHostAddress ( QString testAddr );
		void setTestHostAlias ( QString testAlias );
		void setTestIsIPv4 ( bool testIPv4 );
		void setManagementHostAddress ( QString managAddr );
		void setManagementHostAlias ( QString managAlias );
		void setManagementIsIPv4 ( bool managIPv4 );
		void setManagementUsername ( QString user );
		void setManagementPassword ( QString pass );
		NMGStation * getStation();
		void setWidgetFocus ( NMGHostInfoBaseWidgetFocus focus );
		void setFieldFocus ( const QString & field );
                inline void setShowAllModuleTabs() { showAllModuleTabs = true; }

	signals:
		void widgetHidden();

	public slots:
		void addHostField ( QString module, QString fieldName, QString fieldNameToShow, QString defaultValue );
		void buttonOkClicked();
		void buttonCancelClicked();
		void buttonDeleteClicked();
		void comboAddressAliasActivated ( const QString & string );
		void updateWidgetWithStation ( NMGStation * station = 0 );

	protected:
		void keyReleaseEvent ( QKeyEvent *e );

	private:
		void createModuleTabs();
		void showEvent ( QShowEvent * );
		void hideEvent ( QHideEvent * );
		int indexModule ( QString name );
		QLineEdit * getLineEdit ( QString name );
		void updateWidgetWithStationFields ( NMGStation * s = 0 );

	private:
		QWidget * w;
		Ui::NMGHostInformationMainWidget * mainWidget;
		QString currentModule;
		NMGStation * currentStation;
                bool showAllModuleTabs;
};
#endif
