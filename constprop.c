/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPOGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode, *headNode;
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
void FreeConstList()
{
   refConst* tmp;
   while (headNode != NULL)
    {
       tmp = headNode;
       headNode = headNode->next;
       free(tmp);
    }

}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if(headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node;
    node = headNode; 
    while(node!=NULL){
        if(!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPOGATE THE CONSTANTS WHEN 
  WHEN APPLICABLE. YOU CAN ADD A NEW FUNCTION IF YOU WISH TO MODULARIZE BETTER.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
       Node* node;
       while(statements != NULL) {
	      node = statements->node;
            /*
             ****************************************
                     TODO : YOUR CODE HERE
             ****************************************
            */
        // perform the optimization in this helper

        // if assign statement with only a constant on right side of =, add to ConstList
        if (node->stmtCode == ASSIGN && node->right->exprCode == CONSTANT) {
          UpdateConstList(node->name, node->right->value);
        }

        // if unary operation, check if arg is a stored variable w/ constant value
          // replace if so
        if (node->stmtCode == ASSIGN && node->right->opCode == NEGATE) {
          if (node->right->left->exprCode != CONSTANT) {
            refConst* variable = LookupConstList(node->right->left->name);
            if (variable != NULL) {
              // set negate's left node to new constant node
              Node* new_constant = malloc(sizeof(Node));
              new_constant->value = variable->val;
              new_constant->type = EXPRESSION;
              new_constant->exprCode = CONSTANT;
              // free old variable node
              FreeVariable(node->right->left);
              // make negate's left point to new_constant node
              node->right->left = new_constant;
              // to record change was made
              madeChange == true;

            }
          }
        }

        // do same for binary operations
        if (node->stmtCode == ASSIGN && node->right->exprCode == OPERATION && node->right->opCode != NEGATE && node->right->opCode != FUNCTIONCALL) {
          if (node->right->left->exprCode != CONSTANT) {
            refConst* variable1 = LookupConstList(node->right->left->name);
            if (variable1 != NULL) {
              // set operation's left node to new constant node
              Node* new_constant = malloc(sizeof(Node));
              new_constant->value = variable1->val;
              new_constant->type = EXPRESSION;
              new_constant->exprCode = CONSTANT;
              // free old variable node
              FreeVariable(node->right->left);
              // make operation's left point to new_constant node
              node->right->left = new_constant;
              // to record change was made
              madeChange == true;
            }
          }
          if (node->right->right->exprCode != CONSTANT) {
            refConst* variable2 = LookupConstList(node->right->right->name);
            if (variable2 != NULL) {
              // set operation's right node to new constant node
              Node* new_constant = malloc(sizeof(Node));
              new_constant->value = variable2->val;
              new_constant->type = EXPRESSION;
              new_constant->exprCode = CONSTANT;
              // free old variable node
              FreeVariable(node->right->right);
              // make operation's right point to new_constant node
              node->right->right = new_constant;
              // to record change was made
              madeChange == true;
            }
          }
        }

        // case for function calls with variables as args
        if (node->stmtCode == ASSIGN && node->right->opCode == FUNCTIONCALL) {
          // if args in function call are variables w constant values, replace
          NodeList* arguments = node->right->arguments;
          // iterate linked list of args in function call, replacing args with constants as needed
          while (arguments != NULL) {
            if (arguments->node->exprCode != CONSTANT) {
              refConst* variable = LookupConstList(arguments->node->name);
              if (variable != NULL) {
                // set arg's left node to new constant node
                Node* new_constant = malloc(sizeof(Node));
                new_constant->value = variable->val;
                new_constant->type = EXPRESSION;
                new_constant->exprCode = CONSTANT;
                // free old variable node
                FreeVariable(arguments->node);
                // make arg's left point to new_constant node
                arguments->node = new_constant;
                // to record change was made
                madeChange == true;
              }
            }
            arguments = arguments->next;
          }
        }

        // case for return statement with variable w/ constant value
        if (node->stmtCode == RETURN) {
          if (node->left->exprCode != CONSTANT) {
            refConst* variable = LookupConstList(node->left->name);
            if (variable != NULL) {
              Node* new_constant = malloc(sizeof(Node));
              new_constant->value = variable->val;
              new_constant->type = EXPRESSION;
              new_constant->exprCode = CONSTANT;
              // free old variable node
              FreeVariable(node->left);
              // make operation's right point to new_constant node
              node->left = new_constant;
              // to record change was made
              madeChange == true;
            }
          }
        }

        // if assign statement and right side of = is just a variable w constant value, replace
        if (node->stmtCode == ASSIGN && node->right->exprCode == VARIABLE) {
          refConst* variable = LookupConstList(node->right->name);
          if (variable != NULL) {
              // set assign's right node to new constant node
              Node* new_constant = malloc(sizeof(Node));
              new_constant->value = variable->val;
              new_constant->type = EXPRESSION;
              new_constant->exprCode = CONSTANT;
              // free old variable node
              FreeVariable(node->right);
              // make negate's left point to new_constant node
              node->right = new_constant;
              // to record change was made
              madeChange == true;
            }
        }

        statements = statements->next;
    }
}


bool ConstProp(NodeList* worklist) {
    // NodeList* worklist = NodeList* funcdecls
    while(worklist!=NULL){
            /*
             ****************************************
                     TODO : YOUR CODE HERE
             ****************************************
             */
        // call TrackConst on every NodeList* statements for each function in funcdecls passed as 'worklist'
          // worklist->node->statements = 'funcdecls->function->statements'
        TrackConst(worklist->node->statements);
        worklist = worklist->next;
        FreeConstList();
    }
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
