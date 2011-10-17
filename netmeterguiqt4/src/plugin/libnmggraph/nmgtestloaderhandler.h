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
#ifndef NMGTESTLOADERHANDLER_H
#define NMGTESTLOADERHANDLER_H

#include "nmggraphicserror.h"
#include "nmgtestfiledata.h"
#include "nmgfilecontainer.h"
#include "nmgtestreaderthread.h"
#include "nmgglobals.h"
#include <progressmanager.h>
#include <QTimer>
#include <QStringList>
#include <QQueue>
using namespace Globals;

/**
 * \brief NMGTestLoaderHandler is the class responsible for loading test files into the plugin.
 *
 * This class not only deals with the loading aspects but it saves also the processed information 
 * contained in the files loaded.
 */
class NMGTestLoaderHandler : public QObject
{
  Q_OBJECT

  public:

    /** \brief Default constructor. */
    NMGTestLoaderHandler();

    /** \brief Default destructor. */
    ~NMGTestLoaderHandler();

    /**
     * \brief Notifies to the loader that new files must be loaded. 
     *
     * The new files will be loaded one behind the other in an execution thread.
     * \param fileNames Paths where files to be loaded are located.
     */
    void loadFiles(const QStringList& fileNames);

    /**
     * \brief Returns all the information of a loaded test file.
     * \param fileName Path where the loaded file is located.
     * \throw NMGGraphicsError An error occurs if this method is called when the \a fileName 
     * hasn't been loaded previously or if been loaded, the data has been removed by others ways.
     */
    NMGTestFileData& getTestFileData(const QString& fileName) throw (NMGGraphicsError);
    
    /** \brief Returns the number of tests currently loaded. */
    int numberOfCurrentLoadedTests() const { return filesLoaded.size(); }

  signals:

    /**
     * \brief This signal is emitted when the loading of a file has ended successfully.
     * \param fileName Path of the loaded file.
     */
    void signalFileRead(const QString& fileName);
        
    /**
     * \brief This signal is emitted to notify plotting component that the series identified
     * with \e potentialSeriesId should be deleted because data from which series is supplied, 
     * will be no longer available.
     * 
     * This signal is not suitable in this class in terms of design, because it creates an
     * extra degree of coupling (both, loader and plotting component knows how series ids 
     * are build) but it is necessary because after data deletion in the loader,
     * there is no other way to extract the information needed by plotting component to do a 
     * delete process and maintain its state updated.
     */
    void signalDeleteSeries(const QString& potentialSeriesId);

  public slots:

    /**
     * \brief This slot can be connected if it is necessary remove data loaded for a specific
     * test file.
     * 
     * If this slot is called to delete the same test more than once, an internal error occurs 
     * because data was deleted the first time. Correctly used, this slot shouldn't produce any
     * error. The internal error appearance must be a hint for the programmer that something 
     * wrong is done and it must be corrected.
     * \param id Internal id (contains test file path used in loader handler).
     */
    void slotDeleteFileData(const QString& id);

  private slots:

    void slotUpdateProgress();
    void slotLoadCancelled(ProgressItem*);

  private:

    void loadPendingFiles();
    void endLoadFile();

    QQueue<QString> filesToLoad;
    NMGFileContainer filesLoaded;
    SupportedType typeFilesLoaded;
    QTimer* timer;
    NMGTestReaderThread* readerThread;
    bool readerThreadCancelled;
    ProgressItem* progress;
};

#endif
