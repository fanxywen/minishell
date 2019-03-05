#include "prompt.h"

#include <iostream>
std::string prompt::getName() {
  return shellName;
}
std::string prompt::getPath() {
  return currentPath;
}
void prompt::PrintFn() {
  std::cout << shellName << ":" << currentPath << "$ ";
}
void prompt::changePath(std::string newPath) {
  this->currentPath = newPath;
}

