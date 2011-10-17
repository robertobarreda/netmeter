/*
 *   $Id: nmmgeninterpret.cpp,v 1.19 2007-05-27 11:19:34 rserral Exp $
 *
 *   Copyright (c) 2006 Pau Capella
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nmmgeninterpret.h"
#include "nmmmgenutils.h"

#include <nmfile.h>
#include <nmxmlstreamwriter.h>

#include <iostream>
#include <fstream>
using namespace std;

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

extern int xmlLoadExtDtdDeafultValue;

#define MGEN_SCRIPT_FILE_PREFFIX "/tmp/gscript"
#define DREC_SCRIPT_FILE_PREFFIX "/tmp/dscript"

#define LOCAL_SUFFIX	".local"

#define TEST_FILE_PREFFIX "test_owd"

/**
 * Contructor of the class
 * @return 
 */
NMMGenInterpret::NMMGenInterpret(NMMgenErrorManager * errorMan)
{
	//By default, mgen3's XSLT is configured
	mgenXmlXsltPath = NMString(XML_DIR) + NMString("/conf_mgen.xslt");
	drecXmlXsltPath = NMString(XML_DIR) + NMString("/conf_drec.xslt");
	cleanInputFields();
	testFileContent = "";
	metadataContent = "";
	testFileName = "";
	errorManager = errorMan;
}

/**
 * Destructor of the class
 * @return 
 */
NMMGenInterpret::~NMMGenInterpret()
{
}

/**
 * Returns the input fields of the parsed XML document
 * @return 
 */
NMMGenInputFields & NMMGenInterpret::getInputFields()
{
	return inputFields;
}

/**
 * Generates the xml document for the result of the test
 * @param decodeFile Local path to the decode file
 * @return NMString with the XML 
 */
NMString NMMGenInterpret::getXMLResult(const NMString & testFile, const NMString & decodeFile, const NMString & fileGraphOWD, 
    	const NMString & fileGraphOWDD, const NMString & fileGraphIPDV, const NMString & fileGraphIPDVD)
{
	NMXML xmlResult;
	xmlResult.newTree("data");
	xmlResult.addData("test_file", testFile);
	xmlResult.addData("decode_file", decodeFile);
	if(inputFields.localDistrPath!="")
	{
		xmlResult.addData("result_graph_owd",fileGraphOWD);
		xmlResult.addData("result_graph_owdd",fileGraphOWDD);
		xmlResult.addData("result_graph_ipdv",fileGraphIPDV);
		xmlResult.addData("result_graph_ipdvd",fileGraphIPDVD);
	}
	
	NMString xmlResultStr = xmlResult.toString();
	xmlResult.freeTree();
	return xmlResultStr;
}

/**
 * Parses the start XML document
 * @param xml 
 * @return 
 */
