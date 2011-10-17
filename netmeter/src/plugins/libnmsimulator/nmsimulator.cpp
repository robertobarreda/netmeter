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

#include "nmsimulator.h"
#include <nmstring.h>
#include <iostream>


NMMODULE_CLASS ( NMSimulator );
NMMODULE_NAME ( ( char * ) "nmsimulator" );
NMMODULE_CAPABILITIES ( ( char * ) "test" );
NMMODULE_VERSION ( ( char * ) "0.1" );

/**
 * Constructor of the class
 * @return
 */

NMSimulator::NMSimulator()
{

}

/**
 * Destroyer of the class
 * @return
 */
NMSimulator::~NMSimulator()
{
}

/**
 * Initializes the module
 */
void NMSimulator::initModule()
{

}

/**
 * Executed when the module is removed
 */
void NMSimulator::removeModule()
{

}

/**
 * Start command
 * @param data
 */
void NMSimulator::start ( void *data )
{
	// Test Files:
	
	simulator_file_1 = "/tmp/simulator_ifconfig_" + (NMString)testId() + ".txt";
	simulator_file_2 = "/tmp/simulator_tcdata_" + (NMString)testId() + ".txt"; 
	simulator_script_2 = "/tmp/simulator_tcscript_" + (NMString)testId() + ".sh";
	
	int i, j, interfaceNr, netemNr, protocolNr;
	NMString command, interfaceParam, clean;
	char interfTag[20], netemTag[20],  protTag[20], *line;
	FILE *file;

	NMString *nms_xml = ( NMString * ) data;
	NMXML nmxml_data;
	cout << "DATA:"<< nms_xml->c_str() << endl;

	nmxml_data.loadTree ( nms_xml->c_str() );

	/*************** First If ***********************************/
	// getting the name of the network interfaces on the host
	if ( nms_xml->search ( "nsAddress" ) !=-1 )
	{
		managementAddress = nmxml_data.getValue ( "/*/nsAddress" );
		if ( !managementAddress.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementUsername = nmxml_data.getValue ( "/*/nsUsername" );
		if ( !managementUsername.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementPassword = nmxml_data.getValue ( "/*/nsPassword" );
		/* if(!managementPassword.Length()) {
				*this << NMError(NMError::user, "No command");
				nmxml_data.freeTree();
				return;
			}
		*/
		addressPort = nmxml_data.getValue ( "/*/nsPort" ).toInt();


		NMString protocol = nmxml_data.getValue ( "/*/nsProtocol" );
		if ( !protocol.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}


		//sending to the GUI a message to know in what stage is the connection to the router

		NMXML xmlResponse;
		xmlResponse.newTree ( "data" );
		xmlResponse.addData ( "response", "STEP" );
		NMString responseData = xmlResponse.toString();
		xmlResponse.freeTree();
		*this << responseData; 		//sending to gui


		NMFtp::Error error;
		sftp = new NMFtp ( "sftp_first_if", 0 );

		if ( ( error=sftp->connectToHost ( managementAddress, managementUsername, managementPassword, addressPort ) ) ==0 ) 
		{
			xmlResponse.newTree ( "data" );
			xmlResponse.addData ( "response", "STEP" );
			NMString responseDataStep = xmlResponse.toString();
			xmlResponse.freeTree();
			*this << responseDataStep;

			response="ConnectionOK";

			managementProtocol = protocol=="IPv6"?"IPv6":"IPv4";
			if ( managementProtocol=="IPv4" ) sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv4 );
			else sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv6 );

			simulatorExternal = new NMExternalRemote ( "remote_simulator", true, this );
			( ( NMExternalRemote * ) simulatorExternal )->setHost ( sourceHost,addressPort,true );
			( ( NMExternalRemote * ) simulatorExternal )->setPassword ( managementPassword );
			( ( NMExternalRemote * ) simulatorExternal )->setUsername ( managementUsername );

			#ifdef DEBUG
				cerr << "DEBUG: Querying ifconfig" << endl;
			#endif
			*simulatorExternal << "ifconfig > " + simulator_file_1;
			
			//reading the number of the interfaces, and their manes, and putting them to the file .txt on the remote host
			( ( NMExternalRemote * ) simulatorExternal )->finished.connect ( this, &NMSimulator::getTm1 );
			( ( NMExternalRemote * ) simulatorExternal )->run(); //running the command

			#ifdef DEBUG
				cerr << "DEBUG: Querying ifconfig finished" << endl;
			#endif
		}
		else
		{
			response = "ConnectionNOK";
			NMXML xmlResponse;
			xmlResponse.newTree ( "data" );
			xmlResponse.addData ( "response", response );
			
			cout << RED;
			cout << "[ERROR] :: sftp -> ";

			if ( error == NMFtp::WrongPassword )		cout << "NMFtp::WrongPassword";
			else if ( error == NMFtp::Timeout )		cout << "NMFtp::Timeout";
			else if ( error == NMFtp::IsConnected )		cout <<	"NMFtp::IsConnected";
			else if ( error == NMFtp::Unknow )		cout <<	"NMFtp::Unknown";
			else						cout << "NMFtp::critical";

			cout << ENDCOLOR << endl;

			NMString responseData = xmlResponse.toString();
			xmlResponse.freeTree();
			*this << responseData; 
		} 
	} 
	/*************** End of First If ***********************************/


	/************* Second If ***********************************/

	// getting the interfaces for witch we will create root qdiscs
	if ( nms_xml->search ( "rootTc" ) !=-1 ) //getting a command
	{
		managementAddress = nmxml_data.getValue ( "/*/address" );
		if ( !managementAddress.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementUsername = nmxml_data.getValue ( "/*/nsUsername" );
		if ( !managementUsername.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementPassword = nmxml_data.getValue ( "/*/nsPassword" );
		/* if(!managementPassword.Length()) {
				*this << NMError(NMError::user, "No command");
				nmxml_data.freeTree();
				return;
			}
		*/
		addressPort = nmxml_data.getValue ( "/*/nsPort" ).toInt();


		NMString protocol = nmxml_data.getValue ( "/*/nsProtocol" );
		if ( !protocol.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementProtocol = protocol=="IPv6"?"IPv6":"IPv4";
		if ( managementProtocol=="IPv4" ) sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv4 );
		else sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv6 );


		if ( ( file = fopen ( simulator_script_2.c_str(), "w+" ) ) )  
		//putting the commands in a file to be send later to the router
		{
			fprintf ( file, "%s\n", "#!/bin/sh" );

			//cleanning the iptables mangle table
			cout<<"**************Cleaning iptables mangle table ************"<<endl;
			command="iptables -t mangle -F";

			line=command.c_str();
			fprintf ( file, "%s\n", line );
			cout<<command<<endl;
			cout<<"*********************************************************"<<endl;


			//getting the interfaces for witch we need to make root qdisc
			NMString interface;
			interfaceNr= nmxml_data.getValue ( "/*/rootTc/interfaceNr" ).toInt();
			for ( i=1;i<=interfaceNr;i++ )
			{
				sprintf ( interfTag,"/*/rootTc/clean%d",i );
				clean= nmxml_data.getValue ( interfTag );

				sprintf ( interfTag,"/*/rootTc/interface%d",i );
				interfaceParam= nmxml_data.getValue ( interfTag );

				if ( clean=="YES" )
				{
					command="tc qdisc del dev ";
					command=command+interfaceParam;
					command=command+" root";
					line=command.c_str();
					fprintf ( file, "%s\n", line );
					cout<<command<<endl;
				}

				command="tc qdisc add dev ";
				command=command+interfaceParam;
				command=command+" root handle 1: htb default 0";

				line=command.c_str();
				fprintf ( file, "%s\n", line );
				cout<<command<<endl;
			}

			response="rootOK";
			NMXML xmlResponse;
			xmlResponse.newTree ( "data" );
			xmlResponse.addData ( "response", response );
			xmlResponse.addData ( "nextCommand", "1" );

			NMString responseData = xmlResponse.toString();
			xmlResponse.freeTree();
			*this << responseData; //sending to gui

			fclose ( file ); //closing the file
		}
		else {  cout << RED <<"ERROR :: Can't open the file"<< ENDCOLOR << endl; }

	}//From the second if
	/************* Second If ***********************************/


	/****************** Third IF ********************************/
	//here i get each setting one in a time, and sent the request for the nezt setting
	if ( nms_xml->search ( "iptables" ) !=-1 ) //getting a command
	{
		managementAddress = nmxml_data.getValue ( "/*/address" );
		if ( !managementAddress.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementUsername = nmxml_data.getValue ( "/*/nsUsername" );
		if ( !managementUsername.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementPassword = nmxml_data.getValue ( "/*/nsPassword" );
		/*if(!managementPassword.Length()) {
				*this << NMError(NMError::user, "No command");
				nmxml_data.freeTree();
				return;
			}
		*/
		addressPort = nmxml_data.getValue ( "/*/nsPort" ).toInt();


		NMString protocol = nmxml_data.getValue ( "/*/nsProtocol" );
		if ( !protocol.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementProtocol = protocol=="IPv6"?"IPv6":"IPv4";
		if ( managementProtocol=="IPv4" ) sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv4 );
		else sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv6 );

		simulatorExternal = new NMExternalRemote ( "remote_simulator", true, this );
		( ( NMExternalRemote * ) simulatorExternal )->setHost ( sourceHost,addressPort,true );
		( ( NMExternalRemote * ) simulatorExternal )->setPassword ( managementPassword );
		( ( NMExternalRemote * ) simulatorExternal )->setUsername ( managementUsername );

		if ( ( file=fopen ( simulator_script_2.c_str() , "a" ) ) )  
		// appending the commands in a file to be send later to the router
		{

			/*************Getting the iptables commands****************/
			cout<<"----==== Iptables Command ====----"<<endl;
			NMString iptableParam;
			protocolNr= nmxml_data.getValue ( "/*/protocolNr" ).toInt();
			cout<<"--- We have "<<protocolNr<<" protocols for IPTABLES ---"<<endl;
			for ( i=1;i<=protocolNr;i++ )
			{
				command="iptables -t mangle -A PREROUTING";
				iptableParam= nmxml_data.getValue ( "/*/iptablesBegin" );
				command=command+iptableParam;

				sprintf ( protTag,"/*/protocol%d",i );
				iptableParam= nmxml_data.getValue ( protTag );
				command=command+iptableParam;

				iptableParam= nmxml_data.getValue ( "/*/iptablesEnd" );
				command=command+iptableParam;

				line=command.c_str();
				fprintf ( file, "%s\n", line );
				cout<<command<<endl;
			}
			cout<<"----==== **************** ====----"<<endl;
			/*************Ending the iptables commands****************/


			/****************Getting the htbChild command**************/

			interfaceNr= nmxml_data.getValue ( "/*/interfaces/interfNr" ).toInt();
			cout<<"--- For "<<interfaceNr<<" interfaces ---"<<endl;
			for ( i=1;i<=interfaceNr;i++ )
			{
				sprintf ( interfTag,"/*/interfaces/interf%d",i );
				interfaceParam= nmxml_data.getValue ( interfTag );
				NMString htbChildParam;
				command="tc class add dev ";
				command=command+interfaceParam;
				htbChildParam= nmxml_data.getValue ( "/*/htbChild" );
				command=command+htbChildParam;

				line=command.c_str();
				fprintf ( file, "%s\n", line );
				cout<<command<<endl;
			}
			cout << "----==== **************** ====----" << endl;

			/****************ENding the htbChild command**************/


			/****************Getting the netem commands**************/

			netemNr= nmxml_data.getValue ( "/*/netemNr" ).toInt();
			if ( netemNr>0 )  //verifying if we have netem settings
			{
				for ( i=1;i<=interfaceNr;i++ )
				{
					cout<<"--- For interface nr: "<<i<<" we have the next netem commands ---"<<endl;
					sprintf ( interfTag,"/*/interfaces/interf%d",i );
					interfaceParam= nmxml_data.getValue ( interfTag );
					for ( j=1;j<=netemNr;j++ )
					{
						sprintf ( netemTag,"/*/netem%d",j );
						NMString netemParam;
						netemParam= nmxml_data.getValue ( netemTag );
						command="tc qdisc add dev ";
						command=command+interfaceParam;

						command=command+netemParam;

						line=command.c_str();
						fprintf ( file, "%s\n", line );
						cout<<command<<endl;
					}
					cout<<"--- ************************ ---"<<endl;
				}

			}
			else { cout<<"---=== There are no NetEm settings ===---"<<endl; }

			/****************ENding the netem commands**************/


			/****************Getting the filter commands**************/
			cout<<"--- For "<<interfaceNr<<" interfaces we have next filters---"<<endl;
			for ( i=1;i<=interfaceNr;i++ )
			{
				sprintf ( interfTag,"/*/interfaces/interf%d",i );
				interfaceParam= nmxml_data.getValue ( interfTag );

				NMString filterParam;
				filterParam= nmxml_data.getValue ( "/*/filter" );
				command="tc filter add dev ";
				command=command+interfaceParam;

				command=command+filterParam;

				line=command.c_str();
				fprintf ( file, "%s\n", line );
				cout<<command<<endl;
			}
			cout<<"--- ************************ ---"<<endl;

			/****************ENding the filter commands**************/


			// sending the response
			NMString nextCommand;
			response="settingOK";
			nextCommand= nmxml_data.getValue ( "/*/nextCommandAvailable" );
			NMXML xmlResponse;
			xmlResponse.newTree ( "data" );
			xmlResponse.addData ( "response", response );
			xmlResponse.addData ( "nextCommand", nextCommand );

			NMString responseData = xmlResponse.toString();
			xmlResponse.freeTree();
			*this << responseData; //sending to gui

			if ( nextCommand=="NO" )
			{
				commandNumber=1;
				implementSettings();
			} 
			// if this was the last command containing settings, i will start implementing the settings;
			fclose ( file ); //closing the file
		}
		else {  cout<<"Can't open the file"<<endl; }
	}
	/****************** END Third IF ********************************/


	/****************** Forth IF ********************************/
	// checking on what interfaces i have tc settings
	
	if ( nms_xml->search ( "cleanInterf" ) !=-1 ) //getting a command
	{
		managementAddress = nmxml_data.getValue ( "/*/address" );
		if ( !managementAddress.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementUsername = nmxml_data.getValue ( "/*/nsUsername" );
		if ( !managementUsername.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		managementPassword = nmxml_data.getValue ( "/*/nsPassword" );
		/*if(!managementPassword.Length()) {
				*this << NMError(NMError::user, "No command");
				nmxml_data.freeTree();
				return;
			}
		*/
		addressPort = nmxml_data.getValue ( "/*/nsPort" ).toInt();


		NMString protocol = nmxml_data.getValue ( "/*/nsProtocol" );
		if ( !protocol.Length() )
		{
			*this << NMError ( NMError::user, "No command" );
			nmxml_data.freeTree();
			return;
		}

		// getting the interfaces for witch we have to verify if we have tc settings imlemented
		interfacesNumber= nmxml_data.getValue ( "/*/cleanInterf/interfaceNr" ).toInt();
		for ( i=1;i<=interfacesNumber;i++ )
		{
			sprintf ( interfTag,"/*/cleanInterf/interface%d",i );
			interfaceParam= nmxml_data.getValue ( interfTag );
			interfacesName[i]=interfaceParam;
			cout<<interfacesName[i]<<endl;
		}
		
		// sending to the GUI a message to know in what stage is the connection to the router
		NMXML xmlResponse;
		xmlResponse.newTree ( "data" );
		xmlResponse.addData ( "response", "STEP" );
		NMString responseData = xmlResponse.toString();
		xmlResponse.freeTree();
		*this << responseData; //sending to gui

		NMFtp::Error error;
		sftp = new NMFtp ( "sftp_forth_if",0 );

		if ( ( error=sftp->connectToHost ( managementAddress, managementUsername, managementPassword, addressPort ) ) ==0 ) 
		// connecting to the NS host
		{

			// another step was made in the connecting to the router process
			xmlResponse.newTree ( "data" );
			xmlResponse.addData ( "response", "STEP" );
			NMString responseDataStep = xmlResponse.toString();
			xmlResponse.freeTree();
			*this << responseDataStep; 

			response="CleanOK";

			managementProtocol = protocol=="IPv6"?"IPv6":"IPv4";
			if ( managementProtocol=="IPv4" ) sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv4 );
			else sourceHost = NMDns::resolv ( managementAddress,NMDns::IPv6 );

			simulatorExternal = new NMExternalRemote ( "remote_simulator", true, this );
			( ( NMExternalRemote * ) simulatorExternal )->setHost ( sourceHost,addressPort,true );
			( ( NMExternalRemote * ) simulatorExternal )->setPassword ( managementPassword );
			( ( NMExternalRemote * ) simulatorExternal )->setUsername ( managementUsername );


			*simulatorExternal << "tc -d qdisc show > " + simulator_file_2;
			( ( NMExternalRemote * ) simulatorExternal )->finished.connect ( this, &NMSimulator::getTm2 );
			( ( NMExternalRemote * ) simulatorExternal )->run(); 
		}
		else
		{
			response="CleanNOK";
			NMXML xmlResponse;
			xmlResponse.newTree ( "data" );
			xmlResponse.addData ( "response", response );


			NMString responseData = xmlResponse.toString();
			xmlResponse.freeTree();
			*this << responseData; 
		} 
		// if he can't connect to the destitation, the response will be NotOK
	}
	/********************* END Forth If *******************************/


}
/*********** Ending of the start function ********************/



void NMSimulator::getTm1()
{
	cout << "********************************************" << endl;
	cout << "*********** NMSimulator::getTm1() **********" << endl;
	cout << "********************************************" << endl << endl;


	NMXML xmlResponse;
	xmlResponse.newTree ( "data" );
	xmlResponse.addData ( "response", "STEP" );
	NMString responseData = xmlResponse.toString();
	xmlResponse.freeTree();
	*this << responseData;        	
	
	sftp->commandFinished.connect ( this, &NMSimulator::gettingDataFromTm1 );
	sftp->get( simulator_file_1 + " " + simulator_file_1 );
	sftp->close();

	return;
}

void NMSimulator::getTm2()
{
	cout << "********************************************" << endl;
	cout << "*********** NMSimulator::getTm2() **********" << endl;
	cout << "********************************************" << endl << endl;

	NMXML xmlResponse;
	xmlResponse.newTree ( "data" );
	xmlResponse.addData ( "response", "STEP" );
	NMString responseData = xmlResponse.toString();
	xmlResponse.freeTree();
	*this << responseData;       

	sftp->commandFinished.connect ( this, &NMSimulator::gettingDataFromTm2 );
	sftp->get( simulator_file_2 + " " + simulator_file_2 );
	sftp->close();

	return;
}

void NMSimulator::gettingDataFromTm1()
{
	cout << "********************************************" << endl;
	cout << "**** NMSimulator::gettingDataFromTm1() *****" << endl;
	cout << "********************************************" << endl << endl;
	
	int nrInterf=0;
	NMXML interfaceResponse;
	NMString nmLine, interfaceName, interfaceTag, interfaceNumber;
	char line[50], *word, *interface, tag[20];
	FILE *file;

	word= ( char * ) "HWaddr";
	file=fopen ( simulator_file_1.c_str(), "r" );
	if ( file!=0 )
	{
		cout << "File " + simulator_file_1 + " opened"<<endl;
		interfaceResponse.newTree ( "data" );
		fgets ( line, 50, file );

		while ( feof ( file ) ==0 )
		{
			//getting the interfaces names and their number:
			nmLine=line;

			if ( nmLine.search ( word ) !=-1 )
			{
				nrInterf++;
				interface=strtok ( line," " );
				sprintf ( tag,"interface%d",nrInterf );
				interfaceName=interface;
				interfaceTag=tag;
				cout << interfaceTag<<"  "<< interfaceName<<endl;
				interfaceResponse.addData ( interfaceTag, interfaceName );
			}

			fgets ( line, 50, file );
		}
		fclose ( file );
		cout << "File " + simulator_file_1 + " closed"<<endl;
		cout << GREEN << "[OK] Interfaces successfully collected" << ENDCOLOR << endl;
	}
	else { cout << RED << "[ERROR] Can't open the file /tmp/simulator_data_1.txt" << ENDCOLOR << endl; }

	sprintf ( tag,"%d",nrInterf );
	interfaceNumber=tag;
	interfaceResponse.addData ( "nrInterf", interfaceNumber );
	interfaceResponse.addData ( "response", response );
	NMString interfacesString = interfaceResponse.toString();
	interfaceResponse.freeTree();

	*this << interfacesString; //sending to gui

	cout << interfacesString << endl;
	
	/***** EXIT NMSIMULATOR ******/
	sleep(1);	
	exit();
}


void NMSimulator::gettingDataFromTm2()
{
	cout << "********************************************" << endl;
	cout << "**** NMSimulator::gettingDataFromTm2() *****" << endl;
	cout << "********************************************" << endl << endl;

	int i;
	NMXML cleanResponse;
	NMString nmTag;
	char tag[20];

	cleanResponse.newTree ( "data" );

	for ( i=1; i<=interfacesNumber; i++ )
	{
		if ( checkTC ( interfacesName[i].c_str() ) ==1 )
		{
			sprintf ( tag,"clean%d",i );
			nmTag=tag;
			cleanResponse.addData ( nmTag, "YES" );
		}
		else
		{
			sprintf ( tag,"clean%d",i );
			nmTag=tag;
			cleanResponse.addData ( nmTag, "NO" );
		}
	}

	cleanResponse.addData ( "response", response );
	NMString interfacesString = cleanResponse.toString();
	cout <<interfacesString<<endl;
	cleanResponse.freeTree();
	*this << interfacesString; 
}



int NMSimulator::checkTC ( char *interface )
{
	int set=1;
	char line[200];
	NMString nmLine, word;
	FILE *file;

//checking if there are tc settings for the given interface

	file = fopen ( simulator_file_2.c_str() , "r" );
	if ( file!=0 )
	{
		fgets ( line, 200, file );

		while ( feof ( file ) ==0 )
		{
			//getting the interfaces names and their number
			nmLine=line;
			if ( nmLine.search ( interface ) !=-1 )
			{
				word="qdisc pfifo_fast";
				if ( nmLine.search ( word ) !=-1 )
				{
					word="bands 3 priomap  1 2 2 2 1 2 0 0 1 1 1 1 1 1 1 1";
					if ( nmLine.search ( word ) !=-1 )
					{
						set=0;
					}
					else
					{
						set=1; //there are tc settings on the interface
						fclose ( file );
						return set;
					}

				}
				else
				{
					set=1; //there are tc settings on the interface
					fclose ( file );
					return set;
				}
			}

			fgets ( line, 200, file );
		}//from while
		fclose ( file );
	}
	else { cout << "Can not open the file" + simulator_file_2 << endl; }

	return set;
}


void NMSimulator::implementSettings()
{
	NMString nmLine, nmBorder;
	NMXML xmlResponse;

	simulatorExternal = new NMExternalRemote ( "remote_simulator", true, this );
	( ( NMExternalRemote * ) simulatorExternal )->setHost ( sourceHost,addressPort,true );
	( ( NMExternalRemote * ) simulatorExternal )->setPassword ( managementPassword );
	( ( NMExternalRemote * ) simulatorExternal )->setUsername ( managementUsername );

	if ( commandNumber==1 )  // sending the script file to the host
	{
		cout << endl;	
		cout << GREEN << "[OK] STEP 1: script file sent " << ENDCOLOR << endl;

		NMFtp::Error error;
		sftp = new NMFtp ( "sftp_implementSettings", 0 );

		xmlResponse.newTree ( "data" );
		xmlResponse.addData ( "response", "STEP" );
		NMString responseData = xmlResponse.toString();
		xmlResponse.freeTree();
		*this << responseData; 	

		commandNumber++;

		if ( ( error=sftp->connectToHost ( managementAddress, managementUsername, managementPassword, addressPort ) ) == 0 ) 
		{
			sftp->commandFinished.connect ( this, &NMSimulator::implementSettings );
			sftp->put( simulator_script_2 + " " + simulator_script_2 );
			sftp->close();
		}
		else 
		{
			cout << RED << "[ERROR] sftp -> ";

			if ( error == NMFtp::WrongPassword )		cout << "NMFtp::WrongPassword";
			else if ( error == NMFtp::Timeout )		cout << "NMFtp::Timeout";
			else if ( error == NMFtp::IsConnected )		cout <<	"NMFtp::IsConnected";
			else if ( error == NMFtp::Unknow )		cout <<	"NMFtp::Unknown";
			else						cout << "NMFtp::critical";

			cout << ENDCOLOR << endl;
		}
	}

	else if ( commandNumber == 2 ) // running the script file = 2 steps
	{
		cout << endl;
		cout << GREEN << "[OK] STEP 2: running the script file " << ENDCOLOR << endl;

		xmlResponse.newTree ( "data" );
		xmlResponse.addData ( "response", "STEP" );
		NMString responseData = xmlResponse.toString();
		xmlResponse.freeTree();
		*this << responseData;
		*this << responseData;

		commandNumber++;

		*simulatorExternal << "sh " + simulator_script_2;
		( ( NMExternalRemote * ) simulatorExternal )->finished.connect ( this, &NMSimulator::implementSettings );
		( ( NMExternalRemote * ) simulatorExternal )->run(); 
	}

	else if ( commandNumber == 3 )  // settings were implemented
	{
		cout << endl;
		
		cout << GREEN << "[OK] STEP 3: NMSimulator finished... exiting " << ENDCOLOR << endl;

		xmlResponse.newTree ( "data" );
		xmlResponse.addData ( "response", "STEPSTOP" );
		NMString responseData = xmlResponse.toString();
		xmlResponse.freeTree();
		*this << responseData; 
	}
}


void NMSimulator::configure ( void *data )
{
	cout << RED <<  "[ERROR] NMSimulator::configure isn't defined" << ENDCOLOR << endl;
}


/**
 * Stop command
 * @param
 */
void NMSimulator::stop ( void *data )
{
	exit();
}

/**
 * Status command, for getting the status, for the status bar
 * @param
 */
void NMSimulator::status ( void *data )
{
	cout << RED << "[ERROR] NMSimulator::status isn't defined" << ENDCOLOR << endl;
}


void NMSimulator::processRecived()
{
	cout << RED;
	cout << "[ERROR] NMSimulator::processRecived isn't defined" << ENDCOLOR << endl;
}

void NMSimulator::processExit()
{
	cout << RED;
	cout << "[ERROR] NMSimulator::processExit isn't defined" << ENDCOLOR << endl;
}
