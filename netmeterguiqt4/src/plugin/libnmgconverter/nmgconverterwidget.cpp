/***************************************************************************
 *   Copyright (C) 2008 by Joan Sala Calero <jsala@ac.upc.edu>             *
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

#include <QMessageBox>
#include <QDomDocument>
#include <QDomElement>
#include <QTextStream>
#include <QDir>
#include <QStringListIterator>
#include <QTreeWidgetItem>
#include <QProcess>

#include "nmgconverterwidget.h"
#include "nmgconverterwidgetfromconf.h"

#include <messagebar.h>
#include <nmgmoduleutils.h>
#include <nmgpreferencesmanager.h>

#include <iostream>
#include <cstdlib>
using namespace std;

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */

NMGConverterWidget::NMGConverterWidget(NMGISender * s, NMGIModuleCommunication * mc, QWidget *parent, const char *name,NMGModulePreferencesManager * prefMan) : QWidget ( parent ), preferencesManager ( prefMan )
{
	setObjectName ( name );
	setupUi ( this );
	progressItem = 0;
	toolBar = 0;

	conf = new NMGConverterWidgetFromConf(this, "frameScripts");
	moduleCommunication = mc;
	sender = s;

	descBox->setStyleSheet("background-color: lightblue");
	outputBox->setStyleSheet("background-color: lightblue");
	listScripts->setRootIsDecorated ( true );
	listScripts->setColumnCount ( 1 );

	createMenu();		// create de QMenu
	loadlistScripts();	// update the list of scripts available for conversion

	/** CONNECT'S **/
	connect ( listScripts, SIGNAL ( itemSelectionChanged() ), this, SLOT ( scriptSelected() ) );
	connect ( runButton, SIGNAL (clicked()), this, SLOT ( executeScript() ) );
}

/**
 * Destructor of the class
 */
NMGConverterWidget::~NMGConverterWidget()
{
	delete(conf);	
}


/**
 * Loads one by one all the files listed in the script directory
 */
void NMGConverterWidget::loadlistScripts()
{
	QDir *Directori = new QDir(XML_DIR);
	QStringList filters;
	filters << "*.csf";
	Directori->setNameFilters(filters);

	if(!Directori->exists())
		cerr << RED << "[ERROR] the xml directory doesn't exist." << ENDCOLOR << endl;
	else
	{
		QStringListIterator list = QStringListIterator(Directori->entryList(filters));

		while(list.hasNext())
		{
			QString filename = list.next().toLocal8Bit().constData();
			QString path = Directori->filePath(filename);
			loadFile(path);
		}
	}
}

/**
 * Loads a xml script file description
 * @param filepath 
 */
void NMGConverterWidget::loadFile(QString filepath)
{
	QFile scriptsFile( filepath );
	
	if (!scriptsFile.exists ())
		cout << RED << "[ERROR] Fatal error -> file" << qPrintable(filepath) << "doesn't exist" << ENDCOLOR << endl;

        else if ( scriptsFile.open( QFile::ReadOnly ) )
        {
		// Read file content
		QTextStream in(&scriptsFile);
		QString text = "";	
	
		while (!in.atEnd()) text = text + in.readLine();
		
		// Parse the script file:
		QDomDocument doc;
		int err = 0;

		if(!doc.setContent(text))	err = 1;
		else
		{
			// Update de list of scripts available
			QDomElement root = doc.documentElement();
			QString sdesc,desc,file,usage;

			if(root.tagName() != TAG_CONVERTER)	err = 1;
			else
			{
				QDomNode node1 = root.firstChild();	// TAG FILE
				QDomNode node2 = node1.nextSibling();	// TAG SHORT_DESC
				QDomNode node3 = node2.nextSibling();	// TAG DESC
				QDomNode node4 = node3.nextSibling();	// TAG USAGE

				if(node1.isElement() && node2.isElement())	
				{	
					desc = "0";
					file = "0";
					usage_struct st;

					QDomElement elem_file = node1.toElement();
					QDomElement elem_sdesc = node2.toElement();
					QDomElement elem_desc = node3.toElement();
					QDomElement elem_usage = node4.toElement();
				
					if((elem_file.tagName() == TAG_FILE) && err != 1)
					{
						QDomText id = elem_file.firstChild().toText();
						if(!id.isNull()) file = id.nodeValue();
					}
					else 	err = 1;
				
					if((elem_sdesc.tagName() == TAG_SHORT_DESC) && err != 1)
					{
						QDomText id = elem_sdesc.firstChild().toText();
						if(!id.isNull()) sdesc = id.nodeValue();
					}
					else 	err = 1;
				
					if((elem_desc.tagName() == TAG_DESC) && err != 1)
					{
						
						QDomText id = elem_desc.firstChild().toText();
						if(!id.isNull()) desc = id.nodeValue();
					}
					else	err = 1;
				
					if((elem_usage.tagName() == TAG_USAGE) && err != 1)
					{
						st = parseUsage(elem_usage);
					}
					else	err = 1;
				
					/* Add script */
					if((desc != "0") or (file != "0"))	addScript(file, sdesc ,desc, st);
				}
				else	err = 1;
			}
		}
		if(err != 0)	cerr << RED << "[ERROR] the file " << qPrintable(filepath) << " is not well formed." << ENDCOLOR << endl;
	}

	else	cout << RED << "[ERROR] Fatal error -> file converter.scripts.xml is not readable" << ENDCOLOR << endl;
}

