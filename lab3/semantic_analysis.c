#include "def.h"

int LEV = 0;   //���
int func_size; //�����Ļ��¼��С

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

// �ռ�������Ϣ
void semantic_error(int line, char *msg1, char *msg2){
    printf("��%d��,%s %s\n", line, msg1, msg2);
}

int searchSymbolTable(char *name)
{
    int i;
    for (i = symbolTable.index - 1; i >= 0; i--)
        if (!strcmp(symbolTable.symbols[i].name, name))
            return i;
    return -1;
}

void prn_symbol()
{ //��ʾ���ű�
    int i = 0;
    char symbolsType[20];
    printf("  %6s  %6s   %6s   %6s  %4s  %6s\n", "������", "����", "���", "����", "���", "ƫ����");
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

// ���ȸ���name����ű������ظ����� �ظ����巵��-1
int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset)
{
    int i;
    /*���Ų��أ������ⲿ��������ǰ�к������壬
    ���β������ڷ��ű��У���ʱ���ⲿ������ǰ�������β������������*/
    for (i = symbolTable.index - 1; symbolTable.symbols[i].level == level || (level == 0 && i >= 0); i--)
    {
        if (level == 0 && symbolTable.symbols[i].level == 1)
            continue; //�ⲿ�������ββ��رȽ�����
        if (!strcmp(symbolTable.symbols[i].name, name))
            return -1;
    }
    //��д���ű�����
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
    return symbolTable.index++; //���ص��Ƿ����ڷ��ű��е�λ����ţ��м��������ʱ�������ȡ�����ű���
}

//��д��ʱ���������ű�������ʱ�����ڷ��ű��е�λ��
int fill_Temp(char *name, int level, int type, char flag, int offset)
{
    strcpy(symbolTable.symbols[symbolTable.index].name, "");
    strcpy(symbolTable.symbols[symbolTable.index].alias, name);
    symbolTable.symbols[symbolTable.index].level = level;
    symbolTable.symbols[symbolTable.index].type = type;
    symbolTable.symbols[symbolTable.index].flag = flag;
    symbolTable.symbols[symbolTable.index].offset = offset;
    return symbolTable.index++; //���ص�����ʱ�����ڷ��ű��е�λ�����
}

int match_param(int i, struct ASTNode *T){ // ƥ�亯������
    int j, num = symbolTable.symbols[i].paramnum;
    int type1, type2;
    struct ASTNode *T0 = T;
    if (num == 0 && T == NULL)
        return 1;
    for (j = 1; j < num; j++) {
        type1 = symbolTable.symbols[i + j].type; //�β�����
        type2 = T0->ptr[0]->type;
        if (type1 != type2) {
            semantic_error(T0->pos, "", "�������Ͳ�ƥ��");
            return 0;
        }
        T0 = T0->ptr[1];
    }
    return 1;
}

void semantic_Analysis(struct ASTNode *T)
{ //�Գ����﷨�����ȸ�����,��display�Ŀ��ƽṹ�޸���ɷ��ű������������TAC���ɣ���䲿�֣�
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
        case EXT_VAR_DEF: //�����ⲿ˵��,����һ������(TYPE���)�е������͵��ڶ������ӵ�������
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
        case FUNC_DEC: //���ݷ������ͣ���������д���ű�
            func_dec(T);
            break;
        case PARAM_LIST: //��������ʽ�����б�
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
        case VAR_DEF: //����һ���ֲ���������,����һ������(TYPE���)�е������͵��ڶ������ӵ�������
                      //������������ⲿ����EXT_VAR_DEF������һ�ִ�����
            var_def(T);
            break;
        case STM_LIST:
            stm_list(T);
            break;
        case IF_THEN:
            if_then(T);
            break; 
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
            Exp(T); //����������ʽ
            break;
        }
    }
}

void semantic_Analysis0(struct ASTNode *T)
{
    symbolTable.index = 0;
    fillSymbolTable("read", "", 0, INT, 'F', 4);
    symbolTable.symbols[0].paramnum = 0; //read���βθ���
    fillSymbolTable("x", "", 1, INT, 'P', 12);
    fillSymbolTable("write", "", 0, INT, 'F', 4);
    symbolTable.symbols[2].paramnum = 1;
    symbol_scope_TX.TX[0] = 0; //�ⲿ�����ڷ��ű��е���ʼ���Ϊ0
    symbol_scope_TX.top = 1;
    T->offset = 0; // �ⲿ��������������ƫ����
    semantic_Analysis(T);
    // prn_symbol(); 
    // printf("\n\n\n\n");
    prnIR(T->code);
}