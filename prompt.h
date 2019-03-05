#ifndef PROMPT_H
#define PROMPT_H
#include <unistd.h>

#include <string>
/* Prompt class:
 * This class is used for prompt displaying, it stores all the information prompt needs,
 * i.e. shellName and current directory
 * Two member method:
 * printFn(): to print Prompt in std::cout 
 * changePath(): when cd command is called, change the displaying current path accordingly
 */
class prompt
{
 private:
  std::string shellName;
  std::string currentPath;

 public:
  prompt() : shellName("myShell"), currentPath() {
    char buf[256];
    if (getcwd(buf, sizeof(buf)) != NULL) {
      currentPath = buf;
    }
    else {
      currentPath = "/";
    }
  }
  prompt(std::string curr) : shellName("myShell"), currentPath(curr) {}
  void PrintFn();
  std::string getName();
  std::string getPath();
  void changePath(std::string newPath);
  ~prompt() {}
};
#endif
