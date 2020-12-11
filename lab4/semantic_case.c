#include "def.h"

extern int LEV;   //层号
extern int func_size; //函数的活动记录大�?

extern struct symboltable symbolTable;
extern struct symbol_scope_begin symbol_scope_TX;

char break_label[30];
char continue_label[30];
char case_temp[30];
char case_label[30];
char array_name[30];
char struct_name[33];

int struct_flag = 0;
int struct_dec_flag = 0;
int struct_width = 0;
int array_index = 0;
int struct_var_flag = 0;

int array_flag = 0;

void ext_var_list(struct ASTNode *T)
{ //处理变量列表
    int rtn, num = 1;
    if (T) {
        switch (T->kind)
        {
        case EXT_DEC_LIST:
            T->ptr[0]->type = T->type;                //将类型属性向下传递变量结�?
            T->ptr[0]->offset = T->offset;            //外部变量的偏移量向下传�?
            if (T->ptr[1]) {
                T->ptr[1]->type = T->type;               //将类型属性向下传递变量结�?
                T->ptr[1]->offset = T->offset + T->width; //外部变量的偏移量向下传�?
                T->ptr[1]->width = T->width;
            }
            ext_var_list(T->ptr[0]);
            T->num = T->ptr[0]->num;
            if(T->ptr[1]) {
                ext_var_list(T->ptr[1]);
                T->num += T->ptr[1]->num;
            }
            break;
        case ID:
            rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'V', T->offset); //最后一个变量名
            if (rtn == -1)
                semantic_error(T->pos, T->type_id, "变量重复定义");
            else
                T->place = rtn;
            T->num = 1;
            break;
        case ARRAY_DEC:
            array_index = 0;
            strcpy(array_name, T->type_id);
            rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'A', T->offset);//偏移量为0
			if (rtn == -1) {
                semantic_error(T->pos,T->type_id, "变量重复定义");
            }
            else {
                T->place = rtn;
                T->num = compute_width(T->ptr[0]);
                // printf("%d\n", T->num);
                ext_var_list(T->ptr[0]);
                array_flag = 1;
            }
			break;
        case ARRAY_LIST:
            rtn = searchSymbolTable(array_name);
            if(rtn == -1) {
                semantic_error(T->pos, array_name, "数组未定�?");
            }
            if(T->ptr[0]->type != INT) {
                semantic_error(T->pos, "", "数组定义维数需要用整型");
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
                semantic_error(T->pos, array_name, "数组未定�?");
            }
            if(T->ptr[0]->type != INT) {
                semantic_error(T->pos, "", "数组定义维数需要用整型");
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

    // 语义分析之前先设置偏移地址
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]); //访问外部定义列表中的第一�?
    // 之后合并 code
    T->code = T->ptr[0]->code;

    // 可为�?
    if (T->ptr[1])
    {
        T->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
        // printf("offset-0:%d, width:%d, offset-1:%d\n", 
        //         T->ptr[0]->offset, 
        //         T->ptr[0]->width,
        //         T->ptr[1]->offset);
        semantic_Analysis(T->ptr[1]); //访问该外部定义列表中的其它外部定�?
        T->code = merge(2, T->code, T->ptr[1]->code);
    }
}

void ext_var_def(struct ASTNode *T) {
    if (!strcmp(T->ptr[0]->type_id, "int"))
    {
        T->type = T->ptr[1]->type = INT;
        T->ptr[1]->width = 4;
    }
    else if (!strcmp(T->ptr[0]->type_id, "float"))
    {
        T->type = T->ptr[1]->type = FLOAT;
        T->ptr[1]->width = 8;
    }
    else if (!strcmp(T->ptr[0]->type_id, "char"))
    {
        T->type = T->ptr[1]->type = CHAR;
        T->ptr[1]->width = 1;
    }
    else if (!strcmp(T->ptr[0]->type_id, "string"))
    {
        T->type = T->ptr[1]->type = STRING;
    }
    else{
        T->type = T->ptr[1]->type = STRUCT;
        strcpy(struct_name, T->ptr[0]->ptr[0]->type_id);
        struct_var_flag = 1;
    }
    T->ptr[1]->offset = T->offset; //这个外部变量的偏移量向下传�?
    // T->ptr[1]->width = T->type == INT ? 4 : 8;            //将一个变量的宽度向下传�?
    ext_var_list(T->ptr[1]);                        //处理外部变量说明中的标识符序�?
    // printf("num:%d, width:%d\n", T->ptr[1]->num, T->ptr[1]->width);
    T->width = (T->ptr[1]->width) * (T->ptr[1]->num); //计算这个外部变量说明的宽�?
    T->code = NULL;                                 //这里假定外部变量不支持初始化
}

