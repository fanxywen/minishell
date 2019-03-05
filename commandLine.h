#ifndef COMMANDLINE_H
#define COMMANDLINE_H
#include <string>
#include <vector>
#include "prompt.h"
#include "var.h"
/*commandLine class:
 * this class is used to run all kinds of commands
 * it stores the command name and arguments user typed in
 */
class commandLine
{
 private:
  std::string command;
  std::vector<std::string> args;
  bool slash;
  bool cd;
  bool set;
  bool exp;
  bool inc;
  bool eval;
 public:
 commandLine() : command(""), args() ,slash(false), cd(false),set(false),exp(false),inc(false),eval(false){}
  commandLine(const std::string & cmd, const std::vector<std::string> & arguments);
  commandLine(const std::vector<std::string> & line);
  void changeCmd(std::string& newCmd);
  bool srch(std::string path);
  void changeArgs(std::vector<std::string>& newArgs);
  std::string & getCmd();
  std::vector<std::string> & getArgs();
  bool getslash();
  bool getcd();
  bool getset();
  bool getexp();
  bool getinc();
  bool geteval();
  bool runcd(prompt & pro, var & myvar);
  void parseCmd(std::string);
  bool runset(var& myvar);
  bool runexport(var& myvar);
  bool runinc(var & myvar);
  void clear();
  bool isempty();
  bool checkVar();
 
  ~commandLine() {}
};
#endif
