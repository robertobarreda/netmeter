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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "nmgstatisticexportmanager.h"
#include "nmgstatisticxmltags.h"
#include <nmgmetadatatags.h>
#include <nmxml.h>
#include <messagebar.h>

#include <QFile>
#include <QDir>
#include <QList>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>

#define ID_PREFERENCE_STATISTIC_EXPORT_DIR		"StatisticExportDir"
#define ID_PREFERENCE_STATISTIC_EXPORT_OPTIONS_VALUES	"StatisticExportOptionsValues"
#define ID_PREFERENCE_STATISTIC_EXPORT_OPTIONS_FORMAT	"StatisticExportOptionsFormat"

#define ON 	QChar ('1')
#define OFF 	QChar ('0')

#define DATE()	QDateTime::currentDateTime ().toString("ddMMyyyy-hhmmss")

NMGStatisticExportManager::NMGStatisticExportManager ( QWidget * parent, NMGModulePreferencesManager * prefMan )
		: QDialog ( parent ), preferencesManager ( prefMan )
{
	setObjectName ( "NMGStatisticExportManager" );
	setupUi ( this );
	hide();

	loadPreferences();
	connect ( btnOk, SIGNAL ( clicked ( bool ) ), this, SLOT ( savePreferences() ) );
	connect ( btnCancel, SIGNAL ( clicked ( bool ) ), this, SLOT ( loadPreferences() ) );
	connect ( btnExportPath, SIGNAL ( clicked ( bool ) ), this, SLOT ( selectResultDirectory() ) );
}

NMGStatisticExportManager::~NMGStatisticExportManager() {}

/********************************************************************/
/* Preferences Options 						    */
/********************************************************************/
void NMGStatisticExportManager::loadPreferences()
{
	txtExportDir->setText ( preferencesManager->getValue ( ID_PREFERENCE_STATISTIC_EXPORT_DIR ) );

	int i = 0;
	QString op = preferencesManager->getValue ( ID_PREFERENCE_STATISTIC_EXPORT_OPTIONS_VALUES );
	if ( !op.isEmpty() and op.size() >= 21 )
	{
		// Delay (Mgen)
		chkDelayAve->setChecked ( op.at ( i++ ) == ON );
		chkDelayMin->setChecked ( op.at ( i++ ) == ON );
		chkDelayMax->setChecked ( op.at ( i++ ) == ON );
		chkDelayStDev->setChecked ( op.at ( i++ ) == ON );
		chkDelayJitter->setChecked ( op.at ( i++ ) == ON );
		chkDelayP50->setChecked ( op.at ( i++ ) == ON );
		chkDelayP95->setChecked ( op.at ( i++ ) == ON );
		chkDelayP99->setChecked ( op.at ( i++ ) == ON );
		chkDelayP999->setChecked ( op.at ( i++ ) == ON );

		// IPDV (Mgen)
		chkIPDVAve->setChecked ( op.at ( i++ ) == ON );
		chkIPDVMin->setChecked ( op.at ( i++ ) == ON );
		chkIPDVMax->setChecked ( op.at ( i++ ) == ON );
		chkIPDVStDev->setChecked ( op.at ( i++ ) == ON );
		chkIPDVJitter->setChecked ( op.at ( i++ ) == ON );
		chkIPDVP50->setChecked ( op.at ( i++ ) == ON );
		chkIPDVP95->setChecked ( op.at ( i++ ) == ON );
		chkIPDVP99->setChecked ( op.at ( i++ ) == ON );
		chkIPDVP999->setChecked ( op.at ( i++ ) == ON );

		// Throughtput (Netperf)
		chkThrAve->setChecked ( op.at ( i++ ) == ON );
		chkThrMin->setChecked ( op.at ( i++ ) == ON );
		chkThrMax->setChecked ( op.at ( i++ ) == ON );
	}

	i = 0;
	op = preferencesManager->getValue ( ID_PREFERENCE_STATISTIC_EXPORT_OPTIONS_FORMAT );
	if ( !op.isEmpty() and op.size() >= 9 )
	{
		chkXMLMetadata->setChecked ( op.at ( i++ ) == ON );
		chkXMLOneFile->setChecked ( op.at ( i++ ) == ON );
		chkXHTMLMetadata->setChecked ( op.at ( i++ ) == ON );
		chkXHTMLOneFile->setChecked ( op.at ( i++ ) == ON );
		chkXHTMLGroup->setChecked ( op.at ( i++ ) == ON );
		chkTXTMetadata->setChecked ( op.at ( i++ ) == ON );
		chkTXTOneFile->setChecked ( op.at ( i++ ) == ON );
		chkTXTHeaders->setChecked ( op.at ( i++ ) == ON );

		cmbTXTSeparator->setCurrentIndex ( QString ( op.at ( i++ ) ).toInt() );
		switch ( cmbTXTSeparator->currentIndex() )
		{
			case 0:
				txtseparator = " ";
				break;
			case 1:
				txtseparator = "\t";
				break;
			case 2:
				txtseparator = "|";
				break;
			case 3:
				txtseparator = ";";
				break;
			case 4:
				txtseparator = ",";
				break;
			case 5:
				txtseparator = ":";
				break;
			case 6:
				txtseparator = "&";
				break;
		}
	}
}

void NMGStatisticExportManager::savePreferences()
{
	QString dirPath = txtExportDir->text();
	if ( !dirPath.endsWith ( "/" ) ) dirPath += "/";

	QDir dir ( dirPath );
	if ( !dir.exists() )
	{
		if ( !dir.mkdir ( dirPath ) )
		{
			dirPath = QDir::tempPath() + "/netmeter-exportdir/";
			QDir dirTmp ( dirPath );
			if ( not dirTmp.exists() ) dirTmp.mkdir ( dirPath );

			txtExportDir->setText ( dirPath );

			QMessageBox::critical ( this, "Error while creating the export directory", "Error while creating the export directory.\nUsing " + dirPath + " instead." );
		}
		else
			QMessageBox::warning ( this, "Creating the export directory", "Creating the export directory.\nUsing " + dirPath );
	}
	preferencesManager->addValue ( ID_PREFERENCE_STATISTIC_EXPORT_DIR, dirPath );

	QString op = "";

	// Delay (Mgen)
	chkDelayAve->isChecked() ? op += ON : op += OFF;
	chkDelayMin->isChecked() ? op += ON : op += OFF;
	chkDelayMax->isChecked() ? op += ON : op += OFF;
	chkDelayStDev->isChecked() ? op += ON : op += OFF;
	chkDelayJitter->isChecked() ? op += ON : op += OFF;
	chkDelayP50->isChecked() ? op += ON : op += OFF;
	chkDelayP95->isChecked() ? op += ON : op += OFF;
	chkDelayP99->isChecked() ? op += ON : op += OFF;
	chkDelayP999->isChecked() ? op += ON : op += OFF;

	// IPDV (Mgen)
	chkIPDVAve->isChecked() ? op += ON : op += OFF;
	chkIPDVMin->isChecked() ? op += ON : op += OFF;
	chkIPDVMax->isChecked() ? op += ON : op += OFF;
	chkIPDVStDev->isChecked() ? op += ON : op += OFF;
	chkIPDVJitter->isChecked() ? op += ON : op += OFF;
	chkIPDVP50->isChecked() ? op += ON : op += OFF;
	chkIPDVP95->isChecked() ? op += ON : op += OFF;
	chkIPDVP99->isChecked() ? op += ON : op += OFF;
	chkIPDVP999->isChecked() ? op += ON : op += OFF;

	// Throughtput (Netperf)
	chkThrAve->isChecked() ? op += ON : op += OFF;
	chkThrMin->isChecked() ? op += ON : op += OFF;
	chkThrMax->isChecked() ? op += ON : op += OFF;

	preferencesManager->addValue ( ID_PREFERENCE_STATISTIC_EXPORT_OPTIONS_VALUES, op );

	op = "";
	chkXMLMetadata->isChecked() ? op += ON : op += OFF;
	chkXMLOneFile->isChecked() ? op += ON : op += OFF;
	chkXHTMLMetadata->isChecked() ? op += ON : op += OFF;
	chkXHTMLOneFile->isChecked() ? op += ON : op += OFF;
	chkXHTMLGroup->isChecked() ? op += ON : op += OFF;
	chkTXTMetadata->isChecked() ? op += ON : op += OFF;
	chkTXTOneFile->isChecked() ? op += ON : op += OFF;
	chkTXTHeaders->isChecked() ? op += ON : op += OFF;

	op += INTEGER ( cmbTXTSeparator->currentIndex() );

	switch ( cmbTXTSeparator->currentIndex() )
	{
		case 0:
			txtseparator = " ";
			break;
		case 1:
			txtseparator = "\t";
			break;
		case 2:
			txtseparator = "|";
			break;
		case 3:
			txtseparator = ";";
			break;
		case 4:
			txtseparator = ",";
			break;
		case 5:
			txtseparator = ":";
			break;
	}

	preferencesManager->addValue ( ID_PREFERENCE_STATISTIC_EXPORT_OPTIONS_FORMAT, op );
}

bool NMGStatisticExportManager::selectResultDirectory()
{
	QString dir = QFileDialog::getExistingDirectory ( this, tr ( "Select Export Directory" ), txtExportDir->text(), QFileDialog::ShowDirsOnly );
	if ( dir.isNull() ) return false;
	else
	{
		if ( !dir.isEmpty() ) txtExportDir->setText ( dir );
		return true;
	}
}

/********************************************************************/
/* Display 							    */
/********************************************************************/
QString NMGStatisticExportManager::displayTest ( NMGStatisticExportDataList list )
{
	QString text;
	QXmlStreamWriter xmlw ( &text );

	bool isXHTMLMetadata = chkXHTMLMetadata->isChecked();
	chkXHTMLMetadata->setChecked ( false );

	writeHtmlHeader ( &xmlw, "" );
	if ( list.count() > 1 and list.first()->metadata->getTypeOfTest() == TYPE_PACKETS
	        and chkXHTMLGroup->isChecked() )
	{
		exportTestToXhtml ( &xmlw, list );
	}
	else
	{
		for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
			exportTestToXhtml ( &xmlw, *it );
	}
	xmlw.writeEndDocument();

	chkXHTMLMetadata->setChecked ( isXHTMLMetadata );

	return text;
}

QString NMGStatisticExportManager::displayTest ( NMGStatisticAggregationData * aggregationData, NMGStatisticExportDataList list )
{
	QString text;
	QXmlStreamWriter xmlw ( &text );

	bool isXHTMLMetadata = chkXHTMLMetadata->isChecked();
	chkXHTMLMetadata->setChecked ( false );

	writeHtmlHeader ( &xmlw, "" );
	exportTestToXhtml ( &xmlw, aggregationData );
	for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
		exportTestToXhtml ( &xmlw, *it );
	xmlw.writeEndDocument();

	chkXHTMLMetadata->setChecked ( isXHTMLMetadata );

	return text;
}

