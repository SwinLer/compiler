#include "def.h"

int LEV = 0;   //层号
int func_size; //函数的活动记录大�?

struct symboltable symbolTable;
struct symbol_scope_begin symbol_scope_TX;

extern int struct_var_flag;
extern char struct_name[33];

int compute_width(struct ASTNode *T){
    if(T){
        if(T->type==INT){
		    return T->type_int;
	    }
	    return T->ptr[0]->type_int*compute_width(T->ptr[1]);
    }
    else
    {
        return 1;
    }
}
int compute_arraywidth(int *array,int index){
    int res=1;
    while(array[index]!=0&&index<10){
        res*=array[index];
        index++;
    }
    return res;
}

int compute_width0(struct ASTNode *T, int *array, int index){
    if(T) {
        if(T->type == INT){
		    return T->type_int;
	    }
        if(T->ptr[0]->kind == ID) {
            int rtn = searchSymbolTable(T->ptr[0]->type_id);
            // printf("!!%s: %d\n", T->ptr[0]->type_id, symbolTable.symbols[rtn].const_int);
            return (symbolTable.symbols[rtn].const_int) * compute_arraywidth(array, index+1) + compute_width0(T->ptr[1], array, index+1);
        }
        else 
	        return (T->ptr[0]->type_int) * compute_arraywidth(array, index+1) + compute_width0(T->ptr[1], array, index+1);
    }
    else{
        return 1;
    } 
}

int array_out(struct ASTNode *T, int *a, int index){
	if(index==10)
		return -1;
	if(T->type==INT){
		a[index] = T->type_int;
		return 1;
	}
	else {
		a[index] = T->ptr[0]->type_int;
		return array_out(T->ptr[1],a,index+1);
	}
}

// 收集错误信息
void semantic_error(int line, char *msg1, char *msg2){
    printf("�?%d�?,%s %s\n", line, msg1, msg2);
}

int searchSymbolTable(char *name)
{
    int i;
    for (i = symbolTable.index - 1; i >= 0; i--)
        if (!strcmp(symbolTable.symbols[i].name, name))
            return i;
    return -1;
}


int substr(char dst[],char src[],int start,int len)
{
	char* p = src + start;   //定义指针变量指向需要提取的字符的地址
	int n = strlen(p);       //求字符串长度
	int i = 0;
		if(n < len)
		{
			len = n;
		}
		while(len != 0)
		{
			dst[i] = src[i+start];
			len --;
			i++;
		}                        //复制字符串到dst�?
		dst[i] = '\0';
	return 0;
}

int searchSymbolTableByAlias(char *alias)
{
    int i;
    for (i = symbolTable.index - 1; i >= 0; i--)
        if (!strcmp(symbolTable.symbols[i].alias, alias))
            return i;
    return -1;
}

void prn_symbol()
{ //显示符号�?
    int i = 0;
    char symbolsType[20];
    printf("  %6s  %6s   %6s   %6s  %4s  %6s\n", "变量�?", "别名", "层号", "类型", "标记", "偏移�?");
    for (i = 0; i < symbolTable.index; i++)
    {
        if (symbolTable.symbols[i].type == INT)
            strcpy(symbolsType, "int");
        if (symbolTable.symbols[i].type == FLOAT)
            strcpy(symbolsType, "float");
        if (symbolTable.symbols[i].type == CHAR)
            strcpy(symbolsType, "char");
        if (symbolTable.symbols[i].type == STRING)
            strcpy(symbolsType, "string");
        if (symbolTable.symbols[i].type == STRUCT)
            strcpy(symbolsType, "struct");
        printf("%6s %6s %6d  %6s %4c %6d\n", symbolTable.symbols[i].name,
               symbolTable.symbols[i].alias, symbolTable.symbols[i].level,
               symbolsType,
               symbolTable.symbols[i].flag, symbolTable.symbols[i].offset);
    }
}

// 首先根据name查符号表，不能重复定�? 重复定义返回-1
int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset)
{
    int i;
    /*符号查重，考虑外部变量声明前有函数定义�?
    其形参名还在符号表中，这时的外部变量与前函数的形参重名是允许�?*/
    for (i = symbolTable.index - 1; symbolTable.symbols[i].level == level || (level == 0 && i >= 0); i--)
    {
        if (level == 0 && symbolTable.symbols[i].level == 1)
            continue; //外部变量和形参不必比较重�?
        if (!strcmp(symbolTable.symbols[i].name, name))
            return -1;
    }
    //填写符号表内�?
    strcpy(symbolTable.symbols[symbolTable.index].name, name);
    strcpy(symbolTable.symbols[symbolTable.index].alias, alias);
    symbolTable.symbols[symbolTable.index].level = level;
    symbolTable.symbols[symbolTable.index].type = type;
    symbolTable.symbols[symbolTable.index].flag = flag;
    symbolTable.symbols[symbolTable.index].offset = offset;
    if(struct_var_flag){
        strcpy(symbolTable.symbols[symbolTable.index].struct_name, struct_name);
        struct_var_flag = 0;
    }
    return symbolTable.index++; //返回的是符号在符号表中的位置序号，中间代码生成时可用序号取到符号别名
}

