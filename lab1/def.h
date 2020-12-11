#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "parser.tab.h"
#define MAXLENGTH   200
#define DX 3*sizeof(int)          /*���¼������Ϣ��Ҫ�ĵ�Ԫ�����������ʵ��ϵͳ����*/
//�����﷨��������͡�����ַ������͵ȶ�������ο���ʵ��ʱһ��Ҫ�����Լ������������
int LEV;      //���

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


struct ASTNode * mknode(int num,int kind,int pos,...);