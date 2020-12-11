#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "parser.tab.h"
#define MAXLENGTH   200
#define DX 3*sizeof(int)          /*���¼������Ϣ��Ҫ�ĵ�Ԫ�����������ʵ��ϵͳ����*/
//�����﷨��������͡�����ַ������͵ȶ�������ο���ʵ��ʱһ��Ҫ�����Լ������������
int LEV;      //���

// enum node_kind
// {
//     EXT_DEF_LIST,
//     EXT_VAR_DEF,
//     FUNC_DEF,
//     FUNC_DEC,
//     EXT_STRUCT_DEF,
//     STRUCT_DEF,
//     STRUCT_DEC,
//     STRUCT_TAG,
//     EXP_ELE,
//     EXP_ARRAY,
//     ARRAY_DEC,
//     EXT_DEC_LIST,
//     PARAM_LIST,
//     PARAM_DEC,
//     VAR_DEF,
//     DEC_LIST,
//     DEF_LIST,
//     COMP_STM,
//     STM_LIST,
//     EXP_STMT,
//     FOR_DEC,
//     IF_THEN,
//     IF_THEN_ELSE,
//     FUNC_CALL,
//     ARGS,
//     FUNCTION,
//     PARAM,
//     ARG,
//     CALL,
//     LABEL,
//     GOTO,
//     JLT,
//     JLE,
//     JGT,
//     JGE,
//     EQ,
//     NEQ
// };

struct ASTNode {
        //���¶Խ�����Զ���û�п��Ǵ洢Ч�ʣ�ֻ�Ǽ򵥵��г�Ҫ�õ���һЩ����
	int kind;
	union {
		char type_id[33];             //�ɱ�ʶ�����ɵ�Ҷ���
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
    char flag;       //���ű�ǣ�������'F'  ������'V'   ������'P'  ��ʱ������'T' �ṹ�壺'S'  �ṹ���Ա��'M'  ���飺'A'
    char offset;      //�ⲿ�����;ֲ��������侲̬����������¼�е�ƫ������
    char struct_name[33];
    // struct Array *arrayPtr;
    struct Struct *structPtr;
    int array_size;
        //���¼�������¼��С��Ŀ���������ʱʹ��
    //������ڵ�ʵ����ܻ��õ�������...
};
//���ű�
typedef struct symboltable{
    struct symbol symbols[MAXLENGTH];
    int index;
} symbolTable;
 
typedef struct symbol_scope_begin {
    //��ǰ������ķ����ڷ��ű����ʼλ�����,����һ��ջ�ṹ,��ʹ��˳�����Ϊ���ű�ʱ�����롢�˳�һ��������ʱ��Ҫ�������������ɷ��ű�Ĺ�����������ʽ�ķ��ű���һ����Ҫ�����ݽṹ
    int TX[30];
    int top;
} symbol_scope_TX;

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
int semantic_Analysis(struct ASTNode *T, int type, int level, char flag, int command);
void semantic_Analysis0(struct ASTNode *T);
int searchSymbolTable(char *name);
void semantic_error(int line,char *msg1,char *msg2);
void prn_symbol();
void DisplaySymbolTable();
int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset);
int fill_Temp(char *name, int level, int type, char flag, int offset);
int match_param(int i, struct ASTNode *T);