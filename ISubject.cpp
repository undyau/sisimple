#include "ISubject.h"
#include "IObserver.h"

ISubject::ISubject(){};
ISubject::~ISubject(){};
		
void ISubject::Attach(IObserver* a_Observer)
{
	m_Observers.push_back(a_Observer);
}

void ISubject::Detach(IObserver* a_Observer)
{
	m_Observers.remove(a_Observer);
}

void ISubject::Notify(int a_Hint)
{
	for (std::list<IObserver*>::iterator i = m_Observers.begin(); i != m_Observers.end(); i++)
		(*i)->Update(this, a_Hint);
}
