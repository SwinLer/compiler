#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "parser.tab.h"
#define MAXLENGTH   200
#define DX 3*sizeof(int)          /*���¼������Ϣ��Ҫ�ĵ�Ԫ�����������ʵ��ϵͳ����*/
//�����﷨��������͡�����ַ������͵ȶ�������ο���ʵ��ʱһ��Ҫ�����Լ������������

struct ASTNode {
        //���¶Խ�����Զ���û�п��Ǵ洢Ч�ʣ�ֻ�Ǽ򵥵��г�Ҫ�õ���һЩ����
	int kind; 
	union {
		char type_id[33];             //�ɱ�ʶ�����ɵ�Ҷ���
        char case_id[33];
		int type_int;                 //�����������ɵ�Ҷ���
		float type_float;               //�ɸ��㳣�����ɵ�Ҷ���
        char type_char[5];
        char type_string[32];
        char struct_name[32];
	};
    struct ASTNode *ptr[4];         //��kindȷ���ж��ٿ�����
    int place;                     //��ţ���ʱ�������ڷ��ű��λ�����
    char Etrue[15],Efalse[15];       //�Բ������ʽ�ķ���ʱ�����ת��Ŀ��ı��
    char Snext[15];               //����Ӧ���Sִ�к����һ�����λ�ñ��
    struct codenode *code;          //�ý���м��������ͷָ��
    int type;                      //���Ա�ʶ���ʽ��������
    int pos;                       //�﷨��λ����λ���к�
    int offset;                     //ƫ����
    int width;                     //ռ�����ֽ���
    int num;                      //����������������ͳ���βθ���
};

struct symbol {       //����ֻ�г���һ�����ű���Ĳ������ԣ�û�������Լ�Ļ���
    char name[33];   //����������
    int level;        //���
    int type;         //�������ͻ�������ֵ����
    int  paramnum;  //�Ժ������ã���¼��ʽ��������
    char alias[10];   //������Ϊ���Ƕ�ײ��ʹ��
    char flag;       //���ű�ǣ�������'F'  ������'V'   ������'P'  ��ʱ������'T'
    char offset;      //�ⲿ�����;ֲ��������侲̬����������¼�е�ƫ������
    char struct_name[33];
    int array[10];
    int const_int;
        //���¼�������¼��С��Ŀ���������ʱʹ��
    //������ڵ�ʵ����ܻ��õ�������...
};
//���ű�
struct symboltable{
    struct symbol symbols[MAXLENGTH];
    int index;
};

struct symbol_scope_begin {
    //��ǰ������ķ����ڷ��ű����ʼλ�����,����һ��ջ�ṹ,��ʹ��˳�����Ϊ���ű�ʱ�����롢�˳�һ��������ʱ��Ҫ�������������ɷ��ű�Ĺ�����������ʽ�ķ��ű���һ����Ҫ�����ݽṹ
    int TX[30];
    int top;
};

struct opn
{
    int kind; //��ʶ����������
    int type; //��ʶ������������
    union {
        int const_int;     //������ֵ��������
        float const_float; //���㳣��ֵ��������
        char const_char;   //�ַ�����ֵ��������
        char *const_string;
        char id[33]; //��������ʱ�����ı��������ַ���
        struct Array *type_array;
        struct Struct *type_struct;
    };
    int level;  //�����Ĳ�ţ�0��ʾ��ȫ�ֱ��������ݱ����ھ�̬������
    int offset; //������Ԫƫ�����������ڷ��ű�Ķ���λ����ţ�Ŀ���������ʱ��
};

struct codenode
{                                  //����ַTAC������,����˫��ѭ���������м����Դ���
    int op;                        //TAC��������������
    struct opn opn1, opn2, result; //2����������������
    struct codenode *next, *prior;
};

union Value {
    char type_id[33]; //�ɱ�ʶ�����ɵ�Ҷ���
    int type_int;     //�����������ɵ�Ҷ���
    float type_float; //�ɸ��㳣�����ɵ�Ҷ���
    char type_char;
    char type_string[32];
    struct Array *arrayPtr;
};

