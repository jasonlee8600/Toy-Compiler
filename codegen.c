/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"

int argCounter;
int lastUsedOffset;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;


/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if(!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if(varHead==NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(varList->isConst == true) {
            if(varList->value == val) return varList->location;
        }
        else {
            if(!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if(!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if(varList == NULL) printf("NULL varlist");
       while(varList!=NULL) {
           if(!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if(regHead==NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    while(regList!=NULL) {
        if(regName == regList->regName) {
            regList->avail = avail;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            if(!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if(noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            // registers available
            if(!noAcc) return 1;
            if(noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(!strcmp(regList->regName, name)) {
           if(regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
void CreateRegList() {
    // Create the initial reglist which can be used to store variables.
    // 4 general purpose registers : AX, BX, CX, DX
    // 4 special purpose : SP, BP, SI , DI. 
    // Other registers: r8, r9
    // You need to decide which registers you will add in the register list 
    // use. Can you use all of the above registers?
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ***************************************
    */
    // which registers can we use to store variables?
        // %rax, %rcx, %rdx, %rdi, %rsi, %rsp, and %r8-r9 are caller-save registers, 
            // meaning that they are not necessarily saved across function calls
        // %rdi, %rsi, %rdx, %rcx, %r8, and %r9 
            // are used to pass the first six integer or pointer parameters to called functions
    
    // callee saved registers
        // if function call, these registers should remain same
    // caller saved
        // could change after function call

    AddRegInfo("%rdi", 1); // 1st arg, caller-saved
    AddRegInfo("%rsi", 1); // 2nd arg, caller-saved
    AddRegInfo("%rdx", 1); // 3rd arg, caller-saved
    AddRegInfo("%rcx", 1); // 4th arg, caller-saved, use for second operand
    AddRegInfo("%r8", 1);  // 5th arg, caller-saved
    AddRegInfo("%r9", 1);  // 6th arg, caller-saved
    AddRegInfo("%rax", 1); // tmp register, return value, caller-saved

    
}



/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
int PushArgOnStack(NodeList* arguments) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    while(arguments!=NULL) {
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE 
      FIRST ARGUMENT OF A FUNCTION STORED.
     ************************************************************************
     */ 
        arguments = arguments->next;
    }
    return argCounter;
}


/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
void PopArgFromStack(NodeList* arguments) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    while(arguments!=NULL) {
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE
      FIRST ARGUMENT OF A FUNCTION STORED AND WHERE SHOULD IT BE EXTRACTED
      FROM AND STORED TO..
     ************************************************************************
     */
        arguments = arguments->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */    
    while(statements != NULL) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */          
        Node* statement = statements->node;
        if (statement->stmtCode == ASSIGN) {    // && statement->right->opCode != FUNCTIONCALL
                                                    // where do i put this condition to deal with function calls in ASSIGN's

            // do i reset this to 0 for each function? YES
                // file w multiple functions; does the 'popq %rbp' at the end of each function's assembly 
                    // make %rbp and %rsp to its originals before pushing any arguments onto stack for that function?
                    // should arg1 for next function be at -8(%rbp)? or at (-8+lastOffset)(%rbp)
            LongToCharOffset();
            // add variable on right side of ASSIGN to varlist
            // what if func call and current variable we are reading is temp1 which was previously defined with diff location 'lastOffsetUsed' in caller func
                // AddVarInfo or UpdateVarInfo? make sure no "NULL varlist" comment is ouput from LookupVarInfo and makes optimizations fail
            UpdateVarInfo(statement->name, lastOffsetUsed, INVAL, false);

            if (statement->right->exprCode == OPERATION) {
                OpType opCode = statement->right->opCode;
                Node* left_child = statement->right->left;
                // check if error trying to access NEGATE right child? no error
                Node* right_child = statement->right->right;
                

                switch(opCode) {
                    case MULTIPLY:
                        print_statement_assembly(left_child, right_child, "imulq");
                        break;
                    case DIVIDE:
                        // if left child constant
                        if (left_child->exprCode == CONSTANT) {
                            // move constant value into %rax
                            fprintf(fptr, "\nmovq $%d, %%rax", left_child->value);
                            fprintf(fptr, "\ncqto"); 
                            // lookup location of right child variable
                            char* right_location = LookUpVarInfo(right_child->name, INVAL);
                            // divides %rax (constant aka dividend) by right child variable (divisor)
                            fprintf(fptr, "\nidivq %s", right_location);
                            print_movq_instr("%rax", lastOffsetUsed);
                        }
                        // if right child constant
                        if (right_child->exprCode == CONSTANT) {
                            char* left_location = LookUpVarInfo(left_child->name, INVAL);
                            // move left child variable into %rax
                            print_movq_instr(left_location, "%rax");
                            // move right child constant into %rcx
                            fprintf(fptr, "\nmovq $%d, %%rcx", right_child->value);
                            fprintf(fptr, "\ncqto"); 
                            // divides %rax (left child var aka divident) by constant val (divisor)
                            fprintf(fptr, "\nidivq %%rcx");
                            print_movq_instr("%rax", lastOffsetUsed); 
                        }
                        // if neither constant
                        if (left_child->exprCode != CONSTANT && right_child->exprCode != CONSTANT) {
                            char* left_location = LookUpVarInfo(left_child->name, INVAL);
                            // left child var in %rax bc dividend
                            print_movq_instr(left_location, "%rax");
                            fprintf(fptr, "\ncqto"); 
                            char* right_location = LookUpVarInfo(right_child->name, INVAL);
                            // divides %rax (left child var aka dividend) by right child var (divisor)
                            fprintf(fptr, "\nidivq %s", right_location); 
                            print_movq_instr("%rax", lastOffsetUsed);
                        }
                        break;
                    case ADD:
                        print_statement_assembly(left_child, right_child, "addq");
                        break;
                    case SUBTRACT:
                        print_statement_assembly(left_child, right_child, "subq");
                        break;
                    case NEGATE:
                        char* left_location = LookUpVarInfo(left_child->name, INVAL);
                        print_movq_instr(left_location, "%rax");
                        fprintf(fptr, "\nnegq %rax");
                        print_movq_instr("%rax", lastOffsetUsed);
                        break;
                    case BOR:
                        print_statement_assembly(left_child, right_child, "orq");
                        break;
                    case BAND:
                        print_statement_assembly(left_child, right_child, "andq");
                        break;
                    case BXOR:
                        print_statement_assembly(left_child, right_child, "xorq");
                        break;
                    case BSHR:
                        // if left_child const, store in %rcx for the shift instr
                        if (left_child->exprCode == CONSTANT) {
                            // put left_child val into %rax
                            fprintf(fptr, "\nmovq $%d, %%rax", left_child->value);
                            char* right_location = LookUpVarInfo(right_child->name, INVAL);
                            // put right_child into %rcx
                            print_movq_instr(right_location, "%rcx");
                            // sarq %cl, %rax
                            fprintf(fptr, "\nsarq %%cl, %%rax");
                            // put result in next stack location
                            print_movq_instr("%rax", lastOffsetUsed);
                        }
                        else {
                            print_statement_assembly(left_child, right_child, "sarq");
                        }
                        break;
                    case BSHL:
                        // if left_child const, store in %rcx for the shift instr
                        if (left_child->exprCode == CONSTANT) {
                            // put left_child val into %rax
                            fprintf(fptr, "\nmovq $%d, %%rax", left_child->value);
                            char* right_location = LookUpVarInfo(right_child->name, INVAL);
                            // put right_child into %rcx
                            print_movq_instr(right_location, "%rcx");
                            // salq %cl, %rax
                            fprintf(fptr, "\nsalq %%cl, %%rax");
                            // put result in next stack location
                            print_movq_instr("%rax", lastOffsetUsed);
                        }
                        else {
                            print_statement_assembly(left_child, right_child, "salq");
                        }
                        break;
                    case FUNCTIONCALL:
                        // movq arguments in func all into the correct place: %rdi, %rsi, %rdx, etc.
                        char* regIndex[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
                        // to know which register i am moving func call arg into
                        int index = 0;
                        NodeList* args = statement->right->arguments;
                        while (args != NULL) {
                            // if arg is a constant
                            if (args->node->exprCode == CONSTANT) {
                                // movq const, next_reg
                                fprintf(fptr, "\nmovq $%d, %s", args->node->value, regIndex[index]);
                            }
                            // if arg is a var or func parameter
                            if (args->node->exprCode == VARIABLE || args->node->exprCode == PARAMETER) {
                                char* arg_location = LookUpVarInfo(args->node->name, INVAL);
                                // movq arg_location, next_reg
                                print_movq_instr(arg_location, regIndex[index]);
                            }
                            // increase index to store next one in next register
                            index += 1;
                            // go to next arg
                            args = args->next;
                        }
                        // actually call the function
                        fprintf(fptr, "\ncallq %s", statement->right->left->name);

                        // store the return value (%rax) somewhere (next block in stack)
                            // movq %rax, -next.location.stack(%rbp)
                        print_movq_instr("%rax", lastOffsetUsed);
                        break;
                    default:
                        break;

                    // cd tests
                        // gcc -S example#.c -o #.s
                            // #.s should contain the 'expected' gcc assembly output from the # example file
                    
                    //./323compiler ./tests/example#.c
                        // outputs the assembly my codegen.c produced into assembly.s

                    // examples w/o func calls: 1, 3, 7, 10
                    // examples w/ func calls: 5, 8, 9, 11, 24
                        // passing all now


                }

            }


            // case for ASSIGN but no operation: 
                // exprCode = VARIABLE or PARAMETER
                // opCode = O_NONE
                    // temp1 = arg1;    
                        // OR
                    // temp1 = arg1 + arg2;
                    // temp2 = temp1; <--
            if (statement->right->exprCode == VARIABLE || statement->right->exprCode == PARAMETER) {
                char* arg_location = LookUpVarInfo(statement->right->name, INVAL);
                print_movq_instr(arg_location, lastOffsetUsed);
            }
                
        }

        if (statement->stmtCode == RETURN) {
            Node* left_child = statement->left;
            char* left_location = LookUpVarInfo(left_child->name, INVAL);
            print_movq_instr(left_location, "%rax");
            
            // update all registers to be available after func returns
            UpdateRegInfo("%rdi", 1);
            UpdateRegInfo("%rsi", 1);
            UpdateRegInfo("%rdx", 1);
            UpdateRegInfo("%rcx", 1);
            UpdateRegInfo("%r8", 1);
            UpdateRegInfo("%r9", 1);
        }

        statements = statements->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/

// put every arg onto stack
// manually print each instruction
// make sure no memory issues (valgrind)

// use int NodeListLen(NodeList* nl); function to calculate how many statements there are
/*int NodeListLen(NodeList* nl) {
  if (nl == NULL) return 0;
  return 1 + NodeListLen(nl->next);
}*/
    // subtract rbp by that number * 8
        // movq %rbp, %rsp at end

void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */

    if(fptr == NULL) {
        printf("\n Could not create assembly file");
        return; 
    }
    CreateRegList();
    NodeList *arguments, *statements;

    while(worklist != NULL) {
      /*
       ****************************************
              TODO : YOUR CODE HERE
       ****************************************
      */
        InitAsm(worklist->node->name);

        arguments = worklist->node->arguments;
        statements = worklist->node->statements;


        // number of args + statements (-1 bc return statement doesn't need to store anything) * 8 bytes per long
        int stack_frame_size = (NodeListLen(arguments) + NodeListLen(statements) - 1) * 8;
        // subq $stack_frame_size, %rsp
        fprintf(fptr, "\nsubq $%i, %%rsp", stack_frame_size); 

        // reset %rbp offset prefix
        lastUsedOffset = 0;

        while (arguments != NULL) {
            LongToCharOffset();
            // get next available reg to store func arg in
            char* avail_reg = GetNextAvailReg(false);
            // make that reg unavailable
            UpdateRegInfo(avail_reg, 0);
            // add arg into variable list with location as lastOffsetUsed (for arg1 is is -8(%rbp))
            AddVarInfo(arguments->node->name, lastOffsetUsed, INVAL, false);
            // movq each function arg to stack
            print_movq_instr(avail_reg, lastOffsetUsed);
            arguments = arguments->next;
        }
        

        // create assembly for each statement
        ProcessStatements(statements);
        
        // PrintVarListInfo(); // to check if function args are being stored on stack; arg1 : -8(%rbp)  arg2 : -16(%rbp)    etc.
        // PrintRegListInfo(); // to check reg list

        // movq %rbp, %rsp
        fprintf(fptr, "\nmovq %%rbp, %%rsp"); 

        RetAsm();

        worklist = worklist->next; 
    }
    
    // free var and reg lists
    FreeVarList();
    FreeRegList();

    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/
void print_movq_instr(char* src, char* dest) {
    fprintf(fptr, "\nmovq %s, %s", src, dest); 
}

void print_statement_assembly(Node* left_child, Node* right_child, char* operation) {
    if (left_child->exprCode == CONSTANT) {
        // store left_child val in %rax
        fprintf(fptr, "\nmovq $%d, %%rax", left_child->value);
        char* right_location = LookUpVarInfo(right_child->name, INVAL);
        // opInstruction right_child.location, %rax (left_child const val)
        fprintf(fptr, "\n%s %s, %%rax", operation, right_location); 
        // put result in next stack location
        print_movq_instr("%rax", lastOffsetUsed);
    }
    if (right_child->exprCode == CONSTANT) {
        char* left_location = LookUpVarInfo(left_child->name, INVAL);
        // store left_child val in %rax
        print_movq_instr(left_location, "%rax");
        // opInstruction right_child val, %rax (left_child val)
        fprintf(fptr, "\n%s $%d, %%rax", operation, right_child->value); 
        // put result in next stack location
        print_movq_instr("%rax", lastOffsetUsed);
    }
    if (left_child->exprCode != CONSTANT && right_child->exprCode != CONSTANT) {
        char* left_location = LookUpVarInfo(left_child->name, INVAL);
        char* right_location = LookUpVarInfo(right_child->name, INVAL);
        // put left_child val in %rax
        print_movq_instr(left_location, "%rax");
        // opInstruction right_child val, %rax (left_child val)
        fprintf(fptr, "\n%s %s, %%rax", operation, right_location); 
        // put result in next stack location
        print_movq_instr("%rax", lastOffsetUsed);
    }
}

                        
/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/


