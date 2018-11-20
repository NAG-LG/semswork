#include <iostream>

#include "EventClass.h"


using namespace std;

sem_t ThreadSignalSem;


bool EventHandler::AddEvent(const string& eventName, bool bSingleton, bool bInitiallySignalled, EventID& eventID)
{

        //duplicate event register will fail and return false

        for (std::map<EventID,SEventInfo>::iterator iter = m_eventMap.begin(); iter != m_eventMap.end(); iter++)
        {
            if(iter->second.m_eventName == eventName)
            {
                return false;
            }
        }

        SEventInfo eventInfo;
        eventInfo.m_eventName = eventName;
        eventInfo.m_mutex = new pthread_mutex_t;
        eventInfo.m_hEvent = new pthread_cond_t;

        if(eventInfo.m_mutex != nullptr && eventInfo.m_hEvent != nullptr){

            eventID =  static_cast<EventID> (m_eventMap.size());

            if(m_eventMap.insert(make_pair(eventID,eventInfo)).second)
            {
                m_handleVector.push_back(eventInfo.m_hEvent);
            }

            pthread_mutex_init(eventInfo.m_mutex,NULL);
            pthread_cond_init (eventInfo.m_hEvent, NULL);

            return true;
        }



/*        SEventInfo *eventInfo; = new SEventInfo;
        eventInfo->m_eventName = name;
        eventInfo->m_hEvent = new pthread_cond_t;
        eventInfo->m_mutex = new pthread_mutex_t;


        pthread_mutex_init(eventInfo->m_mutex,NULL);
        pthread_cond_init (eventInfo->m_hEvent, NULL);

        EventList.insert({name, eventInfo});
*/
        return false;
}

bool EventHandler::RemoveEvent(const EventID &eventID)
{
        std::map<EventID,SEventInfo>::const_iterator itr = m_eventMap.find(eventID);

        if(itr != m_eventMap.end())
        {
            pthread_mutex_destroy(itr->second.m_mutex);
            pthread_cond_destroy(itr->second.m_hEvent);


            for(vector<pthread_cond_t*>::iterator vitr = m_handleVector.begin(); vitr != m_handleVector.end(); ++vitr)
            {
                if(*vitr == (itr->second.m_hEvent))
                {
                    m_handleVector.erase(vitr);
                    break;
                }

            }

            free(itr->second.m_mutex);
            free(itr->second.m_hEvent);
            m_eventMap.erase(eventID);

            return true;

        }

        // event not in the list
        return false;
}

bool EventHandler::signalEvent(const EventID& eventID) const
{

        std::map<EventID,SEventInfo>::const_iterator itr = m_eventMap.find(eventID);

        if(itr != m_eventMap.end())
        {
            return pthread_cond_signal(itr->second.m_hEvent);
        }

        // event not in the list
        return false;
}

bool EventHandler::waitForEvent(const EventID& eventID) const
{

        std::map<EventID,SEventInfo>::const_iterator itr = m_eventMap.find(eventID);

        if(itr != m_eventMap.end())
        {
            pthread_mutex_lock(itr->second.m_mutex);
            pthread_cond_wait (itr->second.m_hEvent,itr->second.m_mutex);
            pthread_mutex_unlock(itr->second.m_mutex);

            return true;
        }

        return false;
}

void* EventHandler::ThreadPoolEvents(void* data )
{

    EventID eventID = *(EventID*)data;

    //ListHandler.waitForEvent(eventID);
    waitForEvent(eventID);

    pthread_mutex_lock(&EventsIncrementor);
    numberOfEventsTriggered++;
    pthread_mutex_unlock(&EventsIncrementor);
    //pthread_cond_signal(&ThreadSignal);
    sem_post(&ThreadSignalSem);

}


bool EventHandler::waitForEvents(bool allEvents, const EventID& signalledEventID /*, Timeout timeout*/) //const
{
    // TODO , need to use list properly to poll the event

    numberOfEvents = m_eventMap.size()-1;
    numberOfEventsTriggered = 0;

    pthread_t thread[numberOfEvents];
    pthread_mutex_init(&EventsIncrementor,NULL);
    sem_init(&ThreadSignalSem,0,0);
    //pthread_cond_init(&ThreadSignal,NULL);

    //------------------------

    //for(int i=0 ; i<numberOfEvents ;i++)
    int i=0;

    //threadVar = reinterpret_cast<*(*)(void*)> (ThreadPoolEvents);


    for(std::map<EventID,SEventInfo>::iterator iter = m_eventMap.begin(); iter != m_eventMap.end(); iter++)
    {

        //data.index = i;
        //data.objHandler = this;
        //data.eventID  = (iter->first);

        //pthread_create(&thread[i],NULL, EventHandler::ThreadPoolEvents,(void*)&data);
        pthread_create(&thread[i],NULL, (THREADFUNPTR)&EventHandler::ThreadPoolEvents,(void*)&iter->first);
        //pthread_create(&thread[1],NULL,CounterFun,(void*)&MyEventHandler);

        pthread_join(thread[i],NULL);
        i++;
    }


    bool WorkNotDone = true;
    while(WorkNotDone)
    {

        sem_wait(&ThreadSignalSem);

        pthread_mutex_lock(&EventsIncrementor);
        //pthread_cond_wait (&ThreadSignal,&EventsIncrementor);
        if(numberOfEventsTriggered == numberOfEvents )
            WorkNotDone = false;
        pthread_mutex_unlock(&EventsIncrementor);

    }

    return true;
}


