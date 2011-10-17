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
#include "nmgwidgetfromxml.h"
#include "nmgwidgetfromxmlaction.h"

#include <QFile>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFlag>

#include <iostream>
using namespace std;


/**
 * Contructor of the class
 * @param parent QT's parent parameter
 * @param name QT's name parameter
 * @return
 */
NMGWidgetFromXML::NMGWidgetFromXML( QWidget *parent, const char *name )
		: NMGWidgetFromXMLInterface( parent, name )
{
	numButtons = numLayouts = 0;
	parentWidget = !parent ? this : parent;
	lastRow = lastCol = -1;
}

/**
 * Destructor of the class
 * @return
 */
NMGWidgetFromXML::~NMGWidgetFromXML()
{
}

/**
 * Hides the widget
 */
void NMGWidgetFromXML::hideWidget()
{
	QWidget::hide();
}

/**
 * Action executed whn the widget is hidden
 * @param
 */
void NMGWidgetFromXML::hideEvent( QHideEvent * )
{
	emit widgetHidden();
}

/**
 * Sets the xml file to fileName. It will be parsed and the widget will be created
 * @param fileName Path of the file with the xml file
 * @return true if the creation of the widget was succesful or false otherwise
 */
bool NMGWidgetFromXML::setXmlFile( const QString fileName )
{
	QFile file( fileName );
	if ( !file.open( QIODevice::ReadOnly ) )
	{
		QMessageBox::critical( 0, "Error opening file", "The file " + fileName + " not exists or is not readable." );
		return false;
	}
	QString errorMessage; int errorLine, errorColumn;
	if ( !domTree.setContent( &file, & errorMessage, &errorLine, &errorColumn ) )
	{
		QMessageBox::critical( 0, "Error", "The XML file " + fileName
		                       + " is not well formed (" + errorMessage + " - line "
		                       + QString( "%1" ).arg( errorLine ) + ", column " + QString( "%1" ).arg( errorColumn ) + ")." );
		file.close();
		return false;
	}
	file.close();

	createGUIFromXML();
	return true;
}

/**
 * Shows the xml tree of the node
 * @param node Node to show the content of its subtree
 * @param indent amount of indentation
 */
void NMGWidgetFromXML::showContentFile( QDomNode &node, int indent )
{
	while ( !node.isNull() )
	{
		if ( node.isElement() )
		{
#ifdef DEBUG
			for ( int i = 0; i < indent; i++ ) cout << ". . ";
			if ( node.nodeName() == TAG_BUTTON )
			{
				QDomText textChild = node.firstChild().toText();
				if ( !textChild.isNull() )
					cout << "<" << node.nodeName().toAscii().constData() << ">" << textChild.nodeValue().toAscii().constData() << "\n";
				else 	cout << "<" << node.nodeName().toAscii().constData() << "> null\n";
			}
			else if ( node.nodeName() == TAG_LAYOUT )
				cout << "<" << node.nodeName().toAscii().constData() << "  type=" << node.toElement().attribute( "type" ).toAscii().constData() << ">\n";
			else cout << "<" << node.nodeName().toAscii().constData() << ">\n";
			QDomNode child = node.toElement().firstChild();
			showContentFile( child, indent + 1 );
			for ( int i = 0; i < indent; i++ ) cout << ". . ";
			cout << "</" << node.nodeName().toAscii().constData() << ">" << endl;
#endif
		}
		node = node.nextSibling();
	}
}

/**
 * This method creates the widget parsing the xml tree
 */
void NMGWidgetFromXML::createGUIFromXML()
{
	for ( QDomNode node = domTree.documentElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_INFO )
				readGUIInfo( node );
			else if ( name == TAG_DESCRIPTION )
				readGUIDescription( node );
			else if ( name == TAG_ACTIONS )
				readGUIActions( node );
			else
				cerr << "Unkonwn widget's module description tag " << name.toAscii().constData() << endl;
		}
	}
}

/**
 * This method parses the parent Node to read the information of the module's interface
 * @param parent Node which contains the tag info to parse
 */
void NMGWidgetFromXML::readGUIInfo( QDomNode &parent )
{
	for ( QDomNode node = parent.toElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		//read info of the gui xml file descriptor if interested
	}
}

/**
 * This method parses the content of the tag description where the widget will be described
 * @param parent Node with the tag description
 */
void NMGWidgetFromXML::readGUIDescription( QDomNode &parent )
{

	parseChilds( parent, this );
}

/**
 * This method parses the ations of the widget
 * @param parent Node with the tag actions
 */
