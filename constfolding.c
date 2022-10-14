/*
   CONSTFOLDING.C : THIS FILE IMPLEMENTS THE CONSTANT FOLDING OPTIMIZATION
*/

#include "constfolding.h"
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL 
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO 
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/                                                                                                          
bool madeChange;

/*
******************************************************************************************
FUNCTION TO CALCULATE THE CONSTANT EXPRESSION VALUE 
OBSERVE THAT THIS IMPLEMENTATION CONSIDERS ADDITIONAL OPTIMIZATIONS SUCH AS:
1.  IDENTITY MULTIPLY = 1 * ANY_VALUE = ANY_VALUE - AVOID MULTIPLICATION CYCLE IN THIS CASE
2.  ZERO MULTIPLY = 0 * ANY_VALUE = 0 - AVOID MULTIPLICATION CYCLE
3.  DIVIDE BY ONE = ORIGINAL_VALUE - AVOID DIVISION CYCLE
4.  SUBTRACT BY ZERO = ORIGINAL_VALUE - AVOID SUBTRACTION
5.  MULTIPLICATION BY 2 = ADDITION BY SAME VALUE [STRENGTH REDUCTION]
******************************************************************************************
*/
long CalcExprValue(Node* node)
{
     long result;
     Node *leftNode, *rightNode;
     leftNode = node->left;
     rightNode = node->right; 
     switch(node->opCode){
         case MULTIPLY:
             if(leftNode->value == 1) {
                 result = rightNode->value;
             } 
             else if(rightNode->value == 1) {
                 result = leftNode->value;
             }
             else if(leftNode->value == 0 || rightNode->value == 0) {
                 result = 0;
             }
             else if(leftNode->value == 2) {
                 result = rightNode->value + rightNode->value;
             }              
             else if(rightNode->value == 2) {
                 result = leftNode->value + leftNode->value;
             }
             else {
                 result = leftNode->value * rightNode->value;
             }
             break;
         case DIVIDE:
             if(rightNode->value == 1) {
                 result = leftNode->value;
             }
             else {
                 result = leftNode->value / rightNode->value;
             }
             break;
         case ADD:
             result = leftNode->value + rightNode->value;
             break;
         case SUBTRACT:
             result = leftNode->value - rightNode->value;
             break;
         case NEGATE:
             result = -leftNode->value;
             break;
         case BOR:
            result = leftNode->value | rightNode->value;
            break;
         case BAND:
            result = leftNode->value & rightNode->value;
            break;
         case BXOR:
            result = leftNode->value ^ rightNode->value;
            break;
         case BSHR:
            result = leftNode->value >> rightNode->value;
            break;
         case BSHL:
            result = leftNode->value << rightNode->value;
            break;
         default:
             break;
     }
     return result;
}


/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO PROCESS THE CANDIDATE STATEMENTS AND PERFORM CONSTANT FOLDING 
WHEREVER APPLICABLE.
******************************************************************************************************
*/
long ConstFoldPerStatement(Node* stmtNodeRight){
    long result;
    /*
    *************************************************************************************
          TODO: YOUR CODE HERE
    **************************************************************************************
    */
    // make sure stmtNodeRight is an operator and not a function call
    if (stmtNodeRight->exprCode == OPERATION && stmtNodeRight->opCode != FUNCTIONCALL) {

        if (stmtNodeRight->opCode == NEGATE && stmtNodeRight->left->exprCode == CONSTANT) {
            
            // this way did not work
            /* make new node with updated constant value after folding
            Node* new_constant = malloc(sizeof(Node));
            new_constant->value = CalcExprValue(stmtNodeRight);
            new_constant->type = EXPRESSION;
            new_constant->exprCode = CONSTANT;
            // set tmp to expression that was constant folded
            Node* tmp = stmtNodeRight;
            // make ASSIGN's right node the new constant node
            stmtNodeRight = new_constant;
            // free old expression stored in tmp
            FreeExpression(tmp);*/


            // change node holding operation to store value of constant folding optimization
            stmtNodeRight->value = CalcExprValue(stmtNodeRight);
            // free child node branching off the unary operation that held previous constant
            FreeConstant(stmtNodeRight->left);
            // change node's fields to reflect new constant value stored in it (replaced operation)
            stmtNodeRight->type = EXPRESSION;
            stmtNodeRight->exprCode = CONSTANT;
            stmtNodeRight->opCode = O_NONE;
            stmtNodeRight->stmtCode = S_NONE;

            // to record change was made
            madeChange == true;
        }

        // if binary operation and both args are constants, fold
        if (stmtNodeRight->left->exprCode == CONSTANT && stmtNodeRight->right->exprCode == CONSTANT) {

            // this way did not work
            /* make new node with updated constant value after folding
            Node* new_constant = malloc(sizeof(Node));
            new_constant->value = CalcExprValue(stmtNodeRight);
            new_constant->type = EXPRESSION;
            new_constant->exprCode = CONSTANT;
            // set tmp to expression that was constant folded
            Node* tmp = stmtNodeRight;
            // make ASSIGN's right node the new constant node
            stmtNodeRight = new_constant;
            // free old expression stored in tmp
            FreeExpression(tmp);*/


            // change node holding operation to store value of constant folding optimization
            stmtNodeRight->value = CalcExprValue(stmtNodeRight);
            // free child node branching off the unary operation that held previous constant
            FreeConstant(stmtNodeRight->left);
            FreeConstant(stmtNodeRight->right);
            // change node's fields to reflect new constant value stored in it (replaced operation)
            stmtNodeRight->type = EXPRESSION;
            stmtNodeRight->exprCode = CONSTANT;
            stmtNodeRight->opCode = O_NONE;
            stmtNodeRight->stmtCode = S_NONE;

            // to record change was made
            madeChange == true;
        }
    }
    

    return -1;
}


/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO IDENTIFY THE STATEMENTS THAT ARE ACTUAL CANDIDATES FOR CONSTANT FOLDING
AND CALL THE APPROPRIATE FUNCTION FOR THE IDENTIFIED CANDIDATE'S CONSTANT FOLDING
******************************************************************************************************
*/
void ConstFoldPerFunction(Node* funcNode) {
      Node *rightNode, *leftNode, *stmtNodeRight;
      long result;
      NodeList* statements = funcNode->statements;
      while(statements != NULL) {
          stmtNodeRight = statements->node->right;
          /*
          *************************************************************************************
		TODO: YOUR CODE HERE
          **************************************************************************************
          */       
        // call ConstFoldPerStatement if ASSIGN statement
            // stmtNodeRight = statements->node->right = the right side of an assignment statement (right of the '=')
        if (statements->node->stmtCode == ASSIGN) {
            ConstFoldPerStatement(stmtNodeRight);
        }                                                                                                                 
	    statements = statements->next;
      }
     
}



/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
********************************************************************************************************************************
*/

/*
*****************************************************************************************************
THIS FUNCTION ENSURES THAT THE CONSTANT FOLDING OPTIMIZATION IS DONE FOR EVERY FUNCTION IN THE PROGRAM
******************************************************************************************************
*/

bool ConstantFolding(NodeList* list) {
    // NodeList* list = NodeList* funcdecls
    madeChange = false;
    while(list != NULL) {
          /*
          *************************************************************************************
		TODO: YOUR CODE HERE
          **************************************************************************************
          */
        // call function on every function
            // list->node = 'funcdecls->function'
        ConstFoldPerFunction(list->node);
	    list = list->next;
    }
    return madeChange;
}

/*
****************************************************************************************************************************
 END OF CONSTANT FOLDING
*****************************************************************************************************************************
*/                
