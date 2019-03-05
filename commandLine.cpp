#include "commandLine.h"
#include <iostream>
#include <exception>
#include "var.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>
/*
 * commandLine constructors:
 *--------------------------------------------------------------
 */
commandLine::commandLine(const std::string & cmd, const std::vector<std::string> & arguments) :
    command(cmd),
    args(),slash(false), cd(false), set(false),exp(false),inc(false),eval(false){
  std::vector<std::string>::const_iterator iter = arguments.begin();
  while (iter != arguments.end()) {
    args.push_back(*iter);
    iter++;
  }
}
commandLine::commandLine(const std::vector<std::string> & line) : command(), args(),slash(false), cd(false), set(false),exp(false),inc(false),eval(false){
  if(line.size() != 0){
    command = line[0];
  std::vector<std::string>::const_iterator iter = line.begin() + 1;
  while (iter != line.end()) {
    args.push_back(*iter);
    iter++;
  }
  }
}
/*
 * parse command line argument according to white spaces and slash escaped spaces 
 * if encountered slash escaped space, append string with a space;
 * if encountered white space, push string into vector if string is not empty, otherwise escape and go on;
 */
void commandLine::parseCmd(std::string line){
  std::string::iterator iter = line.begin();
  std::vector<std::string> cmdl;
  std::string s;
  for(;iter < line.end();++iter){
    if(*iter != ' '){
      if(*iter == '\\' && *(iter + 1) == ' '){
	s = s + ' ';
	++iter;
	if(iter + 1 == line.end()){
          cmdl.push_back(s);
	  s.clear();
	}
	continue;
      }else{
        s = s + *iter;
      }
    }else{
      if(s.empty()){
	continue;
      }else{
        cmdl.push_back(s);
	s.clear();
      }
    }
    if(iter+1 == line.end()){
      if(!s.empty()){
      cmdl.push_back(s);
      }
    }
  }
  command = cmdl[0];
  /* set corresponding flag bit to ture*/
  /* eval is true only when $ is in the first position*/
  if(command.find('$') != std::string::npos){
    if(command.find('$') != 0){
      eval = false;
    }else{
    eval = true;
    command = line;
    return;
    }
  }
  if(command.find('/') != std::string::npos){   
      slash = true;
    }
  if(command == "cd"){
    cd = true;
  }
  if(command == "set"){
    set = true;
  }
  if(command == "export"){
    exp = true;
  }
  if(command == "inc"){
    inc = true;
  }
  for(size_t i = 1; i < cmdl.size(); ++i){
    args.push_back(cmdl[i]);
  }
}
/* search directory recursively for command:
 * if the path is relative(contains "." or ".."), convert it to absolute path then conduct search;
 * if command found, update command field to the path;
 */
