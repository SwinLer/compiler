%error-verbose
%locations
%{
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "def.h"
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
void yyerror(const char* fmt, ...);
void display(struct ASTNode *,int);
%}
 
%union {
	int    type_int;
        char   type_char[5];
	float  type_float;
	char   type_id[32];
        char   type_string[32];
        char   struct_name[32];
	struct ASTNode *ptr;
};

//  %type 定义非终结符的语义值类型
%type  <ptr> program ExtDefList ExtDef  Specifier ExtDecList FuncDec CompSt VarList VarDec ParamDec Stmt StmList DefList Def DecList Dec Exp Args Arraylist ForDec StructSpecifier OptTag Tag CaseType

//% token 定义终结符的语义值类型
%token <type_char> CHAR
%token <type_int> INT              /*指定INT的语义值是type_int，有词法分析得到的数值*/
%token <type_id> ID  RELOP TYPE    /*指定ID,RELOP 的语义值是type_id，有词法分析得到的标识符字符串*/
%token <type_float> FLOAT          /*指定ID的语义值是type_id，有词法分析得到的标识符字符串*/
%token <type_string> STRING

%token STRUCT
%token DPLUS LP RP LC RC LB RB SEMI COMMA DOT     /*用bison对该文件编译时，带参数-d，生成的.tab.h中给这些单词进行编码，可在lex.l中包含parser.tab.h使用这些单词种类码*/
%token PLUS MINUS STAR DIV MOD ASSIGNOP PLUSASSIGNOP MINUSASSIGNOP STARASSIGNOP DIVASSIGNOP MODASSIGNOP AND OR NOT AUTOPLUS AUTOMINUS IF ELSE WHILE FOR RETURN COLON DEFAULT CONTINUE BREAK VOID SWITCH CASE
/*以下为接在上述token后依次编码的枚举常量，作为AST结点类型标记*/
%token EXT_DEF_LIST EXT_VAR_DEF FUNC_DEF FUNC_DEC EXT_DEC_LIST PARAM_LIST PARAM_DEC VAR_DEF DEC_LIST DEF_LIST COMP_STM STM_LIST EXP_STMT IF_THEN IF_THEN_ELSE ROOT ARRAY_LIST ARRAY_ID
%token FUNC_CALL ARGS FUNCTION PARAM ARG CALL LABEL GOTO JLT JLE JGT JGE EQ NEQ FOR_DEC STRUCT_DEF STRUCT_TAG EXP_ELE
%token SWITCH_STMT CASE_STMT DEFAULT_STMT

%left ASSIGNOP 
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV MOD
%left DOT
%right AUTOPLUS AUTOMINUS
%left LB RB
%right PLUSASSIGNOP MINUSASSIGNOP STARASSIGNOP DIVASSIGNOP MODASSIGNOP
%right UMINUS NOT DPLUS

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%

program: ExtDefList    { $$=mknode(1,ROOT,yylineno,$1);display($$,3);}     //显示语法树,语义分析
         ; 
/*外部定义列表 整个语法树*/
ExtDefList: {$$=NULL;}
          | ExtDef ExtDefList {$$=mknode(2,EXT_DEF_LIST,yylineno,$1,$2);}   //每一个EXTDEFLIST的结点，其第1棵子树对应一个外部变量声明或函数
          ;  

/*外部声明，声明外部变量或者声明函数*/
ExtDef:   Specifier ExtDecList SEMI   {$$=mknode(2,EXT_VAR_DEF,yylineno,$1,$2);}   //该结点对应一个外部变量声明
        | Specifier SEMI 
        | Specifier FuncDec CompSt    {$$=mknode(3,FUNC_DEF,yylineno,$1,$2,$3);}         //该结点对应一个函数定义
        | error SEMI   { $$ = NULL; fprintf(stderr, "Grammar Error at Line %d Column %d：",yylloc.first_line,yylloc.first_column);}
         ;