// ʹ������洢�������
struct Array
{
    int kind;
    union Value value;
    int index;
    struct Array *next;
};

// ʹ������洢�������
struct Struct
{
    int kind;
    char *name; // �ֶ�����
    union Value value;
    struct Struct *next;
};


struct ASTNode * mknode(int num,int kind,int pos,...);


// semantic_Analysic
void semantic_Analysis(struct ASTNode *T);
void semantic_Analysis0(struct ASTNode *T);
int searchSymbolTable(char *name);
void semantic_error(int line,char *msg1,char *msg2);
void prn_symbol();
void DisplaySymbolTable();
int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset);
int fill_Temp(char *name, int level, int type, char flag, int offset);
int match_param(int i, struct ASTNode *T);
int compute_width(struct ASTNode *T);
int compute_arraywidth(int *array,int index);
int compute_width0(struct ASTNode *T,int *array,int index);
int array_out(struct ASTNode *T,int *a,int index);

// semantic_case
void ext_var_list(struct ASTNode *T);
void ext_def_list(struct ASTNode *T);
void ext_var_def(struct ASTNode *T);
void func_def(struct ASTNode *T);
void func_dec(struct ASTNode *T);
void ext_struct_def(struct ASTNode *T);
void struct_def(struct ASTNode *T);
void struct_dec(struct ASTNode *T);
void array_dec(struct ASTNode *T);
void param_list(struct ASTNode *T);
void param_dec(struct ASTNode *T);
void comp_stm(struct ASTNode *T);
void def_list(struct ASTNode *T);
void var_def(struct ASTNode *T);
void stm_list(struct ASTNode *T);
void if_then(struct ASTNode *T);
void if_then_else(struct ASTNode *T);
void while_dec(struct ASTNode *T);
void for_stmt(struct ASTNode *T);
void break_dec(struct ASTNode *T);
void continue_dec(struct ASTNode *T);
void exp_stmt(struct ASTNode *T);
void return_dec(struct ASTNode *T);
void switch_stmt(struct ASTNode *T);
void case_stmt(struct ASTNode *T);
void default_stmt(struct ASTNode *T);

// exp
void Exp(struct ASTNode *T);
void boolExp(struct ASTNode *T);
void id_exp(struct ASTNode *T);
void int_exp(struct ASTNode *T);
void float_exp(struct ASTNode *T);
void char_exp(struct ASTNode *T);
void string_exp(struct ASTNode *T);
void assignop_exp(struct ASTNode *T);
void other_assignop_exp(struct ASTNode *T);
void auto_op_exp(struct ASTNode *T);
void relop_exp(struct ASTNode *T);
void args_exp(struct ASTNode *T);
void op_exp(struct ASTNode *T);
void func_call_exp(struct ASTNode *T);
void not_exp(struct ASTNode *T);
void ext_var_list(struct ASTNode *T);
void ext_def_list(struct ASTNode *T);
void ext_var_def(struct ASTNode *T);
void func_def(struct ASTNode *T);
void func_dec(struct ASTNode *T);
void param_list(struct ASTNode *T);
void param_dec(struct ASTNode *T);
void comp_stm(struct ASTNode *T);
void def_list(struct ASTNode *T);
void var_def(struct ASTNode *T);
void stmt_list(struct ASTNode *T);
void if_then(struct ASTNode *T);
void if_then_else(struct ASTNode *T);
void while_dec(struct ASTNode *T);
void exp_stmt(struct ASTNode *T);
void return_dec(struct ASTNode *T);
void exp_array(struct ASTNode *T);
void exp_ele(struct ASTNode *T);

// code
char *str_catch(char *s1, char *s2);
char *newAlias();
char *newLabel();
char *newTemp();
struct codenode *genIR(int op, struct opn opn1, struct opn opn2, struct opn result);
struct codenode *genLabel(char *label);
struct codenode *genGoto(char *label);
struct codenode *merge(int num, ...);
void prnIR(struct codenode *head);