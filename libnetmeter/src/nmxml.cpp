/*
 *   $Id: nmxml.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
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
#include "nmxml.h"

#include <string.h>
#include <iostream>
using namespace std;

NMXML::NMXML()
{
	init();
}

NMXML::NMXML ( NMString root_tag )
{
	init();
	newTree ( root_tag );
}

NMXML::~NMXML()
{
}


/*!
    \fn NMXML::init()
    \brief Common code to different constructors
 */
void NMXML::init()
{
	doc = NULL;
	node = NULL;

	/*
	 * Initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION;
}


/*!
    \fn NMXML::newTree(NMString root_tag)
    \brief Method to create a new XML tree
    \param root_tag String identifying the name of the root element of the new tree
 */
void NMXML::newTree ( NMString root_tag )
{
	doc = xmlNewDoc ( BAD_CAST "1.0" );
	node = xmlNewNode ( NULL, BAD_CAST root_tag.c_str() );
	xmlDocSetRootElement ( doc, node );
}


/*!
    \fn NMXML::newTree(NMString root_tag, list<NMXMLAttr> *attlist)
    \brief Method to create a new XML tree
    \param root_tag String identifying the name of the root element of the new tree
    \param attlist List of attributes to be added
 */
void NMXML::newTree ( NMString root_tag, NMXMLAttrListPtr attlist )
{
	doc = xmlNewDoc ( BAD_CAST "1.0" );
	node = xmlNewNode ( NULL, BAD_CAST root_tag.c_str() );
	xmlDocSetRootElement ( doc, node );

	if ( attlist )
	{
		list<NMXMLAttr>::iterator iter;
		for ( iter = attlist->begin(); iter !=attlist->end(); ++iter )
		{
			xmlNewProp ( node, BAD_CAST iter->name.c_str(), BAD_CAST iter->value.c_str() );
		}
	}
}


/*!
    \fn NMXML::freeTree()
    \brief Destroy XML tree and free memory
*/
void NMXML::freeTree()
{
	if ( doc != NULL )
	{
		xmlFreeDoc ( doc );
		doc = NULL;
		node = NULL;
	}
}


/*!
    \fn NMXML::loadTree(char *buffer)
    \brief Parse a string and create XML DOM
*/
int NMXML::loadTree ( char *buffer )
{
	int size = strlen ( buffer );
	if ( size <= 0 )
	{
		cerr << "NMXML loadTree failed: XML buffer empty" << endl;
		return -1;
	}

	doc = xmlParseMemory ( buffer, size );
	if ( doc == NULL )
	{
		cerr << "NMXML loadTree failed: Cannot parse buffer" << endl;
		return -1;
	}

	node = xmlDocGetRootElement ( doc );
	return 0;
}


/*!
    \fn NMXML::addData(NMString tag)
    \brief This function is used to create a tag and descend to its level
    \param tag String identifying the tag name to use
 */
int NMXML::addData ( NMString tag )
{
	node = xmlNewTextChild ( node, NULL, BAD_CAST tag.c_str(), NULL );
	return 0;
}


/*!
    \fn NMXML::addData(NMString tag, NMString content)
    \brief This function is used to create a tag with the desired character data
    \param tag String identifying the tag name to use
    \param content The content of the tag
 */
int NMXML::addData ( NMString tag, NMString content )
{
	xmlNewTextChild ( node, NULL, BAD_CAST tag.c_str(), BAD_CAST content.c_str() );
	return 0;
}


/*!
    \fn NMXML::addData(NMString tag, NMString content, list<NMXMLAttr> *attlist, bool descend)
    \brief This function is used to create a tag with the desired character data
    \param tag String identifying the tag name to use
    \param content The content of the tag
    \param attlist List of attributes to be added
    \param descend If true, the current node will be the newly created node
 */
int NMXML::addData ( NMString tag, NMString content, NMXMLAttrListPtr attlist, bool descend )
{
	xmlNodePtr new_node;
	if ( ( descend ) && ( content != "" ) )
	{
		cerr << "NMXML addData failed: If descent true content must be empty" << endl;
		return -1;
	}
	if ( content == "" )
	{
		new_node = xmlNewTextChild ( node, NULL, BAD_CAST tag.c_str(), NULL );
	}
	else
	{
		new_node = xmlNewTextChild ( node, NULL, BAD_CAST tag.c_str(), BAD_CAST content.c_str() );
	}
	if ( attlist )
	{
		list<NMXMLAttr>::iterator iter;
		for ( iter = attlist->begin(); iter !=attlist->end(); ++iter )
		{
			xmlNewProp ( new_node, BAD_CAST iter->name.c_str(), BAD_CAST iter->value.c_str() );
		}
	}
	if ( descend ) node = new_node;
	return 0;
}


/*!
    \fn NMXML::addData(NMString tag, NMXML content)
    \brief This function is used to create a tag and add a subtree to it
    \param tag String identifying the tag name to use
    \param content NMXML tree to be added
 */
int NMXML::addData ( NMString tag, NMXML content )
{
	xmlNodePtr new_node;
	if ( tag != "" ) node = xmlNewTextChild ( node, NULL, BAD_CAST tag.c_str(), NULL );
	new_node = xmlDocGetRootElement ( content.doc );
	new_node = xmlDocCopyNodeList ( doc, new_node );
	xmlAddChild ( node, new_node );
	if ( tag != "" ) node = node->parent;
	return 0;
}


/*!
    \fn NMXML::delData(NMString xpath)
    \brief This function is used to delete a node
    \param xpath XPath expression for the node to be deleted
 */
int NMXML::delData ( NMString xpath )
{
	xmlXPathObjectPtr result;
	xmlNodeSetPtr nodeset;

	result = getNodeSet ( BAD_CAST xpath.c_str() );
	if ( !result )
	{
		xmlXPathFreeObject ( result );
		return -1;
	}
	nodeset = result->nodesetval;

	xmlUnlinkNode ( nodeset->nodeTab[0] );
	xmlFreeNode ( nodeset->nodeTab[0] );
	xmlXPathFreeObject ( result );
	return 0;
}


/*!
    \fn NMXML::toString()
    \brief Returns the string representation of the XML tree
 */
NMString NMXML::toString()
{
	int len;
	char *buf;
	NMString result = "";

	xmlDocDumpFormatMemory ( doc, ( xmlChar ** ) &buf, &len, 1 );
	result = buf;
	return result;
}


/*!
    \fn NMXML::toFile(FILE *f)
    \brief Saves the XML tree to a file
    \param f FILE * to save to
 */
int NMXML::toFile ( FILE *f )
{
	return xmlDocFormatDump ( f, doc, 1 );
}


/*!
    \fn NMXML::getElement(const NMString xpath)
 */
NMString NMXML::getElement ( const NMString xpath )
{
	NMString element;
	xmlXPathObjectPtr result;
	xmlNodeSetPtr nodeset;
	int i;

	result = getNodeSet ( BAD_CAST xpath.c_str() );
	if ( !result )
	{
		xmlXPathFreeObject ( result );
		return "";
	}
	nodeset = result->nodesetval;

	const xmlChar *elements[nodeset->nodeNr];

	for ( i = 0; i < nodeset->nodeNr; i++ )
	{
		elements[i] = nodeset->nodeTab[i]->name;
	}
	element = ( char * ) elements[0];
	xmlXPathFreeObject ( result );
	return element;
}


/*!
    \fn NMXML::getValue(const NMString xpath)
    \brief Returns the character data content of the node specified
    \param xpath XPath expression specifying the target node
 */
NMString NMXML::getValue ( const NMString xpath )
{
	return getValue ( xpath, 0 );
}


/*!
    \fn NMXML::getValue(const NMString xpath, int index)
    \brief Returns the character data content of the node specified
    \param xpath XPath expression specifying the target node
    \param index Index of the desired node from the result nodeset
 */
NMString NMXML::getValue ( const NMString xpath, int index )
{
	NMString value;
	xmlXPathObjectPtr result;
	xmlNodeSetPtr nodeset;
	int i;

	result = getNodeSet ( BAD_CAST xpath.c_str() );
	if ( !result )
	{
		xmlXPathFreeObject ( result );
		return "";
	}
	nodeset = result->nodesetval;

	xmlChar *values[nodeset->nodeNr];

	for ( i = 0; i < nodeset->nodeNr; i++ )
	{
		values[i] = xmlNodeListGetString ( doc, nodeset->nodeTab[i]->xmlChildrenNode, 1 );
	}
	value = ( char * ) values[index];
	xmlXPathFreeObject ( result );
	return value;
}


/*!
    \fn NMXML::getTree(NMString xpath, NMString new_root)
    \brief Get the subtree defined by the XPath expression, using new_root as the new root element
    \param xpath XPath expression defining the node to be set as current node
    \param new_root The name of the new root element
 */
NMString NMXML::getTree ( NMString xpath, NMString new_root )
{
	xmlXPathObjectPtr result;
	xmlNodePtr cur;

	result = getNodeSet ( BAD_CAST xpath.c_str() );
	if ( result )
	{
		cout << "getTree working..." << endl;
		cur = result->nodesetval->nodeTab[0]->xmlChildrenNode;
		NMXML new_doc ( new_root );
		cout << new_doc.toString() << endl;
		cur = xmlDocCopyNodeList ( new_doc.doc, cur );
		xmlAddChildList ( new_doc.node, cur );
		NMString tree = new_doc.toString();
		//cout << tree <<"\ngetTree called successfully." << endl;
		return tree;
	}
	cout << "getTree called unsuccessfully." << endl;
	return "";
}


/*!
    \fn NMXML::nodeExists(NMString xpath)
    \brief Check if node exists
 */
bool NMXML::nodeExists ( NMString xpath )
{
	xmlXPathObjectPtr result;

	result = getNodeSet ( BAD_CAST xpath.c_str() );
	if ( result )
	{
		xmlXPathFreeObject ( result );
		return true;
	}
	else
	{
		xmlXPathFreeObject ( result );
		return false;
	}
}


/*!
    \fn NMXML::getNodeSet(xmlChar *xpath)
    \brief Helper function to get the nodeset result for an XPath query
    \param xpath The XPath expression to search for
 */
xmlXPathObjectPtr NMXML::getNodeSet ( xmlChar *xpath )
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext ( doc );
	if ( context == NULL )
	{
		cerr << "NMXML getNodeSet failed: Error in xmlXPathNewContext" << endl;
		return NULL;
	}
	result = xmlXPathEvalExpression ( xpath, context );
	xmlXPathFreeContext ( context );
	if ( result == NULL )
	{
		cerr << "NMXML getNodeSet failed: Error in xmlXPathEvalExpression" << endl;
		return NULL;
	}
	if ( xmlXPathNodeSetIsEmpty ( result->nodesetval ) )
	{
		xmlXPathFreeObject ( result );
		cerr << "NMXML getNodeSet warning: No result for XPath expression" << endl;
		return NULL;
	}
	return result;
}


