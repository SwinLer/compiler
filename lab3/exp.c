#include "def.h"

extern int LEV;   //���
extern int func_size; //�����Ļ��¼��С

extern struct symboltable symbolTable;
extern struct symbol_scope_begin symbol_scope_TX;

extern char case_temp[30];

void Exp(struct ASTNode *T)
{ //����������ʽ���ο�����[2]p82��˼��
    int rtn, num, width;
    struct ASTNode *T0;
    struct opn opn1, opn2, result;
    if (T)
    {
        switch (T->kind)
        {
        case ID: //����ű���÷��ű��е�λ�ã�������type
            id_exp(T);
            break;
        case INT:
            int_exp(T);
            break;
        case FLOAT:
            float_exp(T);
            break;
        case CHAR: // new
            char_exp(T);
            break;
        case STRING: // TODO
            string_exp(T);
            break;
        case ASSIGNOP:
            assignop_exp(T);
            break;
        case PLUSASSIGNOP:
        case MINUSASSIGNOP:
        case STARASSIGNOP:
        case DIVASSIGNOP:
        case MODASSIGNOP:
            other_assignop_exp(T);
            break;
        case AUTOMINUS_L: // TODO
        case AUTOMINUS_R:
        case AUTOPLUS_L:
        case AUTOPLUS_R:
            auto_op_exp(T);
            break;
        case AND:   //���������ʽ��ʽ���㲼��ֵ��δд��
        case OR:    //���������ʽ��ʽ���㲼��ֵ��δд��
        case RELOP: //���������ʽ��ʽ���㲼��ֵ��δд��
            relop_exp(T);
            break;
        case PLUS:
        case MINUS:
        case STAR:
        case DIV:
        case MOD:
            op_exp(T);
            break;
        case NOT: 
            not_exp(T);
            break;
        case UMINUS: 
            // uminus_exp(T);
            break;
        case FUNC_CALL: 
            func_call_exp(T);
            break;
        case ARGS: //�˴��������ʵ�α��ʽ����ֵ�Ĵ������У�������ARG��ʵ��ϵ��
            args_exp(T);
            break;
        case EXP_ARRAY: // TODO
            exp_array(T);
            break;
        case EXP_ELE:
            exp_ele(T);
            break;
        }
    }
}