/**
 * Parse the usage of a script
 */
usage_struct NMGConverterWidget::parseUsage(QDomElement usage)
{
/* example:
<nmgconverter>
        <file>MGenDLYParser.py</file>
	<short_desc>DLY to XFF conversion</short_desc>
        <desc>Python script that converts a mgen dly file (version 3.x | 4.x) into a netmeter xff file</desc>
        <usage>
                <in num="2">
                        <pin type="file" ext="*.dly"> Input DLY file: </pin>
                        <pin type="int" min="3" max="4"> MGen version: </pin>
                </in>
                <out num="1">
                        <pout type="file" ext="*.xff"> Output XFF file: </pout>
                </out>
        </usage>
</nmgconverter>
*/
	usage_struct st;

	for(QDomNode node = usage.firstChild(); !node.isNull(); node = node.nextSibling())
	{
		if(node.isElement())
		{
			QDomElement elem = node.toElement();
			if(elem.tagName() == TAG_IN)
			{
				st.N_in = elem.attribute(ATTR_NUM).toInt();
				st.in_info = parseInfo(elem);
			}
			else if(elem.tagName() == TAG_OUT)
			{
				st.N_out = elem.attribute(ATTR_NUM).toInt();
				st.out_info = parseInfo(elem);
			}
			else
			{
				QString id = elem.firstChild().toText().nodeValue();
				cerr << RED << qPrintable(id) << ENDCOLOR << endl;
			}
		}
	}

	return st;
}

QList<infoparam> NMGConverterWidget::parseInfo(QDomElement list)
{
	QList<infoparam> infolist;

	for(QDomNode node = list.firstChild(); !node.isNull(); node = node.nextSibling())
	{
		if(node.isElement())
		{
			QDomElement elem = node.toElement();
			if(elem.tagName() == TAG_PIN || elem.tagName() == TAG_POUT)
			{
				infoparam info;
	
				info.type = elem.attribute(ATTR_TYPE);		// type
				info.ext = elem.attribute(ATTR_EXT);		// extension
				info.flag = elem.attribute(ATTR_FLAG);		// flag
				info.max = elem.attribute(ATTR_MAX).toInt();	// maximum
				info.min = elem.attribute(ATTR_MIN).toInt();	// minimum

				QDomText id = elem.firstChild().toText();
				info.name = id.nodeValue();

				if(!id.isNull()) 
					infolist.append(info);
			}
			else
			{
				QString id = elem.firstChild().toText().nodeValue();
				cerr << RED << qPrintable(id) << ENDCOLOR << endl;
			}
		}
	}

	return infolist;
}


/**
 * Add a script to the tree widget
 */
