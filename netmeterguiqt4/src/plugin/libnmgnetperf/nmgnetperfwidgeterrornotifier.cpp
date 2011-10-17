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
#include "nmgnetperfwidgeterrornotifier.h"
#include "nmgnetperfwidget.h"

#include <QMessageBox>

#include <iostream>
using namespace std;

void NMGNetperfWidgetErrorNotifier::notifyErrorToUser( int errorType, QString & errorMessage )
{
	if ( errorType > 5 )
	{
		/* warning or debug */
		if ( errorType == 10 ) cout << "WARNING: " << qPrintable( errorMessage ) << endl;
		else cout << "DEBUG: " << qPrintable( errorMessage ) << endl;
		return;
	}

	if ( errorMessage == CANNOT_RESOLVE_SOURCE_ADDRESS )
	{
		QMessageBox::critical( widget, "Cannot resolve source address!", "Source address couldn't be resolved." );
		NMGHostInformationComboWidget * combo = widget->hostSourceCombo;
		combo->setHostBaseWidgetFocus( NMGHostInformationComboWidget::NMGHostInfoComboManagementAddress );
		combo->showHostInformationWidget();
	}
	else if ( errorMessage == CANNOT_RESOLVE_DEST_ADDRESS )
	{
		QMessageBox::critical( widget, "Cannot resolve destination address!", "Destination address couldn't be resolved." );
		NMGHostInformationComboWidget * combo = widget->hostDestinationCombo;
		combo->setHostBaseWidgetFocus( NMGHostInformationComboWidget::NMGHostInfoComboManagementUsername );
		combo->showHostInformationWidget();
	}

	else if ( errorMessage == NETPERF_PATH_WRONG )
	{
		QMessageBox::critical( widget, "Wrong Netperf path!", "Netperf's path is not correct." );
		NMGHostInformationComboWidget * combo = widget->hostSourceCombo;
		combo->showHostInformationWidget( 0, ID_NETPERF_PATH );
	}
	else if ( errorMessage == NETPERF_ADRRESS_ALREADY_IN_USE )
		QMessageBox::critical( widget, "Netperf port already in use!",
				       "The source host has already a netperf running at this port." );
	else if ( errorMessage == NETPERF_INVALID_OPTION )
		QMessageBox::critical( widget, "Netperf invalid arguments!", "Error running netperf command because of the arguments." );
	else if ( errorMessage == NETPERF_ERROR_CONNECT_NETSERVER )
		QMessageBox::critical( widget, "Error running netperf!", "Netperf couldn't establish connection to netserver." );
	else if ( errorMessage == NETPERF_ERROR_PERMISSION )
	{
		QMessageBox::critical( widget, "Netperf error!", "Not enough permissions to run netperf in source host." );
		NMGHostInformationComboWidget * combo = widget->hostSourceCombo;
		combo->setHostBaseWidgetFocus( NMGHostInformationComboWidget::NMGHostInfoComboManagementUsername );
		combo->showHostInformationWidget();
	}
	else if ( errorMessage == NETPERF_ERROR_UNKNOWN )
		QMessageBox::critical( widget, "Netperf error!", "An unknown netperf error occurred during execution." );
	
	else if ( errorMessage == NETSERVER_PATH_WRONG )
	{
		QMessageBox::critical( widget, "Wrong Netserver path!", "Netserver's path is not correct." );
		NMGHostInformationComboWidget * combo = widget->hostDestinationCombo;
		combo->showHostInformationWidget( 0, ID_NETSERVER_PATH );
	}
	else if ( errorMessage == NETSERVER_INVALID_OPTION )
		QMessageBox::critical( widget, "Netserver invalid arguments!", "Error running netserver command because of the arguments." );
	else if ( errorMessage == NETSERVER_ERROR_PERMISSION )
	{
		QMessageBox::critical( widget, "Netserver error!", "Not enough permissions to run netserver in destination host." );
		NMGHostInformationComboWidget * combo = widget->hostDestinationCombo;
		combo->setHostBaseWidgetFocus( NMGHostInformationComboWidget::NMGHostInfoComboManagementUsername );
		combo->showHostInformationWidget();
	}
	else if ( errorMessage == NETSERVER_ERROR_UNKNOWN )
		QMessageBox::critical( widget, "Netserver error!", "An unknown netserver error occurred during execution." );
	
	else if ( errorMessage == BAD_PASSWORD_SOURCE_HOST )
	{
		QMessageBox::critical( widget, "Bad source host password!", "User's password at source host is not correct." );
		NMGHostInformationComboWidget * combo = widget->hostSourceCombo;
		combo->setHostBaseWidgetFocus( NMGHostInformationComboWidget::NMGHostInfoComboManagementPassword );
		combo->showHostInformationWidget();
	}
	else if ( errorMessage == NETPERF_SSH_TIMEOUT_ERROR )
		QMessageBox::critical( widget, "Source host error!", "Timeout while connecting via ssh to source host to send netperf command." );
	else if ( errorMessage == NETPERF_SSH_UNKNOWN_ERROR )
		QMessageBox::critical( widget, "Source host error!", "Unknown error while connecting via ssh to source host." );
	
	else if ( errorMessage == BAD_PASSWORD_DEST_HOST )
	{
		QMessageBox::critical( widget, "Bad destination host password!", "User's password at destination host is not correct." );
		NMGHostInformationComboWidget * combo = widget->hostDestinationCombo;
		combo->setHostBaseWidgetFocus( NMGHostInformationComboWidget::NMGHostInfoComboManagementPassword );
		combo->showHostInformationWidget();
	}
	else if ( errorMessage == NETSERVER_SSH_TIMEOUT_ERROR )
		QMessageBox::critical( widget, "Destination host error!", "Timeout while connecting via ssh to destination host to send netserver command." );
	else if ( errorMessage == NETSERVER_SSH_UNKNOWN_ERROR )
		QMessageBox::critical( widget, "Destination host error!", "Unknown error while connecting via sftp to destination host to send netserver script." );
	
	else if ( errorMessage == DECODE_SFTP_TIMEOUT_ERROR )
		QMessageBox::critical( widget, "Destination host error!", "Timeout while connecting via sftp to destination host to get decode result." );
	else if ( errorMessage == DECODE_SFTP_IS_CONNECTED_ERROR )
		QMessageBox::critical( widget, "Destination host error!", "'Is connected error' while connecting via sftp to destination host to get decode result." );
	else if ( errorMessage == DECODE_SFTP_UNKNOWN_ERROR )
		QMessageBox::critical( widget, "Destination host error!", "Unknown error while connecting via sftp to destination host to get decode result." );
	
	else if ( errorMessage == XML_CONFIG_ERROR )
		QMessageBox::critical( widget, "Wrong XML configuration received!", "Wrong XML configuration sended." );
	
	else if ( errorMessage == XML_OUTPUT_ERROR )
		QMessageBox::critical( widget, "Result file error!", "Error parsing netperf's result." );

	else cout << "Warning: Unknown error message: " << qPrintable( errorMessage ) << "." << endl;
}
