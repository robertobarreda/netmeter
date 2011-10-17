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
#include "nmggraphicswidget.h"
#include "nmgtestloaderhandler.h"
#include <nmgmoduleutils.h>
#include <messagebar.h>
#include <QFileDialog>
#include <QMessageBox>

#define TAG_TEST "test"
#define TAG_DATA "data"

NMGGraphicsWidget::NMGGraphicsWidget(QWidget *parent, 
                                     const char *name, 
                                     NMGModulePreferencesManager * preferences)
  : QWidget(parent), preferences(preferences)
{
  setObjectName(name);
  setupUi(this);
  toolbar = 0;
  loader = 0;
  createComponents();
  createConnections();
}

void NMGGraphicsWidget::createComponents()
{
  // TOOLBAR
  if(!toolbar) toolbar = new NMGToolBar(NMGModuleUtils::getMainWindow(), tr("Graphic Analysis"));
  
  // TEST LOADER
  if(!loader) loader = new NMGTestLoaderHandler();
}

void NMGGraphicsWidget::createConnections()
{
  connect(toolbar, SIGNAL(signalAddTests()), this, SLOT(slotAddTests()));
  connect(toolbar, SIGNAL(signalDeleteSelectedMeasures()),
          measureListWidget, SLOT(removeSelectedItems()));
  connect(toolbar, SIGNAL(signalDeleteAllMeasures()),
          this, SLOT(slotDeleteAllMeasures()));
  connect(toolbar, SIGNAL(signalExportTo()), this, SLOT(slotExportTo()));
  connect(toolbar, SIGNAL(signalChartTypeChanged(ChartType)), 
          plotWidget, SLOT(slotSetChartType(ChartType)));
  connect(toolbar, SIGNAL(signalChartTypeChanged(ChartType)), 
          propertyEditor, SLOT(slotRebuildProperties()));
  connect(toolbar, SIGNAL(signalZoom(const double&, const double&, const double&, const double&)),
          plotWidget, SLOT(slotZoom(const double&, const double&, const double&, const double&)));
  connect(toolbar, SIGNAL(signalRestoreZoom()),
          plotWidget, SLOT(slotRestoreZoom()));
  
  connect(loader, SIGNAL(signalFileRead(const QString&)),
          this, SLOT(slotProcessFileRead(const QString&)));
  connect(loader, SIGNAL(signalDeleteSeries(const QString&)),
          plotWidget, SLOT(slotDeleteSeries(const QString&)));

  connect(plotWidget, SIGNAL(signalChartTypeChanged(SupportedType, ChartType)),
          toolbar, SLOT(slotChangeChartType(SupportedType, ChartType)));
  connect(plotWidget, SIGNAL(signalAddedNewChartItem(NMGChartItem*)), 
          objectInspector, SLOT(slotAddNewChartItem(NMGChartItem*)));
  connect(plotWidget, SIGNAL(signalAddedNewChartItems(QList<NMGChartItem*>&)),
          objectInspector, SLOT(slotAddNewChartItems(QList<NMGChartItem*>&)));
  
  connect(plotWidget, SIGNAL(signalSeriesTextChanged(const QString&, const QString&)),
          objectInspector, SLOT(slotChangeSeriesText(const QString&, const QString&)));
  connect(plotWidget, SIGNAL(signalSeriesColorChanged(const QString&, const QColor&)),
          objectInspector, SLOT(slotChangeSeriesColor(const QString&, const QColor&)));
  connect(plotWidget, SIGNAL(signalSeriesTextChanged(const QString&, const QString&)),
          measureListWidget, SLOT(slotChangeMeasureListItemText(const QString&, const QString&)));
  connect(plotWidget, SIGNAL(signalSeriesColorChanged(const QString&, const QColor&)),
          measureListWidget, SLOT(slotChangeMeasureListItemColor(const QString&, const QColor&)));
  connect(plotWidget, SIGNAL(signalAxisRangesChanged(const double&, const double&, int,
                                                     const double&, const double&, int, ChartType)),
          toolbar, SLOT(slotChangeAxisRanges(const double&, const double&, int,
                                             const double&, const double&, int, ChartType)));
  connect(plotWidget, SIGNAL(signalZoomRestoringNeeded()),
          toolbar, SLOT(slotRestoreZoom()));
  
  connect(plotWidget, SIGNAL(signalSeriesDeleted(const QString&)),
          objectInspector, SLOT(slotDeleteSeries(const QString&)));
  connect(plotWidget, SIGNAL(signalEmptyPlot()),
          toolbar, SLOT(slotEmptyPlot()));

  connect(objectInspector, SIGNAL(signalSelectedChartItem(NMGChartItem*)), 
          propertyEditor, SLOT(slotSetupProperties(NMGChartItem*)));
  
  connect(measureListWidget, SIGNAL(itemRemoved(const QString&)),
          loader, SLOT(slotDeleteFileData(const QString&)));  
  connect(measureListWidget, SIGNAL(signalValidAtomicItemMoved(const QString&, int)),
          plotWidget, SLOT(slotOrderSeries(const QString&, int)));
  connect(measureListWidget, SIGNAL(signalValidAtomicItemMoved(const QString&, int)),
          objectInspector, SLOT(slotChangeSeriesOrder(const QString&, int)));
}