//�������ʽ
void boolExp(struct ASTNode *T)
{
    struct opn opn1, opn2, result;
    int op;
    int rtn;
    if (T)
    {
        switch (T->kind)
        {
            case INT:
                if (T->type_int != 0)
                    T->code = genGoto(T->Etrue);
                else
                    T->code = genGoto(T->Efalse);
                T->width = 0;
                break;
            case FLOAT:
                if (T->type_float != 0.0)
                    T->code = genGoto(T->Etrue);
                else
                    T->code = genGoto(T->Efalse);
                T->width = 0;
                break;
            case ID: //����ű���÷��ű��е�λ�ã�������type
                rtn = searchSymbolTable(T->type_id);
                if (rtn == -1)
                    semantic_error(T->pos, T->type_id, "����δ����");
                if (symbolTable.symbols[rtn].flag == 'F')
                    semantic_error(T->pos, T->type_id, "�Ǻ����������Ͳ�ƥ��");
                else{
                    opn1.kind = ID;
                    strcpy(opn1.id, symbolTable.symbols[rtn].alias);
                    opn1.offset = symbolTable.symbols[rtn].offset;
                    opn2.kind = INT;
                    opn2.const_int = 0;
                    result.kind = ID;
                    strcpy(result.id, T->Etrue);
                    T->code = genIR(NEQ, opn1, opn2, result);
                    T->code = merge(2, T->code, genGoto(T->Efalse));
                }
                T->width = 0;
                break;
            case RELOP: //�����ϵ������ʽ,2�������������������ʽ����
                T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
                Exp(T->ptr[0]);
                T->width = T->ptr[0]->width;
                Exp(T->ptr[1]);
                if (T->width < T->ptr[1]->width)
                    T->width = T->ptr[1]->width;
                opn1.kind = ID;
                strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                opn2.kind = ID;
                strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                result.kind = ID;
                strcpy(result.id, T->Etrue);
                if (strcmp(T->type_id, "<") == 0)
                    op = JLT;
                else if (strcmp(T->type_id, "<=") == 0)
                    op = JLE;
                else if (strcmp(T->type_id, ">") == 0)
                    op = JGT;
                else if (strcmp(T->type_id, ">=") == 0)
                    op = JGE;
                else if (strcmp(T->type_id, "==") == 0) {
                    op = EQ;
                } 
                else if (strcmp(T->type_id, "!=") == 0)
                    op = NEQ;
                T->code = genIR(op, opn1, opn2, result);
                T->code = merge(4, T->ptr[0]->code, T->ptr[1]->code, T->code, genGoto(T->Efalse));
                break;
            case AND:
            case OR:
                if (T->kind == AND)
                {
                    strcpy(T->ptr[0]->Etrue, newLabel());
                    strcpy(T->ptr[0]->Efalse, T->Efalse);
                }
                else
                {
                    strcpy(T->ptr[0]->Etrue, T->Etrue);
                    strcpy(T->ptr[0]->Efalse, newLabel());
                }
                strcpy(T->ptr[1]->Etrue, T->Etrue);
                strcpy(T->ptr[1]->Efalse, T->Efalse);
                T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
                boolExp(T->ptr[0]);
                T->width = T->ptr[0]->width;
                boolExp(T->ptr[1]);
                if (T->width < T->ptr[1]->width)
                    T->width = T->ptr[1]->width;
                if (T->kind == AND)
                    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
                else
                    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Efalse), T->ptr[1]->code);
                break;
            case NOT:
                strcpy(T->ptr[0]->Etrue, T->Efalse);
                strcpy(T->ptr[0]->Efalse, T->Etrue);
                boolExp(T->ptr[0]);
                T->code = T->ptr[0]->code;
                break;
            default:
                break;
        }
    }
}


void id_exp(struct ASTNode *T)
{
    int rtn, num, width;

    rtn = searchSymbolTable(T->type_id);
    if (rtn == -1)
        semantic_error(T->pos, T->type_id, "����δ����");
    if (symbolTable.symbols[rtn].flag == 'F')
        semantic_error(T->pos, T->type_id, "�Ǻ����������Ͳ�ƥ��");
    else
    {
        T->place = rtn; //��㱣������ڷ��ű��е�λ��
        T->code = NULL; //��ʶ������Ҫ����TAC
        T->type = symbolTable.symbols[rtn].type;
        T->offset = symbolTable.symbols[rtn].offset;
        T->width = 0; //δ��ʹ���µ�Ԫ
    }
}

void int_exp(struct ASTNode *T){
    struct opn opn1, opn2, result;
    T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset); //Ϊ����������һ����ʱ����
    T->type = INT;
    opn1.kind = INT;
    opn1.const_int = T->type_int;
    result.kind = ID;
    strcpy(result.id, symbolTable.symbols[T->place].alias);
    result.offset = symbolTable.symbols[T->place].offset;
    T->code = genIR(ASSIGNOP, opn1, opn2, result);
    T->width = 4;
}

void float_exp(struct ASTNode *T)
{
    int rtn, num, width;
    struct opn opn1, opn2, result;
    T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset); //Ϊ���㳣������һ����ʱ����
    T->type = FLOAT;
    opn1.kind = FLOAT;
    opn1.const_float = T->type_float;
    result.kind = ID;
    strcpy(result.id, symbolTable.symbols[T->place].alias);
    result.offset = symbolTable.symbols[T->place].offset;
    T->code = genIR(ASSIGNOP, opn1, opn2, result);
    T->width = 4;
}

