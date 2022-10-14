#include "optimizer.h"


void Optimizer(NodeList *funcdecls) {
/*
*************************************
     TODO: YOUR CODE HERE
*************************************
*/
// The Optimizer function should call the following three optimization techniques. 
     // Remember to do so until there are no more optimization opportunities
     bool flag1 = true;
     bool flag2 = true;
     bool flag3 = true;
     while (flag1 == true || flag2 == true || flag3 == true) {
          flag1 = ConstantFolding(funcdecls);
          flag2 = ConstProp(funcdecls);
          flag3 = DeadAssign(funcdecls);
     }

     // only for testing each individually
     // ConstantFolding(funcdecls);
     // ConstProp(funcdecls);
     // DeadAssign(funcdecls);
}