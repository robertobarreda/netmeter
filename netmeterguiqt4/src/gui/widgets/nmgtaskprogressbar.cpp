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
#include "nmgtaskprogressbar.h"

#include <nmgconnection.h>

#include <QWidgetAction>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGTaskProgressBar::NMGTaskProgressBar ( QWidget *parent, const char *name )
	: QProgressBar ( parent )
{	
	setObjectName( name );
	setRange( 0, MAXPROGRESS );
	setMaximumHeight( 16 );
	setMaximumWidth( 200 );
	setTextVisible( false );
	menu = new QMenu ( this );
	count = 0;
	connect ( NMGConnection::getConnection(), SIGNAL ( newTask ( QString, QString ) ), 
		this, SLOT ( newTask ( QString, QString ) ) );
	connect ( NMGConnection::getConnection(), SIGNAL ( finishedTask ( QString, QString, QString ) ), 
		this, SLOT ( finishedTask ( QString, QString, QString ) ) );
}


/**
 * Destructor of the class
 * @return
 */
NMGTaskProgressBar::~NMGTaskProgressBar()
{}

/**
 * Executed when the user clicks on the widget
 * @param e
 */
void NMGTaskProgressBar::mousePressEvent ( QMouseEvent * e )
{
	if ( count > 0 )
	{
		int x = e->globalPos().x(), y = e->globalPos().y();
		y -= 24 * count;
		QPoint p ( x, y );
		menu->popup ( p );
	}
}

/**
 * Returns the widget that has to be the parent of the tasks of the widget
 * @return
 */
QWidget * NMGTaskProgressBar::getParentTasks()
{
	return menu;
}

/**
 * Adds the widget to the progress bar
 * @param t
 */
void NMGTaskProgressBar::addTask ( NMGTaskWidget * t )
{
	if ( t->parent() != getParentTasks() ) 
	{
		t->setParent ( getParentTasks() ); //TODO(remove) [qt3] t->reparent ( getParentTasks(), t->pos() );
		//t->move ( t->pos() );
	}
	QWidgetAction * actionNMGTaskWidget = new QWidgetAction (t);
	menu->addAction ( actionNMGTaskWidget );
 	t->setId ( actionNMGTaskWidget );
	count++;
}

/**
 * Removes the widget from the progress bar
 * @param t
 */
void NMGTaskProgressBar::removeTask ( NMGTaskWidget * t )
{
	menu->removeAction ( t->getId() );
	count--;
}


/**
 * Slot which creates a new task
 * @param id
 * @param moduleName
 */
void NMGTaskProgressBar::newTask ( QString id, QString moduleName )
{
	NMGTaskWidget * t;
	t = new NMGTaskWidget ( this );
	t->setName ( moduleName );
	taskMap.insert ( id,t );
	addTask ( t );
}

/**
 * Returns a new task id
 * @return
 */
QString NMGTaskProgressBar::newTaskId()
{
	QString id;
	do id.setNum ( random.random() ); while ( taskMap.find ( id ) != taskMap.end() );
	return id;
}

/**
 * Slot which finishes a task
 * @param id
 */
void NMGTaskProgressBar::finishedTask ( QString id, QString /*name*/, QString /*data*/ )
{
	TaskMapIterator it = taskMap.find ( id );
	if ( it == taskMap.end() ) cerr << "The task with id " << qPrintable ( id ) << " is not in the progress task bar!!" << endl;
	else
	{
		NMGTaskWidget * t = it.value();
		removeTask ( t );
		taskMap.remove ( id );
	}
}


#include "nmgtaskprogressbar.moc"