void func_def(struct ASTNode *T)
{
    if (!strcmp(T->ptr[0]->type_id, "int"))
    {
        T->ptr[1]->type = INT;
    }
    else if (!strcmp(T->ptr[0]->type_id, "float"))
    {
        T->ptr[1]->type = FLOAT;
    }
    else if (!strcmp(T->ptr[0]->type_id, "char"))
    {
        T->ptr[1]->type = CHAR;
    }
    else if (!strcmp(T->ptr[0]->type_id, "string"))
    {
        T->ptr[1]->type = STRING;
    }
    //填写函数定义信息到符号表
    // T->ptr[1]->type = !strcmp(T->ptr[0]->type_id, "int") ? INT : FLOAT; //获取函数返回类型送到含函数名、参数的结点
    T->width = 0;                  //函数的宽度设置为0，不会对外部变量的地址分配产生影响
    T->offset = DX;                //设置局部变量在活动记录中的偏移量初�?
    semantic_Analysis(T->ptr[1]);  //处理函数名和参数结点部分，这里不考虑用寄存器传递参�?
    T->offset += T->ptr[1]->width; //用形参单元宽度修改函数局部变量的起始偏移�?
    T->ptr[2]->offset = T->offset;
    strcpy(T->ptr[2]->Snext, newLabel()); //函数体语句执行结束后的位置属�?
    semantic_Analysis(T->ptr[2]);         //处理函数体结�?
    //计算活动记录大小,这里offset属性存放的是活动记录大小，不是偏移
    symbolTable.symbols[T->ptr[1]->place].offset = T->offset + T->ptr[2]->width;
    printf("%d\n", symbolTable.symbols[T->ptr[1]->place].offset);
    // printf("!!%d\n", T->ptr[2]->width);
    T->code = merge(3, T->ptr[1]->code, T->ptr[2]->code, genLabel(T->ptr[2]->Snext)); //函数体的代码作为函数的代�?
}

void func_dec(struct ASTNode *T)
{
    int rtn;
    struct opn opn1, opn2, result;
    rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'F', 0); //函数不在数据区中分配单元，偏移量�?0
    if (rtn == -1)
    {
        semantic_error(T->pos, T->type_id, "函数重复定义");
        return;
    }
    else
        T->place = rtn;
    result.kind = ID;
    strcpy(result.id, T->type_id);
    result.offset = rtn;
    T->code = genIR(FUNCTION, opn1, opn2, result); //生成中间代码：FUNCTION 函数�?
    T->offset = DX;                                //设置形式参数在活动记录中的偏移量初�?
    if (T->ptr[0])
    { //判断是否有参�?
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //处理函数参数列表
        T->width = T->ptr[0]->width;
        symbolTable.symbols[rtn].paramnum = T->ptr[0]->num;
        T->code = merge(2, T->code, T->ptr[0]->code); //连接函数名和参数代码序列
    }
    else {
        symbolTable.symbols[rtn].paramnum = 0;
        T->width = 0;
    }
}

void ext_struct_def(struct ASTNode *T)
{
    //填写函数定义信息到符号表
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]); //处理函数名和参数结点部分，这里不考虑用寄存器传递参�?
    T->code = T->ptr[0]->code;
}