/********************************************************************/
/* Generic Export Function 					    */
/********************************************************************/
bool NMGStatisticExportManager::exportTest ( NMGStatisticExportDataList list, exportFormat format )
{
	if ( list.isEmpty() )
	{
		MessageBar::longMessage ( tr ( "No items selected. Export cancelled." ) );
		return false;
	}
	
	QFile file;
	QXmlStreamWriter xmlw ( &file );
	xmlw.setAutoFormatting ( true );

	while ( txtExportDir->text().isEmpty() )
	{
		if ( !selectResultDirectory() )
		{
			MessageBar::longMessage ( tr ( "Export cancelled by user." ) );
			return false;
		}
		savePreferences();
	}
	QString exportDirPath = txtExportDir->text();

	QString exportFilePath;
	switch ( format )
	{
		case XML:
			if ( list.count() == 1 )
			{
				exportFilePath = exportDirPath + ( list.first() )->fileName.replace ( ".xff", ".xml", Qt::CaseInsensitive );
				file.setFileName ( exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					xmlw.writeStartDocument();
					exportTestToXml ( &xmlw, list.first() );
					xmlw.writeEndDocument();
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export file",
					                        "Error while creating the export file" 
					                        + exportFilePath + ". Export cancelled." );
					return false;
				}
				file.close();
			}
			else if ( chkXMLOneFile->isChecked() )
			{
				exportFilePath = exportDirPath + "export_" + DATE() + ".xml";
				file.setFileName ( exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					xmlw.writeStartDocument();
					for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
						exportTestToXml ( &xmlw, *it );
					xmlw.writeEndDocument();
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export file",
					                        "Error while creating the export file" 
					                        + exportFilePath + ". Export cancelled." );
					return false;
				}
				file.close();
			}
			else //not chkXMLOneFile->isChecked()
			{
				exportDirPath += "xml_export-" + DATE() + "/";
				QDir dir ( exportDirPath );
				if ( not dir.exists() )
				{
					if ( not dir.mkdir ( exportDirPath ) )
					{
						QMessageBox::critical ( this, "Error while creating the export directory",
						                        "Error while creating the export directory at\n" 
						                        + exportDirPath + ".\nExport cancelled." );
						return false;
					}
				}

				for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
				{
					exportFilePath = exportDirPath + ( *it )->fileName.replace ( ".xff", ".xml", Qt::CaseInsensitive );
					file.setFileName ( exportFilePath );
					if ( file.open ( QFile::WriteOnly | QFile::Text ) )
					{
						xmlw.writeStartDocument();
						exportTestToXml ( &xmlw, *it );
						xmlw.writeEndDocument();
					}
					else
					{
						QMessageBox::critical ( this, "Error while creating the export file",
						                        "Error while creating the export file" 
						                        + exportFilePath + ". Export cancelled." );
						return false;
					}
					file.close();
				}
			}
			break;

		case XHTML:
			if ( list.count() == 1 )
			{
				exportFilePath = exportDirPath + ( list.first() )->fileName.replace ( ".xff", ".html", Qt::CaseInsensitive );
				file.setFileName ( exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					writeHtmlHeader ( &xmlw, ( list.first() )->fileName );
					exportTestToXhtml ( &xmlw, list.first() );
					xmlw.writeEndDocument();
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export file",
					                        "Error while creating the export file" 
					                        + exportFilePath + ". Export cancelled." );
					return false;
				}
				file.close();
			}
			else if ( list.first()->metadata->getTypeOfTest() == TYPE_PACKETS and chkXHTMLGroup->isChecked() )
			{
				exportFilePath = "export_" + DATE() + ".html";
				file.setFileName ( exportDirPath + exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					writeHtmlHeader ( &xmlw, exportFilePath );
					exportTestToXhtml ( &xmlw, list );
					xmlw.writeEndDocument();
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export file",
					                        "Error while creating the export file" 
					                        + exportDirPath + exportFilePath + ". Export cancelled." );
					return false;
				}
				file.close();
			}
			else if ( chkXHTMLOneFile->isChecked() )
			{
				exportFilePath = "export_" + DATE() + ".html";
				file.setFileName ( exportDirPath + exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					writeHtmlHeader ( &xmlw, exportFilePath );
					for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
						exportTestToXhtml ( &xmlw, *it );
					xmlw.writeEndDocument();
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export file",
					                        "Error while creating the export file" 
					                        + exportDirPath + exportFilePath + ". Export cancelled." );
					return false;
				}
				file.close();
			}
			else //not chkXHTMLOneFile->isChecked()
			{
				exportDirPath += "xhtml_export-" + DATE() + "/";
				QDir dir ( exportDirPath );
				if ( not dir.exists() )
				{
					if ( not dir.mkdir ( exportDirPath ) )
					{
						QMessageBox::critical ( this, "Error while creating the export directory",
						                        "Error while creating the export directory at\n" + exportDirPath + ".\nExport cancelled." );
						return false;
					}
				}

				for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
				{
					exportFilePath = exportDirPath + ( *it )->fileName.replace ( ".xff", ".html", Qt::CaseInsensitive );
					file.setFileName ( exportFilePath );
					if ( file.open ( QFile::WriteOnly | QFile::Text ) )
					{
						writeHtmlHeader ( &xmlw, ( *it )->fileName );
						exportTestToXhtml ( &xmlw, *it );
						xmlw.writeEndDocument();
					}
					else
					{
						QMessageBox::critical ( this, "Error while creating the export file",
						                        "Error while creating the export file" 
						                        + exportFilePath + ". Export cancelled." );
						return false;
					}
					file.close();
				}
				generateIndex ( exportDirPath );
			}
			break;

		case TXT:
			if ( list.count() == 1 )
			{
				exportFilePath = exportDirPath + ( list.first() )->fileName.replace ( ".xff", ".txt", Qt::CaseInsensitive );
				file.setFileName ( exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					if ( chkTXTHeaders->isChecked() ) file.write ( writeTxtHeader ( ( list.first() )->metadata->getTypeOfTest() ).toUtf8() );
					file.write ( exportTestToTxt ( list.first() ).toUtf8() );
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export file",
					                        "Error while creating the export file" 
					                        + exportFilePath + ". Export cancelled." );
					return false;
				}
				file.close();
			}
			else if ( chkTXTOneFile->isChecked() )
			{
				exportFilePath = exportDirPath + "export_" + DATE() + ".txt";
				file.setFileName ( exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					QString text = "";
					if ( chkTXTHeaders->isChecked() ) text += writeTxtHeader ( ( list.first() )->metadata->getTypeOfTest() );
					for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
						text += exportTestToTxt ( *it );
					file.write ( text.toUtf8() );
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export file",
					                        "Error while creating the export file" 
					                        + exportFilePath + ". Export cancelled." );
					return false;
				}
				file.close();
			}
			else //not chkTXTOneFile->isChecked()
			{
				exportDirPath += "txt_export-" + DATE() + "/";
				QDir dir ( exportDirPath );
				if ( not dir.exists() )
				{
					if ( not dir.mkdir ( exportDirPath ) )
					{
						QMessageBox::critical ( this, "Error while creating the export directory",
						                        "Error while creating the export directory at\n" 
						                        + exportDirPath + ".\nExport cancelled." );
						return false;
					}
				}

				for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
				{
					exportFilePath = exportDirPath + ( *it )->fileName.replace ( ".xff", ".txt", Qt::CaseInsensitive );
					file.setFileName ( exportFilePath );
					if ( file.open ( QFile::WriteOnly | QFile::Text ) )
					{
						if ( chkTXTHeaders->isChecked() ) file.write ( writeTxtHeader ( ( *it )->metadata->getTypeOfTest() ).toUtf8() );
						file.write ( exportTestToTxt ( *it ).toUtf8() );
					}
					else
					{
						QMessageBox::critical ( this, "Error while creating the export file",
						                        "Error while creating the export file" 
						                        + exportFilePath + ". Export cancelled." );
						return false;
					}
					file.close();
				}
			}
			break;

		case XSLT:
			QString xsltPath = QFileDialog::getOpenFileName ( this->parentWidget(), tr ( "Select XSLT Stylesheet" ),
			                   QString ( XML_DIR ),
			                   tr ( "XSLT Stylesheet (*.xslt *.xsl);;XML Stylesheet (*.xml);;All files (*.*)" ) );
			if ( xsltPath.isEmpty() )
			{
				QMessageBox::critical ( this, "Error while creating the export file",
				                        "Error while creating the export file. No XSLT stylesheet specified." );
				return false;
			}

			if ( list.count() > 1 )
			{
				exportDirPath += "xslt_export-" + DATE() + "/";
				QDir dir ( exportDirPath );
				if ( not dir.exists() )
				{
					if ( not dir.mkdir ( exportDirPath ) )
					{
						QMessageBox::critical ( this, "Error while creating the export directory",
						                        "Error while creating the export directory at\n" 
						                        + exportDirPath + ".\nExport cancelled." );
						return false;
					}
				}
			}
			for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
			{
				exportFilePath = QDir::tempPath() + "/" + ( *it )->fileName.replace ( ".xff", ".xml", Qt::CaseInsensitive );
				file.setFileName ( exportFilePath );
				if ( file.open ( QFile::WriteOnly | QFile::Text ) )
				{
					xmlw.writeStartDocument();
					exportTestToXml ( &xmlw, *it );
					xmlw.writeEndDocument();
					file.close();

					QString fileResultPath = exportDirPath + ( *it )->fileName.remove ( ".xff", Qt::CaseInsensitive );
					NMXML::applyXsltToDoc ( exportFilePath.toAscii().constData(), 
					                        xsltPath.toAscii().constData(), fileResultPath.toAscii().constData() );
					file.remove(); // Removes TMP file
				}
				else
				{
					QMessageBox::critical ( this, "Error while creating the export base file",
					                        "Error while creating the export base file" 
					                        + exportFilePath + ". Export cancelled." );
					return false;
				}
			}
			break;
	}
	return true;
}

