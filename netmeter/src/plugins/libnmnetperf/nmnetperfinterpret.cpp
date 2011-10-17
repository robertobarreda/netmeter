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
#include "nmnetperfinterpret.h"
#include <nmxml.h>
#include <nmxmlstreamwriter.h>

#include <fstream>
using namespace std;

#define DEBUG 1
#define TEST_FILE_PREFFIX "test_npf"

NMNetperfInterpret::NMNetperfInterpret()
{
	cleanInputFields();
	testFileContent = metadataContent = testFileName = "";
}

NMString NMNetperfInterpret::getXMLResult ( const NMString & testFile, const NMString & decodeFile )
{
#ifdef DEBUG
	cout << "--- NMNetperfInterpret::getXMLResult" << endl;
#endif
	NMXML xmlResult;
	xmlResult.newTree ( "data" );
	xmlResult.addData ( "test_file", testFile );
	xmlResult.addData ( "decode_file", decodeFile );

	NMString xmlResultStr = xmlResult.toString();
	xmlResult.freeTree();
	return xmlResultStr;
}

bool NMNetperfInterpret::parseStartXML ( NMString *xml, NMString  filesSuffixPath )
{
#ifdef DEBUG
	cout << "--- NMNetperfInterpret::parseStartXML" << endl;
#endif
	if ( !xml ) return false;
	cleanInputFields();
	NMXML nmxmlData;
	nmxmlData.loadTree ( xml->c_str() );

	testFileContent = nmxmlData.getTree ( "/*/test", "test" );
	metadataContent = nmxmlData.getTree ( "/*/test/metadata", "metadata" );

	if ( metadataContent.IsEmpty() ) return false;

	NMString resultPath = nmxmlData.getValue ( "/*/test/metadata/result_path" ) != "" ?
	                      nmxmlData.getValue ( "/*/test/metadata/result_path" ) : "/tmp/";
	testFileName = resultPath + TEST_FILE_PREFFIX + filesSuffixPath + ".xff";

	inputFields.testName = nmxmlData.getValue ( "/*/test/metadata/name" );
	inputFields.testIdentifier = nmxmlData.getValue ( "/*/test/metadata/identifier" );

	// Source
	inputFields.sourceAddress = nmxmlData.getValue ( "/*/test/metadata/source/address" );
	inputFields.sourceNetperfPath = nmxmlData.getValue ( "/*/test/metadata/source/netperf_path" );
	inputFields.sourceManagementAddress = nmxmlData.getValue ( "/*/test/metadata/source/management/address" );
	inputFields.sourceManagementAddressPort = nmxmlData.getValue ( "/*/test/metadata/source/management/port" ).toInt();
	inputFields.sourceManagementUsername = nmxmlData.getValue ( "/*/test/metadata/source/management/username" );
	inputFields.sourceManagementPassword = nmxmlData.getValue ( "/*/test/metadata/source/management/password" );

	inputFields.port = nmxmlData.getValue ( "/*/test/metadata/conf_netperf/port" ).toInt();
	inputFields.duration = nmxmlData.getValue ( "/*/test/metadata/conf_netperf/duration" ).toInt();
	inputFields.durationInSeconds = ( nmxmlData.getValue ( "/*/test/metadata/conf_netperf/duration/@type" ) == "seconds" );
	if ( !inputFields.durationInSeconds ) inputFields.duration = -inputFields.duration;
	inputFields.udpProtocol = ( nmxmlData.getValue ( "/*/test/metadata/conf_netperf/proto" ) == "udp" );
	inputFields.noDelayOn = ( nmxmlData.getValue ( "/*/test/metadata/conf_netperf/proto/@nodelay" ) == "yes" );
	if ( nmxmlData.nodeExists ( "/*/test/metadata/conf_netperf/sss" ) )
		inputFields.sss = nmxmlData.getValue ( "/*/test/metadata/conf_netperf/sss" ).toInt();
	if ( nmxmlData.nodeExists ( "/*/test/metadata/conf_netperf/sms" ) )
		inputFields.sms = nmxmlData.getValue ( "/*/test/metadata/conf_netperf/sms" ).toInt();
	if ( nmxmlData.nodeExists ( "/*/test/metadata/conf_netperf/rss" ) )
		inputFields.rss = nmxmlData.getValue ( "/*/test/metadata/conf_netperf/rss" ).toInt();
	if ( nmxmlData.nodeExists ( "/*/test/metadata/conf_netperf/rms" ) )
		inputFields.rms = nmxmlData.getValue ( "/*/test/metadata/conf_netperf/rms" ).toInt();

	// Destination
	inputFields.destAddress = nmxmlData.getValue ( "/*/test/metadata/destination/address" );
	inputFields.destNetserverPath = nmxmlData.getValue ( "/*/test/metadata/destination/netserver_path" );
	inputFields.destManagementAddress = nmxmlData.getValue ( "/*/test/metadata/destination/management/address" );
	inputFields.destManagementAddressPort = nmxmlData.getValue ( "/*/test/metadata/destination/management/port" ).toInt();
	inputFields.destManagementUsername = nmxmlData.getValue ( "/*/test/metadata/destination/management/username" );
	inputFields.destManagementPassword = nmxmlData.getValue ( "/*/test/metadata/destination/management/password" );

	inputFields.ipVersion = -nmxmlData.getValue ( "/*/test/metadata/conf_netserver/ipv" ).toInt();

	nmxmlData.freeTree();

	return true;
}

