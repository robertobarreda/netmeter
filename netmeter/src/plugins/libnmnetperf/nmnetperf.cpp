/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda                                 *
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
#include "nmnetperf.h"

#include <ctime>

#define NETPERF_OUTPUT_FILE_EXTENSION	".npf"
#define NETPERF_OUTPUT_FILE_PREFIX	"/tmp/netperfoutput"

NMMODULE_CLASS ( NMNetperf );
NMMODULE_NAME ( ( char * ) "netperf" );
NMMODULE_CAPABILITIES ( ( char * ) "test" );
NMMODULE_VERSION ( ( char * ) "0.1" );

NMNetperf::NMNetperf()
		: extNetperf ( this, this, this, this ),
		extNetserver ( this, this, this, this, this ),
		extDecode ( this, this, this, this )
{
	testStatus = NotStarted;
	netperfStarted = netserverStarted = false;
	testFileSuffix = netserverOutputFile = "";
}

void NMNetperf::start ( void *data )
{
#ifdef DEBUG
	cout << "* NMNetperf::start" << endl;
#endif
	testFileSuffix = getFilesSuffix();

	if ( not interpret.parseStartXML ( ( NMString * ) data, testFileSuffix ) )
	{
		//check if all the data is ok, if not send error
		*this << NMError ( NMError::fatal, XML_CONFIG_ERROR );
		exit();
		return;
	}

	if ( not extNetserver.configureAndRunNetserver ( interpret.getInputFields() ) )
	{
#ifdef DEBUG
		cerr << RED << "[ERROR] Configuring Netserver" << ENDCOLOR << endl;
#endif
		*this << NMError ( NMError::fatal, NETSERVER_ERROR_UNKNOWN );
		exit();
		return;
	}
	netserverStarted = true;
}

void NMNetperf::netserverRunning()
{
#ifdef DEBUG
	cout << "* NMNetperf::netserverRunning" << endl;
#endif
	netserverOutputFile = NETPERF_OUTPUT_FILE_PREFIX + testFileSuffix + NETPERF_OUTPUT_FILE_EXTENSION;

	if ( !extNetperf.configureAndRunNetperf ( interpret.getInputFields(), netserverOutputFile ) )
	{
#ifdef DEBUG
		cerr << RED << "[ERROR] Configuring Netperf" << ENDCOLOR << endl;
#endif
		*this << NMError ( NMError::fatal, NETPERF_ERROR_UNKNOWN );
		exit();
		return;
	}
	netperfStarted = true;
}

void NMNetperf::finishedNetperf()
{
#ifdef DEBUG
	cout << "* NMNetperf::finishedNetperf" << endl;
#endif
	netperfStarted = false;

	if ( !extDecode.configureAndRunDecode ( interpret.getInputFields(), netserverOutputFile ) )
	{
#ifdef DEBUG
		cerr << RED << "[ERROR] Configuring Decode" << ENDCOLOR << endl;
#endif
		exit();
		return;
	}
}

void NMNetperf::finishedNetserver()
{}

void NMNetperf::decodeFinished()
{
#ifdef DEBUG
	cout << "* NMNetperf::decodeFinished" << endl;
#endif

	if ( interpret.getInputFields()->destManagementAddress != "127.0.0.1" &&
	        interpret.getInputFields()->destManagementAddress != "::1" )
		netserverOutputFile += LOCAL_SUFFIX;

	NMString testFile = interpret.addResultToTestFile ( netserverOutputFile );
	if ( testFile != 0 )
		*this << interpret.getXMLResult ( testFile, netserverOutputFile );
	else
	{
#ifdef DEBUG
		cerr << RED << "[ERROR] Could not be established control connection" << ENDCOLOR << endl;
#endif
		*this << NMError ( NMError::fatal, XML_OUTPUT_ERROR );
	}

	if ( extNetserver.killNetserver() )
		cout << "NMNetperf::finishedNetperf --> netserver has been killed successfully!" << endl;
	else
		cout << "NMNetperf::finishedNetperf --> netserver has NOT been killed!" << endl;

	exit();
}

void NMNetperf::stop ( void * )
{
#ifdef DEBUG
	cout << "* NMNetperf::stop status(" << testStatus << ")" << endl;
#endif
	switch ( testStatus )
	{
		case NotStarted:
			break;
		case SendingNetserverScriptFile:
			break;
		case NetserverStarted:
			extNetserver.killNetserver();
			break;
		case NetserverFinished:
			finishedNetserver();
			break;
		case SendingNetperfScriptFile:
			break;
		case NetperfStarted:
			extNetperf.killNetperf();
			extNetserver.killNetserver();
			break;
		case NetperfFinished:
			extNetserver.killNetserver();
			break;
		case DecodeStarted:
			break;
		case ReceivingDecodeResultFile:
			break;
		case TestFinished:
			decodeFinished();
			break;
		case TestError:
			if ( netperfStarted ) extNetperf.killNetperf();
			if ( netserverStarted ) extNetserver.killNetserver();
			break;
		default:
			cout << "[panic] testStatus with unknown value!" << endl;
			break;
	}
	exit();
}

void NMNetperf::status ( void * )
{
	NMXML xmldata;
	NMStatus status;

	xmldata.newTree ( "data" );
	xmldata.addData ( "status", testStatus );

	status.setMessage ( xmldata.toString() );
	xmldata.freeTree();

	*this << status;
}

void NMNetperf::configure ( void * )
{
	*this << NMError ( NMError::warning, "Configure obsolete" );
}

void NMNetperf::notifyError ( NMError err )
{
#ifdef DEBUG
	cerr << RED << ">> NMNetperf::notifyError #" << err << ENDCOLOR << endl;
#endif
	if ( err.getType() == NMError::fatal or err.getType() == NMError::user ) testStatus = TestError;
	*this << err;
	status ( 0 );
}

void NMNetperf::setStatus ( NetperfStatus newStatus )
{
	testStatus = newStatus;
	status ( 0 );
	sleep ( 1 );
}

NMString NMNetperf::getCurrentTimeSuffix()
{
	time_t currentTime;
	tm localTime;
	time ( &currentTime );
	localTime = * ( localtime ( &currentTime ) );

//   int tm_sec;                   /* Seconds.     [0-60] (1 leap second) */
//   int tm_min;                   /* Minutes.     [0-59] */
//   int tm_hour;                  /* Hours.       [0-23] */
//   int tm_mday;                  /* Day.         [1-31] */
//   int tm_mon;                   /* Month.       [0-11] */
//   int tm_year;                  /* Year - 1900.  */
//   int tm_wday;                  /* Day of week. [0-6] */
//   int tm_yday;                  /* Days in year.[0-365] */
//   int tm_isdst;                 /* DST.         [-1/0/1]*/

	NMString res = NMString::number ( localTime.tm_year + 1900 );
	if ( ( localTime.tm_mon + 1 ) < 10 ) res += "0";
	res += NMString::number ( localTime.tm_mon + 1 );
	if ( localTime.tm_mday < 10 ) res += "0";
	res += NMString::number ( localTime.tm_mday );
	res += "_";
	if ( ( localTime.tm_hour ) < 10 ) res += "0";
	res += NMString::number ( localTime.tm_hour );
	if ( ( localTime.tm_min ) < 10 ) res += "0";
	res += NMString::number ( localTime.tm_min );
	if ( ( localTime.tm_sec ) < 10 ) res += "0";
	res += NMString::number ( localTime.tm_sec );

	return res;
}