bool NMGStatisticExportManager::exportTest ( NMGStatisticAggregationData * aggregationData, 
                                             NMGStatisticExportDataList list, exportFormat format )
{
	if ( list.isEmpty() )
	{
		MessageBar::longMessage ( tr ( "No items selected. Export cancelled." ) );
		return false;
	}

	QFile file;
	QXmlStreamWriter xmlw ( &file );
	xmlw.setAutoFormatting ( true );

	while ( txtExportDir->text().isEmpty() )
	{
		if ( !selectResultDirectory() )
		{
			MessageBar::longMessage ( tr ( "Export cancelled by user." ) );
			return false;
		}
		savePreferences();
	}
	QString exportDirPath = txtExportDir->text();

	QString exportFilePath;
	switch ( format )
	{
		case XML:
			exportFilePath = exportDirPath + "export_" + DATE() + ".xml";
			file.setFileName ( exportFilePath );
			if ( file.open ( QFile::WriteOnly | QFile::Text ) )
			{
				xmlw.writeStartDocument();
				xmlw.writeStartElement ( TAG_TEST );
				exportTestToXml ( &xmlw, aggregationData );
				for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
					exportTestToXml ( &xmlw, *it );
				xmlw.writeEndDocument();
			}
			else
			{
				QMessageBox::critical ( this, "Error while creating the export file",
				                        "Error while creating the export file" + exportFilePath + ". Export cancelled." );
				return false;
			}
			file.close();
			break;

		case XHTML:
			exportFilePath = "export_" + DATE() + ".html";
			file.setFileName ( exportDirPath + exportFilePath );
			if ( file.open ( QFile::WriteOnly | QFile::Text ) )
			{
				writeHtmlHeader ( &xmlw, "Throughtput Aggregation" );
				exportTestToXhtml ( &xmlw, aggregationData );
				for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
					exportTestToXhtml ( &xmlw, *it );
				xmlw.writeEndDocument();
			}
			else
			{
				QMessageBox::critical ( this, "Error while creating the export file",
				                        "Error while creating the export file" 
				                        + exportDirPath + exportFilePath + ". Export cancelled." );
				return false;
			}
			file.close();
			break;

		case TXT:
			exportFilePath = exportDirPath + "export_" + DATE() + ".txt";
			file.setFileName ( exportFilePath );
			if ( file.open ( QFile::WriteOnly | QFile::Text ) )
			{
				QString text = "";
				text += exportTestToTxt ( aggregationData );
				if ( chkTXTHeaders->isChecked() ) text += writeTxtHeader ( ( list.first() )->metadata->getTypeOfTest() );
				for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
					text += exportTestToTxt ( *it );
				file.write ( text.toUtf8() );
			}
			else
			{
				QMessageBox::critical ( this, "Error while creating the export file",
				                        "Error while creating the export file" 
				                        + exportFilePath + ". Export cancelled." );
				return false;
			}
			file.close();

			break;

		case XSLT:
			QString xsltPath = QFileDialog::getOpenFileName ( this->parentWidget(), tr ( "Select XSLT Stylesheet" ), QString ( XML_DIR ),
			                   tr ( "XSLT Stylesheet (*.xslt *.xsl);;XML Stylesheet (*.xml);;All files (*.*)" ) );
			if ( xsltPath.isEmpty() )
			{
				QMessageBox::critical ( this, "Error while creating the export file",
				                        "Error while creating the export file. No XSLT stylesheet specified." );
				return false;
			}

			QString exportFileName = "export_" + DATE();

			exportFilePath = QDir::tempPath() + "/" + exportFileName;
			file.setFileName ( exportFilePath );

			if ( file.open ( QFile::WriteOnly | QFile::Text ) )
			{
				xmlw.writeStartDocument();
				xmlw.writeStartElement ( TAG_TEST );
				exportTestToXml ( &xmlw, aggregationData );
				for ( NMGStatisticExportDataListIterator it = list.begin(); it != list.end(); ++it )
					exportTestToXml ( &xmlw, *it );
				xmlw.writeEndDocument();
				file.close();

				QString fileResultPath = exportDirPath + exportFileName;
				NMXML::applyXsltToDoc ( exportFilePath.toAscii().constData(), xsltPath.toAscii().constData(), fileResultPath.toAscii().constData() );
				file.remove();

			}
			else
			{
				QMessageBox::critical ( this, "Error while creating the export base file",
				                        "Error while creating the export base file" + exportFilePath + ". Export cancelled." );
				return false;
			}
			break;
	}
	return true;
}

