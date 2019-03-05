#include "pipe.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <clocale>
#include <utility>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
/* if command line contains '$', replace it with variable value
 */
std::string* pipeIn::replace(std::string origin){
  std::string firstpart;
  std::string var;
  std::vector<std::string> value;
   if(origin.find('$') != std::string::npos){
     /* firstpart hold everything before $(no change needed)
      * var- variable name
      * value- corresponding value
      */
    firstpart = origin.substr(0,origin.find('$'));
    var = origin.substr(origin.find('$')+1);
    while(var.find('$') != std::string::npos){
      int i = var.find('$');
      value.push_back(myvar.findVal(var.substr(0,i)));
      var = var.substr(i+1);
    }
    value.push_back(myvar.findVal(var));
    origin = firstpart;
    for(size_t i = 0; i < value.size(); i++){
      origin += value[i];
    }
    }
   std::string* newone = new std::string(origin);
   return newone;
}

/* whenever geteval is not true, always change commandline object, just in case it contains $ in the middle
 */
void pipeIn::changeCmdl(commandLine& cmd){
   if(!cmd.geteval()){
     std::string cmdname = cmd.getCmd();
     std::string * newname = replace(cmdname);
    cmd.changeCmd(*newname);
    delete newname;
    std::vector<std::string> oldargs = cmd.getArgs();
    for(size_t i = 0; i < oldargs.size(); i++){
      std::string * newarg = replace(oldargs[i]);
      oldargs[i] = *newarg;
      delete newarg;
    }
    cmd.changeArgs(oldargs);
  }
   return;
}
/* run command
 * first check if reading command is specified command.if so, run them and return;
 * if not, fork a child process, and then check if writing is empty or not,
 * if its empty, meaning no redirect detect, just execute
 * if not empty, redirect according to flags
 */