void NMGConverterWidget::addScript(QString name, QString sdesc, QString desc, usage_struct st)
{
	// 1: Update tree widget
	QTreeWidgetItem * item = new QTreeWidgetItem(listScripts);
	
	if(!sdesc.isEmpty())	item->setText(0, sdesc);
	else			item->setText(0, name);

	// 2: Update Qmaps
	mapScripts.insert(name, desc);		// < script, desc >
	mapUsage.insert(name, st);		// < script, usage >
	mapShort.insert(sdesc, name);		// < sdesc, script>

	cout << LIGHTBLUE << "[INFO] script loaded : "<< qPrintable(name) << ENDCOLOR << endl;
	cout << "DESC:\t" << qPrintable(desc) << endl << endl;
	cout << "USAGE:" << endl;
	cout << "\tInput Parameters:\n" << endl;

	QListIterator<infoparam> ilist(st.in_info);

	while (ilist.hasNext()) 
	{
		infoparam info = ilist.next();

		if (info.type == TAG_TP_FILE)
		{
			if(info.flag != "")	
			{
				cout << "\t\tflag = " << qPrintable(info.flag) << " , ";
				cout << "name = " << qPrintable(info.name) << " , ";
				cout << "type = " << qPrintable(info.type) << " , "; 
				cout << "ext = " << qPrintable(info.ext) << endl;
			}
			else
			{
				cout << "\t\tname = " << qPrintable(info.name) << " , ";
				cout << "type = " << qPrintable(info.type) << " , "; 
				cout << "ext = " << qPrintable(info.ext) << endl;
			}
		}
		else if (info.type == TAG_TP_INT)
		{
			if(info.flag != "")	
			{
				cout << "\t\tflag = " << qPrintable(info.flag) << " , ";
				cout << "name = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << " , ";
				cout << "min = " << info.min << " , ";
				cout << "max = " << info.max << endl;
			}
			else
			{
				cout << "\t\tname = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << " , ";
				cout << "min = " << info.min << " , ";
				cout << "max = " << info.max << endl;
			}
		}
		else if (info.type == TAG_TP_CHECK)
		{
			if(info.flag != "")
			{
				cout << "\t\tflag = " << qPrintable(info.flag) << " , ";
				cout << "name = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << endl;
			}
			else
			{
				cout << "\t\tname = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << endl;
			}
		}
		else
		{
			cout << RED << "[WARNING] Unknown configuration type of parameter " << qPrintable(info.type) << ENDCOLOR << endl;
		}
	}

	cout << "\n\tOutput Parameters:\n" << endl;

	QListIterator<infoparam> olist(st.out_info);

	while (olist.hasNext()) 
	{
		infoparam info = olist.next();

		if (info.type == TAG_TP_FILE)
		{
			if(info.flag != "")	
			{
				cout << "\t\tflag = " << qPrintable(info.flag) << " , ";
				cout << "name = " << qPrintable(info.name) << " , ";
				cout << "type = " << qPrintable(info.type) << " , "; 
				cout << "ext = " << qPrintable(info.ext) << endl;
			}
			else
			{
				cout << "\t\tname = " << qPrintable(info.name) << " , ";
				cout << "type = " << qPrintable(info.type) << " , "; 
				cout << "ext = " << qPrintable(info.ext) << endl;
			}
		}
		else if (info.type == TAG_TP_INT)
		{
			if(info.flag != "")	
			{
				cout << "\t\tflag = " << qPrintable(info.flag) << " , ";
				cout << "name = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << " , ";
				cout << "min = " << info.min << " , ";
				cout << "max = " << info.max << endl;
			}
			else
			{
				cout << "\t\tname = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << " , ";
				cout << "min = " << info.min << " , ";
				cout << "max = " << info.max << endl;
			}
		}
		else if (info.type == TAG_TP_CHECK)
		{
			if(info.flag != "")
			{
				cout << "\t\tflag = " << qPrintable(info.flag) << " , ";
				cout << "name = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << endl;
			}
			else
			{
				cout << "\t\tname = " << qPrintable(info.type) << " , ";
				cout << "type = " << qPrintable(info.name) << endl;
			}
		}
		else
		{
			cout << RED << "[WARNING] Unknown configuration type of parameter " << qPrintable(info.type) << ENDCOLOR << endl;
		}	
	}
	cout << endl << endl;
}


/**
 * Slot executed when the user selection has changed
 */
void NMGConverterWidget::scriptSelected()
{
	QTreeWidgetItem * item = listScripts->currentItem();

        if ( item )
	{
		QString sdesc = item->text(0);
		QString name = mapShort.value(sdesc);
		QString desc = "DESCRIPTION:  " + mapScripts.value(name);
		
		/* Show description */
		descBox->setText(desc);

		/* Re-Display widget */
		conf->reset(frameScripts);
		conf->displayWidget(frameScripts, name, mapUsage.value(name));
	}
}

/**
 * Slot executed when the user clicks the run button
 */
