/*
 *   $Id: nmxml.h,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
 *   Copyright (c) 2008 Roberto Barreda <rbarreda@ac.upc.edu>
 *   Copyright (c) 2005 Lorand Jakab
 *   Copyright (c) 2005 Pau Capella
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

#ifndef NMXML_H
#define NMXML_H

#include "nmstring.h"

#include <list>

#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#define NMXMLNode xmlNodePtr

struct NMXMLAttr
{
	NMString name;
	NMString value;
};

typedef std::list<NMXMLAttr> * NMXMLAttrListPtr;

/**
This class handles low level XML tasks, and offers methods to create and manipulate XML structures.

@author Lorand Jakab
*/
class NMXML
{
	public:
		NMXML();
		NMXML ( NMString root_tag );

		~NMXML();
		void newTree ( NMString root_tag );
		void newTree ( NMString root_tag, NMXMLAttrListPtr attlist );
		void freeTree();
		int loadTree ( char *buffer );
		int addData ( NMString tag );
		int addData ( NMString tag, NMString content );
		int addData ( NMString tag, NMString content, NMXMLAttrListPtr attlist, bool descend );
		int addData ( NMString tag, NMXML content );
		int delData ( NMString xpath );
		NMString toString();
		int toFile ( FILE *f );
		NMString getElement ( const NMString xpath );
		NMString getValue ( const NMString xpath );
		NMString getValue ( const NMString xpath, int index );
		NMString getTree ( NMString xpath, NMString new_root );
		bool nodeExists ( NMString xpath );
		void goUp();
		int setCurrentNode ( NMString xpath );
		static void applyXsltToDoc ( const NMString & xmlDocPath, const NMString & xsltDocPath, const NMString & resFilePath );
		static NMString applyXsltToSection ( NMString & xmlDoc, NMString & xsltDocPath );


	private:
		xmlDocPtr doc;
		xmlNodePtr node;

		void init();
		xmlXPathObjectPtr getNodeSet ( xmlChar *xpath );
};

#endif