/*表示一个类型*/
Specifier:  TYPE    {$$=mknode(0,TYPE,yylineno);strcpy($$->type_id,$1);$$->type=!strcmp($1,"int")?INT:FLOAT;}   
          | StructSpecifier {}
           ;  

/*结构体*/    
StructSpecifier: STRUCT OptTag LC DefList RC {$$=mknode(2, STRUCT_DEF, yylineno, $2,$4);}
               | STRUCT Tag {$$=mknode(1, STRUCT_DEF, yylineno, $2);}
               ;
OptTag: {$$=NULL;}
       | ID {$$=mknode(0, STRUCT_TAG, yylineno);strcpy($$->struct_name, $1);}
       ;
Tag: ID {$$=mknode(0, STRUCT_TAG, yylineno); strcpy($$->struct_name, $1);}
        ;

/*变量名称列表，由一个或多个变量组成，多个变量之间用逗号隔开*/
ExtDecList:  VarDec      {$$=mknode(1,EXT_DEC_LIST,yylineno,$1);}       /*每一个EXT_DECLIST的结点，其第一棵子树对应一个变量名(ID类型的结点),第二棵子树对应剩下的外部变量名*/
           | VarDec COMMA ExtDecList {$$=mknode(2,EXT_DEC_LIST,yylineno,$1,$3);}
           ;  

/*变量名称，由一个ID组成*/
VarDec:  ID          {$$=mknode(0,ID,yylineno);strcpy($$->type_id,$1);}    //ID结点，标识符符号串存放结点的type_id
       | ID Arraylist {$$=mknode(1,ARRAY_LIST,yylineno,$2);strcpy($$->type_id,$1);} 
       ;

/*数组*/
Arraylist:  LB Exp RB                  {$$=mknode(1,ARRAY_LIST,yylineno,$2);}
          | LB Exp RB Arraylist       {$$=mknode(2,ARRAY_LIST,yylineno,$2,$4);} // 多维数组
        ;

/*函数名 参数*/
FuncDec: ID LP VarList RP   {$$=mknode(1,FUNC_DEC,yylineno,$3);strcpy($$->type_id,$1);}//函数名存放在$$->type_id
       | ID LP  RP   {$$=mknode(0,FUNC_DEC,yylineno);strcpy($$->type_id,$1);$$->ptr[0]=NULL;}//函数名存放在$$->type_id
        ;  

/*参数定义列表，有一个到多个参数定义组成，用逗号隔开*/       
VarList: ParamDec  {$$=mknode(1,PARAM_LIST,yylineno,$1);}
       | ParamDec COMMA  VarList  {$$=mknode(2,PARAM_LIST,yylineno,$1,$3);}
        ;

/*参数定义，固定有一个类型和一个变量组成*/
ParamDec: Specifier VarDec         {$$=mknode(2,PARAM_DEC,yylineno,$1,$2);}
         ;

/*复合语句，左右分别用大括号括起来，中间有定义列表和语句列表*/
CompSt: LC DefList StmList RC    {$$=mknode(2,COMP_STM,yylineno,$2,$3);}
       ;

/*语句列表，由0个或多个语句stmt组成*/
StmList: {$$=NULL; }  
        | Stmt StmList  {$$=mknode(2,STM_LIST,yylineno,$1,$2);}
        ;

