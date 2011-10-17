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
#ifndef NMGCOMPRESSEDDEVICE_H
#define NMGCOMPRESSEDDEVICE_H

#include <QIODevice>
#include <zlib.h>

class NMGGzipFile : public QIODevice
{
		Q_OBJECT

	public:
		NMGGzipFile ( const QString& path );
		inline ~NMGGzipFile() {}

		virtual bool open ( OpenMode mode );
		virtual void close();

		virtual bool isSequential () const;

		virtual QString fileName() const;

		virtual bool seek ( qint64 pos );

	protected:
		virtual qint64 readData ( char *data, qint64 maxSize );
		virtual qint64 writeData ( const char *data, qint64 maxSize );

	protected:
		gzFile mHandle;
		QString mPath;
};

#endif
