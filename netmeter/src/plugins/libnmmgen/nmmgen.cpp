/*
 *   $Id: nmmgen.cpp,v 1.28 2007-05-27 11:19:34 rserral Exp $
 *
 *   Copyright (c) 2006 Ernest Nieto
 *   Copyright (c) 2007 Roberto Barreda
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "nmmgen.h"

#include <nmmodulemanager.h>
#include <ctime>
#include <iostream>
using namespace std;

#define DECODE_OUTPUT_FILE_PREFIX	"/tmp/decodeoutput"
#define DECODE_OUTPUT_FILE_EXTENSION	".dly"
#define DREC_OUTPUT_FILE_PREFIX		"/tmp/drecoutput"
#define LOCAL_SUFFIX			".local"

NMMODULE_CLASS ( NMMGen );
NMMODULE_NAME ( ( char * ) "mgen" );
NMMODULE_CAPABILITIES ( ( char * ) "test" );
NMMODULE_VERSION ( ( char * ) "0.1" );

/**
 * Constructor of the class
 * @return
 */
NMMGen::NMMGen()
		: interpret (this), extMgen ( this, this, this, this, this ), extDrec ( this, this, this, this, this, this ), extDecode ( this, this, this, this )
{
	sync = false;
	testStatus = NotStarted;
	testFileSuffix = "";
	/*	
		extDrec.getReadyToRunEvent().connect(this,&NMMGen::readyToRunDrec);
		extDrec.getRunningEvent().connect(this,&NMMGen::drecRunning);
		extMgen.getReadyToRunMgenEvent().connect(this,&NMMGen::readyToRunMgen);
		extMgen.getFinishedEvent().connect(this,&NMMGen::finishedMgen);
		extDrec.getDrecKilledEvent().connect(this,&NMMGen::finishedDrec);
		extDecode.getFinishedEvent().connect(this,&NMMGen::decodeFinished);
	*/
}

/**
 * Destroyer of the class
 * @return
 */
NMMGen::~NMMGen()
{
}

/**
 * Initializes the module
 */
void NMMGen::initModule()
{

}

/**
 * Executed when the module is removed
 */
void NMMGen::removeModule()
{

}

/**
 * Start command
 * @param data
 */
void NMMGen::start ( void *data )
{
#ifdef DEBUG
	cout << "********************************************" << endl;
	cout << "NMMGen::start" << endl;
	cout << "********************************************" << endl;
#endif

	testFileSuffix = getFilesSuffix();
	if(!interpret.parseStartXML ( ( NMString * ) data, testFileSuffix ))	return; 

	// 0) The message is a proper mgen msg (check above):

	decodeOutputFile = DECODE_OUTPUT_FILE_PREFIX + testFileSuffix + DECODE_OUTPUT_FILE_EXTENSION;
	drecOutputFile = DREC_OUTPUT_FILE_PREFIX + testFileSuffix;
	mgenStarted = false;
	
	// 1) check if sync data is needed

	if(interpret.getInputFields().mgen_sync == 1)
	{
		cout << "NMMGen::sync data will be collected" << endl;
		sync = true;
	}

	// 2) check if all the data is ok, if not send error

	if ( not extDrec.configureDrec ( interpret.getInputFields().destDrecPath,
	                                 interpret.getInputFields().destDecodePath,
	                                 interpret.getInputFields().pathDrecScript,
	                                 drecOutputFile,
	                                 interpret.getInputFields().destManagementAddress,
	                                 interpret.getInputFields().destManagementAddressPort,
	                                 interpret.getInputFields().destManagementUsername,
	                                 interpret.getInputFields().destManagementPassword,
	                                 interpret.getInputFields().destManagementProtocol,
	                                 interpret.getInputFields().testMgenVersion ) )
	{
		#ifdef DEBUG
			cerr << RED;
			cerr << "ERROR :: while configuring Drec" << endl;
			cerr << ENDCOLOR;
		#endif
		exit();
	}
}

/**
 * Executed when drec is ready to run (files to remote stations has been sent)
 */
void NMMGen::readyToRunDrec()
{
	#ifdef DEBUG
		cout << "NMMGen::readyToRunDrec" << endl;
	#endif
	
	if ( not extDrec.runDrec() )
	{
		#ifdef DEBUG
			cerr << RED;
			cerr << "Error running Drec" << endl;
			cerr << ENDCOLOR;
		#endif
		exit();
		return;
	}
	else
	{
		testStatus = DrecStarted;
		#ifdef DEBUG
			cout << "NMMGen::Drec running ..." << endl;
		#endif
	}
}

/**
 * Executed when drec is running, so time to run mgen
 */
void NMMGen::drecRunning()
{
	#ifdef DEBUG
		cout << "NMMGen::drecRunning" << endl;
	#endif
	if ( mgenStarted ) return;
	mgenStarted = true;

	if ( !extMgen.configureMgen ( interpret.getInputFields().sourceMgenPath,
	                              interpret.getInputFields().pathMgenScript,
	                              interpret.getInputFields().sourceManagementAddress,
	                              interpret.getInputFields().sourceManagementAddressPort,
	                              interpret.getInputFields().sourceManagementUsername,
	                              interpret.getInputFields().sourceManagementPassword,
	                              interpret.getInputFields().sourceManagementProtocol,
	                              interpret.getInputFields().testMgenVersion ) )
	{
	#ifdef DEBUG
		cout << "NMMGen::Error configuring Mgen..." << endl;
	#endif
		exit();
	}
}

/**
 * Mgen is ready to run
 */
