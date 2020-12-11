#include "def.h"

extern int LEV;   //���
extern int func_size; //�����Ļ��¼��С

extern struct symboltable symbolTable;
extern struct symbol_scope_begin symbol_scope_TX;

char break_label[30];
char continue_label[30];
char case_temp[30];
char case_label[30];
char array_name[30];
char struct_name[33];

int struct_flag = 0;
int array_index = 0;
int struct_var_flag = 0;

void ext_var_list(struct ASTNode *T)
{ //��������б�
    int rtn, num = 1;
    if (T) {
        switch (T->kind)
        {
        case EXT_DEC_LIST:
            T->ptr[0]->type = T->type;                //�������������´��ݱ������
            T->ptr[0]->offset = T->offset;            //�ⲿ������ƫ�������´���
            if (T->ptr[1]) {
                T->ptr[1]->type = T->type;               //�������������´��ݱ������
                T->ptr[1]->offset = T->offset + T->width; //�ⲿ������ƫ�������´���
                T->ptr[1]->width = T->width;
            }
            ext_var_list(T->ptr[0]);
            if(T->ptr[1]) {
                ext_var_list(T->ptr[1]);
                T->num = T->ptr[1]->num + 1;
            }
            break;
        case ID:
            rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'V', T->offset); //���һ��������
            if (rtn == -1)
                semantic_error(T->pos, T->type_id, "�����ظ�����");
            else
                T->place = rtn;
            T->num = 1;
            break;
        case ARRAY_DEC:
            array_index = 0;
            strcpy(array_name, T->type_id);
            rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'A', T->offset);//ƫ����Ϊ0
			if (rtn == -1) {
                semantic_error(T->pos,T->type_id, "�����ظ�����");
            }
            else {
                T->place = rtn;
                T->num = compute_width(T->ptr[0]);
                ext_var_list(T->ptr[0]);
            }
			break;
        case ARRAY_LIST:
            rtn = searchSymbolTable(array_name);
            if(rtn == -1) {
                semantic_error(T->pos, array_name, "����δ����");
            }
            if(T->ptr[0]->type != INT) {
                semantic_error(T->pos, "", "���鶨��ά����Ҫ������");
            }
            else{
                symbolTable.symbols[rtn].array[array_index] = T->ptr[0]->type_int;
                array_index++;
                T->num = compute_width(T->ptr[1]);
                ext_var_list(T->ptr[1]);
            }
            break;
        case ARRAY_LAST:
            rtn = searchSymbolTable(array_name);
            if(rtn == -1) {
                semantic_error(T->pos, array_name, "����δ����");
            }
            if(T->ptr[0]->type != INT) {
                semantic_error(T->pos, "", "���鶨��ά����Ҫ������");
            }
            else{
                symbolTable.symbols[rtn].array[array_index] = T->ptr[0]->type_int;
            }
            break;
        default:
            break;
        }
    }
}

void ext_def_list(struct ASTNode *T)
{
    if (!T->ptr[0])
        return;

    // �������֮ǰ������ƫ�Ƶ�ַ
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]); //�����ⲿ�����б��еĵ�һ��
    // ֮��ϲ� code
    T->code = T->ptr[0]->code;

    // ��Ϊ��
    if (T->ptr[1])
    {
        T->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]); //���ʸ��ⲿ�����б��е������ⲿ����
        T->code = merge(2, T->code, T->ptr[1]->code);
    }
}

