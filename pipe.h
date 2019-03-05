#ifndef __PIPE_H__
#define __PIPE_H__
#include <stdio.h>
#include "commandLine.h"
/* This class 
 */
class pipeIn{
 private:
  int pipefd[2];
  bool piped;
  bool inputredir;
  bool outputredir;
  bool erroredir;
 public:
  commandLine reading;
  commandLine writing;
  prompt myPrompt;
  var myvar;
 pipeIn():pipefd(),piped(false),inputredir(false),outputredir(false),erroredir(false),reading(),writing(),myPrompt(),myvar(){}
  std::string * replace(std::string origin);
  void changeCmdl(commandLine& cmd);
  void run();
  bool parsePipein(std::string line);
  void makePipe();
  void closePipe();
  int * getfd();
  void clean();
  ~pipeIn(){}
};
#endif
