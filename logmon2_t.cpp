#include "logmon2_t.h"
//#include <iostream>
#include <curl/curl.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sys/stat.h>
struct MemoryStruct {
  char *memory;
  size_t size;
};
static size_t noop(void *contents, size_t size, size_t nmemb, void *userp){}
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr =(char*) realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

void writestr (std::string *str,char *fil){
    if (!(*str).empty())
    {
  std::ofstream myfile;// ("example.txt");
  myfile.open (fil);
  if (myfile.is_open())
  {
      std::cout<<"writing to file.."<<"\n";
    myfile <<*str<<"\n";
    myfile.close();
  }
  else std::cout << "Unable to open file";
    }
}
bool fileExists(const std::string& filename)
{
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1)
    {
        return true;
    }
    return false;
}
char filename[100];
void getlogfilename(char* name)
{
  int i=0; 
  
  sprintf(filename,"%s%d.txt",name,i);
  while(fileExists(filename))
  {
    sprintf(filename,"%s%d.txt",name,i);
    i++;
  }
}
void writev (std::vector<std::string> *v)
{
  if (v->empty()) return;
  std::ofstream myfile;// ("example.txt");
  myfile.open (filename); // Append mode
  if (myfile.is_open())
  {
    //std::cout <<"size "<< v->size();
    for (int i=0;i<v->size();i++)
    {myfile <<(*v)[i];
    //std::cout <<"size "<<(*v)[i];
    }
    myfile.close();
  }
  else std::cout << "Unable to open file";
} 
CURL *curl_handle1;
        CURL *curl_handle2;
void* logworker(void *td)
{
    thread_data *tdata= (thread_data*)td;
    DataStruct* ipss=tdata->datapointer;
    CURL *curl_handlew;
    CURLcode res;
    struct MemoryStruct chunk;
    char *plog;
    int chunkadd;
    std::string str;
    std::string *pstrlog;
    if (tdata->thread_id==1) // locallog
    {
        plog=ipss->locallog;chunkadd=4000;pstrlog=&ipss->str1;curl_handlew=curl_handle1;
    }
    else
    {
        plog=ipss->remotelog;chunkadd=6240;pstrlog=&ipss->str2;curl_handlew=curl_handle2;
    }
    (*pstrlog).empty();
    chunk.memory =(char*) malloc(1);  /* will be grown as needed by the realloc above */ 
    //curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handlew, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handlew, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handlew, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.162 Safari/537.36");
    curl_easy_setopt(curl_handlew, CURLOPT_COOKIEFILE, ipss->cookies); 
      curl_easy_setopt(curl_handlew, CURLOPT_URL, plog);
      chunk.size = 0; 
      res = curl_easy_perform(curl_handlew);
  //writez(chunk.memory);
  if(res != CURLE_OK) 
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
    }
  else 
    {
      str=(chunk.memory+chunkadd);
      size_t found = str.find("FALSE>"); 
      if (found != std::string::npos) 
        {
          size_t found2 = str.find("</te",found); 
          *pstrlog=str.substr((size_t)found+6,(size_t)(found2-(found+6)));
           //std::cout<<pstrlog.size()<<"\n";
         /* if (pstrlog->size()<16) 
            {
              pstrlog->clear();
            }*/
        }
    }
//    curl_easy_cleanup(curl_handle);
    free(chunk.memory);
    pthread_exit(NULL);
}
std::string ip;
void getStatic(DataStruct *ipss)
{
    char* url="https://hackerwars.io/ajax.php";
    char *postdata="func=getStatic";
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;
     chunk.memory =(char*) malloc(1); 
     curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
   // curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.162 Safari/537.36");
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, ipss->cookies); 
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postdata);
         chunk.size = 0; 
         std::string str;
         std::string *pstrlog;
        res = curl_easy_perform(curl_handle);
  if(res != CURLE_OK) 
    {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
      curl_easy_strerror(res));
    }
  else 
    {
      str=(chunk.memory);
      size_t found = str.find("ip\\"); 
      if (found != std::string::npos) 
        {
          size_t found2 = str.find(",",found); 
          ip=str.substr((size_t)found+7,(size_t)((found2-2)-(found+7)));
        }
    }

    curl_easy_cleanup(curl_handle);
     free(chunk.memory);
}
void* deletelogsworker(void *td)
{
    thread_data *tdata= (thread_data*)td;
    DataStruct* ipss=tdata->datapointer;
    CURL *curl_handle;
    CURLcode res;
    char *ppost;
     if (tdata->thread_id==2) // locallog
    {
        ppost=ipss->localpost;
    }
    else
    {
        ppost=ipss->remotepost;
    }
     curl_handle = curl_easy_init();
   // curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
   // curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
   // curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.162 Safari/537.36");
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, ipss->cookies); 
        curl_easy_setopt(curl_handle, CURLOPT_URL, ipss->postURL);
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, ppost);
        res = curl_easy_perform(curl_handle);

    curl_easy_cleanup(curl_handle);
    
    pthread_exit(NULL);
}

void* completetasksworker(void *td)
{
    thread_data *tdata= (thread_data*)td;
    DataStruct* ipss=tdata->datapointer;
    CURL *curl_handle;
    CURLcode res;
    char *pid;
     if (tdata->thread_id==1) // locallog
    {
        pid=ipss->pid1;
    }
    else
    {
        pid=ipss->pid2;
    }
        char buff[100];
        sprintf(buff,"%s%s", ipss->completetask,pid);
        curl_handle = curl_easy_init();
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, noop);
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, ipss->cookies); 
        curl_easy_setopt(curl_handle, CURLOPT_URL, buff);
        res = curl_easy_perform(curl_handle);
        curl_easy_cleanup(curl_handle);
        pthread_exit(NULL);
}