void ext_var_def(struct ASTNode *T) {
    if (!strcmp(T->ptr[0]->type_id, "int"))
    {
        T->type = T->ptr[1]->type = INT;
        T->ptr[1]->width = 4;
    }
    if (!strcmp(T->ptr[0]->type_id, "float"))
    {
        T->type = T->ptr[1]->type = FLOAT;
        T->ptr[1]->width = 8;
    }
    if (!strcmp(T->ptr[0]->type_id, "char"))
    {
        T->type = T->ptr[1]->type = CHAR;
        T->ptr[1]->width = 1;
    }
    if (!strcmp(T->ptr[0]->type_id, "string"))
    {
        T->type = T->ptr[1]->type = STRING;
    }
    T->ptr[1]->offset = T->offset; //����ⲿ������ƫ�������´���
    // T->ptr[1]->width = T->type == INT ? 4 : 8;            //��һ�������Ŀ�����´���
    ext_var_list(T->ptr[1]);                        //�����ⲿ����˵���еı�ʶ������
    T->width = (T->ptr[1]->width) * T->ptr[1]->num; //��������ⲿ����˵���Ŀ��
    T->code = NULL;                                 //����ٶ��ⲿ������֧�ֳ�ʼ��
}

void func_def(struct ASTNode *T)
{
    if (!strcmp(T->ptr[0]->type_id, "int"))
    {
        T->ptr[1]->type = INT;
    }
    if (!strcmp(T->ptr[0]->type_id, "float"))
    {
        T->ptr[1]->type = FLOAT;
    }
    if (!strcmp(T->ptr[0]->type_id, "char"))
    {
        T->ptr[1]->type = CHAR;
    }
    if (!strcmp(T->ptr[0]->type_id, "string"))
    {
        T->ptr[1]->type = STRING;
    }
    //��д����������Ϣ�����ű�
    // T->ptr[1]->type = !strcmp(T->ptr[0]->type_id, "int") ? INT : FLOAT; //��ȡ�������������͵����������������Ľ��
    T->width = 0;                  //�����Ŀ������Ϊ0��������ⲿ�����ĵ�ַ�������Ӱ��
    T->offset = DX;                //���þֲ������ڻ��¼�е�ƫ������ֵ
    semantic_Analysis(T->ptr[1]);  //���������Ͳ�����㲿�֣����ﲻ�����üĴ������ݲ���
    T->offset += T->ptr[1]->width; //���βε�Ԫ����޸ĺ����ֲ���������ʼƫ����
    T->ptr[2]->offset = T->offset;
    strcpy(T->ptr[2]->Snext, newLabel()); //���������ִ�н������λ������
    semantic_Analysis(T->ptr[2]);         //����������
    //������¼��С,����offset���Դ�ŵ��ǻ��¼��С������ƫ��
    symbolTable.symbols[T->ptr[1]->place].offset = T->offset + T->ptr[2]->width;
    T->code = merge(3, T->ptr[1]->code, T->ptr[2]->code, genLabel(T->ptr[2]->Snext)); //������Ĵ�����Ϊ�����Ĵ���
}

void func_dec(struct ASTNode *T)
{
    int rtn;
    struct opn opn1, opn2, result;
    rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'F', 0); //���������������з��䵥Ԫ��ƫ����Ϊ0
    if (rtn == -1)
    {
        semantic_error(T->pos, T->type_id, "�����ظ�����");
        return;
    }
    else
        T->place = rtn;
    result.kind = ID;
    strcpy(result.id, T->type_id);
    result.offset = rtn;
    T->code = genIR(FUNCTION, opn1, opn2, result); //�����м���룺FUNCTION ������
    T->offset = DX;                                //������ʽ�����ڻ��¼�е�ƫ������ֵ
    if (T->ptr[0])
    { //�ж��Ƿ��в���
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //�����������б�
        T->width = T->ptr[0]->width;
        symbolTable.symbols[rtn].paramnum = T->ptr[0]->num;
        T->code = merge(2, T->code, T->ptr[0]->code); //���Ӻ������Ͳ�����������
    }
    else {
        symbolTable.symbols[rtn].paramnum = 0;
        T->width = 0;
    }
}

void ext_struct_def(struct ASTNode *T)
{
    //��д����������Ϣ�����ű�
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]); //���������Ͳ�����㲿�֣����ﲻ�����üĴ������ݲ���
    T->code = T->ptr[0]->code;
}