bool NMMGenInterpret::parseStartXML(NMString *xml, NMString  filesSuffixPath)
{
	if(!xml) return false;
	cleanInputFields();
	NMXML nmxmlData;
	nmxmlData.loadTree(xml->c_str());

	NMString sleeptime = nmxmlData.getValue("/*/sleeptime");
        if(sleeptime.Length()) 
	{
		// if we recieve a message destinated to sync we discard it:
		nmxmlData.freeTree();
		return false;
	}

	filesSuffix = filesSuffixPath;

	testFileContent = nmxmlData.getTree( "/*/test", "test");
	metadataContent = nmxmlData.getTree( "/*/test/metadata", "metadata" );

	if ( metadataContent.IsEmpty() ) return false;

	NMString resultPath = nmxmlData.getValue("/*/test/metadata/result_path")!=""?nmxmlData.getValue("/*/test/metadata/result_path"):"/tmp/";
	testFileName = resultPath + TEST_FILE_PREFFIX + filesSuffixPath + ".xff";

	inputFields.testName = nmxmlData.getValue("/*/test/metadata/name");
	inputFields.testIdentifier = nmxmlData.getValue("/*/test/metadata/identifier");
	inputFields.sourceAddress = nmxmlData.getValue("/*/test/metadata/source/address");
	inputFields.sourceMgenPath = nmxmlData.getValue("/*/test/metadata/source/mgen_path");
	inputFields.sourceManagementAddress = nmxmlData.getValue("/*/test/metadata/source/management/address");
	inputFields.sourceManagementAddressPort = nmxmlData.getValue("/*/test/metadata/source/management/port").toInt();
	inputFields.sourceManagementUsername = nmxmlData.getValue("/*/test/metadata/source/management/username");
	inputFields.sourceManagementPassword = nmxmlData.getValue("/*/test/metadata/source/management/password");
	inputFields.destAddress = nmxmlData.getValue("/*/test/metadata/destination/address");
	inputFields.destDecodePath = nmxmlData.getValue("/*/test/metadata/destination/decode_path");
	inputFields.destDrecPath = nmxmlData.getValue("/*/test/metadata/destination/drec_path");
	inputFields.destManagementAddress = nmxmlData.getValue("/*/test/metadata/destination/management/address");
	inputFields.destManagementAddressPort = nmxmlData.getValue("/*/test/metadata/destination/management/port").toInt();
	inputFields.destManagementUsername = nmxmlData.getValue("/*/test/metadata/destination/management/username");
	inputFields.destManagementPassword = nmxmlData.getValue("/*/test/metadata/destination/management/password");
	inputFields.localDistrPath="";
	inputFields.localDistrPath = nmxmlData.getValue("/*/test/metadata/local/distr_path");
	inputFields.testMgenVersion = nmxmlData.getValue("/*/test/metadata/mgen_version").toInt();
	inputFields.mgen_sync = nmxmlData.getValue("/*/test/metadata/mgen_sync").toInt();

	if (inputFields.testMgenVersion == 4) 
	{
		//Redefine XSLT for mgen4
		mgenXmlXsltPath = NMString(XML_DIR) + NMString("/conf_mgen4.xslt");
		drecXmlXsltPath = NMString(XML_DIR) + NMString("/conf_drec4.xslt");
	}

	if (inputFields.mgen_sync == 1)
	{
		cout << "**************************************************************" << endl;
		cout << "NMMGenInterpret::parseStartXML --> sync data will be collected" << endl;
		cout << "**************************************************************" << endl;

	}
	else
	{
		cout << "*****************************************************************" << endl;
		cout << "NMMGenInterpret::parseStartXML --> NO sync data will be collected" << endl;
		cout << "*****************************************************************" << endl;
	}

	NMString endString = "</conf_mgen>";
	int begin = xml->search("<conf_mgen"), end = xml->search(endString);	
	NMString strConfMgen = xml->subString(begin,end-begin+endString.Length());
	if(end==-1) strConfMgen = "<conf_mgen/>";
	
	endString = "</conf_drec>";
	begin =xml->search("<conf_drec");
	end = xml->search(endString);	
	NMString strConfDrec = xml->subString(begin,end-begin+endString.Length());
	if(end==-1) strConfDrec = "<conf_drec/>";
	
	inputFields.pathMgenScript = MGEN_SCRIPT_FILE_PREFFIX + filesSuffix;
	if (inputFields.sourceManagementAddress == "127.0.0.1" || inputFields.sourceManagementAddress == "::1" || inputFields.sourceManagementAddress == "localhost")
		applyXsltToDoc(strConfMgen, mgenXmlXsltPath, inputFields.pathMgenScript);

	else
		applyXsltToDoc(strConfMgen, mgenXmlXsltPath, inputFields.pathMgenScript+LOCAL_SUFFIX);

	inputFields.pathDrecScript = DREC_SCRIPT_FILE_PREFFIX + filesSuffix;
	if (inputFields.destManagementAddress == "127.0.0.1" || inputFields.destManagementAddress == "::1" || inputFields.destManagementAddress == "localhost") 
		applyXsltToDoc(strConfDrec, drecXmlXsltPath, inputFields.pathDrecScript);
	else
		applyXsltToDoc(strConfDrec, drecXmlXsltPath, inputFields.pathDrecScript+LOCAL_SUFFIX);
#ifdef DEBUG
  cerr << "DEBUG: Used drec Path: " << inputFields.destDrecPath << endl;
#endif
	nmxmlData.freeTree();
	
	return true;
}

