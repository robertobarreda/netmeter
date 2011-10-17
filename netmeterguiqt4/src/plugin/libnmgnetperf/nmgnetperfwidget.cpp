/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#include <QFileDialog>

#include "nmgnetperfwidget.h"
#include <messagebar.h>
#include <nmgmoduleutils.h>

#include <iostream>
#include <cstdlib>
using namespace std;

#define ID_PREFERENCE_LAST_TEST_PATH	"NetperfLastTestPath"

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGNetperfWidget::NMGNetperfWidget ( NMGISender * s, NMGIModuleCommunication * mc, NMGIModuleStationManager * sm,
                                     NMGModTabWidgetManager * tabWidgetMan, NMGModulePreferencesManager * prefMan,
                                     QWidget *parent, const char *name )
		: QWidget ( parent ), interpret ( this ), errorNotifier ( this )
{
	setObjectName ( name );
	setupUi ( this );
	idSubscribeStatistic = idSubscribeGraphic = lastResultFile = QString();
	progressItem = 0;
	toolBar = 0;
	sender = s;
	moduleCommunication = mc;
	stationManager = sm;
	tabWidgetManager = tabWidgetMan;
	preferencesManager = prefMan;
	dateStartTime->setDate ( QDate().currentDate() );
	timeStartTime->setTime ( QTime().currentTime() );
	int randomSourcePort = rand() % 1000 + DEFAULT_PORT;
	txtSourcePort->setValue ( randomSourcePort );

	createMenu();

	connect ( cmdStart, SIGNAL ( clicked() ), this, SLOT ( buttonStartClicked() ) );
	connect ( cmdStop, SIGNAL ( clicked() ), this, SLOT ( buttonStopClicked() ) );
	connect ( &progressTimer, SIGNAL ( timeout() ), this, SLOT ( incrementProgressItemBar() ) );
}

/**
 * Destructor of the class
 * @return
 */
NMGNetperfWidget::~NMGNetperfWidget()
{
	if ( progressItem ) progressItem->setComplete();
	progressItem = 0;
}

/**
 * Creates the flow list menu
 */
void NMGNetperfWidget::createMenu()
{
	menu = new QMenu();
	menu->addAction ( "Sta&rt test", this, SLOT ( buttonStartClicked() ), Qt::CTRL + Qt::Key_R );
	menu->addSeparator();
	menu->addAction ( "&Load test from disk", this, SLOT ( loadTestFromDisk() ), Qt::CTRL + Qt::Key_L );
	menu->addAction ( "&Save test to disk", this, SLOT ( saveTestToDisk() ), Qt::CTRL + Qt::Key_S );
}

/**
 * Returns the toolbar of the netperf widget
 * @return
 */
QToolBar * NMGNetperfWidget::getToolBar()
{
	if ( !toolBar )
	{
		toolBar = new QToolBar ( NMGModuleUtils::getMainWindow() );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "fileopen.png" ) ),
		                     tr ( "Load test from disk" ), this, SLOT ( loadTestFromDisk() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "filesave.png" ) ),
		                     tr ( "Save test to disk" ), this, SLOT ( saveTestToDisk() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "launch.png" ) ),
		                     tr ( "Start test" ), this, SLOT ( buttonStartClicked() ) );
		toolBar->addSeparator ();
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "statistic.png" ) ),
		                     tr ( "Send result to statistic" ), this, SLOT ( buttonStatisticClicked() ) );
		toolBar->addAction ( QIcon ( *NMGModuleUtils::getPixmap ( "graphicon.png" ) ),
		                     tr ( "Send result to graphic" ), this, SLOT ( buttonGraphicClicked() ) );
	}
	return toolBar;
}

/**
 * Slot which load a flow from disk
 */
void NMGNetperfWidget::loadTestFromDisk()
{
	QString fileName = QFileDialog::getOpenFileName ( this, tr ( "Select test file" ), getLastTestPath(),
	                   tr ( "XML Flow Format Configuration File (*.xfc);;XML Flow Format Result File (*.xff);;XML (*.xml);;All files (*.*)" ) );

	if ( !fileName.isEmpty() )
	{
		saveLastTestPathFromFilePath ( fileName );
		int reply = QMessageBox::QMessageBox::question ( this, "Confirm operation",
		            "Do you really want to delete current configuration?",
		            QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape );
		if ( reply == QMessageBox::No ) return;
		interpret.updateWidgetFromXMLDocument ( fileName );
	}
}

/**
 * Slot which saves flows to disk
 */
