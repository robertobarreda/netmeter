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

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "nmgconverterwidgetfromconf.h"

#include <iostream>
#include <cstdlib>
using namespace std;


/**
 * Constructor of the class
 */
NMGConverterWidgetFromConf::NMGConverterWidgetFromConf(QWidget *parent, const char *name) : QWidget ( parent )
{
	setObjectName ( name );

	labelOUTPUT = new QLabel("OUTPUT:");
	labelINPUT = new QLabel("INPUT:");
}

/**
 * Destructor of the class
 */
NMGConverterWidgetFromConf::~NMGConverterWidgetFromConf()
{
	delete(labelOUTPUT);
	delete(labelINPUT);
}

/**
 * Destructor of the class
 */
void NMGConverterWidgetFromConf::reset(QFrame *frame)
{
	// 1) Clear widgets
	delete(labelOUTPUT);
	delete(labelINPUT);

	QListIterator<QLabel *> listInlabel = QListIterator<QLabel *>(labelListIN);
	QListIterator<QLabel *> listOutlabel = QListIterator<QLabel *>(labelListOUT);
	QListIterator<QLineEdit *> listInline = QListIterator<QLineEdit *>(lineListIN);
	QListIterator<QLineEdit *> listOutline = QListIterator<QLineEdit *>(lineListOUT);
	QListIterator<QCheckBox *> listIncheck = QListIterator<QCheckBox *>(checkListIN);
	QListIterator<QCheckBox *> listOutcheck = QListIterator<QCheckBox *>(checkListOUT);

	while(listInlabel.hasNext())	delete(listInlabel.next());
	while(listInline.hasNext())	delete(listInline.next());
	while(listIncheck.hasNext())	delete(listIncheck.next());

	while(listOutlabel.hasNext())	delete(listOutlabel.next());
	while(listOutline.hasNext())	delete(listOutline.next());
	while(listOutcheck.hasNext())	delete(listOutcheck.next());


	// 2) Clear lists
		in_exec_order.clear();
		out_exec_order.clear();
		lineListIN.clear();
		labelListIN.clear();
		lineListOUT.clear();
		labelListOUT.clear();
		listFlagsIN.clear();
		listFlagsOUT.clear();
		checkListIN.clear();
		checkListOUT.clear();

	// 3) Repaint frame
		delete(frame->layout());
		frame->repaint();
}

/**
 * Displays the specific interface per script
 */
void NMGConverterWidgetFromConf::displayWidget(QFrame *frame, QString scriptName, const usage_struct st)
{
	exec_script = scriptName;
	
	// 1) Frame initialitzation

	frame->setFrameStyle( QFrame::Box|QFrame::Sunken );
	frame->setMinimumSize( frame->width(), frame->height() );
	frame->setMaximumHeight( frame->height() );

	// 2) Read usage_struct <input,output> params and create items

	int x = 0;
	int y = 0;

	QListIterator<infoparam> it(st.in_info);
	while (it.hasNext()) 
	{
		// Next Item;
		y+=20;
		infoparam info = it.next();
		
		QLineEdit * lineIN = new QLineEdit();		// LINE_EDIT
		QLabel * labelIN = new QLabel(info.name);	// LABEL
		listFlagsIN << info.flag + " ";

		// QLineEdit type:
		if(info.type == TAG_TP_INT)	// Enters positius
		{ 
			QIntValidator *val = new QIntValidator(lineIN);
			val->setBottom(0);

			lineIN->setValidator(val);
			lineIN->setGeometry (x, y, 50, 20 );
			lineIN->setMaximumSize (50,30);
			lineIN->setMinimumSize (50,20);

			// ADD QLineEdit:
			lineListIN.append(lineIN);
			in_exec_order.append(true);
		}
		else if (info.type == TAG_TP_FILE)		// Files
		{
			lineIN->setGeometry (x, y, 300, 20 );
			lineIN->setMinimumSize (200,20);

			// ADD QLineEdit:
			lineListIN.append(lineIN);
			in_exec_order.append(true);
		}
		else if (info.type == TAG_TP_CHECK)		// Checkbox
		{
			QCheckBox * checkIN = new QCheckBox();

			// ADD QCheckBox:
			checkListIN.append(checkIN);
			in_exec_order.append(false);
		}

		// ADD QLabel:
		labelListIN.append(labelIN);
	}

	y += 20;

	QListIterator<infoparam> ot(st.out_info);
	while (ot.hasNext()) 
	{ 
		// Next Item;
		y+=20;
		infoparam info = ot.next();
		
		QLineEdit * lineOUT = new QLineEdit();		// LINE_EDIT
		QLabel * labelOUT = new QLabel(info.name);	// LABEL
		listFlagsOUT << info.flag + " ";

		// QLineEdit type:
		if(info.type == TAG_TP_INT)	// Enters positius
		{ 
			QIntValidator *val = new QIntValidator(lineOUT);
			val->setBottom(0);

			lineOUT->setValidator(val);
			lineOUT->setGeometry (x, y, 50, 20 );
			lineOUT->setMaximumSize (50,30);
			lineOUT->setMinimumSize (50,20);

			// ADD QLineEdit:
			lineListOUT.append(lineOUT);
			out_exec_order.append(true);
		}
		else if (info.type == TAG_TP_FILE)		// Files
		{
			lineOUT->setGeometry (x, y, 300, 20 );
			lineOUT->setMinimumSize (200,20);

			// ADD QLineEdit:
			lineListOUT.append(lineOUT);
			out_exec_order.append(true);
		}
		else if (info.type == TAG_TP_CHECK)		// Checkbox
		{
			QCheckBox * checkOUT = new QCheckBox();

			// ADD QCheckBox:
			checkListOUT.append(checkOUT);
			out_exec_order.append(true);
		}

		// ADD QLabel:
		labelListOUT.append(labelOUT);
	}
	
	// 3) Create and establish the layout for the frame

	QGridLayout *layout = createLayout();
	frame->setLayout(layout);
	frame->repaint();
}

