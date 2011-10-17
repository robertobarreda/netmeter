/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#include "nmgtestreaderthread.h"
#include "nmgfloworientedresultdata.h"
#include "nmgaggregationorientedresultdata.h"
#include "nmggraphpacketorienteddataparser.h"
#include "nmggraphaggregationorienteddataparser.h"
#include <nmgmetadatatags.h>
#include <QFile>
using namespace Globals;

NMGTestReaderThread::NMGTestReaderThread()
{
  fileName.clear();
  testData = 0;
  metaParser = 0;
  dataParser = 0;
  error = FALSE;
  errorMsg = "";
}

NMGTestReaderThread::~NMGTestReaderThread()
{
  if(metaParser) delete metaParser;
  if(dataParser) delete dataParser;
}

void NMGTestReaderThread::loadFile(const QString& fileName)
{
  this->fileName = fileName;
}

QString NMGTestReaderThread::getFileLoaded() const
{
  return fileName;
}

NMGTestFileData* NMGTestReaderThread::getTestFileData() const
{
  return (testData ? testData : 0);
}

int NMGTestReaderThread::getProcessPercent() const
{
  if(metaParser) return metaParser->getCompletedPercent();
  else if(dataParser) return dataParser->getCompletedPercent();
  else return 0;
}

QString NMGTestReaderThread::getProcessStatus() const
{
  if(metaParser) return metaParser->getStatus();
  else if(dataParser) return dataParser->getStatus();
  else return QObject::tr("unknown");
}

bool NMGTestReaderThread::hasError() const
{
  return error;
}

QString NMGTestReaderThread::getError() const
{
  return errorMsg;
}

void NMGTestReaderThread::run()
{
  NMGMetaData metadata;
  if(readMetadata(metadata))
  {
    QString stringType = metadata.getTypeOfTest().toUpper();
    SupportedType type;
    if(stringType == TYPE_PACKETS)
    {
      type = PACKETS;
      testData = new NMGTestFileData(type);
      testData->setMetadata(metadata);
    }
    else if(stringType == TYPE_TPUT)
    {
      type = AGGREGATION;
      testData = new NMGTestFileData(type);
      testData->setMetadata(metadata);
    }
    else if(metadata.isSetUncommonElement(TAG_CONF_MGEN))
    {
      //Backward compatibility with older test files that doesn't have <type> tag
      type = PACKETS;
      metadata.setTypeOfTest("packets");
      testData = new NMGTestFileData(type);
      testData->setMetadata(metadata);
    }
    else if(metadata.isSetUncommonElement(TAG_CONF_NETPERF))
    {
      //Backward compatibility with older test files that doesn't have <type> tag
      type = AGGREGATION;
      metadata.setTypeOfTest("throughput");
      testData = new NMGTestFileData(type);
      testData->setMetadata(metadata);
    }
    else
    {
      QString customError;
      customError.append(QObject::tr("File ")).append(fileName);
      customError.append(QObject::tr(" has an unsupported type of result data or it is undefined"));
      errorMsg = customError;
      error = TRUE;
    }
    if(!error)
    {
      if(!readData(type, testData->getResultData()))
      {
        delete testData;
        testData = 0;
      }
    }
  }
}

bool NMGTestReaderThread::readMetadata(NMGMetaData& metadata)
{ 
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) 
  {
    error = TRUE;
    errorMsg = QObject::tr("File ") + fileName + QObject::tr(" couldn't be read");
  }

  if(!error)
  {
    metaParser = new NMGMetaDataParser(metadata);
    if (!metaParser->read(&file))
    {
      QString customError;
      customError.append(QObject::tr("Parse error reading metadata in file "));
      customError.append(file.fileName());
      customError.append(":\n").append(metaParser->errorString());
      errorMsg = customError;
      error = TRUE;
    }
    delete metaParser;
    metaParser = 0;
  }
  return !error;
}

bool NMGTestReaderThread::readData(SupportedType fileType, NMGResultData* data)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) 
  {
    error = TRUE;
    errorMsg = QObject::tr("File ") + fileName + QObject::tr(" couldn't be read");
  }

  if(!error)
  {
    if(fileType == PACKETS)
    {
      dataParser = new NMGGraphPacketOrientedDataParser((NMGFlowOrientedResultData*)data);
    }
    else if(fileType == AGGREGATION)
    {
      dataParser = new NMGGraphAggregationOrientedDataParser((NMGAggregationOrientedResultData*)data);
    }

    if(!dataParser->read(&file))
    {
      QString customError;
      customError.append(QObject::tr("Parse error reading result data in file "));
      customError.append(file.fileName());
      customError.append(":\n").append(dataParser->errorString());
      errorMsg = customError;
      error = TRUE;
    }
    delete dataParser;
    dataParser = 0;
  }
  return !error;
}

void NMGTestReaderThread::kill()
{
  error = TRUE;
  if(metaParser) metaParser->stop();
  if(dataParser) dataParser->stop(); 
}