void NMGConverterWidget::executeScript()
{
	progressItem = 0;

	if(conf->checkParams())
	{
		// 1) Check that the script file exists and it's executable:
		QPair<QString,QStringList> command = conf->getCommand();
		QString path = SCRIPTS_DIR;
		QString script = path.append("/" + command.first);
		QStringList params = command.second;
	
		if(check_executable(script))
		{
			// 2) Disable run button && Start progress-bar:
			runButton->setEnabled(false);
			initProgress("Running script :: " + script);
			
			// 3) QProcess -> exec script:
			lastUsedScript = script;
			myProcess = new QProcess(this);
			connect(myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(printstdOut()));
			connect(myProcess, SIGNAL(readyReadStandardError()), this, SLOT(printstdErr()));
			connect(myProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinish(int, QProcess::ExitStatus)));
			
			outputBox->setText("");
			printCommand(script, params);
			myProcess->start(script, params);
		}
		else
		{
			cout << RED << "[ERROR] The script is not executable ->  needs: chmod +x file" << ENDCOLOR << endl;
			QMessageBox::warning ( 0, "The script is not executable", "Add +x permissions to it (ex: chmod +x)");
		}
	}
	else
	{
		cout << RED << "[ERROR] Check the params needed for the script " << ENDCOLOR << endl;
		QMessageBox::warning ( 0, "The script wasn't executed", "Select a script and fill in the parameters required");
	}
}

/**
 * Print command
 */
void NMGConverterWidget::printCommand(QString script, QStringList params)
{
	QString paramsList = "";
	QStringListIterator *it = new QStringListIterator(params);
	
	while(it->hasNext())	paramsList.append(it->next() + " ");

	cout << "******************** QPROCESS command ***********************************" << endl << endl;
	cout << "PATH = " << qPrintable(script) << endl;
	cout << "ARGS = " << qPrintable(paramsList) << endl << endl;

	outputBox->append("\n******************** QPROCESS command ***********************************\n");
	outputBox->append("PATH = " + script + "\nARGS = " + paramsList + "\n");
}

/**
 * Slot Stdout
 */
void NMGConverterWidget::printstdOut()
{
	QByteArray qba = myProcess->readAllStandardOutput();
	QString outmsg = "";
	outmsg += qba;

	cout << "******************** QPROCESS standard output ***************************" << endl << endl;
	cout << qPrintable(outmsg) << endl;

	outputBox->append("******************** QPROCESS standard output ***************************\n");
	outputBox->append(outmsg + "\n");
}

/**
 * Slot Stderr
 */
void NMGConverterWidget::printstdErr()
{
	QByteArray qba = myProcess->readAllStandardError();
	QString errormsg = qba;

	cout << "******************** QPROCESS standard error ****************************" << endl << endl;
	cout << qPrintable(errormsg) << endl;

	outputBox->append("******************** QPROCESS standard error ****************************\n");
	outputBox->append(errormsg + "\n");
}

/**
 * Slot executed when the process has finished
 */
void NMGConverterWidget::processFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
	// Run finished:
	finishProgress("Finishing script: " + lastUsedScript);
	runButton->setEnabled(true);

	outputBox->append("Finishing script: " + lastUsedScript);
}

/**
 * Init the progress bar
 */
bool NMGConverterWidget::check_executable(QString path)
{
	QFile *p = new QFile (path);
	if(p->exists())
	{
		QFile::setPermissions(path, QFile::ReadOwner|QFile::ReadUser|QFile::ExeOwner|QFile::ExeGroup);
		return true;
	}
	else	return false;
}

/**
 * Init the progress bar
 */
void NMGConverterWidget::initProgress(QString title)
{
	cout << endl;
	cout << LIGHTBLUE << "[INFO] init *** "<< qPrintable(title) << " ***" << ENDCOLOR << endl << endl;
	progressItem = ProgressManager::createProgressItem (title);
	progressItem->setTotalItems(100);
	connect (progressItem, SIGNAL (progressItemCanceled ( ProgressItem* )), this, SLOT (runCancelled ( ProgressItem * )));
	progressItem->setProgress(10);
	progressItem->setStatus("Running ...");
	progressItem->updateProgress();
}

/**
 * Finish the progress bar
 */
void NMGConverterWidget::finishProgress(QString title)
{
	cout << endl;
	cout << LIGHTBLUE << "[INFO] finish *** "<< qPrintable(title) << " ***" << ENDCOLOR << endl << endl;
	if(progressItem)
	{
		progressItem->setProgress(100);
		progressItem->updateProgress();
		progressItem->setComplete();
	}
}

/**
 * Test cancelled
 */
void NMGConverterWidget::runCancelled(ProgressItem *)
{
	if(progressItem)
	{
		progressItem->cancel();
		progressItem = 0;
	}
	runButton->setEnabled(true);	
}