/**
 * Applies the xslt transformation to the xmlDoc document
 * @param xmlDoc XML document
 * @param xsltDocPath xslt file
 * @param resFilePath File to save the transformation
 */
void NMMGenInterpret::applyXsltToDoc(const NMString & xmlDoc, const NMString & xsltDocPath, const NMString & resFilePath)
{
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, res;

	NMString temporalFile = "/tmp/tmpxslttransformation"+filesSuffix;
	
	xmlSubstituteEntitiesDefault(0);
	xmlLoadExtDtdDefaultValue = 0;
	cur = xsltParseStylesheetFile((const xmlChar *)xsltDocPath.c_str());
	doc = xmlParseMemory(xmlDoc.c_str(),xmlDoc.Length());
	res = xsltApplyStylesheet(cur, doc,0);
	FILE * confMgenFile = fopen(temporalFile.c_str(),"w");
	xsltSaveResultToFile(confMgenFile, res, cur);
	fclose(confMgenFile);
	
	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

        xsltCleanupGlobals();
        xmlCleanupParser();

	cleanXlstOutputFile(temporalFile,resFilePath);
}

/**
 * Cleans the xslt output 
 * @param doc 
 */
void NMMGenInterpret::cleanXlstOutputFile(const NMString & fileIn, const NMString & fileOut)
{
	ifstream inputFile;
	ofstream outputFile;
	
	inputFile.open(fileIn.c_str(), fstream::in);
	outputFile.open(fileOut.c_str(), fstream::out);
	
	char line[1024];
	NMString strLine;
	while(inputFile.getline(line,1024))
	{
		strLine = line;
		strLine = strLine.trim();
		if(strLine.Length()>0)
		{
			outputFile << strLine.c_str() << endl;
		}
	}
	
	inputFile.close();
	outputFile.close();
	remove(fileIn.c_str()); /* removes the temporary file which anybody wants :( */
}

	
/**
 * Converts the decode output file to XML
 * @param decodeOutputFile 
 * @return 
 */
