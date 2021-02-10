    #include <iostream>
    struct DataStruct
    {   
        
        std::string str1;
        std::string str2;
        char *locallog="https://hackerwars.io/log";
        char *remotelog="https://hackerwars.io/internet?view=logs";
        char cookies[100];
        char * postURL="https://hackerwars.io/logEdit";
        char *localpost="id=1&log=";
        char *remotepost="id=0&log=";
        char *completetask="https://hackerwars.io/processes?pid=";
        char pid1[8];
        char pid2[8];

    };

    struct thread_data
    {
        int  thread_id;
        DataStruct *datapointer;
    };

    //void* logworker(void *td);