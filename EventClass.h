#include <iostream>
#include <pthread.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <semaphore.h>
using namespace std;

typedef void* HANDLE;
typedef uint8_t EventID;
typedef uint32_t Timeout;

class EventHandler;

//-------------------

     struct Data{
        int index;
        EventHandler *objHandler;
        EventID eventID;
    };

    static Data data;

    static int numberOfEvents;
    static int numberOfEventsTriggered;
    static pthread_mutex_t EventsIncrementor;
    //static pthread_cond_t ThreadSignal;


//-------------------

typedef void* (*THREADFUNPTR) (void*);


class EventHandler{

public:

    bool AddEvent(const string &eventName, bool bSingleton , bool bInitiallySignalled , EventID &eventID);
    bool RemoveEvent(const EventID &eventID);
    bool signalEvent(const EventID &eventID) const;
    bool waitForEvent(const EventID &eventID) const;
    bool waitForEvents( bool allEvents, const EventID &signalledEventID /*, Timeout timeout*/) ;//const;

private:

    void *ThreadPoolEvents(void *);

    struct SEventInfo{
        string m_eventName;
        pthread_cond_t *m_hEvent;
        pthread_mutex_t *m_mutex;
    };

    map<EventID,SEventInfo> m_eventMap;
    //vector<HANDLE> m_EventHandleVector;
    vector<pthread_cond_t *> m_handleVector;

    //void *(threadVar)(void*) =reinterpret_cast<*(*)(void*)> (ThreadPoolEvents);
};