void struct_def(struct ASTNode *T)
{
    int rtn;
    struct opn opn1, opn2, result;

    T->width = 0;   //�����Ŀ������Ϊ0��������ⲿ�����ĵ�ַ�������Ӱ��
    T->offset = DX; //���þֲ������ڻ��¼�е�ƫ������ֵ
    T->type = STRUCT;
    rtn = fillSymbolTable(T->ptr[0]->type_id, newAlias(), LEV, STRUCT, 'S', 0); //���������������з��䵥Ԫ��ƫ����Ϊ0
    if (rtn == -1)
    {
        semantic_error(T->pos, T->ptr[0]->type_id, "�ṹ���ظ�����");
        return;
    }
    else
        T->place = rtn;

    T->ptr[1]->offset = T->offset;
    struct_flag = 1;
    semantic_Analysis(T->ptr[1]);
    struct_flag = 0;
}

void struct_dec(struct ASTNode *T)
{
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]);
    T->width = T->ptr[1]->width;
}

// void array_dec(struct ASTNode *T){

// }

void param_list(struct ASTNode *T)
{
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]);
    if (T->ptr[1])
    {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]);
        T->num = T->ptr[0]->num + T->ptr[1]->num;             //ͳ�Ʋ�������
        T->width = T->ptr[0]->width + T->ptr[1]->width;       //�ۼӲ�����Ԫ���
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code); //���Ӳ�������
    }
    else
    {
        T->num = T->ptr[0]->num;
        T->width = T->ptr[0]->width;
        T->code = T->ptr[0]->code;
    }
}

void param_dec(struct ASTNode *T)
{
    int rtn;
    struct opn opn1, opn2, result;
    rtn = fillSymbolTable(T->ptr[1]->type_id, newAlias(), 1, T->ptr[0]->type, 'P', T->offset);
    if (rtn == -1)
        semantic_error(T->ptr[1]->pos, T->ptr[1]->type_id, "�������ظ�����");
    else
        T->ptr[1]->place = rtn;
    T->num = 1;                                //������������ĳ�ʼֵ
    // T->width = T->ptr[0]->type == INT ? 4 : (T->ptr[0]->type == FLOAT ? 8 : 1); //�������
    if (T->ptr[0]->type == INT) {
        T->width = 4;
    }
    if (T->ptr[0]->type == FLOAT) {
        T->width = 8;
    }
    if (T->ptr[0]->type == CHAR) {
        T->width = 1;
    }
    result.kind = ID;
    strcpy(result.id, symbolTable.symbols[rtn].alias);
    result.offset = T->offset;
    T->code = genIR(PARAM, opn1, opn2, result); //���ɣ�FUNCTION ������
}

void comp_stm(struct ASTNode *T)
{
    LEV++;
    //���ò�ż�1�����ұ���ò�ֲ������ڷ��ű��е���ʼλ����symbol_scope_TX
    symbol_scope_TX.TX[symbol_scope_TX.top++] = symbolTable.index;
    T->width = 0;
    T->code = NULL;
    if (T->ptr[0])
    {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //����ò�ľֲ�����DEF_LIST
        T->width += T->ptr[0]->width;
        T->code = T->ptr[0]->code;
    }
    if (T->ptr[1])
    {
        T->ptr[1]->offset = T->offset + T->width;
        strcpy(T->ptr[1]->Snext, T->Snext); //S.next�������´���
        semantic_Analysis(T->ptr[1]);       //�����������������
        T->width += T->ptr[1]->width;
        T->code = merge(2, T->code, T->ptr[1]->code);
    }
    // prn_symbol();                                                  //c���˳�һ���������ǰ��ʾ�ķ��ű�
    LEV--;                                                         //��������䣬��ż�1
    symbolTable.index = symbol_scope_TX.TX[--symbol_scope_TX.top]; //ɾ�����������еķ���
}

void def_list(struct ASTNode *T)
{
    T->code = NULL;
    if (T->ptr[0])
    {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //����һ���ֲ���������
        T->code = T->ptr[0]->code;
        T->width = T->ptr[0]->width;
    }
    if (T->ptr[1])
    {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]); //����ʣ�µľֲ���������
        T->code = merge(2, T->code, T->ptr[1]->code);
        T->width += T->ptr[1]->width;
    }
}

