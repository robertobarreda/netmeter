#include "nmmoduleinterface.h"
#include <nmmodulemanager.h>
#include <nminterpret.h>
#include <nmtaskpurgeplugin.h>
#include <nmtaskmanager.h>


NMModuleInterface::NMModuleInterface()
:NMParent(), m_testid(0)
{
}


NMModuleInterface &NMModuleInterface::operator<<(const NMString& arg)
{
	NMInterpret interpret;
	interpret.generateResult( m_testid, parent()->name(), arg);
	return *this;
}

NMModuleInterface &NMModuleInterface::operator<<(const NMError& err)
{
	NMInterpret interpret;
	interpret.generateError ( m_testid, err);
	return *this;
}

NMModuleInterface &NMModuleInterface::operator<<(const NMStatus& status)
{
	NMInterpret interpret;
	interpret.generateStatus( m_testid, status);
	return *this;
}


void NMModuleInterface::exit()
{
	NMTaskPurgePlugin *purge = new NMTaskPurgePlugin( m_testid, parent()->name(), "purge", NMTaskManager::self());
	NMTaskManager::self()->addNewTask(purge);
}

void NMModuleInterface::setTestId(const unsigned int &testid)
{
	m_testid = testid; 
}
	
unsigned int NMModuleInterface::testId()
{
	return m_testid;
}