/*语句，可能为表达式，复合语句，return语句，if语句，if-else语句，while语句，for，switch*/
Stmt:   Exp SEMI    {$$=mknode(1,EXP_STMT,yylineno,$1);}
      | CompSt      {$$=$1;}      //复合语句结点直接最为语句结点，不再生成新的结点
      | RETURN Exp SEMI   {$$=mknode(1,RETURN,yylineno,$2);}
      | IF LP Exp RP Stmt %prec LOWER_THEN_ELSE   {$$=mknode(2,IF_THEN,yylineno,$3,$5);}
      | IF LP Exp RP Stmt ELSE Stmt   {$$=mknode(3,IF_THEN_ELSE,yylineno,$3,$5,$7);}
      | WHILE LP Exp RP Stmt {$$=mknode(2,WHILE,yylineno,$3,$5);}
      | FOR LP ForDec RP Stmt {$$=mknode(2,FOR,yylineno,$3,$5);}
      | CONTINUE SEMI {$$=mknode(0,CONTINUE, yylineno);}
      | BREAK SEMI {$$=mknode(0,BREAK,yylineno);}
      | SWITCH LP Exp RP Stmt {$$=mknode(2,SWITCH_STMT,yylineno,$3,$5);}
      | CASE CaseType COLON Stmt {$$=mknode(2,CASE_STMT, yylineno,$2,$4);}
      | DEFAULT COLON Stmt {$$=mknode(1,DEFAULT_STMT, yylineno,$3);}
      ;

/*case*/
CaseType: INT {$$=mknode(0,INT,yylineno);$$->type_int=$1;$$->type=INT;}
        | CHAR {$$=mknode(0,CHAR,yylineno);strcpy($$->type_char, $1);$$->type=CHAR;}
        ;

/*for*/
ForDec: Exp SEMI Exp SEMI Exp {$$=mknode(3, FOR_DEC, yylineno, $1, $3, $5);}
      | SEMI Exp SEMI {$$=mknode(1,FOR_DEC, yylineno, $2);}
        ;

