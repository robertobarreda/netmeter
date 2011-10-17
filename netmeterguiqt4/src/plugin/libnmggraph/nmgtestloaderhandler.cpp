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
#include "nmgtestloaderhandler.h"
#include "nmgfloworientedresultdata.h"
#include "nmgglobals.h"
#include <messagebar.h>

NMGTestLoaderHandler::NMGTestLoaderHandler()
{
  filesLoaded.clear();
  filesToLoad.clear();
  readerThread = 0;
  readerThreadCancelled = FALSE;
  progress = 0;
  timer = new QTimer();
  connect(timer,SIGNAL(timeout()),this,SLOT(slotUpdateProgress()));
}

NMGTestLoaderHandler::~NMGTestLoaderHandler() 
{
  if(readerThread) delete readerThread;
  if(progress) progress->setComplete();
  progress = 0;
  if(timer) delete timer;
}

void NMGTestLoaderHandler::loadFiles(const QStringList& fileNames)
{
  QStringList::const_iterator it;
  for(it = fileNames.begin(); it != fileNames.end(); ++it) 
  {
    if((*it).contains(INTERNAL_SEPARATOR))
    {
      NMGGraphicsError e(tr("Loader error"), 
                         tr("Files with %1 character ").arg(INTERNAL_SEPARATOR) + tr("like in ") + 
                         *it + tr(" are not allowed.\n") + tr("Change file name to use it.\n") + tr("File ") + *it + tr(" will be omitted"), 
                         NMGGraphicsError::WARNING);
      e.raiseError();
    }
    else
    {
      if(!filesLoaded.contains(*it)) filesToLoad.enqueue(*it);
      else
      {
        NMGGraphicsError e(tr("Loader error"), tr("File ") + *it + tr(" is already loaded."), 
                          NMGGraphicsError::WARNING);
        e.raiseError();
      }
    }
  }
  loadPendingFiles();
}

void NMGTestLoaderHandler::loadPendingFiles()
{
  if(!filesToLoad.isEmpty())
  {
    if(!readerThread)
    {
      readerThread = new NMGTestReaderThread();

      QString file = filesToLoad.dequeue();
      readerThread->loadFile(file);

      QString filteredFile = file.mid(file.lastIndexOf("/")+1);
      
      progress = ProgressManager::createProgressItem(QString(tr("Loading %1")).arg(filteredFile));
      progress->setTotalItems(100);
      connect(progress, SIGNAL(progressItemCanceled(ProgressItem*)), 
              this, SLOT(slotLoadCancelled(ProgressItem*)));
      
      readerThread->start();
      timer->start(100);
    }
  }
  else MessageBar::longMessage(tr("All files loaded"));
}

NMGTestFileData& NMGTestLoaderHandler::getTestFileData(const QString& fileName) 
    throw (NMGGraphicsError)
{
  if(!filesLoaded.contains(fileName))
  {
    throw NMGGraphicsError(tr("Loader error"),
                           tr("Data from file ") + fileName + 
                           tr(" is unavailable or the file hasn't been loaded previously"),
                           NMGGraphicsError::CRITICAL);
  }
  return *(filesLoaded.find(fileName).value());
}

void NMGTestLoaderHandler::endLoadFile()
{
  if(!readerThread->hasError())
  {
    if(readerThread->getTestFileData()->getResultData()->isDataInvalid())
    {
      NMGGraphicsError e(tr("Loader error"), tr("File ") + readerThread->getFileLoaded() + 
          tr(" has invalid data. In spite of this, the loader will try to load this file."), 
          NMGGraphicsError::WARNING);
      e.raiseError();
    }
    
    QString fileType = readerThread->getTestFileData()->getMetadata()->getTypeOfTest().toUpper();
    SupportedType fileTypeConverted;
    if(fileType == TYPE_PACKETS) fileTypeConverted = PACKETS;
    else if(fileType == TYPE_TPUT) fileTypeConverted = AGGREGATION;
        
    if(!filesLoaded.isEmpty() && fileTypeConverted == typeFilesLoaded)
    {
      // only one file with the same path will be inserted thanks to a previous filter
      filesLoaded.insert(readerThread->getFileLoaded(), readerThread->getTestFileData());
      emit signalFileRead(readerThread->getFileLoaded());
    }
    else
    {
      if(filesLoaded.isEmpty())
      {
        typeFilesLoaded = fileTypeConverted;
        filesLoaded.insert(readerThread->getFileLoaded(), readerThread->getTestFileData());
        emit signalFileRead(readerThread->getFileLoaded());
      }
      else if(fileTypeConverted != typeFilesLoaded)
      {
        NMGGraphicsError e(tr("Loader error"), tr("File ") + readerThread->getFileLoaded() +
                          tr(" has a different test type to the loaded ones."), 
                          NMGGraphicsError::WARNING);
        e.raiseError();
        MessageBar::longMessage(tr("File loading aborted"));
      }
    }
  }
  else
  {
    if(readerThreadCancelled)
    {
      readerThreadCancelled = FALSE;
      MessageBar::longMessage(tr("Load cancelled"));
    }
    else
    {
      NMGGraphicsError e(tr("Loader error"), readerThread->getError(), NMGGraphicsError::CRITICAL);
      e.raiseError();
      MessageBar::longMessage(tr("File loading aborted"));
    }
  }
  delete readerThread;
  readerThread = 0;
  loadPendingFiles();
}

