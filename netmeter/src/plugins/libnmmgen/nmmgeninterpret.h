/*
 *   $Id: nmmgeninterpret.h,v 1.15 2007-05-27 11:19:34 rserral Exp $
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
#ifndef NMMGENINTERPRET_H
#define NMMGENINTERPRET_H

#define TAG_SOURCE			"source"
#define TAG_DESTINATION			"destination"
#define TAG_SEQ				"seq"
#define TAG_FLOW			"flow"
#define TAG_FLOWS			"flows"
#define TAG_TX_TIME			"tx_time"
#define TAG_RX_TIME			"rx_time"
#define TAG_SIZE			"size"
#define TAG_ADDRESS			"address"
#define TAG_PORT			"port"
#define TAG_RESULT			"result"
#define PATH_XML_RESULT			"/test/result"
#define PATH_XML_SOURCE_USERNAME	"/test/metadata/source/management/username"
#define PATH_XML_SOURCE_PASSWORD	"/test/metadata/source/management/password"
#define PATH_XML_DEST_USERNAME		"/test/metadata/destination/management/username"
#define PATH_XML_DEST_PASSWORD		"/test/metadata/destination/management/password"
      
#define TAG_	""
#define ATTR_ID	"id"

#include "nmmgenerrormanager.h"

#include <nmstring.h>
#include <nmxml.h>

#include <list>

class PacketInfo;

class NMMGenInputFields
{
public:
    NMString testName;
    NMString testIdentifier;
    NMString sourceAddress;
    NMString sourceMgenPath;
    NMString sourceManagementAddress;
    int sourceManagementAddressPort;
    NMString sourceManagementUsername;
    NMString sourceManagementPassword;
    NMString sourceManagementProtocol;
    NMString destAddress;
    NMString destDrecPath;
    NMString destDecodePath;
    NMString destManagementAddress;
    int destManagementAddressPort;
    NMString destManagementUsername;
    NMString destManagementPassword; 
    NMString destManagementProtocol;
    NMString pathMgenScript;
    NMString pathDrecScript;
    NMString localDistrPath;
    int testMgenVersion;
    int mgen_sync;
};

/**
* Interpret for the XML documents for the class NMMGen
@author Ernest Nieto
*/
class NMMGenInterpret
{
    NMString mgenXmlXsltPath;
    NMString drecXmlXsltPath;
public:
    NMMGenInterpret(NMMgenErrorManager * errorMan);

    ~NMMGenInterpret();

    bool parseStartXML(NMString *xml, NMString  filesSuffixPath);
    
    NMMGenInputFields & getInputFields();
    
    NMString getXMLResult(const NMString & testFile, const NMString & decodeFile, const NMString & fileGraphOWD, 
    	const NMString & fileGraphOWDD, const NMString & fileGraphIPDV, const NMString & fileGraphIPDVD);
    
    NMString addResultToTestFile(const NMString & decodeOutputFile);
    
private:
    void applyXsltToDoc(const NMString & xmlDoc, const NMString & xsltDocPath, const NMString & resFilePath);
    void cleanInputFields();
    void cleanXlstOutputFile(const NMString & fileIn, const NMString & fileOut);
    void putValuesFromDecodeLine(NMString & decodeLine, NMString & flowId, NMString & flowSeq,
    	NMString & flowSrc, NMString & flowDest, NMString & flowTxTime, NMString & flowRxTime, NMString & flowSize);
    void putValuesFromDecodeLine4(NMString & decodeLine, NMString & flowId, NMString & flowSeq,
    	NMString & flowSrc, NMString & flowDest, NMString & flowTxTime, NMString & flowRxTime, NMString & flowSize);
    void createSeqNode(NMXML & seqNode, const PacketInfo& pi);
    
private:
    NMMGenInputFields inputFields;
    NMString filesSuffix;
    NMString testFileContent, metadataContent;
    NMString testFileName;
    NMMgenErrorManager * errorManager;

};


class PacketInfo
{
public:
	PacketInfo ()
	{
		flowId=-1;
		flowSeq=-1;
		flowSrc="";
		flowDest="";
		flowTxTime="";
		flowRxTime="";
		flowSize=0;
	}
	
	PacketInfo (NMString pflowId, NMString pflowSeq, NMString pflowSrc, NMString pflowDest, NMString pflowTxTime, 
		NMString pflowRxTime, NMString pflowSize)
	{
		flowId=getIntFromString(pflowId);
		flowSeq=getIntFromString(pflowSeq);
		flowSrc=pflowSrc.trim();
		flowDest=pflowDest.trim();
		flowTxTime=pflowTxTime.trim();
		flowRxTime=pflowRxTime.trim();
		flowSize=getIntFromString(pflowSize);
	}
	
	PacketInfo (const PacketInfo & second)
	{
		this->flowId = second.flowId;
		this->flowSeq = second.flowSeq;
		this->flowSrc = second.flowSrc;
		this->flowDest = second.flowDest;
		this->flowTxTime = second.flowTxTime;
		this->flowRxTime = second.flowRxTime;
		this->flowSize = second.flowSize;
	}
	
	int flowId;
	int flowSeq;
	NMString flowSrc;
	NMString flowDest;
	NMString flowTxTime;
	NMString flowRxTime;
	int flowSize;
	
	PacketInfo & operator= (const PacketInfo & second) 
	{
		this->flowId = second.flowId;
		this->flowSeq = second.flowSeq;
		this->flowSrc = second.flowSrc;
		this->flowDest = second.flowDest;
		this->flowTxTime = second.flowTxTime;
		this->flowRxTime = second.flowRxTime;
		this->flowSize = second.flowSize;
		
		return *this;
	}
	
	int operator< (const PacketInfo & second) const
	{
		if(flowId<second.flowId) return 1;
		if(flowId>second.flowId) return 0;
		
		if(flowSeq<second.flowSeq) return 1;
		if(flowSeq>second.flowSeq) return 0;
		
		//sequence number must be unique
		return 1;
	}

	int operator== (const PacketInfo & second) const 
	{
		if(flowId<second.flowId) return 0;
		if(flowId>second.flowId) return 0;
		
		if(flowSeq<second.flowSeq) return 0;
		if(flowSeq>second.flowSeq) return 0;
		
		//sequence number must be unique
		return 1;
	}
	
private: 
	int getIntFromString(const NMString & str)
	{
		NMString aux = str.trim();
		while(aux.Length()>1 and aux.subString(0,1)=="0")
			aux = aux.subString(1,aux.Length());
		return aux.toInt();
	}
};

#endif
