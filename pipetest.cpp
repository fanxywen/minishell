#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <stdio.h>
#include <climits>
#include <string>
#include <vector>
#include "commandLine.h"
void runCmd(commandLine * cmdl) {
  char * newarg[256] = {NULL};
  std::vector<std::string>::iterator iter = (cmdl->getArgs()).begin();
  newarg[0] = (char *)(cmdl->getCmd()).c_str();
  int i = 1;
  while (iter != (cmdl->getArgs()).end()) {
    newarg[i] = (char *)(*iter).c_str();
    iter++;
    i++;
  }
  std::string cmd = cmdl->getCmd();
  pid_t pid = fork();
   if (pid == -1) {
    throw std::invalid_argument("fork child process fail");
  }
  else {
    if (pid == 0) {
      execve(newarg[0], newarg, environ);
    }
  }
}
int main(){
  std::string line;
  getline(std::cin,line);
  std::string reading = line.substr(0,line.find("|"));
  std::string writing = line.substr(line.find("|")+1);
  std::vector<std::string> read;
  read.push_back(reading);
  std::vector<std::string> write;
  write.push_back(writing);
  commandLine * r = new commandLine(read);
  commandLine * w = new commandLine(write);
  int pipefd1[2];
  int pipefd2[2];
  pipe(pipefd1);
  pipe(pipefd2);
  
  return EXIT_SUCCESS;
}
