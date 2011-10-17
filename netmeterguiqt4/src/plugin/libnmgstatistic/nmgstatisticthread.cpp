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
#include "nmgstatisticthread.h"
#include <nmgmd5.h>
#include <nmggzipfile.h>
#include <nmstring.h>
//#include <QFile>

NMGStatisticThread::NMGStatisticThread( NMGStatisticData * stats, NMGStatisticMetaData * meta, QString & pathid )
{
	statistics = stats;
	metadata = meta;
	filePathId = pathid;

	metaParser = 0;
	statisticParser = 0;
	error = FALSE;
	errorMsg = testType = fileMD5Id = QString();
}

NMGStatisticThread::~NMGStatisticThread()
{
	if ( metaParser ) delete metaParser;
	if ( statisticParser ) delete statisticParser;
}

int NMGStatisticThread::getProcessPercent() const
{
	if ( metaParser ) return metaParser->getCompletedPercent();
	else if ( statisticParser ) return statisticParser->getCompletedPercent();
	else return 0;
}

QString NMGStatisticThread::getProcessStatus() const
{
	if ( metaParser ) return metaParser->getStatus();
	else if ( statisticParser ) return statisticParser->getStatus();
	else return QObject::tr( "unknown" );
}

void NMGStatisticThread::run()
{
	bool forceReparse;
	if ( ( forceReparse = filePathId.startsWith( REPARSE_FLAG ) ) ) filePathId = filePathId.remove( REPARSE_FLAG );
	
	// Timing
	//QTime t;
	//t.start();

	//MD5 process
	fileMD5Id = DMD5Hash::hashFile( filePathId );
	//qDebug("Time elapsed for MD5: %d ms", t.elapsed());

	if ( !metadata->contains( fileMD5Id ) or forceReparse )
	{
		if ( forceReparse )
		{
			cout << "[INFO] File " << qPrintable( filePathId ) << " reloaded" << endl;
			metadata->remove( fileMD5Id );
			statistics->remove( fileMD5Id );
		}
		if ( readMetadata() )
		{
			if ( !statistics->contains( fileMD5Id ) or forceReparse )
				readResult();
			else if ( statistics->contains( fileMD5Id ) and !forceReparse )
                                // Colored output (light blue)! from nmgutil.h
                                cout << LIGHTBLUE << "[INFO] File " << qPrintable( filePathId ) \
                                     << " recovered from cache" << ENDCOLOR << endl;
			else
			{
				error = TRUE;
				errorMsg = QObject::tr( "File " )  + filePathId + QObject::tr( " has an unsupported type of result data or it is undefined" );
			}
			//qDebug("Time elapsed for PARSING: %d ms", t.elapsed());
		}
	}
	else
	{
		//"File already parsed. Not parsed again"
		error = TRUE;
		errorMsg = TESTALREADYPARED;
	}
}

bool NMGStatisticThread::readMetadata()
{
	NMGGzipFile * file = new NMGGzipFile(filePathId);
	//QFile * file( filePathId );

	if ( !file->open( QFile::ReadOnly | QFile::Text ) )
	{
		error = TRUE;
		errorMsg = QObject::tr( "File " ) + filePathId + QObject::tr( " couldn't be read" );
	}
	else
	{
		metaParser = new NMGMetaDataParser( *( metadataTest = new NMGMetaData() ) );
		if ( !metaParser->read( file ) )
		{
			error = TRUE;
			errorMsg = QObject::tr( "Parse error reading metadata in file " ) + file->fileName() + QString( ":\n" ) + metaParser->errorString();
		}
		file->close();
		
		testType = metadataTest->getTypeOfTest().toLower();

		//Backward compatibility with older test files that doesn't have <type> tag
		if ( testType.isEmpty() )
		{
			if ( metadataTest->isSetUncommonElement( TAG_CONF_MGEN ) ) metadataTest->setTypeOfTest( TYPE_PACKETS );
			else if ( metadataTest->isSetUncommonElement( TAG_CONF_NETPERF ) ) metadataTest->setTypeOfTest( TYPE_TPUT );
			testType = metadataTest->getTypeOfTest().toLower();
		}

		metadata->insert( fileMD5Id, metadataTest );

		delete metaParser;
		metaParser = 0;
	}
	delete file;
	file = 0;

	return !error;
}

bool NMGStatisticThread::readResult( )
{
	NMGGzipFile * file = new NMGGzipFile(filePathId);
	//QFile * file( filePathId );

	if ( !file->open( QFile::ReadOnly | QFile::Text ) )
	{
		error = TRUE;
		errorMsg = QObject::tr( "File " ) + filePathId + QObject::tr( " couldn't be read" );
	}
	else
	{
		if ( testType == TYPE_PACKETS )
			statisticParser = new NMGPacketStatisticParser( statistics, fileMD5Id, 
				metadataTest->getUncommonElement( TAG_CONF_MGEN ) );
		else if ( testType == TYPE_TPUT )
			statisticParser = new NMGAggregationStatisticParser( statistics, fileMD5Id );
		else
		{
			// by default, it's supposed to be TYPE_PACKETS
			statisticParser = new NMGPacketStatisticParser( statistics, fileMD5Id, 
				metadataTest->getUncommonElement( TAG_CONF_MGEN ) );
		}

		if ( !statisticParser->read( file ) )
		{
			error = TRUE;
			errorMsg = QObject::tr( "Parse error reading result data in file " )
			           + file->fileName() + QString( ":\n" ) + statisticParser->errorString();
		}
		file->close();
		
		delete statisticParser;
		statisticParser = 0;
	}
	delete file;
	file = 0;

	return !error;
}

void NMGStatisticThread::kill()
{
	error = TRUE;
	if ( metaParser ) metaParser->stop();
	if ( statisticParser ) statisticParser->stop();
}