/*!
    \fn NMXML::goUp()
    \brief Go up one level in the XML tree
 */
void NMXML::goUp()
{
	node = node->parent;
}


/*!
    \fn NMXML::setCurrentNode(NMString xpath)
    \brief Set the node attribute of the NMXML class to the specified node
    \param xpath XPath expression defining the node to be set as current node
 */
int NMXML::setCurrentNode ( NMString xpath )
{
	xmlXPathObjectPtr result;

	result = getNodeSet ( BAD_CAST xpath.c_str() );
	if ( result )
	{
		node = result->nodesetval->nodeTab[0];
		cout << "setCurrentNode called successfully." << endl;
		return 0;
	}
	cout << "setCurrentNode called unsuccessfully." << endl;
	return -1;
}

/*!
 * \brief Applies the xslt transformation to the xmlDoc document
 * \param xmlDocPath XML document
 * \param xsltDocPath xslt file
 * \param resFilePath File to save the transformation
 */
void NMXML::applyXsltToDoc ( const NMString & xmlDocPath, const NMString & xsltDocPath, const NMString & resFilePath )
{
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, res;

	xmlSubstituteEntitiesDefault ( 1 );
	xmlLoadExtDtdDefaultValue = 1;
	cur = xsltParseStylesheetFile ( ( const xmlChar * ) xsltDocPath.c_str() );

	doc = xmlParseFile ( xmlDocPath.c_str() );

	res = xsltApplyStylesheet ( cur, doc,0 );
	/*FILE * confFile = fopen(resFilePath.c_str(),"w");
	xsltSaveResultToFile(confFile, res, cur);
	fclose(confFile);*/
	xsltSaveResultToFilename ( resFilePath.c_str(), res, cur, 0 );

	xsltFreeStylesheet ( cur );
	xmlFreeDoc ( res );
	xmlFreeDoc ( doc );

	xsltCleanupGlobals();
	xmlCleanupParser();
}

/*!
* \brief Applies the xslt transformation to the xmlDoc document
* \param xmlDoc XML document to convert
* \param xsltDocPath xslt stylesheet file path
* \return NMString with the result of the conversion
*/
NMString NMXML::applyXsltToSection ( NMString & xmlDoc, NMString & xsltDocPath )
{
	xmlDocPtr doc, res;
	xsltStylesheetPtr cur = NULL;

	int len;
	char *buf;
	NMString result = "";

	xmlSubstituteEntitiesDefault ( 1 );
	xmlLoadExtDtdDefaultValue = 1;
	cur = xsltParseStylesheetFile ( ( const xmlChar * ) xsltDocPath.c_str() );
	doc = xmlParseFile ( xmlDoc.c_str() );
	res = xsltApplyStylesheet ( cur, doc, 0 );
	xsltSaveResultToString ( ( xmlChar ** ) &buf, &len, res, cur );

	xsltFreeStylesheet ( cur );
	xmlFreeDoc ( res );
	xmlFreeDoc ( doc );

	xsltCleanupGlobals();
	xmlCleanupParser();

	result = buf;
	return result;
}