void var_def(struct ASTNode *T)
{
    int rtn, num, width;
    struct ASTNode *T0;
    struct opn opn1, opn2, result;
    T->code = NULL;
    if (!strcmp(T->ptr[0]->type_id, "int"))
    {
        T->ptr[1]->type = INT;
        width = 4;
    }
    else if (!strcmp(T->ptr[0]->type_id, "float"))
    {
        T->ptr[1]->type = FLOAT;
        width = 8;
    }
    else if (!strcmp(T->ptr[0]->type_id, "char"))
    {
        T->ptr[1]->type = CHAR;
        width = 1;
    }
    else if (!strcmp(T->ptr[0]->type_id, "string"))
    {
        T->ptr[1]->type = STRING;
    }
    else{
        T->ptr[1]->type = STRUCT;
        strcpy(struct_name, T->ptr[0]->ptr[0]->type_id);
        struct_var_flag = 1;
    }
    // T->ptr[1]->type = !strcmp(T->ptr[0]->type_id, "int") ? INT : FLOAT; //ȷ���������и���������
    T0 = T->ptr[1]; //T0Ϊ�������б�������ָ�룬��ID��ASSIGNOP�����ڵǼǵ����ű���Ϊ�ֲ�����
    num = 0;
    T0->offset = T->offset;
    T->width = 0;
    // width = T->ptr[1]->type == INT ? 4 : 8; //һ���������
    while (T0)
    { //��������DEC_LIST���
        num++;
        T0->ptr[0]->type = T0->type; //�����������´���
        if (T0->ptr[1])
            T0->ptr[1]->type = T0->type;

        T0->ptr[0]->offset = T0->offset; //�����������´���
        if (T0->ptr[1])
            T0->ptr[1]->offset = T0->offset + width;
        if (T0->ptr[0]->kind == ID)
        {
            if(!struct_flag)
                rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
            else {
                rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV+1, T0->ptr[0]->type, 'M', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
            }
            if (rtn == -1)
                semantic_error(T0->ptr[0]->pos, T0->ptr[0]->type_id, "�����ظ�����");
            else
                T0->ptr[0]->place = rtn;
            T->width += width;
        }
        else if (T0->ptr[0]->kind == ASSIGNOP)
        {
            // rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
             if(!struct_flag)
                rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
            else {
                rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'M', T->offset + T->width); //�˴�ƫ����δ���㣬��ʱΪ0
                // struct_flag = 0;
            }
            if (rtn == -1)
                semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "�����ظ�����");
            else
            {
                T0->ptr[0]->place = rtn;
                T0->ptr[0]->ptr[1]->offset = T->offset + T->width + width;
                Exp(T0->ptr[0]->ptr[1]);
                opn1.kind = ID;
                strcpy(opn1.id, symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
                T->code = merge(3, T->code, T0->ptr[0]->ptr[1]->code, genIR(ASSIGNOP, opn1, opn2, result));
            }
            T->width += width + T0->ptr[0]->ptr[1]->width;
        }
        T0 = T0->ptr[1];
    }
}

void stm_list(struct ASTNode *T)
{
    if (!T->ptr[0])
    {
        T->code = NULL;
        T->width = 0;
        return;
    }              //���������
    if (T->ptr[1]) //2������������ӣ������±����Ϊ��һ���������󵽴��λ��
        strcpy(T->ptr[0]->Snext, newLabel());
    else //������н���һ����䣬S.next�������´���
        strcpy(T->ptr[0]->Snext, T->Snext);
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]);
    T->code = T->ptr[0]->code;
    T->width = T->ptr[0]->width;
    if (T->ptr[1])
    { //2�������������,S.next�������´���
        strcpy(T->ptr[1]->Snext, T->Snext);
        T->ptr[1]->offset = T->offset; //˳��ṹ����Ԫ��ʽ
                                       //                  T->ptr[1]->offset=T->offset+T->ptr[0]->width; //˳��ṹ˳����䵥Ԫ��ʽ
        semantic_Analysis(T->ptr[1]);
        //�����е�1��Ϊ���ʽ��䣬������䣬�������ʱ����2��ǰ����Ҫ���
        if (T->ptr[0]->kind == RETURN || T->ptr[0]->kind == EXP_STMT || T->ptr[0]->kind == COMP_STM)
            T->code = merge(2, T->code, T->ptr[1]->code);
        else
            T->code = merge(3, T->code, genLabel(T->ptr[0]->Snext), T->ptr[1]->code);
        if (T->ptr[1]->width > T->width)
            T->width = T->ptr[1]->width; //˳��ṹ����Ԫ��ʽ
                                         //                        T->width+=T->ptr[1]->width;//˳��ṹ˳����䵥Ԫ��ʽ
    }
}