NMString NMMGenInterpret::addResultToTestFile(const NMString & decodeOutputFile)
{
	NMXML xmlResult;
	xmlResult.newTree(TAG_FLOWS);
	
	ifstream inputFile;
	inputFile.open(decodeOutputFile.c_str(), fstream::in);
	#define BUFF_SIZE	2048
	char line[BUFF_SIZE];
	NMString strLine, flowId, flowSeq, flowSrc, flowDest, flowTxTime, flowRxTime, flowSize;
	list<PacketInfo> listVector;

	// 1) Load data from drec output file:
		
	while(inputFile.getline(line,BUFF_SIZE))
	{
		strLine = line;
		strLine = strLine.trim();
		if(strLine.Length()>0) 
		{
			if ((inputFields.testMgenVersion == 4) && (strLine.search("RECV") >= 0)) {
				putValuesFromDecodeLine4(strLine,flowId, flowSeq, flowSrc, flowDest, flowTxTime, flowRxTime, flowSize);
				listVector.push_back(PacketInfo(flowId, flowSeq, flowSrc, flowDest, flowTxTime, flowRxTime, flowSize));
			} else if (inputFields.testMgenVersion == 3) {
				putValuesFromDecodeLine(strLine,flowId, flowSeq, flowSrc, flowDest, flowTxTime, flowRxTime, flowSize);
				listVector.push_back(PacketInfo(flowId, flowSeq, flowSrc, flowDest, flowTxTime,	flowRxTime, flowSize));
			}
		}
	}

	// 2) Sort flows:

	inputFile.close();
	if(listVector.empty())	errorManager->notifyError(NMError(NMError::fatal,MGEN_EMPTY_FILE));
	else			listVector.sort();

	// 3) Write XML:  

	int currentIdFlow = -1;
	int pos = 0;

	// Load metadata
        NMXML xmlMetadata;
        xmlMetadata.loadTree(metadataContent.c_str());
        xmlMetadata.delData("/metadata/source/management/username");
        xmlMetadata.delData("/metadata/source/management/password");
        xmlMetadata.delData("/metadata/destination/management/username");
        xmlMetadata.delData("/metadata/destination/management/password");

	// Add metadata and start introducing the results of the test
	NMXMLStreamWriter xmlw(testFileName);
	xmlw.setAutoFormatting(true);
	xmlw.writeStartDocument();		// Start of DOCUMENT
	xmlw.writeStartElement("test");		// Start of TAG_TEST
	xmlw.writeNMXMLTree(xmlMetadata);	// write metadata
	xmlMetadata.freeTree();	

	xmlw.writeStartElement("result");	// Start of TAG_RESULT
	xmlw.writeStartElement("flows");	// Start of TAG_FLOWS

	for(list<PacketInfo>::iterator it = listVector.begin(); it!=listVector.end(); ++it)
	{
		if(currentIdFlow != (*it).flowId)
		{
			if (currentIdFlow != -1) xmlw.writeEndElement(); // end of TAG_FLOW
			currentIdFlow = (*it).flowId;
			
			// TAG_FLOW:
			xmlw.writeStartElement(TAG_FLOW);
			xmlw.writeAttribute(ATTR_ID, currentIdFlow);

			pos = flowSrc.search("/");
			// TAG_SOURCE:
			xmlw.writeStartElement(TAG_SOURCE);
			xmlw.writeTextElement(TAG_ADDRESS, flowSrc.subString(0,pos));
			xmlw.writeTextElement(TAG_PORT, flowSrc.subString(pos+1, flowSrc.Length()));
			xmlw.writeEndElement();
	
			pos = flowDest.search("/");
			// TAG_DESTINATION:
			xmlw.writeStartElement(TAG_DESTINATION);
			xmlw.writeTextElement(TAG_ADDRESS, flowDest.subString(0,pos));
			xmlw.writeTextElement(TAG_PORT, flowDest.subString(pos+1, flowDest.Length()));
			xmlw.writeEndElement();	
		}
		// TAG_SEQ:		
		xmlw.writeStartElement(TAG_SEQ);
		xmlw.writeAttribute(ATTR_ID, (*it).flowSeq);
		xmlw.writeTextElement(TAG_TX_TIME, (*it).flowTxTime);
		xmlw.writeTextElement(TAG_RX_TIME, (*it).flowRxTime);
		xmlw.writeTextElement(TAG_SIZE, (*it).flowSize);
		xmlw.writeEndElement();		// end TAG_SEQ
	}

	xmlw.writeEndDocument(); // end DOCUMENT (closes TAG_FLOWS,TAG_RESULT,TAG_TEST)

	return testFileName;
}