void struct_def(struct ASTNode *T)
{
    int rtn;
    struct opn opn1, opn2, result;

    T->width = 0;   //函数的宽度设置为0，不会对外部变量的地址分配产生影响
    T->offset = DX; //设置局部变量在活动记录中的偏移量初�?
    T->type = STRUCT;
    rtn = fillSymbolTable(T->ptr[0]->type_id, newAlias(), LEV, STRUCT, 'S', 0); //函数不在数据区中分配单元，偏移量�?0
    if (rtn == -1)
    {
        semantic_error(T->pos, T->ptr[0]->type_id, "结构体重复定�?");
        return;
    }
    else
        T->place = rtn;

    T->ptr[1]->offset = T->offset;
    struct_flag = 1;
    struct_width = 0;
    semantic_Analysis(T->ptr[1]);
    symbolTable.symbols[rtn].offset = struct_width;
    struct_flag = 0;
}

void struct_dec(struct ASTNode *T)
{
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]);
    T->width = T->ptr[1]->width;
}

void param_list(struct ASTNode *T)
{
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]);
    if (T->ptr[1])
    {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]);
        T->num = T->ptr[0]->num + T->ptr[1]->num;             //统计参数个数
        T->width = T->ptr[0]->width + T->ptr[1]->width;       //累加参数单元宽度
        T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code); //连接参数代码
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
        semantic_error(T->ptr[1]->pos, T->ptr[1]->type_id, "参数名重复定�?");
    else
        T->ptr[1]->place = rtn;
    T->num = 1;                                //参数个数计算的初始�?
    // T->width = T->ptr[0]->type == INT ? 4 : (T->ptr[0]->type == FLOAT ? 8 : 1); //参数宽度
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
    T->code = genIR(PARAM, opn1, opn2, result); //生成：FUNCTION 函数�?
}

void comp_stm(struct ASTNode *T)
{
    LEV++;
    //设置层号�?1，并且保存该层局部变量在符号表中的起始位置在symbol_scope_TX
    symbol_scope_TX.TX[symbol_scope_TX.top++] = symbolTable.index;
    T->width = 0;
    T->code = NULL;
    if (T->ptr[0])
    {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //处理该层的局部变量DEF_LIST
        T->width += T->ptr[0]->width;
        T->code = T->ptr[0]->code;
    }
    if (T->ptr[1])
    {

        T->ptr[1]->offset = T->offset + T->width;
        // printf("!%d\n", T->ptr[1]->offset);
        strcpy(T->ptr[1]->Snext, T->Snext); //S.next属性向下传�?
        semantic_Analysis(T->ptr[1]);       //处理复合语句的语句序�?
        // printf("!!%d\n", T->ptr[1]->width);
        T->width += T->ptr[1]->width;
        T->code = merge(2, T->code, T->ptr[1]->code);
    }
    prn_symbol();                                                  //c在退出一个符合语句前显示的符号表
    // printf("!!%d\n", T->width);
    LEV--;                                                         //出复合语句，层号�?1
    symbolTable.index = symbol_scope_TX.TX[--symbol_scope_TX.top]; //删除该作用域中的符号
}

void def_list(struct ASTNode *T)
{
    T->code = NULL;
    if (T->ptr[0])
    {
        T->ptr[0]->offset = T->offset;
        semantic_Analysis(T->ptr[0]); //处理一个局部变量定�?
        T->code = T->ptr[0]->code;
        T->width = T->ptr[0]->width;
    }
    if (T->ptr[1])
    {
        T->ptr[1]->offset = T->offset + T->ptr[0]->width;
        semantic_Analysis(T->ptr[1]); //处理剩下的局部变量定�?
        T->code = merge(2, T->code, T->ptr[1]->code);
        T->width += T->ptr[1]->width;
    }
}