/********************************************************************/
/* XML 								    */
/********************************************************************/
void NMGStatisticExportManager::exportTestToXml ( QXmlStreamWriter * xmlw, NMGStatisticExportData * expData )
{
	NMGMetaData * metadata = expData->metadata;

	xmlw->writeStartElement ( TAG_TEST );

	if ( chkXMLMetadata->isChecked() )
	{
		xmlw->writeStartElement ( TAG_METADATA );

		xmlw->writeTextElement ( TAG_NAME, metadata->getTestName() );

		xmlw->writeTextElement ( TAG_IDENTIFIER, metadata->getTestIdentifier() );

		if ( metadata->getVersion() != -1 )
			xmlw->writeTextElement ( TAG_VERSION, INTEGER ( metadata->getVersion() ) );
		else if ( metadata->isSetUncommonElement("mgen_version") )
			xmlw->writeTextElement ( TAG_VERSION, metadata->getUncommonElement ( "mgen_version" ).remove ( QRegExp ( "<.*>" ) ) );

		xmlw->device()->write ( "\n\t" + metadata->getUncommonElement ( TAG_RESULT_PATH ).toUtf8() );

		xmlw->writeStartElement ( TAG_SOURCE );
		xmlw->writeTextElement ( TAG_ADDRESS, metadata->getSourceAddress() );
		if ( metadata->getTypeOfTest() == TYPE_PACKETS )
			xmlw->device()->write ( "\n\t    " + metadata->getUncommonElement ( TAG_MGEN_PATH ).toUtf8() );
		else if ( metadata->getTypeOfTest() == TYPE_TPUT )
			xmlw->device()->write ( "\n\t    " + metadata->getUncommonElement ( TAG_NETPERF_PATH ).toUtf8() );

		xmlw->writeStartElement ( TAG_MANAGEMENT );
		xmlw->writeTextElement ( TAG_ADDRESS, metadata->getManagementSourceAddress() );
		xmlw->writeTextElement ( TAG_PORT, INTEGER ( metadata->getManagementSourcePort() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();

		xmlw->writeStartElement ( TAG_DESTINATION );
		xmlw->writeTextElement ( TAG_ADDRESS, metadata->getDestinationAddress() );
		if ( metadata->getTypeOfTest() == TYPE_PACKETS )
		{
			xmlw->device()->write ( "\n\t    " + metadata->getUncommonElement ( TAG_DREC_PATH ).toUtf8() );
			xmlw->device()->write ( "\n\t    " + metadata->getUncommonElement ( TAG_DECODE_PATH ).toUtf8() );
		}
		else if ( metadata->getTypeOfTest() == TYPE_TPUT )
			xmlw->device()->write ( "\n\t    " + metadata->getUncommonElement ( TAG_NETSERVER_PATH ).toUtf8() );

		xmlw->writeStartElement ( TAG_MANAGEMENT );
		xmlw->writeTextElement ( TAG_ADDRESS, metadata->getManagementDestinationAddress() );
		xmlw->writeTextElement ( TAG_PORT, INTEGER ( metadata->getManagementDestinationPort() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();

		if ( metadata->getTypeOfTest() == TYPE_PACKETS )
		{
			xmlw->device()->write ( "\n\t" + metadata->getUncommonElement ( TAG_CONF_MGEN ).simplified().toUtf8() );
			xmlw->device()->write ( "\n\t" + metadata->getUncommonElement ( TAG_CONF_DREC ).simplified().toUtf8() );
		}
		else if ( metadata->getTypeOfTest() == TYPE_TPUT )
		{
			xmlw->device()->write ( "\n\t" + metadata->getUncommonElement ( TAG_CONF_NETPERF ).simplified().toUtf8() );
			xmlw->device()->write ( "\n\t" + metadata->getUncommonElement ( TAG_CONF_NETSERVER ).simplified().toUtf8() );
		}

		xmlw->writeEndElement();
	}

	if ( metadata->getTypeOfTest() == TYPE_PACKETS )
		xmlw->writeStartElement ( TAG_STATISTIC );
	else if ( metadata->getTypeOfTest() == TYPE_TPUT )
		xmlw->writeStartElement ( TAG_RESULT );

	NMGStatisticItemDataListIterator itfstart = expData->itemList.begin();
	NMGStatisticItemDataListIterator itfend = expData->itemList.end();
	if ( metadata->getTypeOfTest() == TYPE_PACKETS )
		while ( itfend-- != itfstart )
			exportItemToXml ( xmlw, ( NMGStatisticPacketData * ) *itfend );
	else if ( metadata->getTypeOfTest() == TYPE_TPUT )
		while ( itfend-- != itfstart )
			exportItemToXml ( xmlw, ( NMGStatisticThroughtputData * ) *itfend );

	xmlw->writeEndElement();
	xmlw->writeEndElement();
}

// Packet item
void NMGStatisticExportManager::exportItemToXml ( QXmlStreamWriter * xmlw, NMGStatisticPacketData * item )
{
	xmlw->writeStartElement ( TAG_FLOW );
	xmlw->writeAttribute ( ATTR_ID, INTEGER ( item->flowId() ) );
	xmlw->writeAttribute ( ATTR_NAME, item->flowName() );

	xmlw->writeTextElement ( TAG_SENT, INTEGER ( item->sentPackets() ) );
	xmlw->writeTextElement ( TAG_RECEIVED, INTEGER ( item->receivedPackets() ) );
	xmlw->writeTextElement ( TAG_DURATION, INTEGER ( item->duration() ) );
	xmlw->writeTextElement ( TAG_LOST, INTEGER ( item->lostPackets() ) );
	xmlw->writeTextElement ( TAG_LOSS_RATIO, INTEGER ( item->lossRatio() ) );
	xmlw->writeTextElement ( TAG_DUPLICATED, INTEGER ( item->duplicatedPackets() ) );
	xmlw->writeTextElement ( TAG_DUPLICATED_RATIO, INTEGER ( item->duplicatedRatio() ) );
	xmlw->writeTextElement ( TAG_OUTOFORDER, INTEGER ( item->outOfOrder() ) );
	xmlw->writeTextElement ( TAG_OUTOFORDER_RATIO, INTEGER ( item->outOfOrderRatio() ) );

	// Delay
	if ( chkDelayAve->isChecked() or chkDelayMin->isChecked() or chkDelayMax->isChecked() or chkDelayStDev->isChecked() or chkDelayJitter->isChecked() or chkDelayP50->isChecked() or chkDelayP95->isChecked() or chkDelayP99->isChecked() or chkDelayP999->isChecked() )
	{
		xmlw->writeStartElement ( TAG_DELAY );
		if ( chkDelayAve->isChecked() )
			xmlw->writeTextElement ( TAG_AVERAGE, DOUBLE ( item->averageDelay() ) );
		if ( chkDelayMin->isChecked() )
			xmlw->writeTextElement ( TAG_MIN, DOUBLE ( item->minDelay() ) );
		if ( chkDelayMax->isChecked() )
			xmlw->writeTextElement ( TAG_MAX, DOUBLE ( item->maxDelay() ) );
		if ( chkDelayStDev->isChecked() )
			xmlw->writeTextElement ( TAG_STDEV, DOUBLE ( item->stDevDelay() ) );
		if ( chkDelayJitter->isChecked() )
			xmlw->writeTextElement ( TAG_JITTER, DOUBLE ( item->jitterDelay() ) );
		if ( chkDelayP50->isChecked() )
			xmlw->writeTextElement ( TAG_P50, DOUBLE ( item->delay50Percentile() ) );
		if ( chkDelayP95->isChecked() )
			xmlw->writeTextElement ( TAG_P95, DOUBLE ( item->delay95Percentile() ) );
		if ( chkDelayP99->isChecked() )
			xmlw->writeTextElement ( TAG_P99, DOUBLE ( item->delay99Percentile() ) );
		if ( chkDelayP999->isChecked() )
			xmlw->writeTextElement ( TAG_P999, DOUBLE ( item->delay999Percentile() ) );
		xmlw->writeEndElement();
	}

	// IPDV
	if ( chkIPDVAve->isChecked() or chkIPDVMin->isChecked() or chkIPDVMax->isChecked() or chkIPDVStDev->isChecked() or chkIPDVJitter->isChecked() or chkIPDVP50->isChecked() or chkIPDVP95->isChecked() or chkIPDVP99->isChecked() or chkIPDVP999->isChecked() )
	{
		xmlw->writeStartElement ( TAG_IPDV );
		if ( chkIPDVAve->isChecked() )
			xmlw->writeTextElement ( TAG_AVERAGE, DOUBLE ( item->averageIPDV() ) );
		if ( chkIPDVMin->isChecked() )
			xmlw->writeTextElement ( TAG_MIN, DOUBLE ( item->minIPDV() ) );
		if ( chkIPDVMax->isChecked() )
			xmlw->writeTextElement ( TAG_MAX, DOUBLE ( item->maxIPDV() ) );
		if ( chkIPDVStDev->isChecked() )
			xmlw->writeTextElement ( TAG_STDEV, DOUBLE ( item->stDevIPDV() ) );
		if ( chkIPDVJitter->isChecked() )
			xmlw->writeTextElement ( TAG_JITTER, DOUBLE ( item->jitterIPDV() ) );
		if ( chkIPDVP50->isChecked() )
			xmlw->writeTextElement ( TAG_P50, DOUBLE ( item->ipdv50Percentile() ) );
		if ( chkIPDVP95->isChecked() )
			xmlw->writeTextElement ( TAG_P95, DOUBLE ( item->ipdv95Percentile() ) );
		if ( chkIPDVP99->isChecked() )
			xmlw->writeTextElement ( TAG_P99, DOUBLE ( item->ipdv99Percentile() ) );
		if ( chkIPDVP999->isChecked() )
			xmlw->writeTextElement ( TAG_P999, DOUBLE ( item->ipdv999Percentile() ) );
		xmlw->writeEndElement();
	}

	xmlw->writeEndElement();
}

// Aggregation item
void NMGStatisticExportManager::exportItemToXml ( QXmlStreamWriter * xmlw, NMGStatisticThroughtputData * item )
{
	xmlw->writeStartElement ( item->getTypeOfProtocol() );
	xmlw->writeTextElement ( TAG_SOURCE, item->getSourceAddress() );
	xmlw->writeTextElement ( TAG_DESTINATION, item->getDestinationAddress() );

	if ( item->getTypeOfProtocol() == TAG_UDP )
	{
		xmlw->writeTextElement ( TAG_SSS, INTEGER ( item->getSendingSocketSizeInBytes() ) );
		xmlw->writeTextElement ( TAG_SMS, INTEGER ( item->getSendingMessagesSizeInBytes() ) );
		xmlw->writeTextElement ( TAG_TIME, DOUBLE ( item->getElapsedTimeInSeconds() ) );
		xmlw->writeTextElement ( TAG_SMO, INTEGER ( item->getSendingOkMessages() ) );
		xmlw->writeTextElement ( TAG_SMK, INTEGER ( item->getSendingKoMessages() ) );
		xmlw->writeTextElement ( TAG_STPUT, DOUBLE ( item->getSendingThroughputInKBitsPerSecond() ) );
		xmlw->writeTextElement ( TAG_RSS, INTEGER ( item->getReceivingSocketSizeInBytes() ) );
		xmlw->writeTextElement ( TAG_RMO, INTEGER ( item->getReceivingOkMessages() ) );
		xmlw->writeTextElement ( TAG_RTPUT, DOUBLE ( item->getReceivingThroughputInKBitsPerSecond() ) );
	}
	else
	{
		xmlw->writeTextElement ( TAG_SSS, INTEGER ( item->getSendingSocketSizeInBytes() ) );
		xmlw->writeTextElement ( TAG_SMS, INTEGER ( item->getSendingMessagesSizeInBytes() ) );
		xmlw->writeTextElement ( TAG_TIME, DOUBLE ( item->getElapsedTimeInSeconds() ) );
		xmlw->writeTextElement ( TAG_TPUT, DOUBLE ( item->getReceivingThroughputInKBitsPerSecond() ) );
		xmlw->writeTextElement ( TAG_RSS, INTEGER ( item->getReceivingSocketSizeInBytes() ) );
	}
	xmlw->writeEndElement();
}

// Aggregation Statistics Test
void NMGStatisticExportManager::exportTestToXml ( QXmlStreamWriter * xmlw, NMGStatisticAggregationData * expData )
{
	xmlw->writeStartElement ( TAG_STATISTIC );

	if ( chkThrAve->isChecked() )
		xmlw->writeTextElement ( TAG_AVERAGE, DOUBLE ( expData->averageThroughtput() ) );
	if ( chkThrMin->isChecked() )
		xmlw->writeTextElement ( TAG_MIN, DOUBLE ( expData->minThroughtput() ) );
	if ( chkThrMax->isChecked() )
		xmlw->writeTextElement ( TAG_MAX, DOUBLE ( expData->maxThroughtput() ) );

	xmlw->writeEndElement();
}

/********************************************************************/
/* XHTML 							    */
/********************************************************************/
// Completely Grouped
void NMGStatisticExportManager::exportTestToXhtml ( QXmlStreamWriter * xmlw, NMGStatisticExportDataList list )
{
	NMGStatisticItemDataListIterator itfstart, itfend;
	NMGStatisticPacketData * item;

	xmlw->writeTextElement ( "h3", "Basic Information" );

	xmlw->writeStartElement ( "table" );

	xmlw->writeStartElement ( "tr" );
	xmlw->writeTextElement ( "th", "Test" );
	xmlw->writeTextElement ( "th", "Flow" );
	xmlw->writeStartElement ( "th" );
	xmlw->writeCharacters ( "Sent" );
	xmlw->writeEmptyElement ( "br" );
	xmlw->writeCharacters ( "Packets" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeCharacters ( "Received" );
	xmlw->writeEmptyElement ( "br" );
	xmlw->writeCharacters ( "Packets" );
	xmlw->writeEndElement();
	xmlw->writeTextElement ( "th", "Duration" );
	xmlw->writeEndElement();

	for ( NMGStatisticExportDataListIterator it = list.begin(); it < list.end(); ++it )
	{
		QString testName;
		if ( ( *it )->metadata->getTestName().isEmpty() )
			testName = ( *it )->fileName.mid ( ( *it )->fileName.lastIndexOf ( "/" ) + 1 );
		else
			testName = ( *it )->metadata->getTestName();

		itfstart = ( *it )->itemList.begin();
		itfend = ( *it )->itemList.end();

		while ( itfend-- != itfstart )
		{
			item = ( NMGStatisticPacketData * ) * itfend;

			xmlw->writeStartElement ( "tr" );
			xmlw->writeTextElement ( "td", testName );
			xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
			xmlw->writeTextElement ( "td", INTEGER ( item->sentPackets() ) );
			xmlw->writeTextElement ( "td", INTEGER ( item->receivedPackets() ) );
			xmlw->writeTextElement ( "td", simplifyTime ( ( int ) item->duration() ) );
			xmlw->writeEndElement();
		}
	}

	xmlw->writeEndElement();

	xmlw->writeEmptyElement ( "br" );

	// Delay
	if ( chkDelayAve->isChecked() or chkDelayMin->isChecked() or chkDelayMax->isChecked() or chkDelayStDev->isChecked()
	        or chkDelayJitter->isChecked() or chkDelayP50->isChecked() or chkDelayP95->isChecked() or chkDelayP99->isChecked() or chkDelayP999->isChecked() )
	{
		xmlw->writeTextElement ( "h3", "Delay statistics (msec)" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Test" );
		xmlw->writeTextElement ( "th", "Flow" );
		if ( chkDelayAve->isChecked() )
			xmlw->writeTextElement ( "th", "Average" );
		if ( chkDelayMin->isChecked() )
			xmlw->writeTextElement ( "th", "Minimum" );
		if ( chkDelayMax->isChecked() )
			xmlw->writeTextElement ( "th", "Maximum" );
		if ( chkDelayStDev->isChecked() )
			xmlw->writeTextElement ( "th", "StDev" );
		if ( chkDelayJitter->isChecked() )
			xmlw->writeTextElement ( "th", "Jitter" );
		if ( chkDelayP50->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 50" );
		if ( chkDelayP95->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 95" );
		if ( chkDelayP99->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99" );
		if ( chkDelayP999->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99.9" );
		xmlw->writeEndElement();

		for ( NMGStatisticExportDataListIterator it = list.begin(); it < list.end(); ++it )
		{
			QString testName;
			if ( ( *it )->metadata->getTestName().isEmpty() )
				testName = ( *it )->fileName.mid ( ( *it )->fileName.lastIndexOf ( "/" ) + 1 );
			else
				testName = ( *it )->metadata->getTestName();

			itfstart = ( *it )->itemList.begin();
			itfend = ( *it )->itemList.end();

			while ( itfend-- != itfstart )
			{
				item = ( NMGStatisticPacketData * ) * itfend;

				xmlw->writeStartElement ( "tr" );
				xmlw->writeTextElement ( "td", testName );
				xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
				if ( chkDelayAve->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->averageDelay() ) );
				if ( chkDelayMin->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->minDelay() ) );
				if ( chkDelayMax->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->maxDelay() ) );
				if ( chkDelayStDev->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->stDevDelay() ) );
				if ( chkDelayJitter->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->jitterDelay() ) );
				if ( chkDelayP50->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->delay50Percentile() ) );
				if ( chkDelayP95->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->delay95Percentile() ) );
				if ( chkDelayP99->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->delay99Percentile() ) );
				if ( chkDelayP999->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->delay999Percentile() ) );
				xmlw->writeEndElement();
			}
		}

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}

	// IPDV
	if ( chkIPDVAve->isChecked() or chkIPDVMin->isChecked() or chkIPDVMax->isChecked() or chkIPDVStDev->isChecked()
	        or chkIPDVJitter->isChecked() or chkIPDVP50->isChecked() or chkIPDVP95->isChecked() or chkIPDVP99->isChecked() or chkIPDVP999->isChecked() )
	{
		xmlw->writeTextElement ( "h3", "IPDV statistics (msec)" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Test" );
		xmlw->writeTextElement ( "th", "Flow" );
		if ( chkIPDVAve->isChecked() )
			xmlw->writeTextElement ( "th", "Average" );
		if ( chkIPDVMin->isChecked() )
			xmlw->writeTextElement ( "th", "Minimum" );
		if ( chkIPDVMax->isChecked() )
			xmlw->writeTextElement ( "th", "Maximum" );
		if ( chkIPDVStDev->isChecked() )
			xmlw->writeTextElement ( "th", "StDev" );
		if ( chkIPDVJitter->isChecked() )
			xmlw->writeTextElement ( "th", "Jitter" );
		if ( chkIPDVP50->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 50" );
		if ( chkIPDVP95->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 95" );
		if ( chkIPDVP99->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99" );
		if ( chkIPDVP999->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99.9" );
		xmlw->writeEndElement();

		for ( NMGStatisticExportDataListIterator it = list.begin(); it < list.end(); ++it )
		{
			QString testName;
			if ( ( *it )->metadata->getTestName().isEmpty() )
				testName = ( *it )->fileName.mid ( ( *it )->fileName.lastIndexOf ( "/" ) + 1 );
			else
				testName = ( *it )->metadata->getTestName();

			itfstart = ( *it )->itemList.begin();
			itfend = ( *it )->itemList.end();

			while ( itfend-- != itfstart )
			{
				item = ( NMGStatisticPacketData * ) * itfend;

				xmlw->writeStartElement ( "tr" );
				xmlw->writeTextElement ( "td", testName );
				xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
				if ( chkIPDVAve->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->averageIPDV() ) );
				if ( chkIPDVMin->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->minIPDV() ) );
				if ( chkIPDVMax->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->maxIPDV() ) );
				if ( chkIPDVStDev->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->stDevIPDV() ) );
				if ( chkIPDVJitter->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->jitterIPDV() ) );
				if ( chkIPDVP50->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv50Percentile() ) );
				if ( chkIPDVP95->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv95Percentile() ) );
				if ( chkIPDVP99->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv99Percentile() ) );
				if ( chkIPDVP999->isChecked() )
					xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv999Percentile() ) );
				xmlw->writeEndElement();
			}
		}

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}

	xmlw->writeTextElement ( "h3", "Other Information" );

	xmlw->writeStartElement ( "table" );

	xmlw->writeStartElement ( "tr" );
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "rowspan", "2" );
	xmlw->writeCharacters ( "Test" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "rowspan", "2" );
	xmlw->writeCharacters ( "Flow" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Lost" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Duplicated" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Out-Of-Order" );
	xmlw->writeEndElement();
	xmlw->writeEndElement();

	xmlw->writeStartElement ( "tr" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeEndElement();

	for ( NMGStatisticExportDataListIterator it = list.begin(); it < list.end(); ++it )
	{
		QString testName;
		if ( ( *it )->metadata->getTestName().isEmpty() )
			testName = ( *it )->fileName.mid ( ( *it )->fileName.lastIndexOf ( "/" ) + 1 );
		else
			testName = ( *it )->metadata->getTestName();

		itfstart = ( *it )->itemList.begin();
		itfend = ( *it )->itemList.end();

		while ( itfend-- != itfstart )
		{
			item = ( NMGStatisticPacketData * ) * itfend;

			xmlw->writeStartElement ( "tr" );
			xmlw->writeTextElement ( "td", testName );
			xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
			xmlw->writeTextElement ( "td", INTEGER ( item->lostPackets() ) );
			xmlw->writeTextElement ( "td", DOUBLE ( item->lossRatio() ) );
			xmlw->writeTextElement ( "td", INTEGER ( item->duplicatedPackets() ) );
			xmlw->writeTextElement ( "td", DOUBLE ( item->duplicatedRatio() ) );
			xmlw->writeTextElement ( "td", INTEGER ( item->outOfOrder() ) );
			xmlw->writeTextElement ( "td", DOUBLE ( item->outOfOrderRatio() ) );
			xmlw->writeEndElement();
		}
	}

	xmlw->writeEndElement();

	xmlw->writeEmptyElement ( "br" );
}

void NMGStatisticExportManager::exportTestToXhtml ( QXmlStreamWriter * xmlw, NMGStatisticExportData * expData )
{
	NMGMetaData * metadata = expData->metadata;

	if ( metadata->getTestName().isEmpty() )
		xmlw->writeTextElement ( "h1", expData->fileName.mid ( expData->fileName.lastIndexOf ( "/" ) + 1 ) );
	else
		xmlw->writeTextElement ( "h1", metadata->getTestName() );

	if ( chkXHTMLMetadata->isChecked() )
	{
		QRegExp exp ( "<.*>" );
		exp.setMinimal ( true );

		xmlw->writeStartElement ( "ul" );
		xmlw->writeAttribute ( "class", "metadata" );
		if ( !metadata->getTestName().isEmpty() )
			xmlw->writeTextElement ( "li", "Name: " + metadata->getTestName() );
		if ( !metadata->getTestIdentifier().isEmpty() )
			xmlw->writeTextElement ( "li", "Test Identifier: " + metadata->getTestIdentifier() );
		if ( metadata->getVersion() != -1 )
			xmlw->writeTextElement ( "li", "Application Version: " + INTEGER ( metadata->getVersion() ) );
		else if ( metadata->isSetUncommonElement("mgen_version") )
			xmlw->writeTextElement ( "li", "Application Version: " 
			                         + metadata->getUncommonElement ( "mgen_version" ).remove ( exp ) );

		xmlw->writeTextElement ( "li", "Result Directory Path: " + metadata->getUncommonElement ( TAG_RESULT_PATH ).remove ( exp ) );

		xmlw->writeStartElement ( "li" );
		xmlw->writeCharacters ( "Source" );
		xmlw->writeStartElement ( "ul" );
		xmlw->writeTextElement ( "li", "Address: " + metadata->getSourceAddress() );
		if ( metadata->getTypeOfTest() == TYPE_PACKETS )
			xmlw->writeTextElement ( "li", "Mgen Path: " + metadata->getUncommonElement ( TAG_MGEN_PATH ).remove ( exp ) );
		else if ( metadata->getTypeOfTest() == TYPE_TPUT )
			xmlw->writeTextElement ( "li", "Netperf Path: " + metadata->getUncommonElement ( TAG_NETPERF_PATH ).remove ( exp ) );

		xmlw->writeTextElement ( "li", "Management Address: " + metadata->getManagementSourceAddress() );
		xmlw->writeTextElement ( "li", "Management Port: " + INTEGER ( metadata->getManagementSourcePort() ) );
		xmlw->writeEndElement();
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "li" );
		xmlw->writeCharacters ( "Destination" );
		xmlw->writeStartElement ( "ul" );
		xmlw->writeTextElement ( "li", "Address: " + metadata->getDestinationAddress() );
		if ( metadata->getTypeOfTest() == TYPE_PACKETS )
		{
			xmlw->writeTextElement ( "li", "Drec Path: " + metadata->getUncommonElement ( TAG_DREC_PATH ).remove ( exp ) );
			xmlw->writeTextElement ( "li", "Decode Path: " + metadata->getUncommonElement ( TAG_DECODE_PATH ).remove ( exp ) );
		}
		else if ( metadata->getTypeOfTest() == TYPE_TPUT )
			xmlw->writeTextElement ( "li", "Netserver Path: " + metadata->getUncommonElement ( TAG_NETSERVER_PATH ).remove ( exp ) );

		xmlw->writeTextElement ( "li", "Management Address: " + metadata->getManagementDestinationAddress() );
		xmlw->writeTextElement ( "li", "Management Port: " + INTEGER ( metadata->getManagementDestinationPort() ) );
		xmlw->writeEndElement();
		xmlw->writeEndElement();

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}

	if ( metadata->getTypeOfTest() == TYPE_PACKETS and chkXHTMLGroup->isChecked() ) exportItemToXhtml ( xmlw, expData->itemList );
	else
	{
		NMGStatisticItemDataListIterator itfstart = expData->itemList.begin();
		NMGStatisticItemDataListIterator itfend = expData->itemList.end();
		if ( metadata->getTypeOfTest() == TYPE_PACKETS )
		{
			while ( itfend-- != itfstart )
			{
				exportItemToXhtml ( xmlw, ( NMGStatisticPacketData * ) *itfend );
				xmlw->writeEmptyElement ( "br" );
			}
		}
		else if ( metadata->getTypeOfTest() == TYPE_TPUT )
		{
			while ( itfend-- != itfstart )
			{
				exportItemToXhtml ( xmlw, ( NMGStatisticThroughtputData * ) *itfend );
				xmlw->writeEmptyElement ( "br" );
			}
		}
	}
}

