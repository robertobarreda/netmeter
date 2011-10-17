/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#ifndef NMGNETPERFXMLINTERPRET_H
#define NMGNETPERFXMLINTERPRET_H

#include <QDomElement>
#include <QXmlStreamWriter>
#include <QString>

#define TAG_TEST_FILE		"test_file"
#define TAG_DECODE_FILE		"decode_file"

class NMGNetperfWidget;

class NMGNetperfXMLInterpret
{
	public:
		/**
		 * Constructor of the class
		 * @param w
		 * @return
		 */
		inline NMGNetperfXMLInterpret ( NMGNetperfWidget * w ) : widget ( w ) {}

		/**
		 * Destroyer of the class
		 * @return
		 */
		inline ~NMGNetperfXMLInterpret() {}
	
		QString getResultFilePathFromResult ( const QString & );
		int parseStatusMessage ( const QString & );
		
		void generateXMLTestDocument ( const QString &, bool );
		QString generateXMLTestDocument ( bool );
		void updateWidgetFromXMLDocument ( const QString & );

	private:
		void createXMLConfNetperf ( QXmlStreamWriter & );
		void createXMLConfNetserver ( QXmlStreamWriter & );
		void updateWidgetFromXMLNodeConfNetperf ( const QString & );

	private:
		NMGNetperfWidget * widget;
};

#endif
