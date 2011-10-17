/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#include "nmgstatisticrequestwidget.h"
#include <QFileDialog>
#include <QMessageBox>

NMGStatisticRequestWidget::NMGStatisticRequestWidget ( NMGISender * s, NMGIModuleCommunication * mc,
        NMGModulePreferencesManager * prefMan, QWidget *parent, const char *name )
		: QWidget ( parent )
{
	setObjectName ( name );
	setupUi ( this );

	sender = s;
	moduleCommunication = mc;
	preferencesManager = prefMan;
	connect ( btnRequest, SIGNAL ( clicked() ), this, SLOT ( btnReqClicked() ) );
        connect ( btnSubscribe, SIGNAL ( clicked() ), this, SLOT ( btnSubClicked() ) );
	connect ( btnFile, SIGNAL ( clicked() ), this, SLOT ( btnFileClicked() ) );
}

NMGStatisticRequestWidget::~NMGStatisticRequestWidget() {}

void NMGStatisticRequestWidget::btnFileClicked()
{
	QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Select test file" ), 
                           preferencesManager->getValue ( ID_PREFERENCE_LAST_STATISTIC_PATH ),
	                   tr ( "XML Flow Format (*.xff);;XML (*.xml);;All files (*.*)" ) );

	if ( !fileName.isEmpty() ) txtPath->setText ( fileName );
}

void NMGStatisticRequestWidget::btnReqClicked()
{
	// Indeed, there is no need to verify the existence of the module because
	// the same mechanism of request is responsible for checking it
	if ( !moduleCommunication->existsModule ( "NMGStatistic" ) )
		QMessageBox::warning ( this, "Statistic is not loaded",
		                       "Statistic module is not loaded into memory.\nPlease load the module if you want to execute it." );
	else
	{
		QString xmlMessage = "<data><test type=\"packets\"><path>" + txtPath->text() + "</path><flows>all</flows><statistic>all</statistic></test></data>";
		QString module = moduleCommunication->request ( "NMGStatistic", xmlMessage );
		if ( module.isEmpty() ) txtResult->setPlainText ( "ERROR ON REQUEST" );
	}
}

void NMGStatisticRequestWidget::btnSubClicked()
{
	// Indeed, there is no need to verify the existence of the module because
	// the same mechanism of subscribe is responsible for checking it
	if ( !moduleCommunication->existsModule ( "NMGStatistic" ) )
		QMessageBox::warning ( this, "Statistic is not loaded",
		                       "Statistic module is not loaded into memory.\nPlease load the module if you want to execute it." );
	else
	{
		QString module = moduleCommunication->subscribe ( "NMGStatistic", "<data><subscribe/></data>" );
		if ( module.isEmpty() ) txtResult->setPlainText ( "ERROR ON REQUEST" );
                else sender->sendGUICommand ( Start, "<data><test>" + txtPath->text() + "</test></data>" );
	}
}

#include "nmgstatisticrequestwidget.moc"