void var_def(struct ASTNode *T)
{
    int rtn, num, width;
    int flag = 0;
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
        rtn = searchSymbolTable(T->ptr[0]->ptr[0]->type_id);
        width = symbolTable.symbols[rtn].offset;
        struct_var_flag = 1;
    }
    T0 = T->ptr[1]; //T0为变量名列表子树根指针，对ID、ASSIGNOP类结点在登记到符号表，作为局部变�?
    num = 0;
    T0->offset = T->offset;
    T->width = 0;
    while (T0)
    { //处理所有DEC_LIST结点
        num++;
        T0->ptr[0]->type = T0->type; //类型属性向下传�?
        if (T0->ptr[1])
            T0->ptr[1]->type = T0->type;

        T0->ptr[0]->offset = T0->offset; //类型属性向下传�?
        if (T0->ptr[1])
            T0->ptr[1]->offset = T0->offset + width;
        if (T0->ptr[0]->kind == ID)
        {
            if(!struct_flag){
                // printf("%d\n", T0->offset + T0->width);
                rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T0->offset + T0->width); //此处偏移量未计算，暂时为0
            }
            else {
                rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV+1, T0->ptr[0]->type, 'M', 0); //此处偏移量未计算，暂时为0
                struct_width += (T0->ptr[0]->type == INT ? 4 : (T0->ptr[0]->type ? 8 : 1));
            }
            if (rtn == -1)
                semantic_error(T0->ptr[0]->pos, T0->ptr[0]->type_id, "变量重复定义");
            else
                T0->ptr[0]->place = rtn;
            T->width += width;
        }
        else if (T0->ptr[0]->kind == ASSIGNOP)
        {
            if(!struct_flag){
                // printf("%d\n", T0->ptr[0]->offset + T0->ptr[0]->width);
                rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V', T0->ptr[0]->offset + T0->ptr[0]->width); //此处偏移量未计算，暂时为0
            }
            else {
                rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'M', 0); //此处偏移量未计算，暂时为0
                // struct_flag = 0;
            }
            if (rtn == -1)
                semantic_error(T0->ptr[0]->ptr[0]->pos, T0->ptr[0]->ptr[0]->type_id, "变量重复定义");
            else
            {
                T0->ptr[0]->place = rtn;
                T0->ptr[0]->ptr[1]->offset = T->offset + T->width + width;
                Exp(T0->ptr[0]->ptr[1]);
                opn1.kind = ID;
                strcpy(opn1.id, symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
                opn1.offset = symbolTable.symbols[T0->ptr[0]->ptr[1]->place].offset;
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
                result.offset = symbolTable.symbols[T0->ptr[0]->place].offset;
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
    }              //空语句序�?
    if (T->ptr[1]) //2条以上语句连接，生成新标号作为第一条语句结束后到达的位�?
        strcpy(T->ptr[0]->Snext, newLabel());
    else //语句序列仅有一条语句，S.next属性向下传�?
        strcpy(T->ptr[0]->Snext, T->Snext);
    T->ptr[0]->offset = T->offset;
    semantic_Analysis(T->ptr[0]);
    T->code = T->ptr[0]->code;
    T->width = T->ptr[0]->width;
    if (T->ptr[1])
    { //2条以上语句连�?,S.next属性向下传�?
        strcpy(T->ptr[1]->Snext, T->Snext);
        T->ptr[1]->offset = T->offset + T->ptr[0]->width; //顺序结构共享单元方式

        semantic_Analysis(T->ptr[1]);
        //序列中第1条为表达式语句，返回语句，复合语句时，第2条前不需要标�?
        if (T->ptr[0]->kind == RETURN || T->ptr[0]->kind == EXP_STMT || T->ptr[0]->kind == COMP_STM)
            T->code = merge(2, T->code, T->ptr[1]->code);
        else
            T->code = merge(3, T->code, genLabel(T->ptr[0]->Snext), T->ptr[1]->code);
        // if (T->ptr[1]->width > T->width)
            T->width += T->ptr[1]->width; //顺序结构共享单元方式
                                         //                        T->width+=T->ptr[1]->width;//顺序结构顺序分配单元方式
    }
}

void if_then(struct ASTNode *T)
{
    strcpy(T->ptr[0]->Etrue, newLabel()); //设置条件语句真假转移位置
    strcpy(T->ptr[0]->Efalse, T->Snext);
    T->ptr[0]->offset = T->offset;
    boolExp(T->ptr[0]);
    T->width = T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, T->Snext);
    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
    semantic_Analysis(T->ptr[1]); //if子句
    T->width += T->ptr[1]->width;
    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
}