// Packet item
void NMGStatisticExportManager::exportItemToXhtml ( QXmlStreamWriter * xmlw, NMGStatisticPacketData * item )
{
	xmlw->writeTextElement ( "h2", item->flowName().isEmpty() ? "Flow " + INTEGER ( item->flowId() ) : item->flowName() );

	xmlw->writeTextElement ( "h3", "Basic Information" );

	xmlw->writeStartElement ( "table" );

	xmlw->writeStartElement ( "tr" );

	xmlw->writeStartElement ( "th" );
	xmlw->writeCharacters ( "Sent" );
	xmlw->writeEmptyElement ( "br" );
	xmlw->writeCharacters ( "Packets" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeCharacters ( "Received" );
	xmlw->writeEmptyElement ( "br" );
	xmlw->writeCharacters ( "Packets" );
	xmlw->writeEndElement();

	xmlw->writeTextElement ( "th", "Duration" );
	xmlw->writeEndElement();

	xmlw->writeStartElement ( "tr" );
	xmlw->writeTextElement ( "td", INTEGER ( item->sentPackets() ) );
	xmlw->writeTextElement ( "td", INTEGER ( item->receivedPackets() ) );
	xmlw->writeTextElement ( "td", simplifyTime ( ( int ) item->duration() ) );
	xmlw->writeEndElement();

	xmlw->writeEndElement();

	xmlw->writeEmptyElement ( "br" );

	// Delay
	if ( chkDelayAve->isChecked() or chkDelayMin->isChecked() or chkDelayMax->isChecked() or chkDelayStDev->isChecked()
	        or chkDelayJitter->isChecked() or chkDelayP50->isChecked() or chkDelayP95->isChecked() or chkDelayP99->isChecked() or chkDelayP999->isChecked() )
	{
		xmlw->writeTextElement ( "h3", "Delay statistics (msec)" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		if ( chkDelayAve->isChecked() )
			xmlw->writeTextElement ( "th", "Average" );
		if ( chkDelayMin->isChecked() )
			xmlw->writeTextElement ( "th", "Minimum" );
		if ( chkDelayMax->isChecked() )
			xmlw->writeTextElement ( "th", "Maximum" );
		if ( chkDelayStDev->isChecked() )
			xmlw->writeTextElement ( "th", "StDev" );
		if ( chkDelayJitter->isChecked() )
			xmlw->writeTextElement ( "th", "Jitter" );
		if ( chkDelayP50->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 50" );
		if ( chkDelayP95->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 95" );
		if ( chkDelayP99->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99" );
		if ( chkDelayP999->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99.9" );
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "tr" );
		if ( chkDelayAve->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->averageDelay() ) );
		if ( chkDelayMin->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->minDelay() ) );
		if ( chkDelayMax->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->maxDelay() ) );
		if ( chkDelayStDev->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->stDevDelay() ) );
		if ( chkDelayJitter->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->jitterDelay() ) );
		if ( chkDelayP50->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->delay50Percentile() ) );
		if ( chkDelayP95->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->delay95Percentile() ) );
		if ( chkDelayP99->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->delay99Percentile() ) );
		if ( chkDelayP999->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->delay999Percentile() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}

	// IPDV
	if ( chkIPDVAve->isChecked() or chkIPDVMin->isChecked() or chkIPDVMax->isChecked() or chkIPDVStDev->isChecked()
	        or chkIPDVJitter->isChecked() or chkIPDVP50->isChecked() or chkIPDVP95->isChecked() or chkIPDVP99->isChecked() or chkIPDVP999->isChecked() )
	{
		xmlw->writeTextElement ( "h3", "IPDV statistics (msec)" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		if ( chkIPDVAve->isChecked() )
			xmlw->writeTextElement ( "th", "Average" );
		if ( chkIPDVMin->isChecked() )
			xmlw->writeTextElement ( "th", "Minimum" );
		if ( chkIPDVMax->isChecked() )
			xmlw->writeTextElement ( "th", "Maximum" );
		if ( chkIPDVStDev->isChecked() )
			xmlw->writeTextElement ( "th", "StDev" );
		if ( chkIPDVJitter->isChecked() )
			xmlw->writeTextElement ( "th", "Jitter" );
		if ( chkIPDVP50->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 50" );
		if ( chkIPDVP95->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 95" );
		if ( chkIPDVP99->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99" );
		if ( chkIPDVP999->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99.9" );
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "tr" );
		if ( chkIPDVAve->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->averageIPDV() ) );
		if ( chkIPDVMin->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->minIPDV() ) );
		if ( chkIPDVMax->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->maxIPDV() ) );
		if ( chkIPDVStDev->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->stDevIPDV() ) );
		if ( chkIPDVJitter->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->jitterIPDV() ) );
		if ( chkIPDVP50->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv50Percentile() ) );
		if ( chkIPDVP95->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv95Percentile() ) );
		if ( chkIPDVP99->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv99Percentile() ) );
		if ( chkIPDVP999->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv999Percentile() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}

	xmlw->writeTextElement ( "h3", "Other Information" );

	xmlw->writeStartElement ( "table" );

	xmlw->writeStartElement ( "tr" );
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Lost" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Duplicated" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Out-Of-Order" );
	xmlw->writeEndElement();
	xmlw->writeEndElement();

	xmlw->writeStartElement ( "tr" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeEndElement();

	xmlw->writeStartElement ( "tr" );
	xmlw->writeTextElement ( "td", INTEGER ( item->lostPackets() ) );
	xmlw->writeTextElement ( "td", DOUBLE ( item->lossRatio() ) );
	xmlw->writeTextElement ( "td", INTEGER ( item->duplicatedPackets() ) );
	xmlw->writeTextElement ( "td", DOUBLE ( item->duplicatedRatio() ) );
	xmlw->writeTextElement ( "td", INTEGER ( item->outOfOrder() ) );
	xmlw->writeTextElement ( "td", DOUBLE ( item->outOfOrderRatio() ) );
	xmlw->writeEndElement();

	xmlw->writeEndElement();

	xmlw->writeEmptyElement ( "br" );
}

