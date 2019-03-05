#include "var.h"
#include <ctype.h>
#include <map>
#include <string>
#include <utility>
#include <iostream>
#include <stdlib.h>
#include <vector>
/* find the corresponding value of one variables
 * if it's not in the map, return ""
 * if it's in it, return its value 
 */
std::string var::findVal(std::string var){
  if(varset.find(var) == varset.end()){
    return "";
  }
  return varset[var];
}

/* set variable and its value
 * if variable is already in the map, update its value
 * if variable is not in the map, insert the variable and value pair into map
 */
void var::setVar(std::string var, std::string value) {
  if(varset.find(var) == varset.end()){
  varset.insert(std::make_pair(var, value));
  }else{
    varset[var]=value;
  }

}
/* initialize variable map 
 * initialize variable map with all the existing environment variables
 */
void var::init(char ** environ){
  char * env = *environ;
  std::string var;
  std::string value;
  for(int i =1; env; i++){
    var = env;
    value = var.substr(var.find_first_of('=')+1);
    var = var.substr(0,var.find_first_of('='));
    varset.insert(std::make_pair(var,value));
    env = *(environ+i);
  }
}
/* export variable into environment
 * if the vriable is not in the map, set its value to empty and export
 * if its in the map, simply export
 */
bool var::exportVar(std::string var) {
  std::map<std::string, std::string>::iterator iter = varset.find(var);
  if (iter == varset.end()) {
    setVar(var, "");
  }
  std::string value = varset[var];
  if (setenv(var.c_str(), value.c_str(), 1) == -1){
    std::cerr<<"export variable "<<iter->first<<" failure"<<std::endl;
  return false;
}else{

  return true;
 }
 return true;
}
/* inc variable's value by one (base 10)
 * if its value is not base 10 number, treat it as 0, than increace by 1;
 * if its value is base 10 number, increase it by 1
 */
bool var::incVar(std::string var){
  std::map<std::string, std::string> :: iterator iter = varset.find(var);
  if(iter == varset.end()){
    setVar(var,"1");
  }else{
    size_t i;
    for(i = 0; i < (iter->second).size(); i++){
      if(!isdigit((iter->second)[i]) && (iter->second)[i] != '-'){
	break; 
      }
    }
    if(i == (iter -> second).size()){
      int value = std::stoi(iter->second, nullptr, 10);
      value++;
      std::string newVal = std::to_string(value);
      setVar(var,newVal);
    }else{
      setVar(var,"1");
    }
  }
   return true;
}
/* evaluate variables
 * when meet $, evaluate variable values followed by it along with possible invaild characters 
 */
void var::evaluate(std::string line){
  /* first split the user input according to '$' into a vector
   */
  std::vector<std::string> ans;
    std::string s;
    std::string::iterator iter = line.begin();
    while(iter != line.end()){
        if(*iter != '$'){
                s.push_back(*iter);
        }else{
                if(*iter == '$' && *(iter + 1) == '$'){
                        s.push_back(*iter);
                }else{
                  if(iter != line.begin()&&*iter == '$' && *(iter + 1) != '$'){
                    ans.push_back(s);
                                s.clear();
                        }
                }
        }
        if(iter+1 == line.end()){
          ans.push_back(s);
        }
        iter++;
    }
    /*as typed in variable name may contain invalid characters, 
     now abstract possible invalid charaters in each variable into a sym vector*/
    std::vector<std::string> sym;
  std::vector<std::string>::iterator itans = ans.begin();
  while(itans != ans.end()){
    std::string str;
    std::string::iterator it = (*itans).begin();
    while(it != (*itans).end()){
      if(!isdigit(*it) && !isalpha(*it) && *it != '_'){
        str.push_back(*it);
	if((*itans).erase(it) == (*itans).end()){
	  break;
	}else{
          continue;
	}
      }
      it++;
    }
    sym.push_back(str);
    itans++;
  }
  /*display*/
  for(size_t i = 0; i < ans.size(); i++){
    std::cout<<varset[ans[i]]<<sym[i];
  }
  std::cout<<std::endl;
}