void NMGWidgetFromXML::readGUIActions( QDomNode &parent )
{
	for ( QDomNode node = parent.toElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_ACTION )
			{
				QString id = getAttr( node, ATTR_ID );
				if ( id != "" )
				{
					NMGWidgetFromXMLAction * a = new NMGWidgetFromXMLAction( this, this, node );
					eventHandler.addAction( id, a );
				}
			}
			else cerr << "Unkonwn widget's module description tag " << name.toAscii().constData() << endl;
		}
	}
}

/**
 * This method parses the childs of parent and adds them to a layout if there is one
 * @param parent node whom its childs wil be parsed
 * @param widgetParent QT's parent parameter for the future widgets
 * @param layout QBoxLayout if the there is any to add the widgets to them (0 if not)
 * @param isTopLevel It indicates if the layout is top level (is not top level if is inside another layout)
 * @param gridLayout QGridLayout if the there is any to add the widgets to them (0 if not)
 */
void NMGWidgetFromXML::parseChilds( QDomNode &parent, QWidget* widgetParent, QBoxLayout * layout, bool isTopLevel, QGridLayout * gridLayout )
{
	for ( QDomNode node = parent.toElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_VAR )
				createVariableFromNode( node );
			else if ( name == TAG_LAYOUT )
			{
				QLayout * newLayout;
				QString type = getAttr( node, ATTR_TYPE );
				if ( type == TYPE_GRIDLAYOUT ) newLayout = createGridLayoutFromNode( node, widgetParent, isTopLevel );
				else newLayout = createLayoutFromNode( node, widgetParent, isTopLevel );

				getGlobalAttributes( node );
				if ( layout ) layout->addLayout( newLayout );
				else if ( gridLayout ) gridLayout->addLayout( newLayout, lastRow, lastCol );
			}
			else
			{
				QWidget * w = 0;
				if ( name == TAG_BUTTON )
					w = createButtonFromNode( node, widgetParent );
				else if ( name == TAG_TEXT )
					w = createTextFromNode( node, widgetParent );
				else if ( name == TAG_LABEL )
					w = createLabelFromNode( node, widgetParent );
				else if ( name == TAG_FRAME )
					w = createFrameFromNode( node, widgetParent );
				else if ( name == TAG_OPTIONS )
					w = createOptionsFromNode( node, widgetParent );
				else
					cerr << "Unknown widget's module desctription tag " << name.toAscii().constData() << endl;
				getGlobalAttributes( node );
				if ( layout and w ) layout->addWidget( w );
				else if ( gridLayout and w ) gridLayout->addWidget( w, lastRow, lastCol );
			}
		}
	}
}

/**
 * It creates a variable (it can contain string values but it won't be visible in the widget)
 * @param parent Node with the tag var
 */
void NMGWidgetFromXML::createVariableFromNode( QDomNode &parent )
{
	NMGWidgetFromXMLVariable * var = new NMGWidgetFromXMLVariable();
	QString id = getAttr( parent, ATTR_ID );
	QString value = getAttr( parent, ATTR_VALUE );
	var->setId( id );
	var->setInput( value );
	addInput( var );
}

/**
 * It creates a QBoxLayout
 * @param parent Node with the tag layout
 * @param widgetParent Qt's parent parameter for the future widgets inside the layout
 * @param isTopLevel It indicates if the layout is top level
 * @return Layout created or 0 if error
 */
QBoxLayout * NMGWidgetFromXML::createLayoutFromNode( QDomNode &parent, QWidget* widgetParent, bool isTopLevel )
{
	QString type;
	QBoxLayout * layout;
	if ( parent.toElement().hasAttribute( ATTR_TYPE ) ) type = parent.toElement().attribute( ATTR_TYPE );
	if ( type == TYPE_HLAYOUT )
	{
		layout = new QHBoxLayout( isTopLevel ? widgetParent : 0 );
		layout->setContentsMargins( DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN );
		layout->setSpacing( DEFAULT_SPACING );
	}
	else if ( type == TYPE_VLAYOUT )
	{
		layout = new QVBoxLayout( isTopLevel ? widgetParent : 0 );
		layout->setContentsMargins( DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN, DEFAULT_MARGIN );
		layout->setSpacing( DEFAULT_SPACING );
	}
	else return 0;
	parseChilds( parent, widgetParent, layout, false );
	return layout;
}

/**
 * It creates a QGridLayout
 * @param parent Node with the tag layout
 * @param widgetParent Qt's parent parameter for the future widgets inside the layout
 * @param isTopLevel It indicates if the layout is top level
 * @return Layout created or 0 if error
 */