void if_then_else(struct ASTNode *T)
{
    strcpy(T->ptr[0]->Etrue, newLabel()); //设置条件语句真假转移位置
    strcpy(T->ptr[0]->Efalse, newLabel());
    T->ptr[0]->offset = T->offset;
    boolExp(T->ptr[0]); //条件，要单独按短路代码处�?
    T->width = T->ptr[0]->width;
    // printf("!!%d\n", T->ptr[0]->width);
    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, T->Snext);
    semantic_Analysis(T->ptr[1]); //if子句
    T->width += T->ptr[1]->width;
    strcpy(T->ptr[2]->Snext, T->Snext);
    T->ptr[2]->offset = T->offset + T->ptr[0]->width + T->ptr[1]->width;
    semantic_Analysis(T->ptr[2]); //else子句
    T->width += T->ptr[2]->width;
    T->code = merge(6, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code,
                    genGoto(T->Snext), genLabel(T->ptr[0]->Efalse), T->ptr[2]->code);
}

void while_dec(struct ASTNode *T)
{
    strcpy(T->ptr[0]->Etrue, newLabel()); //子结点继承属性的计算
    strcpy(T->ptr[0]->Efalse, T->Snext);
    T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
    prn_symbol();
    boolExp(T->ptr[0]); //循环条件，要单独按短路代码处�?
    T->width = T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, newLabel());
    strcpy(break_label, T->Snext);
    strcpy(continue_label, T->ptr[1]->Snext);
    semantic_Analysis(T->ptr[1]); //循环�?
    T->width += T->ptr[1]->width;
    T->code = merge(5, genLabel(T->ptr[1]->Snext), T->ptr[0]->code,
                    genLabel(T->ptr[0]->Etrue), T->ptr[1]->code, 
                    genGoto(T->ptr[1]->Snext));
}

void for_stmt(struct ASTNode *T) {
    LEV++;
    // 处理循环初始语句
    T->ptr[0]->offset = T->offset;
    T->ptr[0]->ptr[0]->offset = T->ptr[0]->offset;
    Exp(T->ptr[0]->ptr[0]);
    T->ptr[0]->width = T->ptr[0]->ptr[0]->width;
    // 处理循环条件
    strcpy(T->ptr[0]->ptr[1]->Etrue, newLabel()); //子结点继承属性的计算
    strcpy(T->ptr[0]->ptr[1]->Efalse, T->Snext);
    T->ptr[0]->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
    boolExp(T->ptr[0]->ptr[1]);
    T->ptr[0]->width += T->ptr[0]->ptr[1]->width;
    // 循环�?
    strcpy(T->ptr[1]->Snext, newLabel());
    strcpy(break_label, T->Snext);
    strcpy(continue_label, T->ptr[1]->Snext);
    semantic_Analysis(T->ptr[1]);
    // 自动运算条件
    T->ptr[0]->ptr[2]->offset = T->ptr[0]->offset + T->ptr[0]->width;
    strcpy(T->ptr[0]->ptr[2]->Snext, newLabel());
    Exp(T->ptr[0]->ptr[2]);
    T->ptr[0]->width += T->ptr[0]->ptr[2]->width;
    // T->width = T->ptr[0]->width >= T->ptr[1]->width ? T->ptr[0]->width : T->ptr[1]->width;
    T->width = T->ptr[0]->width + T->ptr[1]->width;
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
            semantic_error(T->pos, "返回值类型错�?", "");
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
   
    // switch选择表达�?
    T->ptr[0]->offset = T->offset;
    Exp(T->ptr[0]);
    T->width = T->ptr[0]->width;
    strcpy(T->Snext, newLabel());

    // case语句
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

    strcpy(T->ptr[0]->Etrue, newLabel()); //设置条件语句真假转移位置
    strcpy(T->ptr[0]->Efalse, T->Snext);
    T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
    boolExp(T->ptr[0]);
    T->width = T->ptr[0]->width;
    strcpy(T->ptr[1]->Snext, T->Snext);
    semantic_Analysis(T->ptr[1]); //if子句
    T->width += T->ptr[1]->width;
    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
}