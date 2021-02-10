#include "logmon2_t.cpp"
//#include <iostream>
#include <chrono>
#include <thread>
//#include <vector>
#include <algorithm>


void parsecputasks(DataStruct *ipss)
{
    char *getcputasks="https://hackerwars.io/processes?page=cpu";
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory =(char*) malloc(1);  /* will be grown as needed by the realloc above */ 
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
   // curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.162 Safari/537.36");
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, ipss->cookies); 
    curl_easy_setopt(curl_handle, CURLOPT_URL, getcputasks);
    chunk.size = 0; 
    res = curl_easy_perform(curl_handle);
    if(res != CURLE_OK) 
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));
    }
    else 
    {
        size_t found2;
        char buff[100];
        ipss->pid1[0]='\0';ipss->pid2[0]='\0';
        std::string str=chunk.memory+4240;
        size_t found = str.find("log at"); 
        if (found != std::string::npos) 
        {
            found2 = str.find("id=",found); 
            found = str.find('"',found2+4);
            strncpy(buff, str.c_str()+found2+4,found-(found2+4));
              buff[found-(found2+4)]='\0';
              strcpy(ipss->pid1,buff);
        }
        found = str.find("log at",found2+200); 
        if (found != std::string::npos) 
        {
            found2 = str.find("id=",found); 
            found = str.find('"',found2+4);
            strncpy(buff, str.c_str()+found2+4,found-(found2+4));
            buff[found-(found2+4)]='\0';
            strcpy(ipss->pid2,buff);
        }
    }
    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
}
void spawnthreads2(DataStruct *dst)
{
    pthread_t threads[4];
    struct thread_data td[4];
    int rc;
    int i;
     for( i = 0; i < 4; i++ ) 
        {
        td[i].thread_id = i;
        td[i].datapointer = dst;
        if (i<2)
        rc = pthread_create(&threads[i], NULL,logworker, (void *)&td[i]);
        else
        rc = pthread_create(&threads[i], NULL,deletelogsworker, (void *)&td[i]);
        if (rc) {
        std::cout << "Error:unable to create thread," << rc << std::endl;
        exit(-1);
                }
        }
    for (int j = 0; j < 4; j++)
     {
        pthread_join (threads [j], NULL);
     }
}
void spawnthreads(int num, void *(*pworker)(void *),DataStruct *dst)
{
    pthread_t threads[num];
    struct thread_data td[num];

    int rc;
    int i;
     for( i = 0; i < num; i++ ) 
        {
        td[i].thread_id = i;
        td[i].datapointer = dst;
        rc = pthread_create(&threads[i], NULL,pworker, (void *)&td[i]);
      
        if (rc) {
        std::cout << "Error:unable to create thread," << rc << std::endl;
        exit(-1);
                }
        }
    for (int j = 0; j < num; j++)
     {
        pthread_join (threads [j], NULL);
     }
}
void logtovdata(char* mark,std::string *str,std::vector<std::string> *vdata,int sw)
{
    if (str->empty()) return;
    int i=0;
    std::string buff,me;
    if (sw==1) me="localhost"; else me=ip;
    std::vector<std::string> v;
    size_t pos=0;
    size_t mepos;
    while(true)
    {
        pos = str->find('\n');
        if ( pos != std::string::npos ) 
            {
                buff=str->substr(0,pos+1);
                mepos = buff.find( me ); //replace ip and localhost with "me"
                if ( mepos != std::string::npos ) 
                if (sw==0)  //if remote log then get rid of brackets else do normal
                buff.replace( mepos-1, me.size()+2, "me" ); else buff.replace( mepos, me.size(), "me" ); 
                v.push_back(mark+buff);
                str->erase(0,pos+1);
                //std::cout<<str;
            }
            else
            {
                if (str->size()>6) v.push_back(str->substr(0,str->size()));break; //if no \n found then check if >6 and add it
            }
            
    }
       for (i=0;i<v.size();i++)
    {
        if (std::find((*vdata).begin(), vdata->end(), v[i]) != vdata->end())
        {
        } else
        {
            vdata->push_back(v[i]);
        }
    }
}
int main(int argc, char const *argv[])
{
    DataStruct dstruc;
    std::vector<std::string> vdata;
    //CURL *curl_handle;
    curl_handle1 = curl_easy_init();
    curl_handle2 = curl_easy_init();
    int threads_num=2;
    char *path="/home/nz/Downloads/";
    sprintf(dstruc.cookies,"%slogmoncookies.txt",path);
    getlogfilename("logs/logmon");
    //sprintf(filename,"dlogmon.txt");
    getStatic(&dstruc);
    //std::cout << "'exit' to quit"<<std::endl ;
    //std::vector<std::string> v,vdata;
   int t=0;
    while (true)
    {
    auto start = std::chrono::high_resolution_clock::now();
    spawnthreads(2,logworker,&dstruc);
    logtovdata("L ",&dstruc.str1,&vdata,1);
    //writestr(&dstruc.str1,"str1");
    //writestr(&dstruc.str2,"str2");
    logtovdata("R ",&dstruc.str2,&vdata,0);
    writev(&vdata);
   
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::this_thread::sleep_for(std::chrono::milliseconds((3000-duration.count())));

    //if ((4000-duration.count())<3000) 
//  std::cout <<" duration2 "<<duration2.count()<<"\n";
    t++;
    }
    curl_easy_cleanup(curl_handle1);
        curl_easy_cleanup(curl_handle2);
    pthread_exit(NULL);
}
