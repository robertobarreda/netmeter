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
#include "nmggzipfile.h"

NMGGzipFile::NMGGzipFile(const QString & path)
{
	mPath = path;
	mHandle = NULL;
}

bool NMGGzipFile::open(OpenMode mode)
{
	if(mode & QIODevice::ReadOnly)
		mHandle = gzopen(mPath.toLocal8Bit().data(), "rb");
	else if(mode & QIODevice::ReadWrite)
		mHandle = gzopen(mPath.toLocal8Bit().data(), "wb+");
	else if(mode & QIODevice::WriteOnly)
		mHandle = gzopen(mPath.toLocal8Bit().data(), "wb");

	if(mHandle == NULL)
		return false;

	return QIODevice::open(mode);
}

void NMGGzipFile::close()
{
	QIODevice::close();
	if(mHandle) gzclose(mHandle);
	mHandle = NULL;
}

bool NMGGzipFile::isSequential() const
{
	return true;
}

QString NMGGzipFile::fileName() const
{
	return mPath;
}

bool NMGGzipFile::seek(qint64 pos)
{
	return (gzseek(mHandle, pos, SEEK_SET) != -1);
}

qint64 NMGGzipFile::readData(char *data, qint64 maxSize)
{
	if(mHandle == NULL)
		return 0;

	return gzread(mHandle, data, maxSize);
}

qint64 NMGGzipFile::writeData(const char *data, qint64 maxSize)
{
	if(mHandle == NULL)
		return 0;

	return gzwrite(mHandle, data, maxSize);
}

#include "nmggzipfile.moc"