void char_exp(struct ASTNode *T)
{
    int rtn, num, width;
    struct opn opn1, opn2, result;
    T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset); //Ϊ����������һ����ʱ����
    T->type = CHAR;
    opn1.kind = CHAR;
    opn1.const_char = T->type_char[1];
    result.kind = ID;
    strcpy(result.id, symbolTable.symbols[T->place].alias);
    result.offset = symbolTable.symbols[T->place].offset;
    T->code = genIR(ASSIGNOP, opn1, opn2, result);
    T->width = 1;
}

void string_exp(struct ASTNode *T)
{
    int rtn, num, width;
    struct opn opn1, opn2, result;
    T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset); //Ϊ����������һ����ʱ����
    T->type = STRING;
    opn1.kind = STRING;
    // strcpy(opn1.const_string, T->type_string);
    opn1.const_string = T->type_string;
    result.kind = ID;
    strcpy(result.id, symbolTable.symbols[T->place].alias);
    result.offset = symbolTable.symbols[T->place].offset;
    T->code = genIR(ASSIGNOP, opn1, opn2, result);
    // T->width = 1;
}

void assignop_exp(struct ASTNode *T)
{
    int rtn, num, width;
    struct opn opn1, opn2, result;
    if (T->ptr[0]->kind != ID && T->ptr[0]->kind != EXP_ELE && T->ptr[0]->kind != EXP_ARRAY)
    {
        semantic_error(T->pos, "", "��ֵ�����Ҫ��ֵ");
    }
    else
    {
        Exp(T->ptr[0]); //������ֵ�����н�Ϊ����
        T->ptr[1]->offset = T->offset;
        Exp(T->ptr[1]);

        if(T->ptr[0]->type != T->ptr[1]->type){
            semantic_error(T->pos,"", "��ֵ�����������Ͳ�ƥ��");
            return;
        }
        if (T->ptr[0]->type==CHAR && T->kind!=ASSIGNOP){
            semantic_error(T->pos,T->ptr[0]->type_id,"���ַ����ͱ��������ܲ�������");
            return;
        }
        if (T->ptr[1]->type==CHAR && T->kind!=ASSIGNOP){
            semantic_error(T->pos,T->ptr[1]->type_id,"���ַ����ͱ��������ܲ�������");
            return;
        }

        if (symbolTable.symbols[T->ptr[0]->place].type == INT) {
            symbolTable.symbols[T->ptr[0]->place].const_int = T->ptr[1]->type_int;
            // printf("%s: %d\n", T->ptr[0]->type_id, symbolTable.symbols[T->ptr[0]->place].const_int);
        }
            

        T->type = T->ptr[0]->type;
        T->width = T->ptr[1]->width;
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);

        if(T->ptr[1]->kind != EXP_ARRAY && T->ptr[1]->kind != EXP_ELE) {
            opn1.kind = ID;
            strcpy(opn1.id, symbolTable.symbols[T->ptr[1]->place].alias); //��ֵһ���Ǹ���������ʱ����
            opn1.offset = symbolTable.symbols[T->ptr[1]->place].offset;
        }
        else{
            opn1.kind=ID;
			char s[10];
			char str[80];
			strcpy (str,symbolTable.symbols[T->ptr[1]->place].alias);
			strcat (str," offset ");
			//ito(T->ptr[0]->offset,s,0);
            sprintf(s,"%d",T->ptr[1]->offset);
            //itoa(no++,s,10);
            //sprintf(s, "%d", no++);
			strcat (str,s);

			strcpy(opn1.id,str);
			opn1.offset=T->ptr[1]->offset;
        }

        if(T->ptr[0]->kind != EXP_ARRAY && T->ptr[0]->kind != EXP_ELE) {
            result.kind = ID;
            strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
            result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        }
        else{
            result.kind=ID;
			char s[10];
			char str[80];
			strcpy (str,symbolTable.symbols[T->ptr[0]->place].alias);
			strcat (str," offset ");
			//ito(T->ptr[0]->offset,s,0);
            sprintf(s,"%d",T->ptr[0]->offset);
            //itoa(no++,s,10);
            //sprintf(s, "%d", no++);
			strcat (str,s);

			strcpy(result.id,str);
			result.offset=T->ptr[0]->offset;
        }
        
        T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
    }
}