void pipeIn::run(){
  /*turn command and arguments to correct format*/
  char * newarg[256] = {NULL};
  std::vector<std::string>::iterator iter = (reading.getArgs()).begin();
  newarg[0] = const_cast<char *>((reading.getCmd()).c_str());
  int i = 1;
  while (iter != (reading.getArgs()).end()) {
    newarg[i] = const_cast<char *>((*iter).c_str());
    iter++;
    i++;
  }
  /*check for evaluate replacement*/
  changeCmdl(reading);
  changeCmdl(writing);
  /*check for 4 specified command*/
 if(reading.geteval()){
        myvar.evaluate(reading.getCmd());
	return;
 }else{
      if(reading.getcd()){
        reading.runcd(myPrompt,myvar);
        return;
      }else{
      if(reading.getset()){
	reading.runset(myvar);
	return;
      }else{
      if(reading.getexp()){
        reading.runexport(myvar);
        return;
      }else{
      if(reading.getinc()){
        reading.runinc(myvar);
	return;
      }
      }
      }
      }
 }
 /*
 makePipe();
 int waitp;
 for(int i = 0; i < 2; i++){
 pid_t first = fork();
 if(first == 0){
   if(i == 0){
   dup2(pipefd[1],1);
   close(pipefd[0]);
   close(pipefd[1]);
   execve(newarg[0], newarg, environ);
   }else{
     dup2(pipefd[0],0);
     close(pipefd[0]);
     close(pipefd[1]);
     execve(warg[0],warg,environ);
   }
 }
 }
 //  closePipe();
 wait(&waitp);
 WIFEXITED(waitp);
 wait(&waitp);
 WIFEXITED(waitp);
 // closePipe();
 return;
 */
  pid_t pid = fork();
  int wstatus;
  pid_t w;
  if (pid == -1) {
    std::cerr<<"fork child process fail : "<<std::strerror(errno)<<std::endl;
    return;
  }
  else {
    if (pid == 0) {
      /* now in child process */
      /*check for redirection*/
       if(!writing.isempty()){
	 std::string filename = writing.getCmd();
	 if(inputredir){
            int fd ;
            if((fd = open(filename.c_str(), O_RDONLY, 0)) < 0){
              std::cerr<<"can't open: "<<filename<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
            if(dup2(fd,0) < 0){
              std::cerr<<"can't dup2: "<<fd<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
            if(close(fd) < 0){
              std::cerr << "can't close: "<<fd<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
        }
        if(outputredir){
          int fd ;
            if((fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0){
             std::cerr<<"can't open: "<<filename<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
            if(dup2(fd,1) < 0){
             std::cerr<<"can't dup2: "<<fd<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
            if(close(fd) < 0){
             std::cerr << "can't close: "<<fd<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
        }
        if(erroredir){
           int fd ;
            if((fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC,0666)) < 0){
             std::cerr<<"can't open: "<<filename<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
            if(dup2(fd,2) < 0){
             std::cerr<<"can't dup2: "<<fd<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
            if(close(fd) < 0){
             std::cerr << "can't close: "<<fd<<" "<<std::strerror(errno)<<std::endl;
	      return;
            }
        }
       }
                    
       /* dup2(pipefd[0],1);
       close(1);
       dup2(pipefd[1],0);
       close(0);
       pid_t c = fork();
	execve(warg[0],warg,environ);
		}*/
         execve(newarg[0], newarg, environ);
    }
    else {
      w = waitpid(pid, &wstatus, WUNTRACED | WCONTINUED);
      if (w == -1) {
  std::cerr<<"waitpid failure" <<std::endl;
  return;
      }
      /*report exit status when command contains slash*/
      if(reading.getslash()){
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
      }
    }
  }

}
/* parse input,
 * first parse according to '|', '>','<',"2>", then parse the left part and right part according to spaces and slash escaped spac * es
 */
bool pipeIn::parsePipein(std::string line){
    
    if(line.find('|') != std::string::npos){
      if(line.find('|') == line.size()-1){
        std::cerr<<"Pipe operation need one more argument "<<std::endl;
	return false;
      }
      if(line.find('|') == 0){
	return false;
      }
      std::string l = line.substr(0,line.find('|'));
      std::string r = line.substr(line.find('|')+1);
      piped = true;
      reading.parseCmd(l);
      writing.parseCmd(r);
    }else{
      if(line.find('<') != std::string::npos){
        if(line.find('<') == line.size()-1){
          std::cerr<<"input redirection needs one more argument"<<std::endl;
	  return false;
        }
	 if(line.find('<') == 0){
        return false;
      }
        std::string l = line.substr(0,line.find('<'));
        std::string r = line.substr(line.find('<')+1);
        reading.parseCmd(l);
        writing.parseCmd(r);
        inputredir = true;
      }else{
	std::string str = "2>";
        if(line.find(str) != std::string::npos){
        if(line.find(str) == line.size()-2){
          std::cerr<<"error redirection needs one more argument"<<std::endl;
          return false;
	}
	 if(line.find(str) == 0){
        return false;
      }
        std::string l = line.substr(0,line.find(str));
        std::string r = line.substr(line.find(str)+2);
        reading.parseCmd(l);
        writing.parseCmd(r);
        erroredir = true;
      }else{
        if(line.find('>') != std::string::npos){
        if(line.find('>') == line.size()-1){
          std::cerr<<"output redirection needs one more argument"<<std::endl;
	  return false;
        }
	 if(line.find('>') == 0){
        return false;
      }
        std::string l = line.substr(0,line.find('>'));
        std::string r = line.substr(line.find('>')+1);
        reading.parseCmd(l);
        writing.parseCmd(r); 
        outputredir = true;
      }else{
        reading.parseCmd(line);
      }
      }
    }
}
    return true;
}
/*creat pipe*/
void pipeIn::makePipe(){
  if(pipe(pipefd)){
    std::cerr << " pipe failed " << std::endl;
    exit(EXIT_FAILURE);
  }
  return;
}
/*close pipe*/
void pipeIn::closePipe(){
  for(int i = 0; i < 2; i++){
    if(close(pipefd[i]) < 0){
      std::cerr << "fail to close pipe " << i <<std::endl;
      exit(EXIT_FAILURE);
    }
      }
  return;
}
/*getter function*/
int* pipeIn::getfd(){
  return pipefd;
}
/*reset object*/
void pipeIn::clean(){
  piped = false;
  inputredir = false;
  outputredir = false;
  erroredir = false;
  reading.clear();
  writing.clear();
  
}
