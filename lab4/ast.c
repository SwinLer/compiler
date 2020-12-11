#include "def.h"
#include "parser.tab.h"
#define OFFSET 3
#define MAX_LOOP 100

struct ASTNode * mknode(int num,int kind,int pos,...){
    struct ASTNode *T=(struct ASTNode *)calloc(sizeof(struct ASTNode),1);
    int i=0;
    T->kind=kind;
    T->pos=pos;
    va_list pArgs;
    va_start(pArgs, pos);
    for(i=0;i<num;i++)
        T->ptr[i]= va_arg(pArgs, struct ASTNode *);
    while (i<4) T->ptr[i++]=NULL;
    va_end(pArgs);
    return T;
}


void display(struct ASTNode *T,int indent)
{//对抽象语法树的先根遍�?
  int i=1;
  struct ASTNode *T0;
  if (T)
	{
	switch (T->kind) {
    case EXT_DEF_LIST:  
        display(T->ptr[0],indent);    //显示该外部定义（外部变量和函数）列表中的第一�?
        display(T->ptr[1],indent);    //显示该外部定义列表中的其它外部定�?
        break;
    case EXT_VAR_DEF:   
        printf("%*c外部变量定义�?(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);        //显示外部变量类型
        printf("%*c变量名：\n",indent+OFFSET,' ');
        display(T->ptr[1],indent+OFFSET*2);        //显示变量列表
        break;
    case TYPE:          
        printf("%*c类型�? %s\n",indent,' ',T->type_id);
        break;
    case EXT_DEC_LIST:  
        if(T->ptr[0]->ptr[0] != NULL)
        {
            printf("%*c数组名：%s\n", indent, ' ', T->ptr[0]->type_id);
            printf("%*c数组大小：\n", indent, ' ');
        }
        display(T->ptr[0],indent);     //依次显示外部变量名，
        display(T->ptr[1],indent);     //后续还有相同的，仅显示语法树此处理代码可以和类似代码合并
        break;
    case FUNC_DEF:      
        printf("%*c函数定义�?(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);      //显示函数返回类型
        display(T->ptr[1],indent+OFFSET);      //显示函数名和参数
        display(T->ptr[2],indent+OFFSET);      //显示函数�?
        break;
    case FUNC_DEC:      
        printf("%*c函数名：%s\n",indent,' ',T->type_id);
        if (T->ptr[0]) {
            printf("%*c函数形参：\n",indent,' ');
            display(T->ptr[0],indent+OFFSET);  //显示函数参数列表
        }
        else printf("%*c无参函数\n",indent+OFFSET,' ');
        break;
    case PARAM_LIST:    
        display(T->ptr[0],indent);     //依次显示全部参数类型和名称，
        display(T->ptr[1],indent);
        break;
    case PARAM_DEC:     
        printf("%*c类型�?%s, 参数名：%s\n",indent,' ',T->ptr[0]->type==INT?"int":"float",T->ptr[1]->type_id);
        break;
    case EXP_STMT:      
        printf("%*c表达式语句：(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);
        break;
    case RETURN:        
        printf("%*c返回语句�?(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);
        break;
    case COMP_STM:      
        printf("%*c复合语句�?(%d)\n",indent,' ',T->pos);
        printf("%*c复合语句的变量定义部分：\n",indent+OFFSET,' ');
        // if(T->ptr[0] == NULL)
        //     printf("\n\n NULL!! \n\n");
        display(T->ptr[0],indent+OFFSET*2);      //显示定义部分
        printf("%*c复合语句的语句部分：\n",indent+OFFSET,' ');
        display(T->ptr[1],indent+OFFSET*2);      //显示语句部分
        break;
    case STM_LIST:      
        display(T->ptr[0],indent);      //显示第一条语�?
        display(T->ptr[1],indent);        //显示剩下语句
        break;
    case WHILE:         
        printf("%*c循环语句(WHILE)�?(%d)\n",indent,' ',T->pos);
        printf("%*c循环条件(WHILE)：\n",indent+OFFSET,' ');
        display(T->ptr[0],indent+OFFSET*2);      //显示循环条件
        printf("%*c循环�?(WHILE)�?(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1],indent+OFFSET*2);      //显示循环�?
        break;
    case FOR:
        printf("%*c循环语句(FOR)�?(%d)\n",indent,' ',T->pos);
        printf("%*c循环条件(FOR)：\n", indent+OFFSET, ' ');
        display(T->ptr[0], indent+OFFSET*2);
        printf("%*c循环�?(FOR)�?(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1], indent+OFFSET*2);
        break;
    case SWITCH_STMT:
        printf("%*cSWITCH语句�?(%d)\n", indent, ' ', T->pos);
        display(T->ptr[0], indent+OFFSET);
        display(T->ptr[1], indent+OFFSET);
        break;
    case CASE_STMT:
        printf("%*cCASE语句�?(%d)\n", indent, ' ', T->pos);
        display(T->ptr[0], indent+OFFSET);
        display(T->ptr[1], indent+OFFSET);
        break;
    case FOR_DEC:
        display(T->ptr[0], indent+OFFSET*2);
        display(T->ptr[1], indent+OFFSET*2);
        display(T->ptr[2], indent+OFFSET*2);
        break;
    case CONTINUE:
        printf("%*cCONTINUE语句�?(%d)\n", indent, ' ', T->pos);
        break;
    case BREAK:
        printf("%*cBREAK语句�?(%d)\n", indent, ' ', T->pos);
        break;
    case IF_THEN:       
        printf("%*c条件语句(IF_THEN)�?(%d)\n",indent,' ',T->pos);
        printf("%*c条件：\n",indent+OFFSET,' ');
        display(T->ptr[0],indent+OFFSET*2);      //显示条件
        printf("%*cIF子句�?(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1],indent+OFFSET*2);      //显示if子句
        break;
    case IF_THEN_ELSE:  
        printf("%*c条件语句(IF_THEN_ELSE)�?(%d)\n",indent,' ',T->pos);
        printf("%*c条件：\n",indent+OFFSET,' ');
        display(T->ptr[0],indent+OFFSET*2);      //显示条件
        printf("%*cIF子句�?(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1],indent+OFFSET*2);      //显示if子句
        printf("%*cELSE子句�?(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[2],indent+OFFSET*2);      //显示else子句
        break;
    case DEF_LIST:     
        display(T->ptr[0],indent);    //显示该局部变量定义列表中的第一�?
        display(T->ptr[1],indent);    //显示其它局部变量定�?
        break;
    case VAR_DEF:       
        printf("%*c局部变量定义：(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);   //显示变量类型
        display(T->ptr[1],indent+OFFSET);   //显示该定义的全部变量�?
        break;
    case DEC_LIST:      
        printf("%*c变量名：\n",indent,' ');
        T0=T;
        while (T0) {
            if (T0->ptr[0]->kind==ID)
                printf("%*c %s\n",indent+OFFSET*2,' ',T0->ptr[0]->type_id);
            else if (T0->ptr[0]->kind==ASSIGNOP)
            {
                printf("%*c %s ASSIGNOP\n ",indent+OFFSET*2,' ',T0->ptr[0]->ptr[0]->type_id);
                display(T0->ptr[0]->ptr[1],indent+strlen(T0->ptr[0]->ptr[0]->type_id)+7);        //显示初始化表达式
            }
            T0=T0->ptr[1];
        }
        break;
    case ARRAY_LIST:
        display(T->ptr[0], indent);
        display(T->ptr[1], indent);
        break;
    case ARRAY_ID:
        printf("%*c数组名： %s\n",indent,' ',T->type_id);
        printf("%*c访问下标：\n", indent, ' ');
        display(T->ptr[0], indent+OFFSET);
        break;
    case STRUCT_DEF:
        printf("%*c结构定义�?(%d)\n", indent, ' ',T->pos);
        display(T->ptr[0], indent+OFFSET);
        display(T->ptr[1], indent+OFFSET);
        break;
    case STRUCT_TAG:
        printf("%*c结构名：%s\n", indent, ' ', T->struct_name);
        break;
    case ID:	        
        printf("%*cID�? %s\n",indent,' ',T->type_id);
        break;
    case INT:	     
        printf("%*cINT�?%d\n",indent,' ',T->type_int);
        break;
    case FLOAT:	        
        printf("%*cFLAOT�?%f\n",indent,' ',T->type_float);
        break;
    case CHAR:
        printf("%*cCHAR: %s\n", indent, ' ', T->type_char);
        break;
    case STRING:
        printf("%*cSTRING: %s\n", indent, ' ', T->type_string);
        break;
	case ASSIGNOP:
    case PLUSASSIGNOP:
    case MINUSASSIGNOP:
    case STARASSIGNOP:
    case DIVASSIGNOP:
    case MODASSIGNOP:
    case AND:
    case OR:
    case RELOP:
    case PLUS:
    case AUTOPLUS:
    case AUTOMINUS:
    case MINUS:
    case STAR:
    case DIV:
    case MOD:
        printf("%*c%s\n",indent,' ',T->type_id);
        display(T->ptr[0],indent+OFFSET);
        display(T->ptr[1],indent+OFFSET);
        break;
    case EXP_ELE:
        printf("%*c结构体访问：\n", indent, ' ');
        display(T->ptr[0], indent+OFFSET);
        printf("%*c访问成员变量�?%s\n", indent+OFFSET, ' ',T->type_id);
        break;
    case NOT:
    case UMINUS:    
        printf("%*c%s\n",indent,' ',T->type_id);
        display(T->ptr[0],indent+OFFSET);
        break;
    case FUNC_CALL: 
        printf("%*c函数调用�?(%d)\n",indent,' ',T->pos);
        printf("%*c函数名：%s\n",indent+OFFSET,' ',T->type_id);
        display(T->ptr[0],indent+OFFSET);
        break;
    case ARGS:      
        i=1;
        while (T) {  //ARGS表示实际参数表达式序列结点，其第一棵子树为其一个实际参数表达式，第二棵子树为剩下的
            struct ASTNode *T0=T->ptr[0];
            printf("%*c�?%d个实际参数表达式：\n",indent,' ',i++);
            display(T0,indent+OFFSET);
            T=T->ptr[1];
        }
//      printf("%*c�?%d个实际参数表达式：\n",indent,' ',i);
  //    display(T,indent+OFFSET);
        printf("\n");
        break;
        }
    }
}