NMString NMNetperfInterpret::addResultToTestFile ( const NMString & decodeOutputFile )
{
#ifdef DEBUG
	cout << "--- NMNetperfInterpret::addResultToTestFile of " << decodeOutputFile << endl;
#endif

	NMXML xmlResult;
	int count = 0;

	ifstream inputFile;
	inputFile.open ( decodeOutputFile.c_str(), fstream::in );

#define BUFF_SIZE	2048
#define BUFF_MINI_SIZE	50

	char * str;
	NMString strLine;
	char line[BUFF_SIZE];
	char sss[BUFF_MINI_SIZE], sms[BUFF_MINI_SIZE], rss[BUFF_MINI_SIZE], time[BUFF_MINI_SIZE], throughput[BUFF_MINI_SIZE];
	char send_mesg_ok[BUFF_MINI_SIZE], send_mesg_ko[BUFF_MINI_SIZE], send_throughput[BUFF_MINI_SIZE], recv_mesg_ok[BUFF_MINI_SIZE], recv_throughput[BUFF_MINI_SIZE];

	if ( inputFile.is_open() )
	{
		while ( inputFile.getline ( line, BUFF_SIZE ) )
		{
			strLine = line;
			strLine = strLine.trim();
			str = strLine.c_str();
			if ( strLine.Length() > 0 )
			{
				if ( strLine.search ( "establish control" ) >= 0 )
					return 0;

				// Result from TCP test
				if ( !inputFields.udpProtocol )
					sscanf ( str, "%s%s%s%s%s", rss, sss, sms, time, throughput );

				// Result from UDP test
				else if ( inputFields.udpProtocol && ( count == 0 ) )
				{
					sscanf ( str, "%s%s%s%s%s%s",
					         sss, sms, time, send_mesg_ok, send_mesg_ko, send_throughput );
					++count;
				}
				else if ( inputFields.udpProtocol && ( count == 1 ) )
				{
					sscanf ( str, "%s%*s%s%s", rss, recv_mesg_ok, recv_throughput );
					++count;
				}
			}
		}

		inputFile.close();

		/*
		// Using NMXML (DOM)
		if ( !inputFields.udpProtocol )
		{
			xmlResult.newTree ( "tcp" );
			xmlResult.addData ( "source", inputFields.sourceAddress );
			xmlResult.addData ( "destination", inputFields.destAddress );

			xmlResult.addData ( "rss", rss );
			xmlResult.addData ( "sss", sss );
			xmlResult.addData ( "sms", sms );
			xmlResult.addData ( "time", time );
			xmlResult.addData ( "throughput", throughput );
		}
		else
		{
			xmlResult.newTree ( "udp" );
			xmlResult.addData ( "source", inputFields.sourceAddress );
			xmlResult.addData ( "destination", inputFields.destAddress );

			xmlResult.addData ( "sss", sss );
			xmlResult.addData ( "sms", sms );
			xmlResult.addData ( "time", time );
			xmlResult.addData ( "send_mesg_ok", send_mesg_ok );
			xmlResult.addData ( "send_mesg_ko", send_mesg_ko );
			xmlResult.addData ( "send_throughput", send_throughput );
			xmlResult.addData ( "rss", rss );
			xmlResult.addData ( "recv_mesg_ok", recv_mesg_ok );
			xmlResult.addData ( "recv_throughput", recv_throughput );
		}

		NMXML xmlTest;
		xmlTest.loadTree ( testFileContent.c_str() );
		xmlTest.delData ( PATH_XML_SOURCE_USERNAME );
		xmlTest.delData ( PATH_XML_SOURCE_PASSWORD );
		xmlTest.delData ( PATH_XML_DEST_USERNAME );
		xmlTest.delData ( PATH_XML_DEST_PASSWORD );
		//     xmlTest.delData ( PATH_XML_RESULT );
		xmlTest.addData ( TAG_RESULT, xmlResult );
		testFileContent = xmlTest.toString();

		xmlResult.freeTree();
		xmlTest.freeTree();
		ofstream outputFile;
		outputFile.open ( testFileName.c_str(), fstream::out );
		outputFile << testFileContent.c_str();
		outputFile.close();*/

		/*! Using NMXMLStreamWriter */
		NMXML xmlMetadata;
		xmlMetadata.loadTree ( metadataContent.c_str() );
		xmlMetadata.delData ( "/metadata/source/management/username" );
		xmlMetadata.delData ( "/metadata/source/management/password" );
		xmlMetadata.delData ( "/metadata/destination/management/username" );
		xmlMetadata.delData ( "/metadata/destination/management/password" );

		NMXMLStreamWriter xmlw ( testFileName );
		xmlw.setAutoFormatting ( true );
		xmlw.writeStartDocument ();
		xmlw.writeStartElement ( "test" );
		xmlw.writeNMXMLTree ( xmlMetadata );
		xmlMetadata.freeTree();

		xmlw.writeStartElement ( "result" );
		if ( !inputFields.udpProtocol )
		{
			xmlw.writeStartElement ( "tcp" );
			xmlw.writeTextElement ( "source", inputFields.sourceAddress );
			xmlw.writeTextElement ( "destination", inputFields.destAddress );

			xmlw.writeTextElement ( "rss", rss );
			xmlw.writeTextElement ( "sss", sss );
			xmlw.writeTextElement ( "sms", sms );
			xmlw.writeTextElement ( "time", time );
			xmlw.writeTextElement ( "throughput", throughput );
			xmlw.writeEndElement (); //tcp
		}
		else
		{
			xmlw.writeStartElement ( "udp" );
			xmlw.writeTextElement ( "source", inputFields.sourceAddress );
			xmlw.writeTextElement ( "destination", inputFields.destAddress );

			xmlw.writeTextElement ( "sss", sss );
			xmlw.writeTextElement ( "sms", sms );
			xmlw.writeTextElement ( "time", time );
			xmlw.writeTextElement ( "send_mesg_ok", send_mesg_ok );
			xmlw.writeTextElement ( "send_mesg_ko", send_mesg_ko );
			xmlw.writeTextElement ( "send_throughput", send_throughput );
			xmlw.writeTextElement ( "rss", rss );
			xmlw.writeTextElement ( "recv_mesg_ok", recv_mesg_ok );
			xmlw.writeTextElement ( "recv_throughput", recv_throughput );
			xmlw.writeEndElement (); //udp
		}

		xmlw.writeEndElement (); //result
		xmlw.writeEndElement (); //test
		xmlw.writeEndDocument ();

		return testFileName;
	}
	else
		return 0;
}

void NMNetperfInterpret::cleanInputFields()
{
	inputFields.testName = "";
	inputFields.testIdentifier = "";

	// Source
	inputFields.sourceAddress = "";
	inputFields.sourceNetperfPath = "";
	inputFields.sourceManagementAddress = "";
	inputFields.sourceManagementAddressPort = 22;
	inputFields.sourceManagementUsername = "";
	inputFields.sourceManagementPassword = "";
	inputFields.sourceManagementProtocol = "";

	inputFields.port = 12865;
	inputFields.duration = 0;
	inputFields.durationInSeconds = inputFields.udpProtocol = inputFields.noDelayOn = false;
	inputFields.sss = inputFields.sms = inputFields.rss = inputFields.rms = -1;

	// Destionation
	inputFields.destAddress = "";
	inputFields.destNetserverPath = "";
	inputFields.destManagementAddress = "";
	inputFields.destManagementAddressPort = 22;
	inputFields.destManagementUsername = "";
	inputFields.destManagementPassword = "";
	inputFields.destManagementProtocol = "";

	inputFields.ipVersion = -4;
}
