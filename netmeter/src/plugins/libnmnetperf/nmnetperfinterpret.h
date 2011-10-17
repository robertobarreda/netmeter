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
#ifndef NMNETPERFINTERPRET_H
#define NMNETPERFINTERPRET_H

#include "nmnetperfinputfields.h"

#define TAG_RESULT			"result"
#define PATH_XML_RESULT			"/test/result"
#define PATH_XML_SOURCE_USERNAME	"/test/metadata/source/management/username"
#define PATH_XML_SOURCE_PASSWORD	"/test/metadata/source/management/password"
#define PATH_XML_DEST_USERNAME		"/test/metadata/destination/management/username"
#define PATH_XML_DEST_PASSWORD		"/test/metadata/destination/management/password"

class NMNetperfInterpret
{
	public:
		/**
		* \brief Contructor of the class
		*/
		NMNetperfInterpret();
		/**
		* \brief Destructor of the class
		*/
		inline ~NMNetperfInterpret() {}
		/**
		 * \brief Parses the start XML document
		 * \param xml input source to parse
		 * \param filesSuffixPath 
		 * \return bool is TRUE is everything is correct, otherwise, returns FALSE
		 */
		bool parseStartXML ( NMString *, NMString );
		/**
		* \brief Returns the input fields of the parsed XML document
		* \return NMNetperfInputFields filled with the test information
		*/
		inline NMNetperfInputFields * getInputFields() { return &inputFields; }
		/**
		 * \brief Generates the xml document for the result of the test
		 * \param testFile Local path to the test file
		 * \param decodeFile Local path to the decode file
		 * \return NMString with the XML
		 */
		NMString getXMLResult ( const NMString &, const NMString & );
		/**
		 * \brief Converts the decode output file to XML
		 * \param decodeOutputFile path to the decode file output
		 * \return NMString with the file name of the XML result file 
		 */
		NMString addResultToTestFile ( const NMString & );

	private:
		/**
		* \brief Cleans the input fields
		*/
		void cleanInputFields();

		NMNetperfInputFields inputFields;
		NMString testFileContent;
		NMString metadataContent;
		NMString testFileName;
};

#endif
