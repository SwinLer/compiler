#include "def.h"
#include "parser.tab.h"
#define OFFSET 3
#define MAX_LOOP 100
 
symbolTable myTable = {{0}, 0}; //���ű�
symbol_scope_TX myScope = {{0}, 0}; //��ǰ������ķ����ڷ��ű����ʼλ����ţ�ջ�ṹ��˳���

int i, j, t, counter = 0; //��������
int rtn, flag1, flag2, num;
int mem, stru_dec = 0, exp_ele = 0; //����ṹ�壬������ṹ��Ա����
int rtn2;
char struct_name[33];
int switch_flag = 0, loop_flag = 0; //����breakλ��
int left_required = 0;
int array_size = 0;

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
{//�Գ����﷨�����ȸ�����
  int i=1;
  struct ASTNode *T0;
  if (T)
	{
	switch (T->kind) {
    case EXT_DEF_LIST:  
        display(T->ptr[0],indent);    //��ʾ���ⲿ���壨�ⲿ�����ͺ������б��еĵ�һ��
        display(T->ptr[1],indent);    //��ʾ���ⲿ�����б��е������ⲿ����
        break;
    case EXT_VAR_DEF:   
        printf("%*c�ⲿ�������壺(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);        //��ʾ�ⲿ��������
        printf("%*c��������\n",indent+OFFSET,' ');
        display(T->ptr[1],indent+OFFSET*2);        //��ʾ�����б�
        break;
    case TYPE:          
        printf("%*c���ͣ� %s\n",indent,' ',T->type_id);
        break;
    case EXT_DEC_LIST:  
        if(T->ptr[0]->ptr[0] != NULL)
        {
            printf("%*c��������%s\n", indent, ' ', T->ptr[0]->type_id);
            printf("%*c�����С��\n", indent, ' ');
        }
        display(T->ptr[0],indent);     //������ʾ�ⲿ��������
        display(T->ptr[1],indent);     //����������ͬ�ģ�����ʾ�﷨���˴��������Ժ����ƴ���ϲ�
        break;
    case FUNC_DEF:      
        printf("%*c�������壺(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);      //��ʾ������������
        display(T->ptr[1],indent+OFFSET);      //��ʾ�������Ͳ���
        display(T->ptr[2],indent+OFFSET);      //��ʾ������
        break;
    case FUNC_DEC:      
        printf("%*c��������%s\n",indent,' ',T->type_id);
        if (T->ptr[0]) {
            printf("%*c�����βΣ�\n",indent,' ');
            display(T->ptr[0],indent+OFFSET);  //��ʾ���������б�
        }
        else printf("%*c�޲κ���\n",indent+OFFSET,' ');
        break;
    case PARAM_LIST:    
        display(T->ptr[0],indent);     //������ʾȫ���������ͺ����ƣ�
        display(T->ptr[1],indent);
        break;
    case PARAM_DEC:     
        printf("%*c���ͣ�%s, ��������%s\n",indent,' ',T->ptr[0]->type==INT?"int":"float",T->ptr[1]->type_id);
        break;
    case EXP_STMT:      
        printf("%*c���ʽ��䣺(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);
        break;
    case RETURN:        
        printf("%*c������䣺(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);
        break;
    case COMP_STM:      
        printf("%*c������䣺(%d)\n",indent,' ',T->pos);
        printf("%*c�������ı������岿�֣�\n",indent+OFFSET,' ');
        // if(T->ptr[0] == NULL)
        //     printf("\n\n NULL!! \n\n");
        display(T->ptr[0],indent+OFFSET*2);      //��ʾ���岿��
        printf("%*c����������䲿�֣�\n",indent+OFFSET,' ');
        display(T->ptr[1],indent+OFFSET*2);      //��ʾ��䲿��
        break;
    case STM_LIST:      
        display(T->ptr[0],indent);      //��ʾ��һ�����
        display(T->ptr[1],indent);        //��ʾʣ�����
        break;
    case WHILE:         
        printf("%*cѭ�����(WHILE)��(%d)\n",indent,' ',T->pos);
        printf("%*cѭ������(WHILE)��\n",indent+OFFSET,' ');
        display(T->ptr[0],indent+OFFSET*2);      //��ʾѭ������
        printf("%*cѭ����(WHILE)��(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1],indent+OFFSET*2);      //��ʾѭ����
        break;
    case FOR:
        printf("%*cѭ�����(FOR)��(%d)\n",indent,' ',T->pos);
        printf("%*cѭ������(FOR)��\n", indent+OFFSET, ' ');
        display(T->ptr[0], indent+OFFSET*2);
        printf("%*cѭ����(FOR)��(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1], indent+OFFSET*2);
        break;
    case SWITCH_STMT:
        printf("%*cSWITCH��䣺(%d)\n", indent, ' ', T->pos);
        display(T->ptr[0], indent+OFFSET);
        display(T->ptr[1], indent+OFFSET);
        break;
    case CASE_STMT:
        printf("%*cCASE��䣺(%d)\n", indent, ' ', T->pos);
        display(T->ptr[0], indent+OFFSET);
        display(T->ptr[1], indent+OFFSET);
        break;
    case FOR_DEC:
        display(T->ptr[0], indent+OFFSET*2);
        display(T->ptr[1], indent+OFFSET*2);
        display(T->ptr[2], indent+OFFSET*2);
        break;
    case CONTINUE:
        printf("%*cCONTINUE��䣺(%d)\n", indent, ' ', T->pos);
        break;
    case BREAK:
        printf("%*cBREAK��䣺(%d)\n", indent, ' ', T->pos);
        break;
    case IF_THEN:       
        printf("%*c�������(IF_THEN)��(%d)\n",indent,' ',T->pos);
        printf("%*c������\n",indent+OFFSET,' ');
        display(T->ptr[0],indent+OFFSET*2);      //��ʾ����
        printf("%*cIF�Ӿ䣺(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1],indent+OFFSET*2);      //��ʾif�Ӿ�
        break;
    case IF_THEN_ELSE:  
        printf("%*c�������(IF_THEN_ELSE)��(%d)\n",indent,' ',T->pos);
        printf("%*c������\n",indent+OFFSET,' ');
        display(T->ptr[0],indent+OFFSET*2);      //��ʾ����
        printf("%*cIF�Ӿ䣺(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[1],indent+OFFSET*2);      //��ʾif�Ӿ�
        printf("%*cELSE�Ӿ䣺(%d)\n",indent+OFFSET,' ',T->pos);
        display(T->ptr[2],indent+OFFSET*2);      //��ʾelse�Ӿ�
        break;
    case DEF_LIST:     
        display(T->ptr[0],indent);    //��ʾ�þֲ����������б��еĵ�һ��
        display(T->ptr[1],indent);    //��ʾ�����ֲ���������
        break;
    case VAR_DEF:       
        printf("%*c�ֲ��������壺(%d)\n",indent,' ',T->pos);
        display(T->ptr[0],indent+OFFSET);   //��ʾ��������
        display(T->ptr[1],indent+OFFSET);   //��ʾ�ö����ȫ��������
        break;
    case DEC_LIST:      
        printf("%*c��������\n",indent,' ');
        T0=T;
        while (T0) {
            if (T0->ptr[0]->kind==ID)
                printf("%*c %s\n",indent+OFFSET*2,' ',T0->ptr[0]->type_id);
            else if (T0->ptr[0]->kind==ASSIGNOP)
            {
                printf("%*c %s ASSIGNOP\n ",indent+OFFSET*2,' ',T0->ptr[0]->ptr[0]->type_id);
                display(T0->ptr[0]->ptr[1],indent+strlen(T0->ptr[0]->ptr[0]->type_id)+7);        //��ʾ��ʼ�����ʽ
            }
            T0=T0->ptr[1];
        }
        break;
    case ARRAY_LIST:
        display(T->ptr[0], indent);
        display(T->ptr[1], indent);
        break;
    case ARRAY_ID:
        printf("%*c�������� %s\n",indent,' ',T->type_id);
        printf("%*c�����±꣺\n", indent, ' ');
        display(T->ptr[0], indent+OFFSET);
        break;
    case STRUCT_DEF:
        printf("%*c�ṹ���壺(%d)\n", indent, ' ',T->pos);
        display(T->ptr[0], indent+OFFSET);
        display(T->ptr[1], indent+OFFSET);
        break;
    case STRUCT_TAG:
        printf("%*c�ṹ����%s\n", indent, ' ', T->struct_name);
        break;
    case ID:	        
        printf("%*cID�� %s\n",indent,' ',T->type_id);
        break;
    case INT:	     
        printf("%*cINT��%d\n",indent,' ',T->type_int);
        break;
    case FLOAT:	        
        printf("%*cFLAOT��%f\n",indent,' ',T->type_float);
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
        printf("%*c�ṹ����ʣ�\n", indent, ' ');
        display(T->ptr[0], indent+OFFSET);
        printf("%*c���ʳ�Ա������%s\n", indent+OFFSET, ' ',T->type_id);
        break;
    case NOT:
    case UMINUS:    
        printf("%*c%s\n",indent,' ',T->type_id);
        display(T->ptr[0],indent+OFFSET);
        break;
    case FUNC_CALL: 
        printf("%*c�������ã�(%d)\n",indent,' ',T->pos);
        printf("%*c��������%s\n",indent+OFFSET,' ',T->type_id);
        display(T->ptr[0],indent+OFFSET);
        break;
    case ARGS:      
        i=1;
        while (T) {  //ARGS��ʾʵ�ʲ������ʽ���н�㣬���һ������Ϊ��һ��ʵ�ʲ������ʽ���ڶ�������Ϊʣ�µ�
            struct ASTNode *T0=T->ptr[0];
            printf("%*c��%d��ʵ�ʲ������ʽ��\n",indent,' ',i++);
            display(T0,indent+OFFSET);
            T=T->ptr[1];
        }
//      printf("%*c��%d��ʵ�ʲ������ʽ��\n",indent,' ',i);
  //    display(T,indent+OFFSET);
        printf("\n");
        break;
        }
    }
}

int searchSymbolTable(char *name) {
    int i,flag=0;
    for(i=myTable.index-1;i>=0;i--){
        if (myTable.symbols[i].level==0)
            flag=1;
        if (flag && myTable.symbols[i].level==1)
            continue;   //����ǰ�溯������ʽ��������
        if (!strcmp(myTable.symbols[i].name, name)) {
            if(myTable.symbols[i].flag == 'M'){
                if(exp_ele)
                    return i;
            }
            else
                return i;
        }
            
    }
    return -1;
}

void semantic_error(int line, char *msg1, char *msg2){
    //�������ֻ�ռ�������Ϣ�����һ����ʾ
    printf("ERROR! ��%d��, %s %s\n",line, msg1, msg2);
}
//
void prn_symbol(){ //��ʾ���ű�
    int i = 0;
    printf("%6s %6s %6s  %6s %4s %6s\n","������","�� ��","�� ��","��  ��","���","ƫ����");
    for(i = 0;i < myTable.index;i++)
        printf("%6s %6s %6d  %6s %4c %6d\n",myTable.symbols[i].name,\
                myTable.symbols[i].alias,myTable.symbols[i].level,\
                myTable.symbols[i].type==INT ? "int" : (myTable.symbols[i].type==FLOAT ? "float" : (myTable.symbols[i].type==CHAR ? "char" : (myTable.symbols[i].type==STRING ? "string" : "void"))),\
                myTable.symbols[i].flag,myTable.symbols[i].offset);
}


void DisplaySymbolTable()
{
    int i;
    printf("\t\t\n���ű�\n");
    printf("----------------------------------------------------------------------\n");
    printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n","Index","Name","Level","Type","Flag","Param_num","Array_size");
    printf("----------------------------------------------------------------------\n");
    for(i = 0;i < myTable.index; i++){
        printf("%d\t",i);
        printf("%s\t",myTable.symbols[i].name);
        printf("%d\t",myTable.symbols[i].level);
        if(myTable.symbols[i].type==INT)
               printf("%s\t","int");
        else if(myTable.symbols[i].type==FLOAT)
            printf("%s\t","float");
        else if(myTable.symbols[i].type==CHAR)
            printf("%s\t","char");
        else if(myTable.symbols[i].type==STRING)
            printf("%s\t","string");
        else if(myTable.symbols[i].type==STRUCT)
            printf("%s\t", "struct");
        else
            printf("%s\t","void");
            
        printf("%c\t",myTable.symbols[i].flag);
        if(myTable.symbols[i].flag=='F')
            printf("%d",myTable.symbols[i].paramnum);
        printf("\t\t");
        if(myTable.symbols[i].flag == 'A')
            printf("%d\n", myTable.symbols[i].array_size);
        else
            printf("\n");
    }
    printf("----------------------------------------------------------------------\n");
    printf("\n");
}

int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset)
{ // ���ȸ���name����ű������ظ����� �ظ����巵��-1

    int i;
    /*���Ų��أ������ⲿ��������ǰ�к������壬
    ���β������ڷ��ű��У���ʱ���ⲿ������ǰ�������β������������*/
    for (i = myTable.index - 1; myTable.symbols[i].level == level || (level == 0 && i >= 0); i--)
    {
        if (level == 0 && myTable.symbols[i].level == 1)
            continue; //�ⲿ�������ββ��رȽ�����
        if (!strcmp(myTable.symbols[i].name, name))
            return -1;
    }
    //��д���ű�����
    strcpy(myTable.symbols[myTable.index].name, name);
    strcpy(myTable.symbols[myTable.index].alias, alias);
    myTable.symbols[myTable.index].level = level;
    myTable.symbols[myTable.index].type = type;
    myTable.symbols[myTable.index].flag = flag;
    myTable.symbols[myTable.index].offset = offset;
    return myTable.index++; //���ص��Ƿ����ڷ��ű��е�λ����ţ��м��������ʱ�������ȡ�����ű���
}
//
//��д��ʱ���������ű�������ʱ�����ڷ��ű��е�λ��
int fill_Temp(char *name, int level, int type, char flag, int offset)
{
    strcpy(myTable.symbols[myTable.index].name, "");
    strcpy(myTable.symbols[myTable.index].alias, name);
    myTable.symbols[myTable.index].level = level;
    myTable.symbols[myTable.index].type = type;
    myTable.symbols[myTable.index].flag = flag;
    myTable.symbols[myTable.index].offset = offset;
    return myTable.index++; //���ص�����ʱ�����ڷ��ű��е�λ�����
}
//
int match_param(int i, struct ASTNode *T)
{ // ƥ�亯������
    int j, num = myTable.symbols[i].paramnum;
    int type1, type2;
    if (num == 0 && T == NULL)
        return 1;
    for (j = 1; j < num; j++)
    {
        if (!T)
        {
            semantic_error(T->pos, "", "�������ò���̫��");
            return 0;
        }
        type1 = myTable.symbols[i + j].type; //�β�����
        type2 = T->ptr[0]->type;
        if (type1 != type2)
        {
            semantic_error(T->pos, "", "�������Ͳ�ƥ��");
            return 0;
        }
        T = T->ptr[1];
    }
    if (T->ptr[1])
    { //num�������Ѿ�ƥ���꣬����ʵ�α��ʽ
        semantic_error(T->pos, "", "�������ò���̫��");
        return 0;
    }
    return 1;
}

int semantic_Analysis(struct ASTNode *T, int type, int level, char flag, int command){
    int type1, type2;
    if(T) {
        switch (T->kind)
        {
        case EXT_DEF_LIST://�ⲿ�����б�
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            semantic_Analysis(T->ptr[1], type, level, flag, command);
            break;
        case EXT_VAR_DEF:   //�����ⲿ˵��,����һ������(TYPE���)�е������͵��ڶ������ӵ�������
            type = semantic_Analysis(T->ptr[0], type, level, flag, command);
            semantic_Analysis(T->ptr[1], type, level, flag, command);
            break;
        case EXT_STRUCT_DEF:  //�ṹ���ⲿ����
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            break;
        case STRUCT_DEF: //�ṹ�嶨��
            rtn = searchSymbolTable(T->ptr[0]->type_id);
            if(rtn != -1){
                if(myTable.symbols[rtn].flag == 'S'){
                    semantic_error(T->pos, "", "�ṹ�����ظ�����");
                }
            }
            strcpy(myTable.symbols[myTable.index].name, T->ptr[0]->type_id);
            myTable.symbols[myTable.index].level = level;
            myTable.symbols[myTable.index].flag = 'S';
            myTable.symbols[myTable.index].type = STRUCT;
            myTable.index++;
            flag = 'M';
            semantic_Analysis(T->ptr[1], type, level+1, flag, command);
            break;
        case STRUCT_DEC: //�ṹ�����
            rtn = searchSymbolTable(T->ptr[0]->type_id);
            if(rtn == -1){
                semantic_error(T->pos, T->type_id, "�ṹ��δ����");
            }
            else{
                stru_dec = 1;
                strcpy(struct_name, T->ptr[0]->type_id);
            }
            break;
        case EXP_ELE: //�ṹ���Ա����
            rtn = searchSymbolTable(T->ptr[0]->type_id);
            flag1 = 0;

            if(rtn == -1){
                semantic_error(T->pos, T->ptr[0]->type_id, "�ṹ�����δ����");
            }
            else{
                if(myTable.symbols[rtn].type != STRUCT){
                    semantic_error(T->pos, T->ptr[0]->type_id, "���ǽṹ��");
                }
                else{
                    rtn = searchSymbolTable(myTable.symbols[rtn].struct_name);
                    if(rtn == -1){
                        semantic_error(T->pos, "", "�ṹ��δ����");
                        return 0;
                    }
                    num = rtn;
                    exp_ele = 1;
                    do{
                        num++;
                        if(!strcmp(myTable.symbols[num].name, T->type_id)){
                            flag1 = 1; //�Ƿ��г�Ա����
                            break;
                        }
                    } while(num < myTable.index && myTable.symbols[num].flag == 'M');
                    if(!flag1){
                        semantic_error(T->pos, "�ṹ�岻����Ա����", T->type_id);
                    }
                    exp_ele = 0;
                    flag1 = 0;
                }
            }
            break;
        case ARRAY_DEC://���鶨��
            rtn = searchSymbolTable(T->type_id);
            if(rtn != -1){
                if(myTable.symbols[rtn].level == level){
                    semantic_error(T->pos, "", "�������ظ�����");
                }
            }
            else{
                strcpy(myTable.symbols[myTable.index].name, T->type_id);
                myTable.symbols[myTable.index].level = level;
                myTable.symbols[myTable.index].flag = 'A';
                myTable.symbols[myTable.index].type = type;
                myTable.symbols[myTable.index].array_size = 0;
                myTable.index++;
                semantic_Analysis(T->ptr[0], type, level, 'A', 0);
            }
            break;
        case ARRAY_LIST://�����±����
            type1 = semantic_Analysis(T->ptr[0], type, level, flag, command);
            if(type1 != INT){
                semantic_error(T->pos, "", "�����±겻�����ͱ��ʽ");
            }
            else{
                if(command == 0){
                    myTable.symbols[myTable.index-1].array_size++;
                }
                else{
                    array_size++;
                }
                semantic_Analysis(T->ptr[1], type, level, flag, command);
            }
            break;
        case EXP_ARRAY://�������
            rtn = searchSymbolTable(T->type_id);
            if(rtn == -1){
                semantic_error(T->pos, T->type_id, "����δ����");
            }
            else{
                if(myTable.symbols[rtn].flag != 'A'){
                    semantic_error(T->pos, T->type_id, "��������");
                }
                else{
                    array_size = 0;
                    semantic_Analysis(T->ptr[0], type, level, flag, 1);
                    // printf("\n\n%d %d\n\n", array_size, myTable.symbols[rtn].array_size);
                    if(array_size != myTable.symbols[rtn].array_size && array_size != 0)
                        semantic_error(T->pos, T->type_id, "����ά����һ��");
                    array_size = 0;
                }
            }
            break;
        case TYPE:
            return T->type;
        case EXT_DEC_LIST://���������б�
            flag = 'V';
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            semantic_Analysis(T->ptr[1], type, level, flag, command);
            break;
        case ID://����
            // printf("\n\n%s\n\n", T->type_id);
            i = (flag == 'P' ? 2 : 0);
            while(myTable.symbols[i].level != level && i < myTable.index)
                i++;
            if(command == 0){ //�������
                while(i < myTable.index){
                    if(!strcmp(myTable.symbols[i].name,T->type_id) && (myTable.symbols[i].flag==flag)){
                        // printf("\n\n%s %s\n\n", myTable.symbols[i].name, T->type_id);
                        if(flag=='V')
                            semantic_error(T->pos, T->type_id, "ȫ�ֱ����ظ�����");
                        else if(flag=='F')
                            semantic_error(T->pos, T->type_id, "�������ظ�����");
                        else if(flag=='T')
                            semantic_error(T->pos, T->type_id, "�ֲ������ظ�����");
                        else if(flag=='M')
                            semantic_error(T->pos, "", "��Ա�����ظ�����");
                        else
                            semantic_error(T->pos, "", "�������������ظ�");
                        return 0;
                    }
                    i++;
                }
                strcpy(myTable.symbols[myTable.index].name, T->type_id);
                myTable.symbols[myTable.index].level = level;
                myTable.symbols[myTable.index].flag = flag;
                myTable.symbols[myTable.index].type = type;
                if (stru_dec) {
                    strcpy(myTable.symbols[myTable.index].struct_name, struct_name);
                }
                myTable.index++;
                stru_dec = 0;
                return type;
            }
            else{ // ʹ�ñ���
                i = myTable.index - 1;
                while(i >= 0){
                    if(myTable.symbols[i].level <= level && !strcmp(myTable.symbols[i].name, T->type_id) && (myTable.symbols[i].flag == 'T' || myTable.symbols[i].flag == 'V' || myTable.symbols[i].flag == 'P'))
                    {
                        return myTable.symbols[i].type;
                    }    
                    i--;
                }
                if(i < 0){
                    semantic_error(T->pos, T->type_id, "����δ����");
                }
            }
            break;
        case FUNC_DEF://�ⲿ��������
            type = semantic_Analysis(T->ptr[0], type, level+1, flag, command);
            if (type != VOID)
                flag1 = 1;
            semantic_Analysis(T->ptr[1], type, 1, flag, command);
            semantic_Analysis(T->ptr[2], type, 1, flag, command);
            num = myTable.index;
            do{
                num--;
            } while (myTable.symbols[num].flag == 'P');
            myTable.index = num+1;
            if(flag1 && !flag2){ //flag1���Ƿ��巵��ֵvoid flag2�������Ƿ���return��� 
                semantic_error(T->pos, T->type_id, "����û�з���ֵ");
            }   
            flag1 = 0;
            flag2 = 0;
            break;
        case FUNC_DEC:   //���ݷ������ͣ���������д���ű�
            rtn = searchSymbolTable(T->type_id);
            if(rtn != -1){
                if(myTable.symbols[rtn].flag == 'F'){
                    semantic_error(T->pos, T->type_id, "�������ظ�����");
                }
            }
            strcpy(myTable.symbols[myTable.index].name, T->type_id);
            myTable.symbols[myTable.index].flag = 'F';
            myTable.symbols[myTable.index].level = 0;
            myTable.symbols[myTable.index].type = type;
            myTable.index++;
            counter = 0;
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            myTable.symbols[myTable.index - counter - 1].paramnum = counter;
            break;
        case PARAM_LIST:
            counter++;
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            semantic_Analysis(T->ptr[1], type, level, flag, command);
            break;
        case PARAM_DEC:
            flag = 'P';
            type = semantic_Analysis(T->ptr[0], type, level+1, flag, command);
            semantic_Analysis(T->ptr[1], type, level, flag, command);
            break;
        case COMP_STM://�������(�����ŷ�Χ)
            flag = 'T';
            command = 0;
            myScope.TX[myScope.top++] = myTable.index;
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            command = 1;
            semantic_Analysis(T->ptr[1], type, level+1, flag, command);
            DisplaySymbolTable();
            myTable.index = myScope.TX[--myScope.top];
            break;
        case STM_LIST://����б�
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            semantic_Analysis(T->ptr[1], type, level, flag, command);
            break;
        case DEF_LIST://�����б�
            semantic_Analysis(T->ptr[0],type,level,flag,command);
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            break;
        case VAR_DEF://�������
            type = semantic_Analysis(T->ptr[0],type,level+1,flag,command);
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            break;
        case DEC_LIST://����б�
            if(stru_dec){
                type = STRUCT;
            }
            semantic_Analysis(T->ptr[0],type,level,flag,command);
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            break;
        case EXP_STMT:
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            break;
        case RETURN:
            flag2 = 1;
            if(flag1){
                if(!T->ptr[0]){
                    semantic_error(T->pos, "", "ȱ�ٷ���ֵ");
                    break;
                }
            }
            type = semantic_Analysis(T->ptr[0], type, level, flag, command);
            num = myTable.index;
            do{
                num--;
            }while (myTable.symbols[num].flag != 'F');
            if( !(!flag1 && !T->ptr[0]) && type != myTable.symbols[num].type){
                semantic_error(T->pos, "", "����ֵ���ʹ���");
            }
            break;
        case IF_THEN:
            semantic_Analysis(T->ptr[0],type,level,flag,command);
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            break;
        case WHILE:
        case FOR:
            loop_flag++;
            semantic_Analysis(T->ptr[0],type,level,flag,command);
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            loop_flag--;
            break;
        case SWITCH_STMT:
            switch_flag++;
            type = semantic_Analysis(T->ptr[0],type,level,flag,command);
            if(type != INT && type != CHAR){
                semantic_error(T->pos, "", "switch���ֻ����int, char����");
            }
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            switch_flag--;
            break;
        case CASE_STMT:
            semantic_Analysis(T->ptr[0],type,level,flag,command);
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            break;
        case BREAK:
            if(!switch_flag && !loop_flag)
                semantic_error(T->pos, "", "break���Ҫ��ѭ������switch�����");
            break;
        case CONTINUE:
            if(!loop_flag)
                semantic_error(T->pos, "", "continue���Ҫ��ѭ�������");
            break;
        case IF_THEN_ELSE:
            semantic_Analysis(T->ptr[0],type,level,flag,command);
            semantic_Analysis(T->ptr[1],type,level,flag,command);
            semantic_Analysis(T->ptr[2],type,level,flag,command);
            break;
        case DEFAULT_STMT:
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            break;
        case FOR_DEC:
            semantic_Analysis(T->ptr[0], type, level, flag, command);
            semantic_Analysis(T->ptr[1], type, level, flag, command);
            semantic_Analysis(T->ptr[2], type, level, flag, command);
            break;
        case ASSIGNOP:
        case PLUSASSIGNOP:
        case MINUSASSIGNOP:
        case STARASSIGNOP:
        case DIVASSIGNOP:
        case MODASSIGNOP:
            if(T->ptr[0]->kind != ID && T->ptr[0]->kind != EXP_ARRAY && T->ptr[0]->kind != EXP_ELE) {
                semantic_error(T->pos, "", "��ֵ���ʽ��Ҫ��ֵ");
            }
            if(T->ptr[0]->kind == ID){
                rtn = searchSymbolTable(T->ptr[0]->type_id);
                if(myTable.symbols[rtn].type == STRUCT){
                    semantic_error(T->pos, "", "��ֵ���ʽ��Ҫ��ֵ");
                }
                else{
                    type1 = semantic_Analysis(T->ptr[0], type, level, flag, command);
                    type2 = semantic_Analysis(T->ptr[1], type, level, flag, command);
                    if(type1 == type2)
                        return type1;
                }
            }
            else{
                type1 = semantic_Analysis(T->ptr[0], type, level, flag, command);
                type2 = semantic_Analysis(T->ptr[1], type, level, flag, command);
                if(type1 == type2)
                    return type1;
            }
            break;
        case AND:
        case OR:
        case RELOP:
        case PLUS:
        case MINUS:
        case STAR:
        case DIV:
        case MOD:
            type1 = semantic_Analysis(T->ptr[0], type, level, flag, command);
            type2 = semantic_Analysis(T->ptr[1], type, level, flag, command);
            if(type1 == type2)
                return type1;
            break;
        case AUTOPLUS_L:
        case AUTOPLUS_R:
        case AUTOMINUS_L:
        case AUTOMINUS_R:
        case UMINUS:
        case NOT:
            if(T->ptr[0]->kind == ID){
                rtn = searchSymbolTable(T->ptr[0]->type_id);
                if(rtn == -1){
                    semantic_error(T->pos, T->ptr[0]->type_id, "����δ����");
                }
                else{
                    if(myTable.symbols[rtn].type == STRUCT)
                        semantic_error(T->pos, T->ptr[0]->type_id, "������ֵ");
                    else{
                        if(T->ptr[0]->kind != ID && T->ptr[0]->kind != EXP_ARRAY && T->ptr[0]->kind != EXP_ELE){
                            semantic_error(T->pos, "", "�����Լ����ʽ��Ҫ��ֵ");
                            break;
                        }
                        if(T->ptr[0]->kind == ID){
                            rtn = searchSymbolTable(T->ptr[0]->type_id);
                            if(myTable.symbols[rtn].type == STRUCT){
                                semantic_error(T->pos, "", "��ֵ���ʽ��Ҫ��ֵ");
                            }
                        }
                        type1 = semantic_Analysis(T->ptr[0], type, level, flag, command);
                        return type1;
                    }
                }
            }
            else if(T->ptr[0]->kind == EXP_ELE || T->ptr[0]->kind == EXP_ARRAY || !strcmp(T->ptr[0]->type_id, "AUTO")){
                type1 = semantic_Analysis(T->ptr[0], type, level, flag, command);
                return type1;
            }
            else{
                semantic_error(T->pos, "", "�����Լ����ʽ��Ҫ��ֵ");
            }
            break;
        case INT:
            return INT;
        case CHAR:
            return CHAR;
        case FLOAT:
            return FLOAT;
        case STRING:
            return STRING;
        case VOID:
            return VOID;
        case FUNC_CALL://��������
            rtn = searchSymbolTable(T->type_id);
            if(rtn != -1){
                if(myTable.symbols[rtn].flag != 'F'){
                    semantic_error(T->pos, T->type_id, "���Ǻ���");
                    return 0;
                }
                type = myTable.symbols[rtn].type;
                counter = 0;
                semantic_Analysis(T->ptr[0], type, level, flag, command);
                if(myTable.symbols[rtn].paramnum != counter)
                    semantic_error(T->pos, "", "����������ƥ��");
                return myTable.symbols[rtn].type;
            }
            else{
                semantic_error(T->pos, T->type_id, "����δ����");
                return 0;
            }
            break;
        case ARGS:
            counter++;
            t = semantic_Analysis(T->ptr[0], type, level, flag, command);
            if(type != t){
                semantic_error(T->pos, "", "�������ò������Ͳ�ƥ��");
            }
            type = myTable.symbols[j+counter+1].type;
            semantic_Analysis(T->ptr[1], type, level, flag, command);       
            break;


        default:
            break;
        } 
    }
}

void semantic_Analysis0(struct ASTNode *T){
    myTable.index = 0;
    fillSymbolTable("read", "", 0, INT, 'F', 4);
    myTable.symbols[0].paramnum = 0; //read���βθ���
    fillSymbolTable("x", "", 1, INT, 'P', 12);
    fillSymbolTable("write", "", 0, INT, 'F', 4);
    myTable.symbols[2].paramnum = 1;
    myScope.TX[0] = 0; //�ⲿ�����ڷ��ű��е���ʼ���Ϊ0
    myScope.top = 0;
    T->offset = 0; // �ⲿ��������������ƫ����
    semantic_Analysis(T, 0, 0, ' ', 0);
    DisplaySymbolTable();
}