void other_assignop_exp(struct ASTNode *T) {
    int rtn, num, width;
    struct opn opn1, opn2, result;
    struct opn result2;
    if (T->ptr[0]->kind != ID)
    {
        semantic_error(T->pos, "", "��ֵ�����Ҫ��ֵ");
    }
    else{
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]);
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        Exp(T->ptr[1]);
        T->type = T->ptr[0]->type;
        T->width = T->ptr[1]->width;
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);

        opn1.kind = ID;
        strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias); //��ֵһ���Ǹ���������ʱ����
        opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;

        opn2.kind = ID;
        strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
        opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
        
        result.kind = ID;
        strcpy(result.id, newTemp());
        if (T->kind == PLUSASSIGNOP)
            T->code = merge(2, T->code, genIR(PLUS, opn1, opn2, result));
        else if(T->kind == MINUSASSIGNOP)
            T->code = merge(2, T->code, genIR(MINUS, opn1, opn2, result));
        else if(T->kind == STARASSIGNOP)
            T->code = merge(2, T->code, genIR(STAR, opn1, opn2, result));
        else if(T->kind == DIVASSIGNOP)
            T->code = merge(2, T->code, genIR(DIV, opn1, opn2, result));
        else
            T->code = merge(2, T->code, genIR(MOD, opn1, opn2, result));

        result2.kind = ID;
        strcpy(result2.id, symbolTable.symbols[T->ptr[0]->place].alias);
        result2.type = T->type;
        result2.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        T->code = merge(2, T->code, genIR(ASSIGNOP, result, opn2, result2));
    }
}

void auto_op_exp(struct ASTNode *T) {
    int rtn, num, width;
    struct opn opn1, opn2, result;
    struct opn result2;
    if (T->ptr[0]->kind != ID){
        semantic_error(T->pos, "", "��ֵ�����Ҫ��ֵ");
    }
    else {
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]);
        T->type = T->ptr[0]->type;
        T->width = T->ptr[0]->width;

        T->place = T->ptr[0]->place;

        opn1.kind = INT;
        opn1.const_int = 1;
        result.kind = ID;
        strcpy(result.id, newTemp());
        T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));

        opn1.kind = ID;
        strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
        result2.kind = ID;
        strcpy(result2.id, newTemp());
        if (T->kind == AUTOPLUS_L || T->kind == AUTOPLUS_R)
            T->code = merge(2, T->code, genIR(PLUS, opn1, result, result2));
        else if(T->kind == AUTOMINUS_L || T->kind == AUTOMINUS_R)
            T->code = merge(2, T->code, genIR(MINUS, opn1, result, result2));

        T->code = merge(2, T->code, genIR(ASSIGNOP, result2, opn2, opn1));
    }
}

void relop_exp(struct ASTNode *T)
{
    T->type = INT;
    T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
    Exp(T->ptr[0]);
    Exp(T->ptr[1]);
}

void args_exp(struct ASTNode *T)
{
    T->ptr[0]->offset = T->offset;
    Exp(T->ptr[0]);
    T->type = T->ptr[0]->type;
    T->width = T->ptr[0]->width;
    T->code = T->ptr[0]->code;
    if (T->ptr[1]) {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        Exp(T->ptr[1]);
        T->width += T->ptr[1]->width;
        T->code = merge(2, T->code, T->ptr[1]->code);
    }
}

