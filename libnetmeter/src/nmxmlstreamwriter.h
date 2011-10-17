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
#ifndef NMXMLSTREAMWRITER_H
#define NMXMLSTREAMWRITER_H

#include "nmxml.h"

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

#define NMXMLENCODING "UTF-8"

class NMString;

class NMXMLStreamWriter
{
	public:
		NMXMLStreamWriter ( const NMString & uri );
		~NMXMLStreamWriter();
		void setAutoFormatting ( bool indent ) { xmlTextWriterSetIndent ( writer, indent ); }
		inline void writeNMXMLTree ( NMXML xmlData ) { writeNMXMLTree(xmlData.toString()); }
		void writeNMXMLTree ( const NMString & xmlData );
// 		void writeAttribute ( const NMString & namespaceUri, const NMString & name, const NMString & value );
		void writeAttribute ( const NMString & qualifiedName, const NMString & value );
// 		void writeAttribute ( NMXMLAttr & attribute );
// 		void writeAttributes ( list<NMXMLAttr> * attributes );
// 		void writeCDATA ( const NMString & text );
		void writeCharacters ( const NMString & text );
// 		void writeComment ( const NMString & text );
// 		void writeDTD ( const NMString & dtd );
// 		void writeDefaultNamespace ( const NMString & namespaceUri );
// 		void writeEmptyElement ( const NMString & namespaceUri, const NMString & name );
		void writeEmptyElement ( const NMString & qualifiedName );
		void writeEndDocument ();
		void writeEndElement ();
// 		void writeEntityReference ( const NMString & name );
// 		void writeNamespace ( const NMString & namespaceUri, const NMString & prefix = NMString() );
// 		void writeProcessingInstruction ( const NMString & target, const NMString & data = NMString() );
		void writeStartDocument ( const NMString & version );
		void writeStartDocument ();
// 		void writeStartElement ( const NMString & namespaceUri, const NMString & name );
		void writeStartElement ( const NMString & qualifiedName );
// 		void writeTextElement ( const NMString & namespaceUri, const NMString & name, const NMString & text );
		void writeTextElement ( const NMString & qualifiedName, const NMString & text );

	private:
		void writeNMXMLTree ( xmlTextReaderPtr reader );
		
		xmlTextWriterPtr writer;
		int rc;
};

#endif
