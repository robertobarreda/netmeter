/***************************************************************************
 *   Copyright (C) 2005 by Rus Andrei Bogdan                               *
 *   bogdanrus2004@yahoo.com                                               *
 *   Copyright (C) 2008 by Joan Sala Calero                                *
 *   jsala@ac.upc.edu                                                      *
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
#include <nmghostinformationmanager.h>

#include "nmgsimulator.h"
#include <nmstring.h>

#include <iostream>
using namespace std;

/**
 * Constructor of the class
 * @return
 */
NMGSimulator::NMGSimulator()
{
	moduleName = MOD_NAME;
	moduleCapabilities = MOD_CAPABILITIES;
	moduleVersion = MOD_VERSION;
	moduleCoreMatching = CORE_MATCHING;
	moduleGUIName = GUI_NAME;
	widget = 0;
}

/**
 * Destructor of the class
 * @return
 */
NMGSimulator::~NMGSimulator()
{
}


void NMGSimulator::dataReceived ( QString s )
{
	bool ok;
	int interfNumber, index, iniPos, lastPos;
	QString tag, interf, nextCommand;

	/*****************Getting the status of connection to the router host******************/

	if ( s.indexOf ( "STEP" ) != -1 )
	{
		if ( s.indexOf ( "STEPSTOP" ) != -1 )
		{
			widget->progressItem->setComplete();
			widget->progressItem->updateProgress();
			widget->progressItem = 0;

			cout << GREEN << "[OK] All the settings were implemented to the host" << ENDCOLOR << endl;
			
			widget->loadButton->setEnabled ( 1 );
			widget->saveButton->setEnabled ( 1 );
			widget->applyButton->setEnabled ( 1 );
			widget->delFlowButton->setEnabled ( 1 );
			widget->addFlowButton->setEnabled ( 1 );
			return;
		}
		widget->progressItem->incCompletedItems(); //incrementing the progressBar
		widget->progressItem->updateProgress();
	}

	/************************************************************************/


	/******************Getting the interfaces*******************************/
	else if ( s.indexOf ( "Connection" ) != -1 )
	{
		if ( s.indexOf ( "NOK" ) != -1 )
		{
			widget->progressItem->setComplete();//stropping the progressbar
			widget->progressItem = 0;
			QMessageBox::critical (0, "Error", "Can't connect to the host\nPlease verify the paswword, address and port.");
			widget->connectButton->setEnabled ( 1 );
			
			cout << RED << "[ERROR] Can't connect to the host" << ENDCOLOR << endl;

			return;
		}
		else
		{
			iniPos = s.indexOf ( "<nrInterf>" );
			lastPos = s.indexOf ( "</nrInterf>" );
			QString nrInterf = s.mid ( iniPos + QString ( "<nrInterf>" ).length(), lastPos - ( iniPos + QString ( "<nrInterf>" ).length() ) );

			interfNumber = nrInterf.toInt ( &ok, 10 );
			if ( interfNumber < 2 )
			{
				widget->progressItem->setComplete();//stropping the progressbar
				widget->progressItem = 0;
				QMessageBox::critical ( 0, "Error", "The host where you want to connect doesen't have at least two network interfaces\nPlease chose another host." );
				widget->connectButton->setEnabled ( 1 );
				cout << RED << "[ERROR] The host has to have at least two network interfaces" << ENDCOLOR << endl;
				return;
			}
			else
			{
				widget->connectButton->setEnabled ( 0 );
				widget->srcInterface->setEnabled ( 1 );
				widget->pixmapLabel4->setEnabled ( 1 );
				widget->destInterface->setEnabled ( 1 );
				widget->applyButton->setEnabled ( 1 );
				widget->saveButton->setEnabled ( 1 );
				widget->loadButton->setEnabled ( 1 );
				index = 0;

				widget->progressItem->setComplete();//stropping the progressbar
				widget->progressItem = 0;
				for ( index = 1; index <= interfNumber; index++ )
				{
					tag.sprintf ( "interface%d", index );
					iniPos = s.indexOf ( "<" + tag + ">" );
					lastPos = s.indexOf ( "</" + tag + ">" );
					interf = s.mid ( iniPos + QString ( "<" + tag + ">" ).length(), lastPos - ( iniPos + QString ( "<" + tag + ">" ).length() ) );
					widget->srcInterface->insertItem ( widget->srcInterface->count(), interf, 0 );
					widget->destInterface->insertItem ( widget->destInterface->count(), interf, 0 );
				}
				cout << GREEN << "[OK] The host has at least two active interfaces" << ENDCOLOR << endl;
			}

		}
	}
	/**************************************************************************/


	/******************Getting the data about witch interfaces to be cleaned ************************/
	else if ( s.indexOf ( "Clean" ) != -1 )
	{
		if ( s.indexOf ( "NOK" ) != -1 )
		{
			widget->progressItem->setComplete();//stropping the progressbar
			widget->progressItem = 0;
			QMessageBox::critical ( 0, "Error", "Can't connect to the host\nPlease verify the password, address and port." );
			cout << RED <<  "[ERROR] Can't connect to the host : Please verify the password, address and port." << ENDCOLOR << endl;
			
			widget->connectButton->setEnabled ( 1 );
			return;
		}
		else
		{

			interfNumber = widget->destInterface->count();

			for ( index = 1; index <= interfNumber; index++ )
			{
				tag.sprintf ( "clean%d", index );
				iniPos = s.indexOf ( "<" + tag + ">" );
				lastPos = s.indexOf ( "</" + tag + ">" );
				interf = s.mid ( iniPos + QString ( "<" + tag + ">" ).length(), lastPos - ( iniPos + QString ( "<" + tag + ">" ).length() ) );

				//if the actual interface has tc settings, then cleanInterface[index]=TRUE
				if ( interf == "YES" ) { widget->cleanInterface[index] = TRUE; }
				else { widget->cleanInterface[index] = FALSE; }
			}
			widget->progressItem->incCompletedItems();//incrementing the progressBar
			widget->progressItem->updateProgress();
			sleep ( 1 );
			widget->sendSettingsSlot();
		}
	}
	/**************************************************************************/


	/*********************The response that the root qdisc were made***********/
	else if ( s.indexOf ( "root" ) != -1 )
	{
		if ( s.indexOf ( "rootNOK" ) != -1 )
		{
			widget->progressItem->setComplete();
			widget->progressItem = 0;
			QMessageBox::critical ( 0, "Error", "Can't apply the settings to the router\nPlease verify the connection." );
			widget->connectButton->setEnabled ( 1 );

			cout << RED << "[ERROR] Can't apply the settings to the router" << ENDCOLOR << endl;
			return;
		}

		iniPos = s.indexOf ( "<nextCommand>" );
		lastPos = s.indexOf ( "</nextCommand>" );
		nextCommand = s.mid ( iniPos + QString ( "<nextCommand>" ).length(), lastPos - ( iniPos + QString ( "<nextCommand>" ).length() ) );

		widget->progressItem->incCompletedItems();//incrementing the progressBar
		widget->progressItem->updateProgress();
		sleep ( 1 );
		widget->nextSettingSlot ( nextCommand.toInt ( &ok, 10 ) );//starting the first command
	}
	/**************************************************************************/



	/*********Getiing the response that the setting was received correctly****/
	else if ( s.indexOf ( "setting" ) != -1 )
	{
		if ( s.indexOf ( "settingNOK" ) != -1 )
		{
			widget->progressItem->setComplete();
			widget->progressItem = 0;
			QMessageBox::critical ( 0, "Error", "Can't apply the settings to the router\nPlease verify the connection." );
			widget->connectButton->setEnabled ( 1 );
			cout << RED << "[ERROR] Can't apply the settings to the router" << ENDCOLOR << endl;
			return;
		}

		iniPos = s.indexOf ( "<nextCommand>" );
		lastPos = s.indexOf ( "</nextCommand>" );
		nextCommand = s.mid ( iniPos + QString ( "<nextCommand>" ).length(), lastPos - ( iniPos + QString ( "<nextCommand>" ).length() ) );

		if ( nextCommand != "NO" )
		{
			widget->progressItem->incCompletedItems();
			widget->progressItem->updateProgress(); //incrementing the progressBar
			sleep ( 1 );
			widget->nextSettingSlot ( nextCommand.toInt ( &ok, 10 ) );
		}
		else
		{
			widget->progressItem->incCompletedItems();
			widget->progressItem->updateProgress();
			sleep ( 1 );
			cout << LIGHTBLUE <<"[INFO] There are no more settings to be send" << ENDCOLOR << endl;
		}

	}
	/*************************************************************************/

	return;
}


bool NMGSimulator::setExecutionData ( QString data )
{
	sendCommand ( Start, data );
	return true;
}

/**
 * Returns the widget of the plugin
 * @return
 */
QWidget * NMGSimulator::getWidget()
{
	if ( !widget ) widget = new NMGSimulatorWidget(this, &getTabWidgetManager(), &getPreferencesManager());
	return widget;
}

/**
 * Returns the toolbar associated to the module
 * @return 
 */
QToolBar * NMGSimulator::getToolBar()
{
	getWidget();
	return widget->getToolBar();
}

/**
 * Returns the popup menu associated to the module
 * @return 
 */
QMenu * NMGSimulator::getPopupMenu()
{
	getWidget();
	return widget->getPopupMenu();
} 