void NMGNetperfWidget::saveTestToDisk()
{
	if ( not allFieldsNeededCorrect() ) return;
	else
	{
		QString fileName = QFileDialog::getSaveFileName ( this, tr ( "File to save the configuration" ),
		                   getLastTestPath(), tr ( "XML Flow Format Configuration File (*.xfc);;XML (*.xml);;All files (*.*)" ) );

		if ( !fileName.isEmpty() )
		{
			if ( !fileName.contains ( ".xfc", Qt::CaseInsensitive ) ) fileName = fileName + ".xfc";
			saveLastTestPathFromFilePath ( fileName );

			interpret.generateXMLTestDocument ( fileName, FALSE );
		}
	}
}

/**
 * Slot executed when the user clicks the start button to start a new test
 */
void NMGNetperfWidget::buttonStartClicked()
{
	if ( not allFieldsNeededCorrect() ) return;

	QString xmlTest = interpret.generateXMLTestDocument ( TRUE );

	if ( sender->sendCommand ( Start, xmlTest ) )
	{
		cmdStop->setEnabled ( TRUE );
		cmdStart->setEnabled ( FALSE );
		tabWidgetManager->setTabImage ( "run.png" );
		maxTestTime = getMaximumEndTime();
		QString title = txtTestName->text() == "" ? "Netperf" : txtTestName->text();
		progressItem = ProgressManager::createProgressItem ( title );
		progressItem->setTotalItems ( 100 );
		connect ( progressItem, SIGNAL ( progressItemCanceled ( ProgressItem* ) ),
		          this, SLOT ( testCancelled ( ProgressItem * ) ) );
		progressItem->setStatus ( "Preparing test..." );
	}
	else MessageBar::longMessage ( "Command not started\nThere is no connection with the server!" );
}

/**
 * Checks if all the fields are correct to send a start message to the server
 * If they are not correct a message box is shown to the server
 * @return errorMessage
 */
bool NMGNetperfWidget::allFieldsNeededCorrect()
{
	QString errorMessage = "";

	//Source host check
	if ( ( hostSourceCombo->getStationAddressFromCombo() ).isEmpty() )
		errorMessage += "The source address is not an optional field.\n";
	else
	{
		NMGStation * sourceStation = hostSourceCombo->getStationFromCombo();
		if ( !sourceStation )
			errorMessage += "The source host is not optional.\n";
		else
		{
			if ( ( sourceStation->getStationFieldsManager().
			        getField ( stationManager->getModuleStationKey(), ID_NETPERF_PATH ) ).isEmpty() )
				errorMessage += "Netperf path is not an optional field for the source host.\n";
			if ( ( sourceStation->getManagementUsername() ).isEmpty() )
				errorMessage += "Management username is not an optional field for the source host.\n";
		}
	}

	//Destination host check
	if ( ( hostDestinationCombo->getStationAddressFromCombo() ).isEmpty() )
		errorMessage += "The destination address is not an optional field.\n";
	else
	{
		NMGStation * destionationStation = hostDestinationCombo->getStationFromCombo();
		if ( !destionationStation )
			errorMessage += "The destination host is not optional.\n";
		else
		{
			if ( destionationStation->getStationFieldsManager().
			        getField ( stationManager->getModuleStationKey(), ID_NETSERVER_PATH ) == "" )
				errorMessage += "Netserver path is not an optional field for the destination host.\n";
			if ( ( destionationStation->getManagementUsername() ).isEmpty() )
				errorMessage += "Management username is not an optional field for the destination host.\n";
		}
	}

	if ( !errorMessage.isEmpty() )
		QMessageBox::warning ( this, "Command not started", errorMessage );
	return errorMessage == "" ? TRUE : FALSE;
}


/**
 * Slot executed when the user clicks the stop button to stop a test which is being executed
 */
void NMGNetperfWidget::buttonStopClicked()
{
	sender->sendCommand ( Stop, "" );
	updateWidgetBecauseOfTestEnd();
}


/**
 * Slot executed when the user wants to send the test to the statistic module
 */