/**
 * Creates the Qmenu
 */
void NMGConverterWidget::createMenu()
{
	menu = new QMenu();
}

/**
 * Returns the toolbar of the converter widget
 * @return
 */
QToolBar * NMGConverterWidget::getToolBar()
{
	if ( !toolBar )
	{
		toolBar = new QToolBar ( NMGModuleUtils::getMainWindow() );

		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "statistic.png" ) ),
		                     tr ( "Send result to statistic" ), this, SLOT ( buttonStatisticClicked() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "graphicon.png" ) ),
		                     tr ( "Send result to graphic" ), this, SLOT ( buttonGraphicClicked() ) );

		toolBar->addSeparator();

		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "launch.png" ) ), 
		                     tr ( "Run script" ), this, SLOT ( executeScript() ) );
	}
	return toolBar;
}

/**
 * Updates the widgets because the end has ended
 */
void NMGConverterWidget::updateWidgetBecauseOfTestEnd()
{
	if ( progressItem ) progressItem->setComplete();
	progressItem = 0;
}

/**
 * Notifies the arrival of the reply of the start message
 * @param data XML message to be parsed
 */
void NMGConverterWidget::notifyResult ( QString data )
{
	#ifdef DEBUG
		cout << GREEN << "[ERROR] notifyResult not implemented " << ENDCOLOR << endl;
	#endif
}

/**
 * Notifies the error of the core module
 * @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
 * @param errorMessage Error Message
 */
void NMGConverterWidget::notifyError ( int errorType, QString errorMessage )
{
	#ifdef DEBUG
		cout << "NMGConverterWidget::notifyError " << qPrintable ( status ) << endl;
		cout << RED << "[ERROR] notifyError not implemented" << ENDCOLOR << endl;
	#endif
}

/**
 * Notifies an status reply form the core
 * @param status XMl document with root data
 */
void NMGConverterWidget::notifyStatus ( QString status )
{
#ifdef DEBUG
	cout << "NMGConverterWidget::notifyStatus " << qPrintable ( status ) << endl;
	cout << RED << "[ERROR] notifyStatus not implemented" << ENDCOLOR << endl;
#endif
}

/**
 * Slot executed when the user wants to send the test to the statistic module
 */
void NMGConverterWidget::buttonStatisticClicked()
{
	if (lastResultFile.isEmpty())	return;

	if ( idSubscribeStatistic.isEmpty() )
	{
		// Subscribe in advance to the statistic plugin if you haven't done it before
		idSubscribeStatistic = moduleCommunication->subscribe ( "NMGStatistic", "<data><subscribe/></data>" );

		if ( !idSubscribeStatistic.isEmpty() ) 
			cout << GREEN << "[OK] Subscripted to NMGStatistic" << ENDCOLOR << endl;
		else 
			cout << RED <<"[ERROR] Failed to subscript to NMGStatistic " << ENDCOLOR <<endl;
	}

	if ( !sender->sendGUICommand ( Start, "<data><test>" + lastResultFile + "</test></data>", idSubscribeStatistic ) )
	{
                // If there's an error sending the command it's because the subscription is finished. 
                // Retry to subscribribe to a new instance of the plugin
		idSubscribeStatistic = QString();
                buttonStatisticClicked();
	}
}

/**
 * Slot executed when the user wants to send the test to the graphic module
 */
void NMGConverterWidget::buttonGraphicClicked()
{
	if (lastResultFile.isEmpty())	return;

	if ( idSubscribeGraphic.isEmpty() )
	{
		// Subscribe in advance to the graphic plugin if you haven't done it before
		idSubscribeGraphic = moduleCommunication->subscribe ( "NMGGraphics", "<data><subscribe/></data>" );

		if ( !idSubscribeGraphic.isEmpty() ) 
			cout << GREEN << "[OK] Subscripted to NMGGraphics" << ENDCOLOR<<endl;
		else 
			cout << RED << "[ERROR] Failed to subscript to NMGGraphics " << ENDCOLOR<< endl;
	}

	if ( !sender->sendGUICommand ( Start, "<data><test>" + lastResultFile + "</test></data>", idSubscribeGraphic ) )
	{
                // If there's an error sending the command it's because the subscription is finished. 
                // Retry to subscribribe to a new instance of the plugin
		idSubscribeGraphic = QString();
                buttonGraphicClicked();
	}
}

#include "nmgconverterwidget.moc"