NMGGraphicsWidget::~NMGGraphicsWidget() 
{
  if(toolbar) delete toolbar;
  if(loader) delete loader;
}

NMGToolBar* NMGGraphicsWidget::getToolBar()
{
  return toolbar;
}

// ********* //
// * SLOTS * //
// ********* //

void NMGGraphicsWidget::slotAddTests()
{
  QString lastTestPath = preferences->getValue("GRAPHICS_TEST_PATH");
	
  QStringList fileNames = QFileDialog::getOpenFileNames(this, 
                                                        tr("Select test files"), 
                                                        lastTestPath, 
                                                        tr("XML Flow Format (*.xff)"));
	
  if (!fileNames.isEmpty())
  { // at least there is one file
    QStringList::Iterator it = fileNames.begin();
    // save file path
    int lastPos = it->lastIndexOf("/",-1);
    preferences->addValue("GRAPHICS_TEST_PATH",it->left(lastPos));

    loader->loadFiles(fileNames);
  }
}

void NMGGraphicsWidget::slotDeleteAllMeasures()
{
  if(!measureListWidget->isEmpty())
  {
    QMessageBox message(QMessageBox::Question,
                        "Delete all measures",
                        "Do you really want to delete all measures?",
                        QMessageBox::Yes | QMessageBox::No,
                        NMGModuleUtils::getMainWindow());
    message.setDefaultButton(QMessageBox::No);
    message.setEscapeButton(QMessageBox::No);
    if(message.exec() == QMessageBox::Yes) measureListWidget->slotDeleteAllItems();
  }
}

void NMGGraphicsWidget::slotExportTo()
{
  plotWidget->slotExportTo(preferences);
}

void NMGGraphicsWidget::slotProcessFileRead(const QString& fileName)
{
  // notify measure list
  measureListWidget->addNewTest(fileName, *loader);
  
  // notify plot widget
  plotWidget->addNewTest(fileName, *loader);
}

bool NMGGraphicsWidget::loadData ( QString data )
{
        QStringList fileNames = QStringList();
	QXmlStreamReader xml ( data );

	/** \par Received Message Format to START command when the plugin is subscribed
	 * <data>
	 *  	<test>testPath</test>
	 * 	...
	 * </data>
	 **/

	bool error = true;
	QString filePath;
	
	while ( !xml.atEnd() )
	{
		xml.readNext();
		if ( xml.isStartElement() )
		{
			if ( xml.name() == TAG_DATA ) error = false;
			else if ( xml.name() == TAG_TEST and !error )
			{
				filePath = xml.readElementText();
				if ( ( error = filePath.isEmpty() ) ) xml.raiseError ( "[ERROR] Empty test path" );
				else fileNames.append( filePath );
			}
			else 
			{
				xml.raiseError ( "[ERROR] Error in XML format" );
				error = true;
			}
		}
	}
	if ( xml.hasError() )
		cout << qPrintable ( xml.errorString () ) << endl;
        else
                loader->loadFiles(fileNames);

	return !error;
}

#include "nmggraphicswidget.moc"