// �������㣺�Ӽ��˳�
void op_exp(struct ASTNode *T)
{
    int rtn, num, width;
    struct opn opn1, opn2, result;
    T->ptr[0]->offset = T->offset;
    Exp(T->ptr[0]);
    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
    Exp(T->ptr[1]);
    //�ж�T->ptr[0]��T->ptr[1]�����Ƿ���ȷ�����ܸ�����������ɲ�ͬ��ʽ�Ĵ��룬��T��type��ֵ
    //������������Լ��㣬û�п��Ǵ��������
    if (T->ptr[0]->type == FLOAT || T->ptr[1]->type == FLOAT)
        T->type = FLOAT, T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
    else
        T->type = INT, T->width = T->ptr[0]->width + T->ptr[1]->width + 2;
    T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
    
    opn1.kind = ID;
    strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
    opn1.type = T->ptr[0]->type;
    opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;

    opn2.kind = ID;
    strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
    opn2.type = T->ptr[1]->type;
    opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;

    result.kind = ID;
    strcpy(result.id, symbolTable.symbols[T->place].alias);
    result.type = T->type;
    result.offset = symbolTable.symbols[T->place].offset;

    T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(T->kind, opn1, opn2, result));
    if (T->type == INT) {
        T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
    }
    else if(T->type == FLOAT) {
        T->width = T->ptr[0]->width + T->ptr[1]->width + 8;
    }
    else if(T->type == CHAR) {
        T->width = T->ptr[0]->width + T->ptr[1]->width + 1;
    }
}

void func_call_exp(struct ASTNode *T)
{
    int rtn, num, width;
    int count = 0, param_num;
    struct ASTNode *T0;
    struct ASTNode *T1;
    struct opn opn1, opn2, result;
    rtn = searchSymbolTable(T->type_id);
    if (rtn == -1)
    {
        semantic_error(T->pos, T->type_id, "����δ����");
        return;
    }
    if (symbolTable.symbols[rtn].flag != 'F')
    {
        semantic_error(T->pos, T->type_id, "����һ������");
        return;
    }
    T->type = symbolTable.symbols[rtn].type;
    // ��ź�������ֵ�ĵ����ֽ���
    if(T->type == INT) {
        width = 4;
    }
    else if(T->type == FLOAT) {
        width = 8;
    }
    else if(T->type == CHAR) {
        width = 1;
    }
    if (T->ptr[0]) {
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]);                      //��������ʵ�α��ʽ��ֵ��������
        T->width = T->ptr[0]->width + width; //�ۼ��ϼ���ʵ��ʹ����ʱ�����ĵ�Ԫ��
        T->code = T->ptr[0]->code;
    }
    else{
        T->width = width;
        T->code = NULL;
    }
    T1 = T->ptr[0];
    while(T1 != NULL) {
        count++;
        num = rtn;
        if(symbolTable.symbols[rtn+count].flag == 'P' && T1->type != symbolTable.symbols[rtn+count].type){
            semantic_error(T1->pos, "", "ʵ�����β����Ͳ���");
        }
        T1 = T1->ptr[1];
    }
    param_num = symbolTable.symbols[rtn].paramnum;
    if (count > param_num) {
        semantic_error(T->pos, "", "������������̫��");
    }
    else if(count < param_num) {
        semantic_error(T->pos, "", "������������̫��");
    }
    else
        match_param(rtn, T->ptr[0]); //�������Բ�����ƥ��
    //��������б���м����
    T0 = T->ptr[0];
    while (T0){
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
        result.offset = symbolTable.symbols[T0->ptr[0]->place].offset;
        T->code = merge(2, T->code, genIR(ARG, opn1, opn2, result));
        T0 = T0->ptr[1];
    }
    T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->width - width);
    opn1.kind = ID;
    strcpy(opn1.id, T->type_id); //���溯����
    opn1.offset = rtn;           //����offset���Ա��溯���������,��Ŀ���������ʱ���ܻ�ȡ��Ӧ��Ϣ
    result.kind = ID;
    strcpy(result.id, symbolTable.symbols[T->place].alias);
    result.offset = symbolTable.symbols[T->place].offset;
    T->code = merge(2, T->code, genIR(CALL, opn1, opn2, result)); //���ɺ��������м����
}

void not_exp(struct ASTNode *T)
{
    T->type = INT;
    T->ptr[0]->offset = T->offset;
    Exp(T->ptr[0]);
}

void unminus_exp(struct ASTNode *T)
{
    T->type = T->ptr[0]->type;
    T->ptr[0]->offset = T->offset;
    Exp(T->ptr[0]);
}