void NMGNetperfWidget::buttonStatisticClicked()
{
	if ( idSubscribeStatistic.isEmpty() )
	{
		// Subscribe in advance to the statistic plugin if you haven't done it before
		idSubscribeStatistic = moduleCommunication->subscribe ( "NMGStatistic", "<data><subscribe/></data>" );

		if ( !idSubscribeStatistic.isEmpty() ) 
			cout << GREEN << "[OK] Subscripted to NMGStatistic" << ENDCOLOR << endl;
		else 
			cout << RED << "[ERROR] Failed to subscript to NMGStatistic " << ENDCOLOR <<endl;
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
void NMGNetperfWidget::buttonGraphicClicked()
{
	if ( idSubscribeGraphic.isEmpty() )
	{
		// Subscribe in advance to the graphic plugin if you haven't done it before
		idSubscribeGraphic = moduleCommunication->subscribe ( "NMGGraphics", "<data><subscribe/></data>" );

		if ( !idSubscribeGraphic.isEmpty() ) 
			cout << GREEN << "[OK] Subscripted to NMGGraphics" << ENDCOLOR<<endl;
		else 
			cout << RED << "[ERROR] Failed to subscript to NMGGraphics " << ENDCOLOR<<endl;

	}

	if ( !sender->sendGUICommand ( Start, "<data><test>" + lastResultFile + "</test></data>", idSubscribeGraphic ) )
	{
                // If there's an error sending the command it's because the subscription is finished. 
                // Retry to subscribribe to a new instance of the plugin
		idSubscribeGraphic = QString();
                buttonGraphicClicked();
	}
}

/**
 * Updates the widgets becuse the end has ended
 */
void NMGNetperfWidget::updateWidgetBecauseOfTestEnd()
{
	cmdStop->setEnabled ( FALSE );
	cmdStart->setEnabled ( TRUE );

	if ( progressItem ) progressItem->setComplete();
	progressItem = 0;
}

/**
 * Returns TRUE if the protocol used in the test is IPv4
 * @return
 */
bool NMGNetperfWidget::isProtocolIPv4()
{
	NMGStation * station = hostDestinationCombo->getStationFromCombo();
	if ( !station ) return TRUE;
	else return station->getTestHost().isHostIPv4();
}

/**
 * Returns the last test path (or "" if undefined)
 * @return
 */
QString NMGNetperfWidget::getLastTestPath()
{
	QString lastTestPath = preferencesManager->getValue ( ID_PREFERENCE_LAST_TEST_PATH );
	if ( lastTestPath.isEmpty() ) lastTestPath = "";
	return lastTestPath;
}

/**
 * Saves the last test path from the filename path of the test
 * @param filePath
 */
void NMGNetperfWidget::saveLastTestPathFromFilePath ( const QString & filePath )
{
	if ( !filePath.isEmpty() )
	{
		int lastPos = filePath.lastIndexOf ( "/" );
		preferencesManager->addValue ( ID_PREFERENCE_LAST_TEST_PATH, filePath.left ( lastPos ) );
	}
}

/**
 * Notifies the arrival of the reply of the start message
 * @param data XML message to be parsed
 */
void NMGNetperfWidget::notifyResult ( QString data )
{
	lastResultFile = interpret.getResultFilePathFromResult ( data );
	if ( lastResultFile.isEmpty() )
		MessageBar::longMessage ( "The execution of the Netperf test wasn't succesful." );
	else
		MessageBar::longMessage ( "Netperf test finished\nResult at " + lastResultFile + "." );

	cout << GREEN << "[OK] Netperf test finished --> Result at " << qPrintable(lastResultFile) << ENDCOLOR << endl;

	updateWidgetBecauseOfTestEnd();
}

/**
 * Notifies the error of the core module
 * @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
 * @param errorMessage Error Message
 */
void NMGNetperfWidget::notifyError ( int errorType, QString errorMessage )
{
	if ( errorType >= 1 and errorType <= 5 ) 
		updateWidgetBecauseOfTestEnd();
	
	cout << RED << "NMGNetperfWidget::notifyError -->" << qPrintable(errorMessage) << ENDCOLOR << endl;
	errorNotifier.notifyErrorToUser ( errorType, errorMessage );
}

/**
 * Notifies an status reply form the core
 * @param status XMl document with root data
 */
void NMGNetperfWidget::notifyStatus ( QString status )
{
#ifdef DEBUG
	cout << "NMGNetperfWidget::notifyStatus " << qPrintable ( status ) << endl;
#endif
	enum NetperfStatusReply { NotStarted, SendingNetserverScriptFile, NetserverStarted, NetserverFinished,
	                          SendingNetperfScriptFile, NetperfStarted, NetperfFinished,
	                          DecodeStarted, ReceivingDecodeResultFile,
	                          TestFinished, TestError
	                        };

	if ( !progressItem )
		cerr << RED << "[ERROR] Status arrived without item at the progress bar!" << ENDCOLOR <<endl;
	else
	{
		int statusNum = interpret.parseStatusMessage ( status );
	
		if ( statusNum < 0 ) cerr << RED <<"[ERROR] at status message." << ENDCOLOR << endl;
		else
		{
			NetperfStatusReply reply = ( NetperfStatusReply ) statusNum;
			cout << LIGHTBLUE << "NMGNetperfWidget::notifyStatus --> ";
			switch ( reply )
			{
				case NotStarted: //0%
					runningNetperfAndNetserver = FALSE;
					progressItem->setStatus ( "Test not started" );
					cout << "Test not started" << ENDCOLOR << endl;
					break;
				case SendingNetserverScriptFile: //10%
					progressItem->updateProgress();
					progressItem->setProgress ( 10 );
					runningNetperfAndNetserver = FALSE;
					progressItem->setStatus ( "Sending Netserver command" );
					cout << "Sending Netserver command" << ENDCOLOR << endl;
					break;
				case NetserverStarted: //15%
					progressItem->updateProgress();
					progressItem->setProgress ( 15 );
					runningNetperfAndNetserver = FALSE;
					progressItem->setStatus ( "Netserver started" );
					cout << "Netserver started" << ENDCOLOR << endl;
					break;
				case SendingNetperfScriptFile: //20%
					progressItem->updateProgress();
					progressItem->setProgress ( 20 );
					runningNetperfAndNetserver = FALSE;
					progressItem->setStatus ( "Sending Netperf command" );
					cout << "Sending Netperf command" << ENDCOLOR << endl;
					break;
				case NetperfStarted: // 25% to 75% (50 increments to do)
					progressItem->setCompletedItems ( 25 );
					if ( !runningNetperfAndNetserver )
					{
						//start the timer to increase the progress bar
						stepTime = maxTestTime / 50;
						leftTime = maxTestTime;
						progressTimer.start ( stepTime );
						runningNetperfAndNetserver = TRUE;
					}
					progressItem->setStatus ( "Netperf and Netserver running" );
					cout << "Netperf and Netserver running" << ENDCOLOR << endl;
					break;
				case NetperfFinished://75%
					progressTimer.stop();
					runningNetperfAndNetserver = FALSE;
					progressItem->updateProgress();
					progressItem->setProgress ( 75 );
					progressItem->setStatus ( "Netperf finished" );
					cout << "Netperf finished" << ENDCOLOR << endl;
					break;
					/*case NetserverFinished://80%
						progressTimer.stop();
						runningNetperfAndNetserver = FALSE;
						progressItem->updateProgress();
						progressItem->setProgress(80);
						progressItem->setStatus ( "Netserver finished" );
						break;*/
				case DecodeStarted://95%
					progressItem->updateProgress();
					progressItem->setProgress ( 80 );
					progressItem->setStatus ( "Requesting result file" );
					cout << "Requesting result file" << ENDCOLOR << endl;
					break;
				case ReceivingDecodeResultFile: //90%
					progressItem->updateProgress();
					progressItem->setProgress ( 90 );
					progressItem->setStatus ( "Receiving result file" );
					cout << "Receiving result file" << ENDCOLOR << endl;
					break;
				case TestFinished: //100%
					progressItem->updateProgress();
					progressItem->setComplete();
					progressItem->setStatus ( "Test finished" );
					cout << "Test finished" << ENDCOLOR << endl;
					progressItem = 0;
					break;
				case TestError:
					progressTimer.stop();
					runningNetperfAndNetserver = FALSE;
					progressItem->setComplete();
					cout << ENDCOLOR << endl;
					cout << RED << "[ERROR] executing test" << ENDCOLOR << endl;
					progressItem->setStatus ( "Error executing test" );
					progressItem = 0;
					break;
				default:
					progressItem->setStatus ( "Unknown status!" );
					cout << ENDCOLOR << endl;
					cout << RED << "[ERROR] Unknown status!" << ENDCOLOR << endl;	
			}
		}
	}
}

/**
 * Increments the progress bar
 */
void NMGNetperfWidget::incrementProgressItemBar()
{
	if ( progressItem and leftTime > 1 )
	{
		leftTime -= stepTime;
		progressItem->incCompletedItems();
		progressItem->updateProgress();
		progressItem->setStatus ( "Netperf and Netserver running (" + getLeftTimeTestString() + " left)" );
	}
}

/**
 * Returns the string corresponding to the left execution time of the test (in a friendly way hoiur, min, ...);
 * @return
 */
QString NMGNetperfWidget::getLeftTimeTestString()
{
	if ( leftTime <= 0 ) leftTime = 1;
	if ( leftTime < 1000 ) //less than a second --> ms
		return QString::number ( leftTime ) + " ms";
	else if ( leftTime < 60*1000 ) //less than a minute --> sec
		return QString::number ( leftTime / 1000 ) + " sec";
	else if ( leftTime < 60*60*1000 ) //less than an hour --> minutes
		return QString::number ( leftTime / ( 60*1000 ) ) + " min " + QString::number ( leftTime % ( 60*1000 ) / 1000 ) + " sec";
	else
		return QString::number ( leftTime / ( 60*60*1000 ) ) + " h " + QString::number ( leftTime % ( 60*60*1000 ) / ( 60*1000 ) ) + " min";;
}

/**
 * Returns the maximum end time of all the flows of the test
 * @return
 */
int NMGNetperfWidget::getMaximumEndTime()
{
	if ( radSeconds->isChecked() )
		return ( txtTestDuration->value() ) * 1000;
	else
		return ( int ) ( ( txtTestDuration->value() / 8 ) * 1000 ); //We suppose a link of 100 Mbps
}

#include "nmgnetperfwidget.moc"
