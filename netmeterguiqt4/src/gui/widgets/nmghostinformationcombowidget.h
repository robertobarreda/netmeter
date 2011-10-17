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
#ifndef NMGHOSTINFORMATIONCOMBOWIDGET_H
#define NMGHOSTINFORMATIONCOMBOWIDGET_H

#include <nmgstation.h>

#include <QWidget>

namespace Ui {
    class NMGHostInformationComboWidgetBase;
} // namespace Ui

/**
@author Ernest Nieto
*/
class NMGHostInformationComboWidget : public QWidget
{
		Q_OBJECT
	public:
		NMGHostInformationComboWidget ( QWidget *parent = 0, const char *name = 0 );
		~NMGHostInformationComboWidget();
		QString getStationAddressFromCombo();
		NMGStation * getStationFromCombo();
		void notifyStation ( NMGStation * station );
		void addAddressToCombo ( QString address );
		void setAddressOfStation ( const QString & address );
		void clear();

		typedef enum
		{
		    NMGHostInfoComboTestAddress=0,
		    NMGHostInfoComboTestAlias=1,
		    NMGHostInfoComboTestProtocol=2,
		    NMGHostInfoComboManagementAddress=3,
		    NMGHostInfoComboManagementUsername=4,
		    NMGHostInfoComboManagementPassword=5,
		    NMGHostInfoComboManagementProtocol=6
		} NMGHostInfoComboBaseWidgetFocus;

		void setHostBaseWidgetFocus ( NMGHostInfoComboBaseWidgetFocus baseWidget );

	signals:
		void stationSelected ( QString addressStation );
		void stationChanged ( NMGStation * station );


	public slots:
		void showHostInformationWidget();
		void showHostInformationWidget ( const QString & host, const QString & fieldToFocus=0 );

	private slots:
		void buttonClicked();
		void hostSelected ( const QString & string );
		void slotTextChanged ( const QString &text );

	private:
		QWidget * w;
		Ui::NMGHostInformationComboWidgetBase * mainWidget;

};

#endif
