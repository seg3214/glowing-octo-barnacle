#include <iostream>
#include <vector>
int main(int argc, char const *argv[])
{
    std::vector<std::string> v;
//std::string strs="line2 [123.123.123.123]\n";
std::string strs="line2 localhost\n";
std::string *str;
str=&strs;
std::string buff;
std::string me="localhost";
int sw=1;
std::string mark="L";
size_t pos=0,mepos;
    while (pos != std::string::npos )
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
    }
   // std::cout<<str;
  /*  std::vector<std::string> v;
    std::string me="me";
    std::string b;
    std::string str="shitass logged [localhost]";
    size_t pos = str.find("localhost");
    b=str.replace( pos-1, me.size()+9, "me" );*/
    return 0;
}