/*定义列表，由0个或多个定义语句组成*/
DefList: {$$=NULL; }
        | Def DefList {$$=mknode(2,DEF_LIST,yylineno,$1,$2);}
        | error SEMI   {$$=NULL;fprintf(stderr, "Grammar Error at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);}
        ;

/*定义一个或多个语句语句，由分号隔开*/
Def:    Specifier DecList SEMI {$$=mknode(2,VAR_DEF,yylineno,$1,$2);}
        ;

/*语句列表，由一个或多个语句组成，由逗号隔开，最终都成一个表达式*/
DecList: Dec  {$$=mknode(1,DEC_LIST,yylineno,$1);}
       | Dec COMMA DecList  {$$=mknode(2,DEC_LIST,yylineno,$1,$3);}
	   ;

/*语句，一个变量名称或者一个赋值语句（变量名称等于一个表达式）*/
Dec:     VarDec  {$$=$1;}
       | VarDec ASSIGNOP Exp  {$$=mknode(2,ASSIGNOP,yylineno,$1,$3);strcpy($$->type_id,"ASSIGNOP");}
       ;

/*表达式*/
Exp:    Exp ASSIGNOP Exp {$$=mknode(2,ASSIGNOP,yylineno,$1,$3);strcpy($$->type_id,"ASSIGNOP");}//$$结点type_id空置未用，正好存放运算符
      | Exp PLUSASSIGNOP Exp {$$=mknode(2,PLUSASSIGNOP,yylineno,$1,$3);strcpy($$->type_id, "PLUSASSIGNOP");}
      | Exp MINUSASSIGNOP Exp {$$=mknode(2,MINUSASSIGNOP,yylineno,$1,$3);strcpy($$->type_id, "MINUSASSIGNOP");}
      | Exp STARASSIGNOP Exp {$$=mknode(2, STARASSIGNOP,yylineno,$1,$3);strcpy($$->type_id,"STARASSIGNOP");}
      | Exp DIVASSIGNOP Exp {$$=mknode(2,DIVASSIGNOP,yylineno,$1,$3);strcpy($$->type_id,"DIVASSIGNOP");}
      | Exp MODASSIGNOP Exp {$$=mknode(2,MODASSIGNOP,yylineno,$1,$3);strcpy($$->type_id, "MODASSIGNOP");}
      | Exp AND Exp   {$$=mknode(2,AND,yylineno,$1,$3);strcpy($$->type_id,"AND");}
      | Exp OR Exp    {$$=mknode(2,OR,yylineno,$1,$3);strcpy($$->type_id,"OR");}
      | Exp RELOP Exp {$$=mknode(2,RELOP,yylineno,$1,$3);strcpy($$->type_id,$2);}  //词法分析关系运算符号自身值保存在$2中
      | Exp PLUS Exp  {$$=mknode(2,PLUS,yylineno,$1,$3);strcpy($$->type_id,"PLUS");}
      | Exp MINUS Exp {$$=mknode(2,MINUS,yylineno,$1,$3);strcpy($$->type_id,"MINUS");}
      | Exp STAR Exp  {$$=mknode(2,STAR,yylineno,$1,$3);strcpy($$->type_id,"STAR");}
      | Exp DIV Exp   {$$=mknode(2,DIV,yylineno,$1,$3);strcpy($$->type_id,"DIV");}
      | Exp MOD Exp   {$$=mknode(2,MOD,yylineno,$1,$3);strcpy($$->type_id,"MOD");}
      | Exp AUTOPLUS  {$$=mknode(1,AUTOPLUS,yylineno,$1);strcpy($$->type_id, "RPAUTOPLUS");}
      | Exp AUTOMINUS {$$=mknode(1,AUTOMINUS,yylineno,$1);strcpy($$->type_id,"RPAUTOMINUS");}
      | AUTOPLUS Exp  {$$=mknode(1,AUTOPLUS,yylineno,$2);strcpy($$->type_id, "LPAUTOPLUS");}
      | AUTOMINUS Exp {$$=mknode(1,AUTOMINUS,yylineno,$2);strcpy($$->type_id, "LPAUTOMINUS");}
      | LP Exp RP     {$$=$2;}
      | MINUS Exp %prec UMINUS   {$$=mknode(1,UMINUS,yylineno,$2);strcpy($$->type_id,"UMINUS");}
      | NOT Exp       {$$=mknode(1,NOT,yylineno,$2);strcpy($$->type_id,"NOT");}
      | DPLUS  Exp      {$$=mknode(1,DPLUS,yylineno,$2);strcpy($$->type_id,"DPLUS");}
      |   Exp DPLUS      {$$=mknode(1,DPLUS,yylineno,$1);strcpy($$->type_id,"DPLUS");}
      | ID LP Args RP {$$=mknode(1,FUNC_CALL,yylineno,$3);strcpy($$->type_id,$1);}
      | ID LP RP      {$$=mknode(0,FUNC_CALL,yylineno);strcpy($$->type_id,$1);}
      | ID            {$$=mknode(0,ID,yylineno);strcpy($$->type_id,$1);}
      | INT           {$$=mknode(0,INT,yylineno);$$->type_int=$1;$$->type=INT;}
      | CHAR          {$$=mknode(0,CHAR,yylineno);strcpy($$->type_char, $1);$$->type=CHAR;}
      | FLOAT         {$$=mknode(0,FLOAT,yylineno);$$->type_float=$1;$$->type=FLOAT;}
      | STRING        {$$=mknode(0,STRING,yylineno);strcpy($$->type_string,$1);$$->type=STRING}
      | ID Arraylist  {$$=mknode(1,ARRAY_ID,yylineno,$2);strcpy($$->type_id,$1);}
      | Exp DOT ID    {$$=mknode(1,EXP_ELE,yylineno,$1); strcpy($$->type_id,$3);}
      | {$$=NULL;}
      ; 

/*逗号隔开参数*/
Args:    Exp COMMA Args    {$$=mknode(2,ARGS,yylineno,$1,$3);}
       | Exp               {$$=mknode(1,ARGS,yylineno,$1);}
       ;
       
%%

int main(int argc, char *argv[]){
        yyin=fopen(argv[1],"r");
	if (!yyin) 
        return 0;
	yylineno=1;
	yyparse();
	return 0;
}

void yyerror(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Grammar Error at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}