//填写临时变量到符号表，返回临时变量在符号表中的位�?
int fill_Temp(char *name, int level, int type, char flag, int offset)
{
    strcpy(symbolTable.symbols[symbolTable.index].name, "");
    strcpy(symbolTable.symbols[symbolTable.index].alias, name);
    symbolTable.symbols[symbolTable.index].level = level;
    symbolTable.symbols[symbolTable.index].type = type;
    symbolTable.symbols[symbolTable.index].flag = flag;
    symbolTable.symbols[symbolTable.index].offset = offset;
    return symbolTable.index++; //返回的是临时变量在符号表中的位置序号
}

int match_param(int i, struct ASTNode *T){ // 匹配函数参数
    int j, num = symbolTable.symbols[i].paramnum;
    int type1, type2;
    struct ASTNode *T0 = T;
    if (num == 0 && T == NULL)
        return 1;
    for (j = 1; j < num; j++) {
        type1 = symbolTable.symbols[i + j].type; //形参类型
        type2 = T0->ptr[0]->type;
        if (type1 != type2) {
            semantic_error(T0->pos, "", "参数类型不匹�?");
            return 0;
        }
        T0 = T0->ptr[1];
    }
    return 1;
}

void semantic_Analysis(struct ASTNode *T)
{ //对抽象语法树的先根遍�?,按display的控制结构修改完成符号表管理和语义检查和TAC生成（语句部分）
    int rtn, num, width;
    struct ASTNode *T0;
    struct opn opn1, opn2, result;
    if (T)
    {
        switch (T->kind)
        {
        case EXT_DEF_LIST:
            ext_def_list(T);
            break;
        case EXT_VAR_DEF: //处理外部说明,将第一个孩�?(TYPE结点)中的类型送到第二个孩子的类型�?
            ext_var_def(T);
            break;
        case EXT_STRUCT_DEF: // TODO
            ext_struct_def(T);
            break;
        case STRUCT_DEF: // TODO
            struct_def(T);
            break;
        case STRUCT_DEC: // TODO
            struct_dec(T);
            break;
        case FUNC_DEF:
            func_def(T);
            break;
        case FUNC_DEC: //根据返回类型，函数名填写符号�?
            func_dec(T);
            break;
        case PARAM_LIST: //处理函数形式参数列表
            param_list(T);
            break;
        case PARAM_DEC:
            param_dec(T);
            break;
        case COMP_STM:
            comp_stm(T);
            break;
        case DEF_LIST:
            def_list(T);
            break;
        case VAR_DEF: //处理一个局部变量定�?,将第一个孩�?(TYPE结点)中的类型送到第二个孩子的类型�?
                      //类似于上面的外部变量EXT_VAR_DEF，换了一种处理方�?
            var_def(T);
            break;
        case STM_LIST:
            stm_list(T);
            break;
        case IF_THEN:
            if_then(T);
            break; //控制语句都还没有处理offset和width属�?
        case IF_THEN_ELSE:
            if_then_else(T);
            break;
        case WHILE:
            while_dec(T);
            break;
        case FOR:
            for_stmt(T);
            break;
        case BREAK:
            break_dec(T);
            break;
        case CONTINUE:
            continue_dec(T);
            break;
        case EXP_STMT:
            exp_stmt(T);
            break;
        case SWITCH_STMT:
            switch_stmt(T);
            break;
        case CASE_STMT:
            case_stmt(T);
            break;
        case RETURN:
            return_dec(T);
            break;
        case ID:
        case STRUCT_TAG:
        case INT:
        case FLOAT:
        case CHAR:
        case STRING:
        case ASSIGNOP:
        case AND:
        case OR:
        case RELOP:
        case PLUS:
        case AUTOPLUS_L:
        case AUTOPLUS_R:
        case PLUSASSIGNOP:
        case MINUS:
        case AUTOMINUS_L:
        case AUTOMINUS_R:
        case MINUSASSIGNOP:
        case STAR:
        case STARASSIGNOP:
        case DIV:
        case DIVASSIGNOP:
        case MOD:
        case MODASSIGNOP:
        case NOT:
        case UMINUS:
        case FUNC_CALL:
        case EXP_ARRAY:
        case EXP_ELE:
            Exp(T); //处理基本表达�?
            break;
        }
    }
}

void semantic_Analysis0(struct ASTNode *T)
{
    symbolTable.index = 0;
    fillSymbolTable("read", "", 0, INT, 'F', 4);
    symbolTable.symbols[0].paramnum = 0; //read的形参个�?
    fillSymbolTable("x", "", 1, INT, 'P', 12);
    fillSymbolTable("write", "", 0, INT, 'F', 4);
    symbolTable.symbols[2].paramnum = 1;
    symbol_scope_TX.TX[0] = 0; //外部变量在符号表中的起始序号�?0
    symbol_scope_TX.top = 1;
    T->offset = 0; // 外部变量在数据区的偏移量
    semantic_Analysis(T);
    prn_symbol();
    // 打印中间代码
    // printf("\n\n\n\n");
    prnIR(T->code);
    // 生成目标代码
    object_code(T->code);
}



