/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
bool VarExists(char* name) {
   refVar *node;
   node = head;
   while(node != NULL) {
       if(!strcmp(name, node->name)) {
           return true;
       }
       node = node->next;
    }
    return false;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = name;
    node->next = NULL;
    if(head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if(node==NULL) {
        printf("\nList is empty"); 
        return;
    }
    while(node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
// takes operation node as input
void UpdateRef(Node* node) {
      if(node->right != NULL && node->right->exprCode == VARIABLE) {
          if(!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name);
          }
      }
      if(node->left != NULL && node->left->exprCode == VARIABLE) {
          if(!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

// Update referenced variable list for argument nodes in function calls specifically
void UpdateRefFuncCallArgs(Node* node) {
  if(!VarExists(node->name)) {
    UpdateRefVarList(node->name);
  }
}

/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

void TrackRef(Node* funcNode) {
     NodeList* statements = funcNode->statements;
     Node *node;
     while(statements != NULL) {
        node = statements->node;
        /*****************************************
              TODO : YOUR CODE HERE
        *****************************************/    
        // add variable to list if it is referenced after its initialization

        // case for unary operation with variable as arg
        if (node->stmtCode == ASSIGN && node->right->opCode == NEGATE) {
          if (node->right->left->exprCode == VARIABLE) {
            // input operation node into UpdateRef
            UpdateRef(node->right);
          }
        }

        // case for binary operation with either arg as referenced variable
        if (node->stmtCode == ASSIGN && node->right->exprCode == OPERATION && node->right->opCode != NEGATE && node->right->opCode != FUNCTIONCALL) {
          // if either operand is a variable, input operator node into UpdateRef
          if (node->right->left->exprCode == VARIABLE || node->right->right->exprCode == VARIABLE) {
            UpdateRef(node->right);
          }
        }

        // case for function calls with any of args as referenced variable
        if (node->stmtCode == ASSIGN && node->right->opCode == FUNCTIONCALL) {
          NodeList* arguments = node->right->arguments;
          // iterate linked list of args in function call
          while (arguments != NULL) {
            // if arg in function call is variable, add to referenced list
            if (arguments->node->exprCode == VARIABLE) {
              UpdateRefFuncCallArgs(arguments->node);
            }
            arguments = arguments->next;
          }
        }

        // case for assignment and right side is just variable
          // ex: long temp2 = temp1;  (temp1 defined earlier w constant value)
        if (node->stmtCode == ASSIGN && node->right->exprCode == VARIABLE) {
          UpdateRef(node);
        }

        // case for return statement with referenced variable
        if (node->stmtCode == RETURN) {
          if (node->left->exprCode == VARIABLE) {
            UpdateRef(node);
          }
        }

	      statements = statements->next;
     }
}

/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/
NodeList* RemoveDead(NodeList* statements) {
    refVar* varNode;
    NodeList *prev, *tmp, *first;
        /*
         ****************************************
              TODO : YOUR CODE HERE
         ****************************************
        */
    // update later in case first statement needs to be removed
    first = statements;
    prev = NULL;

    // remove all statements that have unreferenced variables as 'name' field in ASSIGN statements
      // if 'name' of ASSIGN statement not in referenced list, 
        // free the entire statement, make prev statement point to next one
    while(statements != NULL) {
        /*
         ****************************************
              TODO : YOUR CODE HERE
         ****************************************
        */
        // if ASSIGN statement and variable name on left of ASSIGN is not in referenced list, remove entire statement
        if (statements->node->stmtCode == ASSIGN && !VarExists(statements->node->name)) {
          // if its the first statement in NodeList* statements, 
          if (first == statements) {
            // make next statement new first
            first = statements->next;
            // make curr statement tmp, point it to NULL, and free it later
            tmp = statements;
            tmp->next = NULL;
            // update statements for next iteration
            statements = statements->next;
            FreeStatements(tmp);
            // to record change was made
            madeChange = true;
            // continue to first iteration
            continue;
          }
          // else
          else {
            // make prev->next point to next statement
            prev->next = statements->next;
            // make curr statement tmp, point it to NULL, and free it later
            tmp = statements;
            tmp->next = NULL;
            // update statements for next iteration
            statements = statements->next;
            // update prev for next iteration
            prev = statements;
            FreeStatements(tmp);
            // to record change was made
            madeChange = true;
            // continue to next iteration
            continue;
          }
        }

        // if return statement, remove all statements after it
        if (statements->node->stmtCode == RETURN) {
          tmp = statements->next;
          statements->next = NULL;
          FreeStatements(tmp);
        }

        prev = statements;
        statements = statements->next;
    }
  
   return first;
}

/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/
bool DeadAssign(NodeList* worklist) {
   while(worklist != NULL) {
            /*
             ****************************************
              TODO : YOUR CODE HERE
             ****************************************
             */
        // INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED VARIABLE LIST
        init();
        // NodeList* worklist = NodeList* funcdecls
          // worklist->node = 'funcdecls->function'
        TrackRef(worklist->node);
          // worklist->node->statements = 'funcdecls->function->statements'
        worklist->node->statements = RemoveDead(worklist->node->statements);
        // print referenced variables list to make sure updating correctly
        // PrintRefVarList();
        // free the referenced variable list
        FreeList();
        worklist = worklist->next;
    }
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 
