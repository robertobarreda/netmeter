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
#include "nmgplotwidget.h"
#include <QFileDialog>
#include <QSvgGenerator>
#include <QPrinter>
#include <QApplication>
//#ifndef QT_NO_OPENGL
//#include <QtOpenGL>
//#endif

NMGPlotWidget::NMGPlotWidget(QWidget* parent)
  :QGraphicsView(parent)
{
  // Commented for performance issues. Uncomment for obtain experimental accelerated graphics.
  //#ifndef QT_NO_OPENGL
  //  setViewport(new QGLWidget(QGLFormat(QGL::DoubleBuffer)));
  //#endif
  //#ifdef QT_NO_OPENGL
  setViewport(new QWidget);
  //#endif
  setScene(&scene);
  connect(this, SIGNAL(signalResize(const int&, const int&)),
          &scene, SLOT(slotResizeScene(const int&, const int&)));
  connect(&scene, SIGNAL(signalChartTypeChanged(SupportedType, ChartType)),
          this, SIGNAL(signalChartTypeChanged(SupportedType, ChartType)));
  connect(&scene, SIGNAL(signalAddedNewChartItem(NMGChartItem*)),
          this, SIGNAL(signalAddedNewChartItem(NMGChartItem*)));
  connect(&scene, SIGNAL(signalAddedNewChartItems(QList<NMGChartItem*>&)),
          this, SIGNAL(signalAddedNewChartItems(QList<NMGChartItem*>&)));
  connect(&scene, SIGNAL(signalSeriesTextChanged(const QString&, const QString&)),
          this, SIGNAL(signalSeriesTextChanged(const QString&, const QString&)));
  connect(&scene, SIGNAL(signalSeriesColorChanged(const QString&, const QColor&)),
          this, SIGNAL(signalSeriesColorChanged(const QString&, const QColor&)));
  connect(&scene, SIGNAL(signalSeriesDeleted(const QString&)),
          this, SIGNAL(signalSeriesDeleted(const QString&)));
  connect(&scene, SIGNAL(signalEmptyScene()), this, SIGNAL(signalEmptyPlot()));
  connect(&scene, SIGNAL(signalAxisRangesChanged(const double&, const double&, int,
                                                 const double&, const double&, int,
                                                 ChartType)), 
          this, SIGNAL(signalAxisRangesChanged(const double&, const double&, int,
                                               const double&, const double&, int,
                                               ChartType)));
  connect(&scene, SIGNAL(signalZoomRestoringNeeded()),
          this, SIGNAL(signalZoomRestoringNeeded()));
}

void NMGPlotWidget::addNewTest(const QString& fileName, NMGTestLoaderHandler& loader)
{
  scene.addNewTest(fileName, loader);
}

void NMGPlotWidget::resizeEvent(QResizeEvent *event)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  emit signalResize(width()-4, height()-4); 
  // NOTE -4 is a correction to an unaccountable X11 problem
  QGraphicsView::resizeEvent(event);
  QApplication::restoreOverrideCursor();
}

// ********* //
// * SLOTS * //
// ********* //

void NMGPlotWidget::slotExportTo(NMGModulePreferencesManager* preferences)
{
  QString lastExportPath;
  if(preferences)
  {
    lastExportPath = preferences->getValue("GRAPHICS_EXPORT_PATH");
  }
  if(lastExportPath.isEmpty()) lastExportPath = QDir::homePath();
  
  QString filters = tr("Portable Network Graphics (*.png);;") + 
                    tr("Joint Photographic Experts Group (*.jpg);;") + 
                    tr("Scalable Vector Graphics (*.svg);;") + 
                    tr("Portable Document Format(*.pdf)");
  QFileDialog dialog(this, tr("Exporting to..."), lastExportPath, filters);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setLabelText(QFileDialog::Accept, tr("&Export"));
  if(dialog.exec())
  {
    QString exportFile = dialog.selectedFiles().value(0);
    if (!exportFile.isEmpty())
    {
      // save export path
      if(preferences)
      {
        QString path = exportFile;
        int lastPos = path.lastIndexOf("/",-1);
        preferences->addValue("GRAPHICS_EXPORT_PATH", path.left(lastPos));
      }
      QString selectedFilter = dialog.selectedFilter();
      if(selectedFilter.contains("*.png")) exportToPNG(exportFile);
      else if(selectedFilter.contains("*.jpg")) exportToJPG(exportFile);
      else if(selectedFilter.contains("*.svg")) exportToSVG(exportFile);
      else if(selectedFilter.contains("*.pdf")) exportToPDF(exportFile);
    }
  }
}

void NMGPlotWidget::exportToPNG(QString filePath)
{
  if(!filePath.endsWith(".png", Qt::CaseInsensitive)) filePath.append(".png");
  QPixmap pix(width()-4, height()-4);
  pix.fill(Qt::white);
  QPainter p(&pix);
  render(&p);
  pix.save(filePath, "PNG");
}

void NMGPlotWidget::exportToJPG(QString filePath)
{
  if(!filePath.endsWith(".jpg", Qt::CaseInsensitive)) filePath.append(".jpg");
  QPixmap pix(width()-4, height()-4);
  pix.fill(Qt::white);
  QPainter p(&pix);
  render(&p);
  pix.save(filePath, "JPG", 100);
}

void NMGPlotWidget::exportToSVG(QString filePath)
{
  if(!filePath.endsWith(".svg", Qt::CaseInsensitive)) filePath.append(".svg");
  QSvgGenerator svgGen;
  svgGen.setFileName(filePath);
  svgGen.setSize(QSize(width()-4,height()-4));
  QPainter p(&svgGen);
  render(&p);
}

void NMGPlotWidget::exportToPDF(QString filePath)
{
  if(!filePath.endsWith(".pdf", Qt::CaseInsensitive)) filePath.append(".pdf");
  QPrinter printer(QPrinter::HighResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setOutputFileName(filePath);
  printer.setOrientation(QPrinter::Landscape);
  printer.setPageSize(QPrinter::A4);
  printer.setResolution(96); // by default should be 1200 but then text is not correctly sized
  QPainter pdfPainter(&printer);
  render(&pdfPainter);
}

#include "nmgplotwidget.moc"