// ********* //
// * SLOTS * //
// ********* //

void NMGTestLoaderHandler::slotDeleteFileData(const QString& id)
{
  // NOTE because INTERNAL_SEPARATOR appears only once in id,
  // identifiers can have only one or two items
    
  QStringList identifiers = id.split(INTERNAL_SEPARATOR, QString::SkipEmptyParts);
  QString beforeSeparator;
  int afterSeparator;
  
  beforeSeparator = identifiers.at(0); // filePath
  if(identifiers.size() == 2) afterSeparator = identifiers.at(1).toInt(); // flow id
  
  Q_ASSERT_X(filesLoaded.contains(beforeSeparator), 
             "slotDeleteFileData",
             QString(tr("Data from file ") + beforeSeparator +
             tr(" cannot be deleted because it doesn't exist\n")).toUtf8().data());
  /* NOTE Assertion should be always true with a correct use, because deletion can 
   * only be done if at least one test file has been previously added. However, it can 
   * occur that deletion of data in loader for some id can derive into a more extensive 
   * programatically deletion (for example when not exists more flows for a specific test,
   * the test itself is also deleted). 
   * Then if the signal notifying the deletion of that test is emitted later, assertion
   * will result false.
   * For that reason has been decided to catch the posible errors and notify them to the
   * programmer (because they mean an incorrect use of the slot), instead of filter them.
   */
  
  switch(typeFilesLoaded)
  {
    case AGGREGATION:
      // delete because we are using pointers to structures 
      // and we must free memory before delete item
      delete filesLoaded.find(beforeSeparator).value();
      // removed is used instead of take method because there aren't repeated keys
      filesLoaded.remove(beforeSeparator);
      emit signalDeleteSeries(beforeSeparator);
      break;
    case PACKETS:
      if(identifiers.size() == 2) 
      {
        // only one flow must be deleted. However, if test has no more flows after delete
        // the flow, it has also to be deleted.
        NMGTestFileData* data = filesLoaded.find(beforeSeparator).value();
        NMGFlowOrientedResultData* result = (NMGFlowOrientedResultData*)data->getResultData();
        result->deleteFlowData(afterSeparator);
        emit signalDeleteSeries(id);
      }
      else if(identifiers.size() == 1)
      { 
        // completed test is deleted. Because flows are converted into series, we must
        // first discover which flows are contained in the test and emit proper deletion 
        // signals to notify external elements that only knows about the existence of 
        // series, to update their state accordingly.
        NMGTestFileData* data = filesLoaded.find(beforeSeparator).value();
        NMGFlowOrientedResultData* result = (NMGFlowOrientedResultData*)data->getResultData();
        
        QMap<int, NMGFlowData*>::const_iterator it;
        for(it = result->constBegin(); it != result->constEnd(); it++) 
        {
          NMGFlowData* flow = it.value();
          QString seriesIdInferedFromFlow;
          seriesIdInferedFromFlow.append(beforeSeparator);
          seriesIdInferedFromFlow.append(INTERNAL_SEPARATOR);
          seriesIdInferedFromFlow.append(QString::number(flow->getFlowId()));
          emit signalDeleteSeries(seriesIdInferedFromFlow);
        }
        delete data;
        filesLoaded.remove(beforeSeparator);
      }
      break;
  }
}

void NMGTestLoaderHandler::slotUpdateProgress()
{
  int percent = readerThread->getProcessPercent();
  QString status = readerThread->getProcessStatus();
  bool ended = readerThread->isFinished();
  if (ended) 
  {
    timer->stop();
    progress->setComplete();
    progress = 0;

    endLoadFile();
  }
  else
  {
    progress->setStatus(status);
    progress->setCompletedItems(percent);
    progress->updateProgress();
  }
}

void NMGTestLoaderHandler::slotLoadCancelled(ProgressItem*) 
{  
  readerThreadCancelled = TRUE;
  readerThread->kill();
}

#include "nmgtestloaderhandler.moc"