/*
	for(list<PacketInfo>::iterator it = listVector.begin(); it!=listVector.end(); ++it)
	{
		if(currentIdFlow!=(*it).flowId)
		{
			currentIdFlow = (*it).flowId;
			if(xmlFlow)
			{
				xmlResult.addData("",*xmlFlow);
				xmlFlow->freeTree();
			}
	
			xmlFlow = new NMXML();
			list<NMXMLAttr> *attlist = new list<NMXMLAttr>;
			NMXMLAttr attribute;
			attribute.name = ATTR_ID;
			attribute.value = currentIdFlow;
			attlist->push_back( attribute);
			xmlFlow->newTree(TAG_FLOW,attlist);

			int pos;
			NMXML sourceNode(TAG_SOURCE);
			pos = flowSrc.search("/");
			sourceNode.addData(TAG_ADDRESS, flowSrc.subString(0,pos));
			sourceNode.addData(TAG_PORT, flowSrc.subString(pos+1, flowSrc.Length()));
			xmlFlow->addData("",sourceNode);
	
			NMXML destNode(TAG_DESTINATION);
			pos = flowDest.search("/");
			destNode.addData(TAG_ADDRESS, flowDest.subString(0,pos));
			destNode.addData(TAG_PORT, flowDest.subString(pos+1, flowDest.Length()));
			xmlFlow->addData("",destNode);

			delete attlist;	
		}
		NMXML seqNode;
		createSeqNode(seqNode,*it);
		xmlFlow->addData("",seqNode);
		seqNode.freeTree();
	}
	if(xmlFlow)
	{
		xmlResult.addData("",*xmlFlow);
		xmlFlow->freeTree();
	}

	NMXML xmlTest;
	xmlTest.loadTree(testFileContent.c_str());
	xmlTest.delData(PATH_XML_SOURCE_USERNAME);
	xmlTest.delData(PATH_XML_SOURCE_PASSWORD);
	xmlTest.delData(PATH_XML_DEST_USERNAME);
	xmlTest.delData(PATH_XML_DEST_PASSWORD);
	xmlTest.delData(PATH_XML_RESULT);
	xmlTest.addData(TAG_RESULT,xmlResult);
	testFileContent = xmlTest.toString();

	xmlResult.freeTree();
	xmlTest.freeTree();
	ofstream outputFile;
	outputFile.open(testFileName.c_str(), fstream::out);
	outputFile << testFileContent.c_str();
	outputFile.close();
*/


/**
 * Creates the sed node of the document
 * @param pi 
 * @return 
 */
void NMMGenInterpret::createSeqNode(NMXML & seqNode, const PacketInfo& pi)
{
	list<NMXMLAttr> *attlist = new list<NMXMLAttr>;
	NMXMLAttr attribute;
	attribute.name = ATTR_ID;
	attribute.value = pi.flowSeq;
	attlist->push_back( attribute);
	seqNode.newTree(TAG_SEQ,attlist);
	delete attlist;
	
	seqNode.addData(TAG_TX_TIME,pi.flowTxTime);
	seqNode.addData(TAG_RX_TIME,pi.flowRxTime);
	seqNode.addData(TAG_SIZE,pi.flowSize);

/*
<flow id="2">
	<seq id="4611">
		<source version="4">
			<address>192.168.2.2</address>
			<port>1024</port>
		</source>
		<destination version="4">
			<address>192.168.1.2</address>
			<port>33033</port>
		</destination>
		<tx_time>2006-01-01T22:00:00Z</tx_time>
		<rx_time>2006-01-01T23:23:00Z</rx_time>
		<size>186</size>
	</seq>
</flow>
*/

}

/**
 * Reads the info of a decode file line
 * @param decodeLine 
 * @param strLine 
 * @param flowId 
 * @param flowSeq 
 * @param flowSrc 
 * @param flowDest 
 * @param flowTxTime 
 * @param flowRxTime 
 * @param flowSize 
 */
void NMMGenInterpret::putValuesFromDecodeLine(NMString & decodeLine, NMString &flowId, NMString &flowSeq,
    	NMString &flowSrc, NMString &flowDest, NMString &flowTxTime, NMString &flowRxTime, NMString &flowSize)
{
	#define DECODE_SEPARATOR ">"
	int pos;
	NMString aux;

	pos = decodeLine.search("Seq")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowId = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

	pos = decodeLine.search("Src")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowSeq = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

	pos = decodeLine.search("Dest")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowSrc = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length()).trimLeft();

	pos = decodeLine.search("TxTime")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowDest = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length()).trimLeft();
	
	pos = decodeLine.search("RxTime")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowTxTime = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());
 	
	pos = decodeLine.search("Size")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowRxTime = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

	aux = decodeLine;
	flowSize = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

//cout << "ID: " << flowId << " SEQ: " << flowSeq << " SRC: " << flowSrc << " DEST: " << flowDest << " Tx: " << flowTxTime << " Rx: " << flowRxTime << " Size: " << flowSize << endl;