void exp_array(struct ASTNode *T){
    int rtn;
    struct ASTNode *T0;
    rtn=searchSymbolTable(T->type_id);
	if (rtn==-1)
        semantic_error(T->pos,T->type_id, "����δ����");
    else if(symbolTable.symbols[rtn].flag != 'A')
        semantic_error(T->pos,T->type_id, "������������");
    else {
        int index = 0;
		T0 = T->ptr[0];
        T->place = rtn;       //��㱣������ڷ��ű��е�λ��
        T->code = NULL;       //��ʶ������Ҫ����TAC
        T->type = symbolTable.symbols[rtn].type; // ���ID����
        // printf("%d\n", compute_width0(T->ptr[0], symbolTable.symbols[rtn].array, 0));
        T->offset = (T->type == INT ? 4 : (T->type == FLOAT ? 8 : 1)) * compute_width0(T->ptr[0], symbolTable.symbols[rtn].array, 0); // �ڴ���ƫ��ֵ
        // printf("%d\n", T->offset);
        T->width = 0;   //δ��ʹ���µ�Ԫ
		while(T0->kind==ARRAY_LIST){
			Exp(T0->ptr[0]);
			if(T0->ptr[0]->type != INT){
                semantic_error(T->pos,"", "����ά����Ҫ����");
                break;                      
            }
			if(index == 8){
				semantic_error(T->pos,"", "����ά�ȳ������ֵ");
				break;
			}
			else if(symbolTable.symbols[rtn].array[index] <= T0->type_int){
				semantic_error(T->pos,"", "����ά�ȳ�������ֵ");
				break;
            }
			index++;
			T0=T0->ptr[1];
		}
        //�������һά
		if(T0->kind == ARRAY_LAST){
			Exp(T0->ptr[0]);
            if(T0->ptr[0]->type!=INT){
                semantic_error(T->pos,"", "����ά����Ҫ����");                     
            }
            else if(index == 8){
                semantic_error(T->pos,"", "����ά�ȳ������ֵ");
            }
            if(T0->ptr[0]->kind == ID){
                if(symbolTable.symbols[rtn].array[index] <= symbolTable.symbols[T0->ptr[0]->place].const_int){
                    semantic_error(T->pos,"", "����ά�ȳ�������ֵ");
                }
            }
            else{
                if(symbolTable.symbols[rtn].array[index] <= T0->ptr[0]->type_int){
                    semantic_error(T->pos,"", "����ά�ȳ�������ֵ");
                    }
            }   
            if(symbolTable.symbols[rtn].array[index+1] > 0 && index < 7){
                semantic_error(T->pos,"", "����ά�Ȳ����㶨��ֵ");
            }
	    }
    }
}

void exp_ele(struct ASTNode *T)
{
    int rtn,flag=0;
    int rtn2;

    Exp(T->ptr[0]);
    if (symbolTable.symbols[T->ptr[0]->place].type != STRUCT) {
        semantic_error(T->pos, symbolTable.symbols[T->ptr[0]->place].name, "�������ǽṹ");
    }
    rtn = searchSymbolTable(symbolTable.symbols[T->ptr[0]->place].struct_name);
    rtn2 = searchSymbolTable(symbolTable.symbols[T->ptr[0]->place].name);
    if(rtn == -1) {
        semantic_error(T->pos, symbolTable.symbols[T->ptr[0]->place].struct_name, "�ṹδ����");
        return;
    }

    T->place = rtn2;

    do{
        rtn++;
        if(!strcmp(symbolTable.symbols[rtn].name, T->type_id)) {
            flag = 1;
            break;
        }
    } while(rtn < symbolTable.index && symbolTable.symbols[rtn].flag == 'M');

    if(!flag) {
        semantic_error(T->pos, T->type_id, "���Ǹýṹ�ĳ�Ա����");
    }
    else{
        T->type = symbolTable.symbols[rtn].type;
        T->offset = symbolTable.symbols[rtn].offset;
    }
}