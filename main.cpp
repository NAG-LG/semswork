#include <iostream>
#include "EventClass.h"

using namespace std;

int counter=0;

pthread_mutex_t CountMutex;
EventID InsertEventOfType1ID =  0;
EventID EventID1 =  1;
EventID EventID2 =  2;
EventID EventID3 =  3;
EventHandler MyEventHandlerAll;

void *ThreadCode(void *data)
{

    for(int i=0;i<10;i++)
    {
        cout<<"ThreadCode: "<<i<<endl;
    }
    MyEventHandlerAll.signalEvent(EventID1);
    pthread_exit(NULL);

}



/*void *ThreadCode(void *data)
{
    EventHandler *handler = reinterpret_cast<EventHandler *> (data);

    bool WorkNotDone= true;
    while(WorkNotDone){

        handler->waitForEvent(InsertEventOfType1ID);
        pthread_mutex_lock(&CountMutex);
         //cout<<"EventTriggered position entering...!!!!"<<counter<<endl;
        cout<<"EventTriggered...!!!!"<<counter<<endl;
        if(counter == 10)
        {
            WorkNotDone = false;
        }
        pthread_mutex_unlock(&CountMutex);

    }
    MyEventHandlerAll.signalEvent(EventID1);
    pthread_exit(NULL);
}
*/

void *CounterFun(void *data)
{

    for(int i=20;i<30;i++)
    {
        cout<<"CounterFun: "<<i<<endl;
    }
    MyEventHandlerAll.signalEvent(EventID2);
    pthread_exit(NULL);

}

/*void *CounterFun(void *data)
{

    EventHandler *handler = reinterpret_cast<EventHandler *> (data);


    for(int i=1 ; i <= 10 ; i++)
    {
        pthread_mutex_lock(&CountMutex);
        counter++;
        cout<<"Event increment in counter...!!!"<<counter<<endl;
        pthread_mutex_unlock(&CountMutex);

        if(i%5 == 0)
        {
            cout<<"inside i%5 came/..."<<endl;
            handler->signalEvent(InsertEventOfType1ID);
            usleep (50000);
        }
    }
    MyEventHandlerAll.signalEvent(EventID2);
    pthread_exit(NULL);

}

*/
int main()
{
    EventHandler MyEventHandler;

    MyEventHandlerAll.AddEvent("Event1",false,false,EventID1);
    MyEventHandlerAll.AddEvent("Event2",false,false,EventID2);
    //MyEventHandler1.AddEvent("Event1",false,false,InsertEventOfType1ID);

    //pthread_mutex_init(&CountMutex,NULL);
    //MyEventHandler.AddEvent("InsertEventOfType1",false,false,InsertEventOfType1ID);


    pthread_t thread[2];

    pthread_create(&thread[0],NULL,ThreadCode,(void*)&MyEventHandler);

    pthread_create(&thread[1],NULL,CounterFun,(void*)&MyEventHandler);

    //pthread_create()


    MyEventHandlerAll.waitForEvents(true,EventID2);

    for(int i=0 ; i<2 ;i++)
    {

        pthread_join(thread[i],NULL);
    }

    //MyEventHandler.RemoveEvent(InsertEventOfType1ID);
    MyEventHandlerAll.RemoveEvent(EventID1);
    MyEventHandlerAll.RemoveEvent(EventID2);

    return 0;
}