void if_then(struct ASTNode *T)
{
    strcpy(T->ptr[0]->Etrue, newLabel()); //��������������ת��λ��
    strcpy(T->ptr[0]->Efalse, T->Snext);
    T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
    boolExp(T->ptr[0]);
    T->width = T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, T->Snext);
    semantic_Analysis(T->ptr[1]); //if�Ӿ�
    if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
}

void if_then_else(struct ASTNode *T)
{
    strcpy(T->ptr[0]->Etrue, newLabel()); //��������������ת��λ��
    strcpy(T->ptr[0]->Efalse, newLabel());
    T->ptr[0]->offset = T->ptr[1]->offset = T->ptr[2]->offset = T->offset;
    boolExp(T->ptr[0]); //������Ҫ��������·���봦��
    T->width = T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, T->Snext);
    semantic_Analysis(T->ptr[1]); //if�Ӿ�
    if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
    strcpy(T->ptr[2]->Snext, T->Snext);
    semantic_Analysis(T->ptr[2]); //else�Ӿ�
    if (T->width < T->ptr[2]->width)
        T->width = T->ptr[2]->width;
    T->code = merge(6, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code,
                    genGoto(T->Snext), genLabel(T->ptr[0]->Efalse), T->ptr[2]->code);
}

void while_dec(struct ASTNode *T)
{
    strcpy(T->ptr[0]->Etrue, newLabel()); //�ӽ��̳����Եļ���
    strcpy(T->ptr[0]->Efalse, T->Snext);
    T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
    boolExp(T->ptr[0]); //ѭ��������Ҫ��������·���봦��
    T->width = T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, newLabel());
    strcpy(break_label, T->Snext);
    strcpy(continue_label, T->ptr[1]->Snext);
    semantic_Analysis(T->ptr[1]); //ѭ����
    if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
    T->code = merge(5, genLabel(T->ptr[1]->Snext), T->ptr[0]->code,
                    genLabel(T->ptr[0]->Etrue), T->ptr[1]->code, 
                    genGoto(T->ptr[1]->Snext));
}

void for_stmt(struct ASTNode *T) {
    LEV++;
    // ����ѭ����ʼ���
    T->ptr[0]->offset = T->offset;
    T->ptr[0]->ptr[0]->offset = T->ptr[0]->offset;
    Exp(T->ptr[0]->ptr[0]);
    T->ptr[0]->width = T->ptr[0]->ptr[0]->width;
    // ����ѭ������
    strcpy(T->ptr[0]->ptr[1]->Etrue, newLabel()); //�ӽ��̳����Եļ���
    strcpy(T->ptr[0]->ptr[1]->Efalse, T->Snext);
    T->ptr[0]->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
    boolExp(T->ptr[0]->ptr[1]);
    if(T->ptr[0]->width < T->ptr[0]->ptr[1]->width)
        T->ptr[0]->width = T->ptr[0]->ptr[1]->width;
    // ѭ����
    strcpy(T->ptr[1]->Snext, newLabel());
    strcpy(break_label, T->Snext);
    strcpy(continue_label, T->ptr[1]->Snext);
    semantic_Analysis(T->ptr[1]);
    // �Զ���������
    T->ptr[0]->ptr[2]->offset = T->ptr[0]->offset + T->ptr[0]->width;
    strcpy(T->ptr[0]->ptr[2]->Snext, newLabel());
    Exp(T->ptr[0]->ptr[2]);
    if(T->ptr[0]->width < T->ptr[0]->ptr[2]->width)
        T->ptr[0]->width = T->ptr[0]->ptr[2]->width;
    T->width = T->ptr[0]->width >= T->ptr[1]->width ? T->ptr[0]->width : T->ptr[1]->width;
    T->code = merge(8, T->ptr[0]->ptr[0]->code, 
                    genLabel(T->ptr[0]->ptr[2]->Snext), 
                    T->ptr[0]->ptr[1]->code,
                    genLabel(T->ptr[0]->ptr[1]->Etrue),
                    T->ptr[1]->code,
                    genLabel(T->ptr[1]->Snext),
                    T->ptr[0]->ptr[2]->code,
                    genGoto(T->ptr[0]->ptr[2]->Snext));
}