// Packet item by List Group by Test
void NMGStatisticExportManager::exportItemToXhtml ( QXmlStreamWriter * xmlw, NMGStatisticItemDataList itemList )
{
	NMGStatisticItemDataListIterator itfstart, itfend;
	NMGStatisticPacketData * item;

	xmlw->writeTextElement ( "h3", "Basic Information" );

	xmlw->writeStartElement ( "table" );

	xmlw->writeStartElement ( "tr" );
	xmlw->writeTextElement ( "th", "Flow" );
	xmlw->writeStartElement ( "th" );
	xmlw->writeCharacters ( "Sent" );
	xmlw->writeEmptyElement ( "br" );
	xmlw->writeCharacters ( "Packets" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeCharacters ( "Received" );
	xmlw->writeEmptyElement ( "br" );
	xmlw->writeCharacters ( "Packets" );
	xmlw->writeEndElement();
	xmlw->writeTextElement ( "th", "Duration" );
	xmlw->writeEndElement();

	itfstart = itemList.begin();
	itfend = itemList.end();

	while ( itfend-- != itfstart )
	{
		item = ( NMGStatisticPacketData * ) * itfend;

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
		xmlw->writeTextElement ( "td", INTEGER ( item->sentPackets() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->receivedPackets() ) );
		xmlw->writeTextElement ( "td", simplifyTime ( ( int ) item->duration() ) );
		xmlw->writeEndElement();
	}

	xmlw->writeEndElement();

	xmlw->writeEmptyElement ( "br" );

	// Delay
	if ( chkDelayAve->isChecked() or chkDelayMin->isChecked() or chkDelayMax->isChecked() or chkDelayStDev->isChecked()
	        or chkDelayJitter->isChecked() or chkDelayP50->isChecked() or chkDelayP95->isChecked() or chkDelayP99->isChecked() or chkDelayP999->isChecked() )
	{
		xmlw->writeTextElement ( "h3", "Delay statistics (msec)" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Flow" );
		if ( chkDelayAve->isChecked() )
			xmlw->writeTextElement ( "th", "Average" );
		if ( chkDelayMin->isChecked() )
			xmlw->writeTextElement ( "th", "Minimum" );
		if ( chkDelayMax->isChecked() )
			xmlw->writeTextElement ( "th", "Maximum" );
		if ( chkDelayStDev->isChecked() )
			xmlw->writeTextElement ( "th", "StDev" );
		if ( chkDelayJitter->isChecked() )
			xmlw->writeTextElement ( "th", "Jitter" );
		if ( chkDelayP50->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 50" );
		if ( chkDelayP95->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 95" );
		if ( chkDelayP99->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99" );
		if ( chkDelayP999->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99.9" );
		xmlw->writeEndElement();

		itfstart = itemList.begin();
		itfend = itemList.end();

		while ( itfend-- != itfstart )
		{
			item = ( NMGStatisticPacketData * ) * itfend;

			xmlw->writeStartElement ( "tr" );
			xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
			if ( chkDelayAve->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->averageDelay() ) );
			if ( chkDelayMin->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->minDelay() ) );
			if ( chkDelayMax->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->maxDelay() ) );
			if ( chkDelayStDev->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->stDevDelay() ) );
			if ( chkDelayJitter->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->jitterDelay() ) );
			if ( chkDelayP50->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->delay50Percentile() ) );
			if ( chkDelayP95->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->delay95Percentile() ) );
			if ( chkDelayP99->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->delay99Percentile() ) );
			if ( chkDelayP999->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->delay999Percentile() ) );
			xmlw->writeEndElement();
		}

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}

	// IPDV
	if ( chkIPDVAve->isChecked() or chkIPDVMin->isChecked() or chkIPDVMax->isChecked() or chkIPDVStDev->isChecked()
	        or chkIPDVJitter->isChecked() or chkIPDVP50->isChecked() or chkIPDVP95->isChecked() or chkIPDVP99->isChecked() or chkIPDVP999->isChecked() )
	{
		xmlw->writeTextElement ( "h3", "IPDV statistics (msec)" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Flow" );
		if ( chkIPDVAve->isChecked() )
			xmlw->writeTextElement ( "th", "Average" );
		if ( chkIPDVMin->isChecked() )
			xmlw->writeTextElement ( "th", "Minimum" );
		if ( chkIPDVMax->isChecked() )
			xmlw->writeTextElement ( "th", "Maximum" );
		if ( chkIPDVStDev->isChecked() )
			xmlw->writeTextElement ( "th", "StDev" );
		if ( chkIPDVJitter->isChecked() )
			xmlw->writeTextElement ( "th", "Jitter" );
		if ( chkIPDVP50->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 50" );
		if ( chkIPDVP95->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 95" );
		if ( chkIPDVP99->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99" );
		if ( chkIPDVP999->isChecked() )
			xmlw->writeTextElement ( "th", "Percentile 99.9" );
		xmlw->writeEndElement();

		itfstart = itemList.begin();
		itfend = itemList.end();

		while ( itfend-- != itfstart )
		{
			item = ( NMGStatisticPacketData * ) * itfend;

			xmlw->writeStartElement ( "tr" );
			xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
			if ( chkIPDVAve->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->averageIPDV() ) );
			if ( chkIPDVMin->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->minIPDV() ) );
			if ( chkIPDVMax->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->maxIPDV() ) );
			if ( chkIPDVStDev->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->stDevIPDV() ) );
			if ( chkIPDVJitter->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->jitterIPDV() ) );
			if ( chkIPDVP50->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv50Percentile() ) );
			if ( chkIPDVP95->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv95Percentile() ) );
			if ( chkIPDVP99->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv99Percentile() ) );
			if ( chkIPDVP999->isChecked() )
				xmlw->writeTextElement ( "td", DOUBLE ( item->ipdv999Percentile() ) );
			xmlw->writeEndElement();
		}

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}

	xmlw->writeTextElement ( "h3", "Other Information" );

	xmlw->writeStartElement ( "table" );

	xmlw->writeStartElement ( "tr" );
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "rowspan", "2" );
	xmlw->writeCharacters ( "Flow" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Lost" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Duplicated" );
	xmlw->writeEndElement();
	xmlw->writeStartElement ( "th" );
	xmlw->writeAttribute ( "colspan", "2" );
	xmlw->writeCharacters ( "Out-Of-Order" );
	xmlw->writeEndElement();
	xmlw->writeEndElement();

	xmlw->writeStartElement ( "tr" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeTextElement ( "th", "Packets" );
	xmlw->writeTextElement ( "th", "Ratio" );
	xmlw->writeEndElement();

	itfstart = itemList.begin();
	itfend = itemList.end();

	while ( itfend-- != itfstart )
	{
		item = ( NMGStatisticPacketData * ) * itfend;

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "td", item->flowName().isEmpty() ? INTEGER ( item->flowId() ) : item->flowName() );
		xmlw->writeTextElement ( "td", INTEGER ( item->lostPackets() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->lossRatio() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->duplicatedPackets() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->duplicatedRatio() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->outOfOrder() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->outOfOrderRatio() ) );
		xmlw->writeEndElement();
	}

	xmlw->writeEndElement();

	xmlw->writeEmptyElement ( "br" );
}

