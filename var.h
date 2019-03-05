#ifndef __VAR_H__
#define __VAR_H__
#include <map>
#include <string>
#include <vector>
/*calss var:
 * contains a map storing all the variable and its value;
 * update the variable and its value according to the command
 */
class var
{
 private:
  std::map<std::string, std::string> varset;

 public:
  var() : varset() {}
  std::string findVal(std::string var);
  void setVar(std::string var, std::string value);
  void init(char ** environ);
  bool exportVar(std::string var);
  bool incVar(std::string var);
  void evaluate(std::string line);
  ~var() {}
};
#endif