void NMMGen::readyToRunMgen()
{
	#ifdef DEBUG
		cout << "NMMGen::readyToRunMgen" << endl;
	#endif


	if ( not extMgen.runMgen() )
	{
		#ifdef DEBUG
			cerr << RED;
			cerr << "ERROR :: running Mgen" << endl;
			cerr << ENDCOLOR;
		#endif
		exit();
	}
	else
	{
		#ifdef DEBUG
			cout << "NMMGen::Mgen running..." << endl;
		#endif
		testStatus = MgenStarted;
	}

}

/**
 * Executed when the mgen finishes the execution: so time to run decode at drec output file
 */
void NMMGen::finishedMgen()
{
	#ifdef DEBUG
		cout << "NMMGen::finishedMgen" << endl;
	#endif
	testStatus = MgenFinished;

	#ifdef DEBUG
		cout << "NMMGen::Drec waiting 5 seconds for mgen packets" << endl;
	#endif
	sleep ( 5 );
	extDrec.killDrec();
}

/**
 * Executed when the drec process has been killed
 */
void NMMGen::finishedDrec()
{
	#ifdef DEBUG
		cout << "NMMGen::finishedDrec" << endl;
	#endif

	extDecode.configureDecode ( interpret.getInputFields().destDecodePath,
	                            drecOutputFile,
	                            decodeOutputFile,
	                            interpret.getInputFields().destManagementAddress,
	                            interpret.getInputFields().destManagementAddressPort,
	                            interpret.getInputFields().destManagementUsername,
	                            interpret.getInputFields().destManagementPassword,
	                            interpret.getInputFields().destManagementProtocol,
	                            interpret.getInputFields().testMgenVersion );

	if ( not extDecode.startDecode() )
	{
	#ifdef DEBUG
		cerr << RED;
		cerr << "NMMGen::Error running Decode" << endl;
		cerr << ENDCOLOR;
	#endif
		exit();
	}
	else
	{
	#ifdef DEBUG
		cout << "NMMGen::Decode running..." << endl;
	#endif
	}
}

/**
 * Execute when the decode process has finished
 */
void NMMGen::decodeFinished()
{
	#ifdef DEBUG
		cout << "NMMGen::decodeFinished" << endl;
	#endif

	if ( interpret.getInputFields().destManagementAddress != "127.0.0.1" &&
	        interpret.getInputFields().destManagementAddress != "::1" )
		decodeOutputFile += LOCAL_SUFFIX;

	testFile = interpret.addResultToTestFile ( decodeOutputFile );
	*this << interpret.getXMLResult ( testFile,decodeOutputFile,fileGraphOWD,fileGraphOWDD,fileGraphIPDV,fileGraphIPDVD );

	if(sync)
	{
		cout << "NMMGen :: Waiting 5 secs for sync plugin to finish..." << endl;
		sleep(5);
	}
	exit();
}

/**
 * Stop command
 * @param
 */
void NMMGen::stop ( void * )
{
	if(sync)	return;		// We ignore stops if we're collecting sync data

	switch ( testStatus )
	{
		case NotStarted:
			break;
		case SendingDrecScriptFile:
			break;
		case DrecStarted:
			extDrec.killDrec();
			break;
		case SendingMgenScriptFile:
			extDrec.killDrec();
			break;
		case MgenStarted:
			extDrec.killDrec();
			extMgen.killMgen();
			break;
		case MgenFinished:
			extDrec.killDrec();
			break;
		case DecodeStarted:
			extDecode.killDecode();
			break;
		case DecodeFinished:
			break;
		case ReceivingDecodeResultFile:
			break;
		case TestFinished:
			break;
		case TestError:
			break;
		default:
			cout << RED <<"NMMGen:: [panic] testStatus with unknown value!" << ENDCOLOR << endl;
			break;
	}
	if (!sync)	exit();
}

void NMMGen::configure ( void * )
{
	*this << NMError ( NMError::warning, "Configure obsolete" );
}


/**
 * Status command
 * @param
 */
void NMMGen::status ( void * )
{
	NMXML xmldata;
	NMStatus status;

	xmldata.newTree ( "data" );
	xmldata.addData ( "status", testStatus );

	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout <<  xmldata.toString() << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	
	status.setMessage ( xmldata.toString() );
	xmldata.freeTree();

	*this << status;
}

/**
 * Notifies the error
 * @param err
 */
void NMMGen::notifyError ( NMError err )
{
	if ( err.getType() ==NMError::fatal or err.getType() ==NMError::user ) testStatus = TestError;
	*this<<err;
}

/**
 * Sets the status of the test
 * @param status
 */
void NMMGen::setStatus ( MgenStatus newStatus )
{
	testStatus = newStatus;
	status ( 0 );
	sleep ( 2 );
}

/**
 * Returns the suffixs of the files
 * @return
 */
NMString NMMGen::getFilesSuffix()
{
	return "_" + getCurrentTimeSuffix() + "_" + NMString::random().subString ( 2,4 );
}

/**
 * Returns mgen's version in use
 * @return
 */
int NMMGen::getMgenVersion()
{
	return interpret.getInputFields().testMgenVersion;
}

/**
 * Returns the current time suffix with the format yyyymmdd_hhmmss
 * @return
 */
NMString NMMGen::getCurrentTimeSuffix()
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
	if ( ( localTime.tm_mon+1 ) <10 ) res += "0";
	res += NMString::number ( localTime.tm_mon+1 );
	if ( localTime.tm_mday<10 ) res += "0";
	res += NMString::number ( localTime.tm_mday );
	res += "_";
	if ( ( localTime.tm_hour ) <10 ) res += "0";
	res += NMString::number ( localTime.tm_hour );
	if ( ( localTime.tm_min ) <10 ) res += "0";
	res += NMString::number ( localTime.tm_min );
	if ( ( localTime.tm_sec ) <10 ) res += "0";
	res += NMString::number ( localTime.tm_sec );

	return res;
}