void break_dec(struct ASTNode *T) {
    T->code = merge(2, T->code, genGoto(break_label));
}

void continue_dec(struct ASTNode *T) {
    T->code = merge(2, T->code, genGoto(continue_label));
}

void exp_stmt(struct ASTNode *T)
{
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]);
    T->code = T->ptr[0]->code;
    T->width = T->ptr[0]->width;
}

void return_dec(struct ASTNode *T)
{
    int rtn, num;
    struct opn opn1, opn2, result;
    if (T->ptr[0])
    {
        T->ptr[0]->offset = T->offset;
        Exp(T->ptr[0]);
        num = symbolTable.index;
        do
            num--;
        while (symbolTable.symbols[num].flag != 'F');
        if (T->ptr[0]->type != symbolTable.symbols[num].type)
        {
            semantic_error(T->pos, "����ֵ���ʹ���", "");
            T->width = 0;
            T->code = NULL;
            return;
        }
        T->width = T->ptr[0]->width;
        result.kind = ID;
        strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
        result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
        T->code = merge(2, T->ptr[0]->code, genIR(RETURN, opn1, opn2, result));
    }
    else
    {
        T->width = 0;
        result.kind = 0;
        T->code = genIR(RETURN, opn1, opn2, result);
    }
}

void switch_stmt(struct ASTNode *T){
   
    // switchѡ����ʽ
    T->ptr[0]->offset = T->offset;
    Exp(T->ptr[0]);
    T->width = T->ptr[0]->width;
    strcpy(T->Snext, newLabel());

    // case���
    strcpy(case_temp, symbolTable.symbols[T->ptr[0]->place].name);
    strcpy(break_label, T->Snext);
    strcpy(case_label, newLabel());
    semantic_Analysis(T->ptr[1]);
    T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
}

void case_stmt(struct ASTNode *T) {
    struct ASTNode *left = (struct ASTNode *)malloc(sizeof(struct ASTNode));
    struct ASTNode *right = (struct ASTNode *)malloc(sizeof(struct ASTNode));

    left->kind = ID;
    strcpy(left->type_id, case_temp);
    if(T->ptr[0]->type == INT) {
        right->kind = INT;
        right->type_int = T->ptr[0]->type_int;
    }
    else if(T->ptr[0]->type == CHAR) {
        right->kind = CHAR;
        strcpy(right->type_char, T->ptr[0]->type_char);
    }
    T->ptr[0]->ptr[0] = left;
    T->ptr[0]->ptr[1] = right;
    T->ptr[0]->kind = RELOP;
    strcpy(T->ptr[0]->type_id, "==");

    strcpy(T->ptr[0]->Etrue, newLabel()); //��������������ת��λ��
    strcpy(T->ptr[0]->Efalse, T->Snext);
    T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
    boolExp(T->ptr[0]);
    T->width = T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, T->Snext);
    semantic_Analysis(T->ptr[1]); //if�Ӿ�
    if (T->width < T->ptr[1]->width)
        T->width = T->ptr[1]->width;
    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
}