/*

From:
Flow>0000 Seq>000000 Src>       127.0.0.1/5000  Dest>       127.0.0.1/6959  TxTime>22:03:15.170155 RxTime>22:03:15.170212 Size>0100
Flow>0001 Seq>000000 Src>       127.0.0.1/5000  Dest>       127.0.0.1/6960  TxTime>22:03:15.170280 RxTime>22:03:15.170296 Size>0100

To:
<flow id="2">
	<seq id="4611">
		<source version="4">
			<address>192.168.2.2</address>
			<port>1024</port>
		</source>
		<destination version="4">
			<address>192.168.1.2</address>
			<port>33033</port>
		</destination>
		<tx_time>2006-01-01T22:00:00Z</tx_time>
		<rx_time>2006-01-01T23:23:00Z</rx_time>
		<size>186</size>
	</seq>
</flow>

*/

	
}

/**
 * Reads the info of a decode file line for mgen v4
 * @param decodeLine 
 * @param strLine 
 * @param flowId 
 * @param flowSeq 
 * @param flowSrc 
 * @param flowDest 
 * @param flowTxTime 
 * @param flowRxTime 
 * @param flowSize 
 */
void NMMGenInterpret::putValuesFromDecodeLine4(NMString & decodeLine, NMString &flowId, NMString &flowSeq,
    	NMString &flowSrc, NMString &flowDest, NMString &flowTxTime, NMString &flowRxTime, NMString &flowSize)
{
	#define DECODE_SEPARATOR ">"
	int pos;
	NMString aux;

	pos = decodeLine.search("RECV")-1;
	flowRxTime = decodeLine.subString(pos-15,pos);
	decodeLine = decodeLine.subString(decodeLine.search("flow"),decodeLine.Length()).trimLeft();

	pos = decodeLine.search("seq")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowId = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

	pos = decodeLine.search("src")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowSeq = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

	pos = decodeLine.search("dst")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowSrc = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length()).trimLeft();

	pos = decodeLine.search("sent")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowDest = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length()).trimLeft();
 	
	pos = decodeLine.search("size")-1;
	aux = decodeLine.subString(0,pos);
	decodeLine = decodeLine.subString(pos+1,decodeLine.Length()).trimLeft();
	flowTxTime = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

	pos = decodeLine.search("gps")-1;
	if (pos < 0) aux = decodeLine;
	else aux = decodeLine.subString(0,pos);
	flowSize = aux.subString(aux.search(DECODE_SEPARATOR)+1,aux.Length());

//cout << "ID: " << flowId << " SEQ: " << flowSeq << " SRC: " << flowSrc << " DEST: " << flowDest << " Tx: " << flowTxTime << " Rx: " << flowRxTime << " Size: " << flowSize << endl;

/*

From:
22:59:52.312721 RECV flow>1 seq>1 src>132.250.68.21/5000
dst>132.259.43.96/5002 sent>22:59:52.310324 size>1024

To:
<flow id="2">
	<seq id="4611">
		<source version="4">
			<address>192.168.2.2</address>
			<port>1024</port>
		</source>
		<destination version="4">
			<address>192.168.1.2</address>
			<port>33033</port>
		</destination>
		<tx_time>2006-01-01T22:00:00Z</tx_time>
		<rx_time>2006-01-01T23:23:00Z</rx_time>
		<size>186</size>
	</seq>
</flow>

*/

	
}

/**
 * Cleans the input fields
 */
void NMMGenInterpret::cleanInputFields()
{
	inputFields.testName = "";
	inputFields.testIdentifier = "";
	inputFields.testMgenVersion = 3;
	inputFields.sourceAddress = "";
	inputFields.sourceMgenPath = "";
	inputFields.sourceManagementAddress = "";
	inputFields.sourceManagementAddressPort = 22;
	inputFields.sourceManagementUsername = "";
	inputFields.sourceManagementPassword = "";
	inputFields.sourceManagementProtocol = "";
	inputFields.destAddress = "";
	inputFields.destDrecPath = "";
	inputFields.destDecodePath = "";
	inputFields.destManagementAddress = "";
	inputFields.destManagementAddressPort = 22;
	inputFields.destManagementUsername = "";
	inputFields.destManagementPassword = ""; 
	inputFields.destManagementProtocol = "";
}
