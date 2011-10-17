/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
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

#include <QMessageBox>

#include "nmgmgenwidgeterrornotifier.h"
#include "nmgmgenwidget.h"

#include <iostream>

using namespace std;

/**
 * Contructor of the class
 * @return 
 */
NMGMgenWidgetErrorNotifier::NMGMgenWidgetErrorNotifier(NMGMGenWidget * w)
{
	widget = w;
}


/**
 * Destructor of the class
 * @return 
 */
NMGMgenWidgetErrorNotifier::~NMGMgenWidgetErrorNotifier()
{
}

/**
 * Notifies an error to the user
 * @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
 * @param errorMessage Error Message
 */
void NMGMgenWidgetErrorNotifier::notifyErrorToUser(int errorType, QString errorMessage)
{
	if(errorType>5) 
	{ /* warning or debug */
		if(errorType==10) cout << "WARNING: " << qPrintable(errorMessage) << endl;
		else cout << "DEBUG: " << qPrintable(errorMessage) << endl;
		return;
	}
	
	if(errorMessage==MGEN_PATH_WRONG)
	{
		QMessageBox::critical( 0,"Wrong Mgen path!","The Mgen path is not correct.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostSourceCombo;
		combo->showHostInformationWidget(0,ID_MGEN_PATH);
	}
	else if(errorMessage==DREC_PATH_WRONG)
	{
		QMessageBox::critical( 0,"Wrong Drec path!","The Drec path is not correct.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostDestinationCombo;
		combo->showHostInformationWidget(0,ID_DREC_PATH);
	}
	else if(errorMessage==DECODE_PATH_WRONG)
	{
		QMessageBox::critical( 0,"Wrong Decode path!","The Decode path is not correct.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostDestinationCombo;
		combo->showHostInformationWidget(0,ID_DECODE_PATH);
	}
	else if(errorMessage==MGEN4_PATH_WRONG)
	{
		QMessageBox::critical( 0,"Wrong Mgen (v4) path!","The Mgen path is not correct.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostDestinationCombo;
		combo->showHostInformationWidget(0,ID_MGEN4_PATH);
	}
	else if(errorMessage==CANNOT_RESOLVE_SOURCE_ADDRESS)
	{
		QMessageBox::critical( 0,"Cannot resolve source address!","The source address couldn't be resolved.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostSourceCombo;
		combo->setHostBaseWidgetFocus(NMGHostInformationComboWidget::NMGHostInfoComboManagementAddress);
		combo->showHostInformationWidget();
	}
	else if(errorMessage==CANNOT_RESOLVE_DEST_ADDRESS)
	{
		QMessageBox::critical( 0,"Cannot resolve destination address!","The destination address couldn't be resolved.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostDestinationCombo;
		combo->setHostBaseWidgetFocus(NMGHostInformationComboWidget::NMGHostInfoComboManagementUsername);
		combo->showHostInformationWidget();	
	}
	else if(errorMessage==BAD_PASSWORD_SOURCE_HOST)
	{
		QMessageBox::critical( 0,"Bad source host password!","The password for the user at source host is not correct.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostSourceCombo;
		combo->setHostBaseWidgetFocus(NMGHostInformationComboWidget::NMGHostInfoComboManagementPassword);
		combo->showHostInformationWidget();
	}
	else if(errorMessage==BAD_PASSWORD_DEST_HOST)
	{
		QMessageBox::critical( 0,"Bad destination host password!","The password for the user at destination host is not correct.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostDestinationCombo;
		combo->setHostBaseWidgetFocus(NMGHostInformationComboWidget::NMGHostInfoComboManagementPassword);
		combo->showHostInformationWidget();
	}
	else if(errorMessage==ERROR_CONNECTING_SOURCE_SFTP)
	{
		QMessageBox::critical( 0,"Cannot connect to source host!",
			"The connection to the source host using sftp was not possible.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostDestinationCombo;
		combo->setHostBaseWidgetFocus(NMGHostInformationComboWidget::NMGHostInfoComboManagementUsername);
		combo->showHostInformationWidget();
	}
	else if(errorMessage==ERROR_CONNECTING_DEST_SFTP)
	{
		QMessageBox::critical( 0,"Cannot connect to destination host!",
			"The connection to the destination host using sftp was not possible.");
		NMGHostInformationComboWidget * combo = widget->mainWidget->hostDestinationCombo;
		combo->setHostBaseWidgetFocus(NMGHostInformationComboWidget::NMGHostInfoComboManagementUsername);
		combo->showHostInformationWidget();	
	}
	else if(errorMessage==MGEN_ADRRESS_ALREADY_IN_USE)
		QMessageBox::critical( 0,"Mgen port already in use!",
			"The source host has already a mgen running at this port.");
	else if(errorMessage==ERROR_RUNNING_MGEN)
		QMessageBox::critical( 0,"Error running mgen!","An error occurred while running mgen.");
	else if(errorMessage==ERROR_RUNNING_DREC)
		QMessageBox::critical( 0,"Error running drec!","An error occurred while running drec.");
	else if(errorMessage==ERROR_RUNNING_DECODE)
		QMessageBox::critical( 0,"Error running decode!","An error occurred while running decode.");
	else if(errorMessage==MGEN_ERROR_PARSING_SCRIPT)
		QMessageBox::critical( 0,"Mgen script error!","Error parsing mgen script.");
	else if(errorMessage==MGEN_ERROR_LOADING_SCRIPT)
		QMessageBox::critical( 0,"Mgen script error!","Error loading mgen script.");
	else if(errorMessage==MGEN_ERROR_UNKNOWN)
		QMessageBox::critical( 0,"Mgen error!","An unknown mgen error occurred during execution.");
	else if(errorMessage==DREC_ERROR_PARSING_SCRIPT)
		QMessageBox::critical( 0,"Drec script error!","Error parsing drec script.");
	else if(errorMessage==DREC_ERROR_LOADING_SCRIPT)
		QMessageBox::critical( 0,"Drec script error!","Error loading drec script.");
	else if(errorMessage==DREC_ERROR_UNKNOWN)
		QMessageBox::critical( 0,"Drec error!","An unknown drec error occurred during execution.");
	else if(errorMessage==MGEN_SFTP_TIMEOUT_ERROR)
		QMessageBox::critical( 0,"Source host error!","Timeout while connecting via sftp to source host.");
	else if(errorMessage==MGEN_SFTP_IS_CONNECTED_ERROR)
		QMessageBox::critical( 0,"Source host error!","'Is connected error' while connecting via sftp to source host.");
	else if(errorMessage==MGEN_SFTP_UNKNOWN_ERROR)
		QMessageBox::critical( 0,"Source host error!","Unknown error while connecting via sftp to source host.");
	else if(errorMessage==DREC_SFTP_TIMEOUT_ERROR)
		QMessageBox::critical( 0,"Destination host error!","Timeout while connecting via sftp to destination host to send drec script.");
	else if(errorMessage==DREC_SFTP_IS_CONNECTED_ERROR)
		QMessageBox::critical( 0,"Destination host error!","'Is connected error' while connecting via sftp to destination host to send drec script.");
	else if(errorMessage==DREC_SFTP_UNKNOWN_ERROR)
		QMessageBox::critical( 0,"Destination host error!","Unknown error while connecting via sftp to destination host to send drec script.");
	else if(errorMessage==DECODE_SFTP_TIMEOUT_ERROR)
		QMessageBox::critical( 0,"Destination host error!","Timeout while connecting via sftp to destination host to get decode result.");
	else if(errorMessage==DECODE_SFTP_IS_CONNECTED_ERROR)
		QMessageBox::critical( 0,"Destination host error!","'Is connected error' while connecting via sftp to destination host to get decode result.");
	else if(errorMessage==DECODE_SFTP_UNKNOWN_ERROR)
		QMessageBox::critical( 0,"Destination host error!","Unknown error while connecting via sftp to destination host to get decode result.");
	else if(errorMessage==MGEN_EMPTY_FILE)
		QMessageBox::critical(0,"Mgen Error!", "Mgen returned an empty file due to an execution error");
	else cout << "Warning: Unknown error message: " << qPrintable(errorMessage) << "." << endl;
}

    