/**
 * Creates the Grid Layout for the labels + lineEdits + CheckBoxes
 */
QGridLayout* NMGConverterWidgetFromConf::createLayout()
{
	QGridLayout *layout = new QGridLayout();

	layout->setVerticalSpacing(10);
	layout->setColumnMinimumWidth(0, 40);
	layout->setColumnStretch( 0, 0 );
	layout->setColumnStretch( 1, 1 );
	
	int Row = 0; 
	QFont font("Helvetica", 12, QFont::Bold);
	QLabel * voidlabel = new QLabel(" ");

	labelINPUT = new QLabel("INPUT:");
	labelINPUT->setFont(font);
	layout->addWidget(labelINPUT, Row, 0, 1, 1); Row++;

	QListIterator<QLabel *> listInlabel = QListIterator<QLabel *>(labelListIN);
	QListIterator<QLineEdit *> listInline = QListIterator<QLineEdit *>(lineListIN);
	QListIterator<QCheckBox *> listIncheck = QListIterator<QCheckBox *>(checkListIN);
	QListIterator<bool> in = QListIterator<bool>(in_exec_order);

	/** INPUT (label + lineedit) */

	while(listInlabel.hasNext())
	{
		if(in.next())	// LINEEDIT
		{
			layout->addWidget(listInlabel.next(), Row, 0, 1, 1);
			layout->addWidget(listInline.next(), Row, 1, 1, 1); Row++;
		}
		else		// CHECKBOX
		{
			layout->addWidget(listIncheck.next(), Row, 1, 1, 1);
			layout->addWidget(listInlabel.next(), Row, 0, 1, 1); Row++;
		}
	}
	layout->addWidget(voidlabel, Row, 0, 1, 1); Row++;

	labelOUTPUT = new QLabel("OUTPUT:");
	labelOUTPUT->setFont(font);
	layout->addWidget(labelOUTPUT, Row, 0, 1, 1); Row++;

	QListIterator<QLabel *> listOutlabel = QListIterator<QLabel *>(labelListOUT);
	QListIterator<QLineEdit *> listOutline = QListIterator<QLineEdit *>(lineListOUT);
	QListIterator<QCheckBox *> listOutcheck = QListIterator<QCheckBox *>(checkListOUT);
	QListIterator<bool> out = QListIterator<bool>(in_exec_order);

	/** OUTPUT (label + lineedit) */

	while(listOutlabel.hasNext())
	{
		if(out.next())		// LINEEDIT
		{
			layout->addWidget(listOutlabel.next(), Row, 0, 1, 1);
			layout->addWidget(listOutline.next(), Row, 1, 1, 1); Row++;
		}

		else			// CHECKBOX
		{
			layout->addWidget(listOutcheck.next(), Row, 1, 1, 1);
			layout->addWidget(listOutlabel.next(), Row, 0, 1, 1); Row++;
		}
	}

	QSpacerItem * verticalSpacer = new QSpacerItem(400, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	layout->addItem(verticalSpacer, Row, 0, 1, 1);	
	Row++;

	return layout;
}

/**
 * Checks the params needed to execute the command
 */
bool NMGConverterWidgetFromConf::checkParams()
{
	bool ok = true;

	// exec_script:
	if(exec_script.isEmpty())	
	{
		ok = false;	
		cout << RED << "[ERROR] Script not found " << ENDCOLOR << endl;
	}

	/** INPUT */

	QStringListIterator *itFlagsIN = new QStringListIterator(listFlagsIN);
	QListIterator<QLabel *> listInlabel = QListIterator<QLabel *>(labelListIN);
	QListIterator<QLineEdit *> listInline = QListIterator<QLineEdit *>(lineListIN);
	QListIterator<bool> in = QListIterator<bool>(in_exec_order);

	while(listInlabel.hasNext() && itFlagsIN->hasNext() && ok)
	{
		QString flag = itFlagsIN->next().trimmed();

		if(in.next())	// LINEEDIT
		{
			QString param = listInline.next()->text().trimmed();
			if(flag.length() == 0) ok = !param.isEmpty();		// mandatory lineedits
		}
	}

	QStringListIterator *itFlagsOUT = new QStringListIterator(listFlagsOUT);
	QListIterator<QLabel *> listOutlabel = QListIterator<QLabel *>(labelListOUT);
	QListIterator<QLineEdit *> listOutline = QListIterator<QLineEdit *>(lineListOUT);
	QListIterator<bool> out = QListIterator<bool>(in_exec_order);

	/** OUTPUT */

	while(listOutlabel.hasNext() && itFlagsOUT->hasNext() && ok)
	{
		QString flag = itFlagsOUT->next().trimmed();

		if(out.next())	// LINEEDIT
		{
			QString param = listOutline.next()->text().trimmed();
			if(flag.length() == 0) ok = !param.isEmpty();		// mandatory lineedits
		}
	}

	return ok;
}

/**
 * Assembles the runcommand script_name + argument_list
 */
QPair<QString,QStringList> NMGConverterWidgetFromConf::getCommand()
{
	QStringList args;

	/** INPUT */

	QStringListIterator *itFlagsIN = new QStringListIterator(listFlagsIN);
	QListIterator<QLabel *> listInlabel = QListIterator<QLabel *>(labelListIN);
	QListIterator<QLineEdit *> listInline = QListIterator<QLineEdit *>(lineListIN);
	QListIterator<QCheckBox *> listIncheck = QListIterator<QCheckBox *>(checkListIN);
	QListIterator<bool> in = QListIterator<bool>(in_exec_order);

	while(listInlabel.hasNext() && itFlagsIN->hasNext())
	{
		QString flag = itFlagsIN->next().trimmed();

		if(in.next())	// LINEEDIT
		{
			QString param = listInline.next()->text().trimmed();

			if ((flag.length() > 0) && (param.length() > 0))	args << flag;
			if (param.length() > 0) 				args << param;
		}
		else		// CHECKBOX
		{
			if ((flag.length() > 0) && (listIncheck.next()->isChecked()))
				args << flag;
		}
	}

	QStringListIterator *itFlagsOUT = new QStringListIterator(listFlagsOUT);
	QListIterator<QLabel *> listOutlabel = QListIterator<QLabel *>(labelListOUT);
	QListIterator<QLineEdit *> listOutline = QListIterator<QLineEdit *>(lineListOUT);
	QListIterator<QCheckBox *> listOutcheck = QListIterator<QCheckBox *>(checkListOUT);
	QListIterator<bool> out = QListIterator<bool>(in_exec_order);

	/** OUTPUT */

	while(listOutlabel.hasNext() && itFlagsOUT->hasNext())
	{
		QString flag = itFlagsOUT->next().trimmed();

		if(out.next())	// LINEEDIT
		{
			QString param = listOutline.next()->text().trimmed();

			if ((flag.length() > 0) && (param.length() > 0))	args << flag;
			if (param.length() > 0) 				args << param;
		}
		else		// CHECKBOX
		{
			if ((flag.length() > 0) && (listOutcheck.next()->isChecked()))
				args << flag;
		}
	}

	return qMakePair(exec_script, args);
}