QGridLayout * NMGWidgetFromXML::createGridLayoutFromNode( QDomNode &parent, QWidget* widgetParent, bool isTopLevel )
{
	int rows = 0, cols = 0;
	if ( parent.toElement().hasAttribute( ATTR_ROWS ) ) rows = parent.toElement().attribute( ATTR_ROWS ).toInt();
	if ( parent.toElement().hasAttribute( ATTR_COLS ) ) cols = parent.toElement().attribute( ATTR_COLS ).toInt();
	if ( rows == 0 or cols == 0 ) return 0;
	lastGridRows = rows; lastGridCols = cols;
	lastRow = 0; lastCol = -1;
	QGridLayout * layout = new QGridLayout( isTopLevel ? widgetParent : 0 );

	layout->setMargin( DEFAULT_MARGIN );
	layout->setSpacing( DEFAULT_SPACING );
	parseChilds( parent, widgetParent, 0, false, layout );
	return layout;
}

/**
 * It creates a button
 * @param parent Node with the tag button
 * @param widgetParent Qt's parent parameter for the button
 * @return The Button created
 */
QPushButton * NMGWidgetFromXML::createButtonFromNode( QDomNode &parent, QWidget* widgetParent )
{
	NMGWidgetFromXMLButton * button;
	QString id = getAttr( parent, ATTR_ID );
	if ( id.isEmpty() ) id = DEFAULT_NAME;
	QDomText textChild = parent.firstChild().toText();
	button = new NMGWidgetFromXMLButton( textChild.isNull() ? DEFAULT_TEXT : textChild.nodeValue(), widgetParent, qPrintable( id ) );
	if ( id != DEFAULT_NAME ) eventHandler.addButton( id, button );
	return button;
}

/**
 * It creates a text line
 * @param parent Node with the tag text
 * @param widgetParent Qt's parent parameter for the text
 * @return The widget created
 */
QLineEdit * NMGWidgetFromXML::createTextFromNode( QDomNode &parent, QWidget* widgetParent )
{
	QString id = getAttr( parent, ATTR_ID );
	QString validate = getAttr( parent, ATTR_VALIDATE );
	if ( id.isEmpty() ) id = DEFAULT_NAME;

	NMGWidgetFromXMLTextLine * text = new NMGWidgetFromXMLTextLine( widgetParent, qPrintable( id ) );
	if ( !validate.isEmpty() )
		text->setValidator( createValidator( validate, text ) );
	if ( id != DEFAULT_NAME ) addInput( text );
	QDomText textChild = parent.firstChild().toText();

	if ( !textChild.isNull() ) text->setText( textChild.nodeValue() );
	return text;
}

/**
 * It creates a label
 * @param parent Node with the tag label
 * @param widgetParent Qt's parent parameter for the label
 * @return The widget created
 */
QLabel * NMGWidgetFromXML::createLabelFromNode( QDomNode &parent, QWidget* widgetParent )
{
	QDomText textChild = parent.firstChild().toText();
	return new QLabel( !textChild.isNull() ? textChild.nodeValue() : DEFAULT_TEXT, widgetParent );
}

/**
 * It creates a frame
 * @param parent Node with the tag frame
 * @param widgetParent Qt's parent parameter for the frame
 * @return The widget created
 */
QFrame * NMGWidgetFromXML::createFrameFromNode( QDomNode &parent, QWidget* widgetParent )
{
	QFrame * f = new QFrame( widgetParent );

	QSizePolicy sizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	sizePolicy.setHorizontalStretch( 0 );
	sizePolicy.setVerticalStretch( 0 );
	sizePolicy.setHeightForWidth( f->sizePolicy().hasHeightForWidth() );
	f->setSizePolicy( sizePolicy );
	f->setFrameShape( QFrame::StyledPanel );
	f->setFrameShadow( QFrame::Raised );
	parseChilds( parent, f );
	return f;
}

QGroupBox * NMGWidgetFromXML::createOptionsFromNode( QDomNode &parent, QWidget* widgetParent )
{
	QString type = getAttr( parent, ATTR_TYPE ), text = getAttr( parent, ATTR_TEXT ), id = getAttr( parent, ATTR_ID );
	if ( id == "" ) id = DEFAULT_NAME;

	NMGWidgetFromXMLButtonGroup * bg = new NMGWidgetFromXMLButtonGroup( widgetParent, qPrintable( id ) );
	bg->setTitle( text );

	QBoxLayout * layout;
	if ( type == TYPE_HLAYOUT ) layout = new QHBoxLayout;
	else layout = new QVBoxLayout;
	bg->setLayout( layout );

	bool oneChecked = false;
	for ( QDomNode node = parent.toElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_OPTION )
			{
				QString checkedAttr = getAttr( node, ATTR_CHECKED );
				bool checked = false;
				if ( not oneChecked and( checkedAttr == TYPE_TRUE or node.nextSibling().isNull() ) ) oneChecked = checked = true;
				QDomText textChild = node.firstChild().toText();
				if ( !textChild.isNull() ) bg->newOption( textChild.nodeValue(), checked );
			}
			else cerr << "Awaiting the tag " << TAG_OPTION << " not the tag " << name.toAscii().constData() << "." << endl;
		}
	}

	if ( id != DEFAULT_NAME ) addInput( bg );
	return bg;
}

