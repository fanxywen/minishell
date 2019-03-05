#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <cerrno>
#include <climits>
#include <cstring>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <sys/stat.h>
#include <fcntl.h>
#include "pipe.h"
#include "prompt.h"
extern char** environ;
std::vector<std::string> splitpath(std::string path) {
  std::vector<std::string> re;
  std::string::iterator iter = path.begin();
  std::string sub;
  while (iter != path.end()) {
    if (*iter != ':') {
      sub.push_back(*iter);
    }
    if (*iter == ':') {
      re.push_back(sub);
      sub.clear();
    }
    iter++;
  }
  return re;
}
/*void checkstatus(int wstatus) {
  if (WIFEXITED(wstatus)) {
    std::cerr << "Program exited with status " << WEXITSTATUS(wstatus) << std::endl;
  }
  else {
    if (WIFSIGNALED(wstatus)) {
      std::cerr << "Program was killed by signal " << WTERMSIG(wstatus) << std::endl;
    }else{ if (WIFSTOPPED(wstatus)) {
                printf("stopped by signal %d\n", WSTOPSIG(wstatus));
            } else if (WIFCONTINUED(wstatus)) {
                printf("continued\n");
    }
    }
  }
}*/

bool ifexit(std::string line){
  if(line.find("exit") == std::string::npos){
        return false;
   }else{
        return true;
   }
}

int main() {
  std::string line;
  pipeIn pipeinput;
  /*initialize variable map*/
  pipeinput.myvar.init(environ);
  pipeinput.myPrompt.PrintFn();
  char * ECE551PATH;
  while (getline(std::cin, line)) {
 ECE551PATH = getenv("PATH");  //get enviromental variable  
 if(!line.empty()){
 if (ifexit(line)) {
     break;
     }else{
      /*if user typed in invalid command format, goes on*/
      if(!pipeinput.parsePipein(line)){
    pipeinput.myPrompt.PrintFn();
     pipeinput.clean();
    continue;
  }
      /*if its not specified 4 command(cd, set. export. inc), search in directories*/
     if(!pipeinput.reading.getcd() && !pipeinput.reading.getset()
	&&!pipeinput.reading.getexp()&&!pipeinput.reading.getinc()
	&&!pipeinput.reading.geteval()){
      if (!pipeinput.reading.getslash()) {
	pipeinput.changeCmdl(pipeinput.reading);
        std::vector<std::string> path = splitpath(ECE551PATH);
        bool re;
	for (size_t i = 0; i < path.size(); i++) {
	  re=pipeinput.reading.srch(path[i]);
	     if(re == true){
                break;
	        }
	      }   
	     if(re == false){
	        std::cerr<<"Command "<<pipeinput.reading.getCmd()<<" not found"<<std::endl;
                pipeinput.myPrompt.PrintFn();
                pipeinput.clean();
	         continue;
	      }
      }
      else {
	       std::string path = pipeinput.reading.getCmd();
	       size_t pos = path.find_last_of('/');
	       path=path.substr(0,pos);
	       bool re = pipeinput.reading.srch(path);
       if(re == false){
          std::cerr<<"Command "<<pipeinput.reading.getCmd()<<" not found"<<std::endl;
	  pipeinput.myPrompt.PrintFn();
	   pipeinput.clean();
	        continue;
        }
      }
     }
      pipeinput.run();
   }
 }
   pipeinput.myPrompt.PrintFn();
   /*reset object*/
   pipeinput.clean();
 }
    
  return EXIT_SUCCESS;
}