// Aggregation item
void NMGStatisticExportManager::exportItemToXhtml ( QXmlStreamWriter * xmlw, NMGStatisticThroughtputData * item )
{
	xmlw->writeTextElement ( "h2", "Netperf (" + item->getTypeOfProtocol().toUpper() + " Test)" );
// 	xmlw->writeTextElement ( "h3", "Source: " + item->getSourceAddress() );
// 	xmlw->writeTextElement ( "h3", "Destination: " + item->getDestinationAddress() );
	xmlw->writeEmptyElement ( "br" );

	if ( item->getTypeOfProtocol() == TAG_UDP )
	{
		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Send Socket Size (bytes)" );
		xmlw->writeTextElement ( "th", "Send Message Size (bytes)" );
		xmlw->writeTextElement ( "th", "Time (secs.)" );
		xmlw->writeTextElement ( "th", "#Send Message Ok" );
		xmlw->writeTextElement ( "th", "#Send Message Error" );
		xmlw->writeTextElement ( "th", "Send Throughput (kbps)" );
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "td", INTEGER ( item->getSendingSocketSizeInBytes() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->getSendingMessagesSizeInBytes() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->getElapsedTimeInSeconds() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->getSendingOkMessages() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->getSendingKoMessages() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->getSendingThroughputInKBitsPerSecond() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Recieved Socket Size (bytes)" );
		xmlw->writeTextElement ( "th", "#Received Message Ok" );
		xmlw->writeTextElement ( "th", "Received Throughput (kbps)" );
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "td", INTEGER ( item->getReceivingSocketSizeInBytes() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->getReceivingOkMessages() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->getReceivingThroughputInKBitsPerSecond() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}
	else
	{
		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Send Socket Size (bytes)" );
		xmlw->writeTextElement ( "th", "Send Message Size (bytes)" );
		xmlw->writeTextElement ( "th", "Time (secs.)" );
		xmlw->writeTextElement ( "th", "Send Throughput (kbps)" );
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "td", INTEGER ( item->getSendingSocketSizeInBytes() ) );
		xmlw->writeTextElement ( "td", INTEGER ( item->getSendingMessagesSizeInBytes() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->getElapsedTimeInSeconds() ) );
		xmlw->writeTextElement ( "td", DOUBLE ( item->getReceivingThroughputInKBitsPerSecond() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "th", "Recieved Socket Size (bytes)" );
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "tr" );
		xmlw->writeTextElement ( "td", INTEGER ( item->getReceivingSocketSizeInBytes() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();
		xmlw->writeEmptyElement ( "br" );
	}
}

// Aggregation Statistics Test
void NMGStatisticExportManager::exportTestToXhtml ( QXmlStreamWriter * xmlw, NMGStatisticAggregationData * expData )
{
	if ( chkThrAve->isChecked() or chkThrMin->isChecked() or chkThrMax->isChecked() )
	{
		xmlw->writeTextElement ( "h1", "Netperf Aggregation Statistics" );
		xmlw->writeEmptyElement ( "br" );

		xmlw->writeStartElement ( "table" );

		xmlw->writeStartElement ( "tr" );
		if ( chkThrAve->isChecked() )
			xmlw->writeTextElement ( "th", "Average Throughtput (kbps)" );
		if ( chkThrMin->isChecked() )
			xmlw->writeTextElement ( "th", "Min Throughtput (kbps)" );
		if ( chkThrMax->isChecked() )
			xmlw->writeTextElement ( "th", "Max Throughtput (kbps)" );
		xmlw->writeEndElement();

		xmlw->writeStartElement ( "tr" );
		if ( chkThrAve->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( expData->averageThroughtput() ) );
		if ( chkThrMin->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( expData->minThroughtput() ) );
		if ( chkThrMax->isChecked() )
			xmlw->writeTextElement ( "td", DOUBLE ( expData->maxThroughtput() ) );
		xmlw->writeEndElement();

		xmlw->writeEndElement();

		xmlw->writeEmptyElement ( "hr" );
	}
}

/********************************************************************/
/* TXT 								    */
/********************************************************************/
QString NMGStatisticExportManager::exportTestToTxt ( NMGStatisticExportData * expData )
{
	NMGMetaData * metadata = expData->metadata;

	QString text;

	if ( metadata->getTestName().isEmpty() )
		text = expData->fileName.mid ( expData->fileName.lastIndexOf ( "/" ) + 1 ) + txtseparator;
	else
		text =  metadata->getTestName() + txtseparator;

	//TODO writeMetadata with it->metadata
	/*text += expData->fileName.mid ( expData->fileName.lastIndexOf ( "/" ) + 1 ) + "\n";
	if (chkTXTMetadata->isChecked() )
		text += writeEmptyElement( TAG_METADATA );*/

	NMGStatisticItemDataListIterator itfstart = expData->itemList.begin();
	NMGStatisticItemDataListIterator itfend = expData->itemList.end();
	if ( metadata->getTypeOfTest() == TYPE_PACKETS )
		while ( itfend-- != itfstart )
			text += exportItemToTxt ( ( NMGStatisticPacketData * ) * itfend ) + "\n";
	else if ( metadata->getTypeOfTest() == TYPE_TPUT )
		while ( itfend-- != itfstart )
			text += exportItemToTxt ( ( NMGStatisticThroughtputData * ) * itfend ) + "\n";

	return text;
}

// Packet item
QString NMGStatisticExportManager::exportItemToTxt ( NMGStatisticPacketData * item )
{
	QString txtitem = "";

	// Flow ID
	txtitem += INTEGER ( item->flowId() ) + txtseparator;
	// Sent Packets
	txtitem +=  INTEGER ( item->sentPackets() ) + txtseparator;
	// Received Packets
	txtitem +=  INTEGER ( item->receivedPackets() ) + txtseparator;
	// Duration
	txtitem +=  INTEGER ( item->duration() ) + txtseparator;
	// Lost Packets
	txtitem += INTEGER ( item->lostPackets() ) + txtseparator;
	txtitem += INTEGER ( item->lossRatio() ) + txtseparator;
	// Duplicated Packets
	txtitem += INTEGER ( item->duplicatedPackets() ) + txtseparator;
	txtitem += INTEGER ( item->duplicatedRatio() ) + txtseparator;
	// Out-Of-Order Packets
	txtitem += INTEGER ( item->outOfOrder() ) + txtseparator;
	txtitem += INTEGER ( item->outOfOrderRatio() ) + txtseparator;

	// Delay
	if ( chkDelayAve->isChecked() )
		txtitem += DOUBLE ( item->averageDelay() ) + txtseparator;
	if ( chkDelayMin->isChecked() )
		txtitem += DOUBLE ( item->minDelay() ) + txtseparator;
	if ( chkDelayMax->isChecked() )
		txtitem += DOUBLE ( item->maxDelay() ) + txtseparator;
	if ( chkDelayStDev->isChecked() )
		txtitem += DOUBLE ( item->stDevDelay() ) + txtseparator;
	if ( chkDelayJitter->isChecked() )
		txtitem += DOUBLE ( item->jitterDelay() ) + txtseparator;
	if ( chkDelayP50->isChecked() )
		txtitem += DOUBLE ( item->delay50Percentile() ) + txtseparator;
	if ( chkDelayP95->isChecked() )
		txtitem += DOUBLE ( item->delay95Percentile() ) + txtseparator;
	if ( chkDelayP99->isChecked() )
		txtitem += DOUBLE ( item->delay99Percentile() ) + txtseparator;
	if ( chkDelayP999->isChecked() )
		txtitem += DOUBLE ( item->delay999Percentile() ) + txtseparator;

	// IPDV
	if ( chkIPDVAve->isChecked() )
		txtitem += DOUBLE ( item->averageIPDV() ) + txtseparator;
	if ( chkIPDVMin->isChecked() )
		txtitem += DOUBLE ( item->minIPDV() ) + txtseparator;
	if ( chkIPDVMax->isChecked() )
		txtitem += DOUBLE ( item->maxIPDV() ) + txtseparator;
	if ( chkIPDVStDev->isChecked() )
		txtitem += DOUBLE ( item->stDevIPDV() ) + txtseparator;
	if ( chkIPDVJitter->isChecked() )
		txtitem += DOUBLE ( item->jitterIPDV() ) + txtseparator;
	if ( chkIPDVP50->isChecked() )
		txtitem += DOUBLE ( item->ipdv50Percentile() ) + txtseparator;
	if ( chkIPDVP95->isChecked() )
		txtitem += DOUBLE ( item->ipdv95Percentile() ) + txtseparator;
	if ( chkIPDVP99->isChecked() )
		txtitem += DOUBLE ( item->ipdv99Percentile() ) + txtseparator;
	if ( chkIPDVP999->isChecked() )
		txtitem += DOUBLE ( item->ipdv999Percentile() ) + txtseparator;

	return txtitem;
}

// Aggregation item
QString NMGStatisticExportManager::exportItemToTxt ( NMGStatisticThroughtputData * item )
{
	QString txtitem = QString();

	// Common data (from TCP or UPD test)
	txtitem += item->getSourceAddress() + txtseparator;
	txtitem += item->getDestinationAddress() + txtseparator;
	txtitem += INTEGER ( item->getReceivingSocketSizeInBytes() ) + txtseparator;
	txtitem += INTEGER ( item->getSendingSocketSizeInBytes() ) + txtseparator;
	txtitem += INTEGER ( item->getSendingMessagesSizeInBytes() ) + txtseparator;
	txtitem += DOUBLE ( item->getElapsedTimeInSeconds() ) + txtseparator;
	txtitem += DOUBLE ( item->getReceivingThroughputInKBitsPerSecond() ) + txtseparator;
	txtitem += item->getTypeOfProtocol() + txtseparator;

	// Uncommon data (from UDP test)
	if ( item->getTypeOfProtocol() == TAG_UDP )
	{
		txtitem += DOUBLE ( item->getSendingThroughputInKBitsPerSecond() ) + txtseparator;
		txtitem += INTEGER ( item->getSendingOkMessages() ) + txtseparator;
		txtitem += INTEGER ( item->getSendingKoMessages() ) + txtseparator;
		txtitem += INTEGER ( item->getReceivingOkMessages() ) + txtseparator;
	}

	return txtitem;
}

// Aggregation Statistics Test
QString NMGStatisticExportManager::exportTestToTxt ( NMGStatisticAggregationData * expData )
{
	QString text = QString();

	if ( chkTXTHeaders->isChecked() )
	{
		if ( chkThrAve->isChecked() )
			text += TAG_AVERAGE + txtseparator;
		if ( chkThrMin->isChecked() )
			text += TAG_MIN + txtseparator;
		if ( chkThrMax->isChecked() )
			text += TAG_MAX;
		text += QString ( "\n" );
	}

	if ( chkThrAve->isChecked() )
		text += DOUBLE ( expData->averageThroughtput() ) + txtseparator;
	if ( chkThrMin->isChecked() )
		text += DOUBLE ( expData->minThroughtput() ) + txtseparator;
	if ( chkThrMax->isChecked() )
		text += DOUBLE ( expData->maxThroughtput() ) + txtseparator;

	return text + "\n";
}

/********************************************************************/
/* XSLT to Source Files 					    */
/********************************************************************/
bool NMGStatisticExportManager::applyXsltToSourceTest ( QStringList list )
{
	QString xsltPath = QFileDialog::getOpenFileName ( this->parentWidget(), tr ( "Select XSLT Stylesheet" ), QString ( XML_DIR ),
	                   tr ( "XSLT Stylesheet (*.xslt *.xsl);;XML Stylesheet (*.xml);;All files (*.*)" ) );

	if ( xsltPath.isEmpty() )
	{
		QMessageBox::critical ( this, "Error while creating the export file",
		                        "Error while creating the export file. No XSLT stylesheet specified." );
		return false;
	}

	while ( txtExportDir->text().isEmpty() )
	{
		if ( !selectResultDirectory() )
		{
			MessageBar::longMessage ( tr ( "Export cancelled by user." ) );
			return false;
		}
		savePreferences();
	}
	QString exportDirPath = txtExportDir->text();
	
	if ( list.isEmpty() ) return false;
	else if ( list.count() > 1 )
	{
		exportDirPath += "xslt_export-" + DATE() + "/";
		QDir dir ( exportDirPath );
		if ( not dir.exists() )
		{
			if ( not dir.mkdir ( exportDirPath ) )
			{
				QMessageBox::critical ( this, "Error while creating the export directory",
				                        "Error while creating the export directory at\n" + exportDirPath + ".\nExport cancelled." );
				return false;
			}
		}
	}

	foreach ( QString path, list )
	{
		QString fileName = path.mid ( path.lastIndexOf ( "/" ) + 1 );
		QString fileResultPath = exportDirPath + "xsltest_" + DATE() + "_" + fileName.remove ( ".xff", Qt::CaseInsensitive );
		NMXML::applyXsltToDoc ( path.toAscii().constData(), xsltPath.toAscii().constData(), fileResultPath.toAscii().constData() );
	}
	return true;
}

/********************************************************************/
/* Extra Function 						    */
/********************************************************************/
void NMGStatisticExportManager::generateIndex ( QString & exportPath )
{
	if ( QFile::exists ( exportPath + "index.html" ) )
		QFile::copy ( exportPath + "index.html", exportPath + "index-exported.html" );

	QDir dir ( exportPath );
	QStringList ls = dir.entryList ( QDir::Files, QDir::Name );

	QFile file ( exportPath + "index.html" );
	if ( file.open ( QFile::WriteOnly | QFile::Text ) )
	{
		QXmlStreamWriter xmlw ( &file );
		xmlw.setAutoFormatting ( true );

		writeHtmlHeader ( &xmlw, exportPath );
		xmlw.writeTextElement ( "h1", "Export file list:" );

		xmlw.writeStartElement ( "ul" );
		for ( QStringList::const_iterator it = ls.constBegin(); it != ls.constEnd(); ++it )
		{
			xmlw.writeStartElement ( "li" );
			xmlw.writeStartElement ( "a" );
			xmlw.writeAttribute ( "href", *it );
			xmlw.writeCharacters ( *it );
			xmlw.writeEndElement();
			xmlw.writeEndElement();
		}

		xmlw.writeEndDocument();
	}
	else
		QMessageBox::critical ( this, "Error while creating the export file index",
		                        "Error while creating the export file index at " + exportPath );
	file.close();
}

void NMGStatisticExportManager::writeHtmlHeader ( QXmlStreamWriter * xmlw, QString fileName )
{
	xmlw->writeDTD ( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" );

	xmlw->writeStartElement ( "html" );
	xmlw->writeAttribute ( "xmlns", "http://www.w3.org/1999/xhtml" );
	xmlw->writeAttribute ( "xml:lang", "en" );
	xmlw->writeAttribute ( "lang", "en" );

	xmlw->writeStartElement ( "head" );
	if ( !fileName.isEmpty() ) xmlw->writeTextElement ( "title", "Statistic XHTML Export of " + fileName );
	else xmlw->writeTextElement ( "title", "Statistic XHTML Export" );

	//<meta http-equiv="content-type" content="text/html; charset=utf-8"/>
	xmlw->writeEmptyElement ( "meta" );
	xmlw->writeAttribute ( "http-equiv", "Content-Type" );
	xmlw->writeAttribute ( "content", "text/html;charset=utf-8" );

	//<link rel="stylesheet" type="text/css" href="/<XML_CONF_PATH>/default.css"/>
	xmlw->writeEmptyElement ( "link" );
	xmlw->writeAttribute ( "rel", "stylesheet" );
	xmlw->writeAttribute ( "type", "text/css" );
	xmlw->writeAttribute ( "href", QDir::cleanPath ( QString ( XML_DIR ) + "/default.css" ) );

	xmlw->writeEndElement();
	xmlw->writeStartElement ( "body" );
}

QString NMGStatisticExportManager::writeTxtHeader ( QString testType )
{
	QString txtheader = "";

	if ( testType == TYPE_PACKETS )
	{
		QString sep = "_";

		txtheader += TAG_IDENTIFIER + txtseparator;
		txtheader += TAG_FLOW + txtseparator;
		txtheader += TAG_SENT + txtseparator;
		txtheader += TAG_RECEIVED + txtseparator;
		txtheader += TAG_DURATION + txtseparator;
		txtheader += TAG_LOST + txtseparator;
		txtheader += TAG_LOSS_RATIO + txtseparator;
		txtheader += TAG_DUPLICATED + txtseparator;
		txtheader += TAG_DUPLICATED_RATIO + txtseparator;
		txtheader += TAG_OUTOFORDER + txtseparator;
		txtheader += TAG_OUTOFORDER_RATIO + txtseparator;

		// Delay
		if ( chkDelayAve->isChecked() )
			txtheader +=  TAG_DELAY + sep + TAG_AVERAGE + txtseparator;
		if ( chkDelayMin->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_MIN + txtseparator;
		if ( chkDelayMax->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_MAX + txtseparator;
		if ( chkDelayStDev->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_STDEV + txtseparator;
		if ( chkDelayJitter->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_JITTER + txtseparator;
		if ( chkDelayP50->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_P50 + txtseparator;
		if ( chkDelayP95->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_P95 + txtseparator;
		if ( chkDelayP99->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_P99 + txtseparator;
		if ( chkDelayP999->isChecked() )
			txtheader += TAG_DELAY + sep + TAG_P999 + txtseparator;

		// IPDV
		if ( chkIPDVAve->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_AVERAGE + txtseparator;
		if ( chkIPDVMin->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_MIN + txtseparator;
		if ( chkIPDVMax->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_MAX + txtseparator;
		if ( chkIPDVStDev->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_STDEV + txtseparator;
		if ( chkIPDVJitter->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_JITTER + txtseparator;
		if ( chkIPDVP50->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_P50 + txtseparator;
		if ( chkIPDVP95->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_P95 + txtseparator;
		if ( chkIPDVP99->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_P99 + txtseparator;
		if ( chkIPDVP999->isChecked() )
			txtheader += TAG_IPDV + sep + TAG_P999 + txtseparator;
	}

	else if ( testType == TYPE_TPUT )
	{
		// Common data (from TCP or UPD test)
		txtheader += TAG_SOURCE + txtseparator;
		txtheader += TAG_DESTINATION + txtseparator;
		txtheader += TAG_RSS + txtseparator;
		txtheader += TAG_SSS + txtseparator;
		txtheader += TAG_SMS + txtseparator;
		txtheader += TAG_TIME + txtseparator;
		txtheader += TAG_RTPUT + txtseparator;
		txtheader += TAG_PROTOCOL + txtseparator;

		// Uncommon data (from UDP test)
		txtheader += TAG_STPUT + txtseparator;
		txtheader += TAG_SMO + txtseparator;
		txtheader += TAG_SMK + txtseparator;
		txtheader += TAG_RMO + txtseparator;
	}

	return txtheader + "\n";
}

QString NMGStatisticExportManager::simplifyTime ( int time, int startTimeUnit )
{
	QString simplification;
	switch ( startTimeUnit )
	{
		case MILISECS:
			if ( time >= 1000 )
			{
				simplification = simplifyTime ( time / 1000, SECS );
				if ( time % 1000 > 0 ) simplification += QString ( tr ( " %1 ms" ) ).arg ( time % 1000 );
			}
			else simplification = QString ( tr ( "%1 ms" ) ).arg ( time );
			break;
		case SECS:
			if ( time >= 60 )
			{
				simplification = simplifyTime ( time / 60, MINS );
				if ( time % 60 > 0 ) simplification += QString ( tr ( " %1 sec" ) ).arg ( time % 60 );
			}
			else simplification = QString ( tr ( "%1 sec" ) ).arg ( time );
			break;
		case MINS:
			if ( time >= 60 )
			{
				simplification = simplifyTime ( time / 60, HOURS );
				if ( time % 60 > 0.0 ) simplification += QString ( tr ( " %1 min" ) ).arg ( time % 60 );
			}
			else simplification = QString ( tr ( "%1 min" ) ).arg ( time );
			break;
		case HOURS:
			simplification = QString ( tr ( "%1 hr" ) ).arg ( time );
			break;
	}
	return simplification;
}

#include "nmgstatisticexportmanager.moc"