/**
 * It gets the attributes of the node which are global (row, col, id, ... if it has them)
 * @param node Node with the possible attributes
 */
void NMGWidgetFromXML::getGlobalAttributes( QDomNode &node )
{
	/* Attributes row and col (predict them if they are not there) */
	QString row = getAttr( node, ATTR_ROW ), col = getAttr( node, ATTR_COL );
	int r, c;
	r = ( row == "" ? -1 : row.toInt() );
	c = ( col == "" ? -1 : col.toInt() );
	if ( r == -1 or c == -1 )
	{
		lastCol++;
		if ( lastCol == lastGridCols )
		{
			lastCol = 0;
			lastRow++;
		}
	}
	else
	{
		lastRow = r;
		lastCol = c;
	}

	/* Attribute */
	lastId = getAttr( node, ATTR_ID );
}

/**
 * Returns the value wich have the NMGWidgetFromXMLInputWidget with name id
 * @param id Identifier of the widget to get the value
 * @return Value of the widget or "" if it doesnt' exist
 */
QString NMGWidgetFromXML::getInput( QString id )
{
	QInputMapIterator it = inputMap.find( id );
	if ( it == inputMap.end() )
	{
		cerr << "There is no input widget with id " << id.toAscii().constData() << "." << endl;
		return "";
	}
	else return it.value()->getInput();
}

/**
 * Sets the value of the input widget with id
 * @param id Id of the input widget
 * @param value New value for the input widget
 */
void NMGWidgetFromXML::setInput( QString id, QString value )
{
	QInputMapIterator it = inputMap.find( id );
	if ( it == inputMap.end() )
		cerr << "There is no input widget with id " << id.toAscii().constData() << "." << endl;
	else it.value()->setInput( value );
}

/**
 * Sets the value of all the inputs widget to ""
 */
void NMGWidgetFromXML::cleanAllInput()
{
	for ( QInputMapIterator it = inputMap.begin(); it != inputMap.end(); ++it )
		setInput( it.key(), "" );
}


/**
 * Adds a input widget
 * @param iw Input widget to add
 */
void NMGWidgetFromXML::addInput( NMGWidgetFromXMLInputWidget *iw )
{
	inputMap.insert( iw->getId(), iw );
}

/**
 * Returns a validator of the type indicated
 * @param type Validator type
 * @param parent Qt's parent parameter
 * @return
 */
QValidator * NMGWidgetFromXML::createValidator( QString type, QObject * parent )
{
	QValidator * v = 0;
	if ( type == TYPE_INTEGER )
		v = new QIntValidator( TYPE_MIN_INTEGER, TYPE_MAX_INTEGER, parent );
	else if ( type == TYPE_PINTEGER )
		v = new QRegExpValidator( QRegExp( "[0-9]+" ), parent );
	else if ( type == TYPE_NINTEGER )
		v = new QIntValidator( TYPE_MIN_INTEGER, 0, parent );
	else if ( type == TYPE_DOUBLE )
		v = new QDoubleValidator( TYPE_MIN_DOUBLE, TYPE_MAX_DOUBLE, TYPE_DECIMALS_DOUBLE, parent );
	else if ( type == TYPE_PDOUBLE )
		v = new QDoubleValidator( 0, TYPE_MAX_DOUBLE, TYPE_DECIMALS_DOUBLE, parent );
	else if ( type == TYPE_NDOUBLE )
		v = new QDoubleValidator( TYPE_MIN_DOUBLE, 0, TYPE_DECIMALS_DOUBLE, parent );
	else cerr << "Unknown validate type " << type.toAscii().constData()  << " for the widget." << endl;

	return v;
}

/**
 * The Widget becomes a Modal widget
 */
void NMGWidgetFromXML::setModal()
{
	this->setWindowFlags( Qt::Dialog );
}

/**
 * Returns the button associated to the id
 * @param id Id of the button
 * @return Button or 0 if there is no such button
 */
QAbstractButton * NMGWidgetFromXML::getButton( QString id )
{
	return eventHandler.getButton( id );
}



/**
 * Makes the widget visible
 */
void NMGWidgetFromXML::show()
{
	NMGWidgetFromXMLInterface::show();
	/*UNUSED
	int w = QApplication::desktop()->width(), h = QApplication::desktop()->height();
	setGeometry((w-frameGeometry().width())/2,(h-frameGeometry().height())/2,geometry().width(),geometry().height());*/
}

#include "nmgwidgetfromxml.moc"
