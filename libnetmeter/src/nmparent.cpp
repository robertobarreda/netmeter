/***************************************************************************
                          nmparent.cpp  -  description
                             -------------------
    begin                : ds feb 12 2005
    copyright            : (C) 2005 by Pau Capella
    email                : pcapella@ac.upc.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <nmparent.h>
#include <nmeventmanager.h>
#include <nmxml.h>

list<NMParent *> *NMParent::objectslist = 0;
NMMutex *NMParent::mutex = new NMMutex();

NMParent::NMParent(const char *name, NMParent *parent)
:m_parent(parent),
isevent(false)
{
	memset(objname,'\0',MAXNAME);

	if(name) 
		strncpy(objname, name, strlen(name));
	else 
		strncpy(objname, NMString::random().c_str(), 8);

	bool iseventmanagerup = true;

// 	#ifdef DEBUG
// 	cout << "INFO Object " << objname << " created";
// 
// 	if (m_parent != 0)
// 		cout << " with parent: " << m_parent->objname << "." << endl;
// 	else 
// 		cout << "." << endl;
// 	#endif
	//Si no hem creat la llista la cream al principi, si no dona segmention fault!.
	mutex->lock();
	if(!objectslist) { 
		//Creem la llista per els objectes
		objectslist = new list<NMParent *>;
		iseventmanagerup = false;
	}
	objectslist->push_front(this);
	mutex->unlock();
	
	//Engegem el gestor d'events
	if (!iseventmanagerup) NMEventManager::self()->start();
}

NMParent::NMParent(NMParent *parent)
:m_parent(parent),
isevent(false)
{
	memset(objname,'\0',MAXNAME);
	strncpy(objname, NMString::random().c_str(), 8);
	
// 	#ifdef DEBUG
//         cout << "INFO Object " << objname << " created";
// 
// 	if (m_parent != 0)
// 		cout << " with parent: " << m_parent->objname << "." << endl;
// 	else 
// 		cout << "." << endl;
// 	#endif
	
	mutex->lock();
	if(!objectslist) objectslist = new list<NMParent *>;
	objectslist->push_front(this);
	mutex->unlock();

}

NMParent::~NMParent(){

	//Busquem si algun objecte te com a pare l'objecte a eliminar...
	NMParent *object;
	list<NMParent *>::iterator iter;
	
	mutex->lock();
	for ( iter = objectslist->begin(); iter != objectslist->end(); iter++ ) {
	 		object = (*iter);
			if(object->parent() && this == object->parent()) {
//  				#ifdef DEBUG
//  				cout << "INFO SOON: " << object->name() << " must be destroyed first!!!" << endl;
//  				#endif
				mutex->unlock();
				delete object;
				mutex->lock();
				iter = objectslist->begin();
			}
	}
// 	#ifdef DEBUG
//  	cout << "INFO: Object " << objname << " destroy.....";
// 	#endif
	objectslist->remove(this);
// 	cout << "ok" << endl;
	
	mutex->unlock();
	
	if(objectslist->size() == 1) { //Quan sols tenim un objecte a llista, segur que serà l'event manager... per tant el parem.
		NMEventManager::self()->exitLoop();
		delete NMEventManager::self();
	}
	else if(!objectslist->size()) {
		cout << "Yes!, all memory is free :)" << endl;
		delete objectslist;
		delete mutex;
	}
}

// bool NMParent::operator ==(const NMParent &object) const
// {
// 	return (strcmp(this->objname, object.objname)==0);
// }


/*!
    \fn NMParent::events()
    \return A list with all the objects that are events.
*/
list<NMParent *> NMParent::events()
{
	list<NMParent *> events;
	list<NMParent *>::iterator iter;

	mutex->lock();
	for ( iter = objectslist->begin(); iter != objectslist->end(); iter++ ) {
	 		NMParent *object = (*iter);
			if(object->isevent) {
				if (object->parent() == NULL) {
					cout << "WARNING: Event " << object->name() << " don't have father. Try to destroy this object." << endl;
					delete object;
				}
				events.push_back(object);
			}
	}
	mutex->unlock();
	return events;
}

NMString NMParent::listMemoryObjects()
{
	NMXML xmlobjects("objects");
	
	list<NMParent *>::iterator iter;
	mutex->lock();
	
	for ( iter = objectslist->begin(); iter != objectslist->end(); iter++ ) {
			NMParent *obj = *iter;
			xmlobjects.addData("object");
			xmlobjects.addData("name", obj->name());

			if(obj->parent() != 0)  //Tenim pare....
				xmlobjects.addData("parent", obj->parent()->name());
			xmlobjects.goUp();
	}
	mutex->unlock();
	
	NMString objs = xmlobjects.toString();
	xmlobjects.freeTree();
	return objs;
}

