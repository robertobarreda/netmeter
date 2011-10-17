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
#include "nmxmlstreamwriter.h"
#include "nmstring.h"

#include <string.h>
#include <iostream>
using namespace std;

NMXMLStreamWriter::NMXMLStreamWriter ( const NMString & uri )
{
	writer = xmlNewTextWriterFilename ( uri.c_str(), 0 );
#ifdef DEBUG
	if ( writer == NULL )
	{
		cerr << "NMXMLStreamWriter( NMString " << uri << " ): Error creating the xml writer" << endl;
		return;
	}
#endif
}

NMXMLStreamWriter::~NMXMLStreamWriter()
{}

void NMXMLStreamWriter::writeNMXMLTree ( const NMString & xmlData )
{
	const char * buff = xmlData.c_str();
	int size = strlen ( buff );
	if ( size <= 0 )
	{
		cerr << "NMXMLStreamWriter writeNMXMLTree failed: XML buffer empty" << endl;
		return;
	}

	xmlTextReaderPtr reader = xmlReaderForMemory ( buff, size, NULL, NULL, 0 );
	if ( reader == NULL ) cerr << "Unable to open " << xmlData.c_str() << endl;
	else
	{
		int ret = 1;
		while ( ( ret = xmlTextReaderRead ( reader ) ) == 1 )
			writeNMXMLTree ( reader );

		xmlFreeTextReader ( reader );
		if ( ret != 0 )
			cerr << xmlData.c_str() << " : failed to parse" << endl;
	}
}

void NMXMLStreamWriter::writeNMXMLTree ( xmlTextReaderPtr reader )
{
	bool empty;
	switch ( ( xmlReaderTypes ) xmlTextReaderNodeType ( reader ) )
	{
		case XML_READER_TYPE_ELEMENT:
			xmlTextWriterStartElement ( writer, xmlTextReaderConstName ( reader ) );
			empty = xmlTextReaderIsEmptyElement(reader);
			if ( xmlTextReaderHasAttributes ( reader ) )
			{
				xmlTextReaderMoveToFirstAttribute ( reader );
				writeNMXMLTree ( reader );
				while ( xmlTextReaderMoveToNextAttribute ( reader ) )
					writeNMXMLTree ( reader );
			}
			if ( empty ) xmlTextWriterEndElement ( writer );
			break;
		case XML_READER_TYPE_ATTRIBUTE:
			xmlTextWriterWriteAttribute ( writer, xmlTextReaderConstName ( reader ),
			                              xmlTextReaderConstValue ( reader ) );
			break;
		case XML_READER_TYPE_TEXT:
			xmlTextWriterWriteString ( writer, xmlTextReaderConstValue ( reader ) );
			break;
		case XML_READER_TYPE_END_ELEMENT:
			xmlTextWriterEndElement ( writer );
			break;
		default:
			break;
	}
}

void NMXMLStreamWriter::writeAttribute ( const NMString & qualifiedName, const NMString & value )
{
	rc = xmlTextWriterWriteAttribute ( writer, BAD_CAST qualifiedName.c_str(), BAD_CAST value.c_str() );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeStartElement" << endl;
		return;
	}
#endif
}

void NMXMLStreamWriter::writeCharacters ( const NMString & text )
{
	rc = xmlTextWriterWriteString ( writer, BAD_CAST text.c_str() );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeStartElement" << endl;
		return;
	}
#endif
}

void NMXMLStreamWriter::writeEmptyElement ( const NMString & qualifiedName )
{
	rc = xmlTextWriterStartElement ( writer, BAD_CAST qualifiedName.c_str() );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeEmptyElement - StartElement" << endl;
		return;
	}
#endif
	rc = xmlTextWriterFullEndElement ( writer );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeEmptyElement - EndElement" << endl;
		return;
	}
#endif

}

void NMXMLStreamWriter::writeEndDocument ()
{
	rc = xmlTextWriterEndDocument ( writer );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeEndDocument" << endl;
		return;
	}
#endif
	xmlFreeTextWriter ( writer );
}

void NMXMLStreamWriter::writeEndElement ()
{
	rc = xmlTextWriterEndElement ( writer );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeEndElement" << endl;
		return;
	}
#endif
}

void NMXMLStreamWriter::writeStartDocument ( const NMString & version )
{
	rc = xmlTextWriterStartDocument ( writer, version.c_str(), NMXMLENCODING, NULL );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeStartDocument" << endl;
		return;
	}
#endif
}

void NMXMLStreamWriter::writeStartDocument ()
{
	rc = xmlTextWriterStartDocument ( writer, NULL, NMXMLENCODING, NULL );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeStartDocument" << endl;
		return;
	}
#endif
}

void NMXMLStreamWriter::writeStartElement ( const NMString & qualifiedName )
{
	rc = xmlTextWriterStartElement ( writer, BAD_CAST qualifiedName.c_str() );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeStartElement" << endl;
		return;
	}
#endif
}

void NMXMLStreamWriter::writeTextElement ( const NMString & qualifiedName, const NMString & text )
{
	rc = xmlTextWriterWriteElement ( writer, BAD_CAST qualifiedName.c_str(), BAD_CAST text.c_str() );
#ifdef DEBUG
	if ( rc < 0 )
	{
		cerr <<  "ERROR at NMXMLStreamWriter::writeTextElement" << endl;
		return;
	}
#endif
}