bool commandLine::srch(std::string path){
   std::string cmdname =command;
  char dot[3] = ".";
  char dotdot[6] = "..";
  const char * p = path.c_str();
  char* abs = NULL;
  /* convert relative path to absolute path */
  if (strchr(p, '.') != NULL || strstr(p, dotdot) != NULL) {
    abs = realpath(p, abs);
    path = abs;
  }
  /* if command contains slash, abstract command name from path */
  if (slash) {
    size_t ind = cmdname.find_last_of('/');
    cmdname = cmdname.substr(ind + 1);
  }
  DIR * dir = opendir(path.c_str());
  if (dir == NULL) {
    std::cerr<<"no such file or directory: " <<path <<"/"<<cmdname<<std::endl;
    return false;
  }
  else {
    struct dirent * ent = NULL;
    while ((ent = readdir(dir)) != NULL) {
      if (strcmp(ent->d_name, dot) != 0 && (strcmp(ent->d_name, dotdot)) != 0) {
        if (ent->d_type == DT_DIR) {
          std::string newDir = path + std::string("/") + ent->d_name;
          if (path[path.size() - 1] == '/') {
            newDir = path + std::string(ent->d_name);
          }
	  /* search sub directory */
          srch(newDir);
       
  } else {
          std::string newpath = path + std::string("/") + std::string(ent->d_name);
          if (path[path.size() - 1] == '/') {
            newpath = path + std::string(ent->d_name);
          }
          if (cmdname.compare(ent->d_name) == 0) {
            command = newpath;  
            closedir(dir);
            free(abs);
      return true;
          }
        }
  }
    }
    free(abs);
    closedir(dir);
    return false;
  }
}
/*-------------------------------------------------------------*/
/*determine wheather commandLine object is empty */
bool commandLine::isempty(){
  if(command.empty() && args.empty()){
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------
 */
/*
 * change command name to the new command name
 */
void commandLine:: changeCmd(std::string& newCmd){
  command = newCmd;
}
/*
 * change arguments to store new arguments
 */
void commandLine::changeArgs(std::vector<std::string>& newArgs){
  args.clear();
  for(size_t i = 0; i < newArgs.size(); i++){
  args.push_back(newArgs[i]);
  }
}
/* getter functions:
 *--------------------------------------------------------------
 */
std::string & commandLine::getCmd() {
  return command;
}
std::vector<std::string> & commandLine::getArgs() {
  return args;
}
bool commandLine::getslash(){
  return slash;
}
bool commandLine::getcd(){
  return cd;
}
bool commandLine::getset(){
  return set;
}
bool commandLine::getexp(){
  return exp;
}
bool commandLine::getinc(){
  return inc;
}
bool commandLine::geteval(){
  return eval;
}
/*
 *--------------------------------------------------------------
 */
/*
 * execute cd command
 * cd command only takes 0 or 1 argument
 * 0 argument: change to home directory
 * 1 argument: change to the corresponding directory
 */
bool commandLine::runcd(prompt & pro, var & myvar) {
  char buf[256];                                 
    if (args.size() > 1) {
      //if more than one argument passed in, report error message
      std::cerr<<"invalid argument"<<std::endl;
    }
    if(args.size() == 0){
      //no arguments passed in, change the destination to HOME directory
      std::string path = getenv("HOME");
      args.push_back(path);
    }
    /* if argument is -, change directory to old pwd and update the value of "PWD", "OLDPWD" in variable map accordingly*/
    if(args[0] == "-"){
      const char * oldpwd = getenv("OLDPWD");
      std::cout<<oldpwd<<std::endl;
      std::string newpath = oldpwd;
      args[0] = newpath;
    }
    if(args[0] == "~"){
      const char* home = getenv("HOME");
      std::string newpath = home;
      args[0] = newpath;
    }
    if (chdir(args[0].c_str()) == 0) {  //if change directory success                                              
      if (getcwd(buf, sizeof(buf)) == NULL) {  //if get current working directory fail
	    std::cerr<<"Get current working directory fail "<<std::endl;
	    return false;
	  }
      else {
      std::string path = buf;
      /*update PWD*/
      args.push_back(path);
      args[0]="PWD";
      runset(myvar);
      args.erase(args.begin()+1);
      runexport(myvar);
      /*update OLDPWD*/
      args[0] = "OLDPWD";
      args.push_back(pro.getPath());
      runset(myvar);
      args.erase(args.begin()+1); 
      runexport(myvar);
      /*change prompt*/
      pro.changePath(path);
      }
      return true;
    }
    else {
      //change directory fails, print out error message
      std::cerr<<"cd: no such file or directory: "<< args[0]<<std::endl; 
          }
  
  return false;
}
/* checkVar: check the validility of variables
 * variables can only contain numbers, letters and underscores
 */
bool commandLine :: checkVar(){
  if(args.size() == 0){
    //if no variables passed in, print out error message
    std::cerr<< "invalid variable: more argument needed" <<std::endl;
    return false;;
  }else{
    //check every character's validility
    std::string::iterator iter = args[0].begin();
    while(iter != args[0].end()){
      if(!isdigit(*iter) && !isalpha(*iter) && *iter != '_'){
	//when invaild characters found, print out error msg
	std::cerr<<"invalid variable name " <<args[0]<<std::endl;
	return false;
      }
      iter++;
    }
    return true;
  }
}
/* execute set command
 * set the variable var to the string on the rest of the command line 
 */
bool commandLine :: runset(var & myvar){
  /* first check variable name validity*/
    if(checkVar()){
      std::string value;
      /*if args size > 2, first concatenate all the args, then set var to the concatenated value*/
      if(args.size() > 2){
        for(size_t i=1;i<args.size(); i++){
	  value = value+args[i]+" ";
	 }
	myvar.setVar(args[0],value);
	return true;
      }
      if(args.size() == 1){
	args.push_back("");
      }
      myvar.setVar(args[0],args[1]);
      return true;
    }
    if(!args.empty()){
    std::cerr<<"set: fail to set "<<args[0]<<std::endl;
    }
    return false;
}
/* execute export command
 * export current variable into environment
 */
bool commandLine :: runexport(var&  myvar){
    if(args.size() == 0){
      std::cerr<<"export: variable name needed"<<std::endl;
      return false;
    }
    for(size_t i = 0; i < args.size(); i++){
    myvar.exportVar(args[i]);
    }
    return true;
}
/* execute inc command
 * increament value by 1
 */
bool commandLine::runinc(var & myvar){
    if(args.size() == 0){
      std::cerr<< "inc: variable name needed"<<std::endl;
      return false;
    }
    for(size_t i = 0; i < args.size(); i++){
      myvar.incVar(args[i]);
    }
    return true;
}
/* clear command and args content, set flags to default false
 */
void commandLine::clear(){
  command.clear();
  args.clear();
  slash = false;
  cd = false;
  set = false;
  exp = false;
  inc = false;
  eval = false;
}
