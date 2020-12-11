#include "def.h"
#include "parser.tab.h"
#define OFFSET 3

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
    case ROOT:
        printf("ROOT:\n");
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
    case DEFAULT_STMT:
        printf("%*cDEFAULT��䣺(%d)\n", indent, ' ', T->pos);
        display(T->ptr[0], indent+OFFSET);
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