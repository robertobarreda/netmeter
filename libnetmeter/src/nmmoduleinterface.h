/***************************************************************************
                          nmmoduleinterface.h  -  description
                             -------------------
    begin                : dt feb 15 2005
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

#ifndef NMMODULEINTERFACE_H
#define NMMODULEINTERFACE_H

#include "nmstring.h"
#include "nmerror.h"
#include "nmparent.h"
#include "nmstatus.h"

#define NMMODULE_CLASS(x) extern "C"{ \
NMModuleInterface *getInterface(){ return new x;} }

#define NMMODULE_NAME(x) extern "C"{ \
char * getModuleName(){ return x;}}

#define NMMODULE_CAPABILITIES(x) extern "C"{ \
char * getModuleCapabilities(){ return x;}}

#define NMMODULE_VERSION(x) extern "C" { \
char *getModuleVersion() {return x;}} 


/**
  *@author Pau Capella
  */

class NMModuleInterface : public NMParent {

public:
 
	NMModuleInterface();
	virtual ~NMModuleInterface(){}

	//virtual int notifyEvent(const int &event, const void *buffer, const int &size)=0; 
  	virtual void initModule()=0;
	virtual void removeModule()=0;
	virtual void start(void *data)=0;
	virtual void stop(void *data)=0;
	virtual void status(void *data)=0;
	virtual void configure(void *data)=0;

	void setTestId(const unsigned int &testid);
	unsigned int testId();

protected:
	//void sendToGui(const NMString &result); 
	NMModuleInterface &operator<<(const NMString& arg);
	NMModuleInterface &operator<<(const NMError& err);
	NMModuleInterface &operator<<(const NMStatus& status);
	void exit();

private:

	unsigned int m_testid;

  
};

#endif
