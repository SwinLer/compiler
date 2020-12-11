%{
#include "parser.tab.h"
#include "string.h"
#include "def.h"
int yycolumn=1;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;
typedef union {
	int type_int; 
	char type_char[5];
	float type_float;
	char type_id[32];
	char type_string[32];
	char struct_name[32];
	struct node *ptr;
} YYLVAL;
#define YYSTYPE YYLVAL

%}
%option yylineno

ID   [A-Za-z_]([A-Za-z0-9_])*  
INT_DEX [1-9][0-9]*|[0]
INT_HEX [0][Xx]([1-9a-fA-F][0-9a-fA-F]*|[0])
INT_OCT [0][0-7]*
FLOAT [0-9]*[.][0-9]+([eE][-+]?([1-9][0-9*]|[0]))?
CHAR \'(\\.|[^'])\'
STRING \"(\\.|[^"\\])*\"


%%
{INT_DEX} {printf("(CONSTANTINT, %s)\n", yytext);yylval.type_int=atoi(yytext); return INT;}
{INT_HEX} {//十六进制
	char sub[20] = {'\0'};
	int s = 0;
	int base = 1;
	printf("(CONSTANTINTH, %s)\n", yytext); 
	strncpy(sub, yytext+2, strlen(yytext)-2);
	int i;
	for(i = strlen(sub)-1; i >= 0;i--){
		if(sub[i] >= '0' && sub[i] <= '9'){
			s += (sub[i] - '0') * base;
		}
		else if(sub[i] >= 'a' && sub[i] <= 'f'){
			s += (sub[i] - 'a' + 10) * base;
		}
		else if(sub[i] >= 'A' && sub[i] <= 'F'){
			s += (sub[i] - 'A' + 10) * base;
		}
		base *= 16;
	}
	yylval.type_int = s;
	return INT;
}
{INT_OCT} {//八进制
	char sub[20] = {'\0'};
	int s = 0;
	int base = 1;
	printf("(CONSTANTINTO, %s)\n", yytext); 
	strncpy(sub, yytext+1, strlen(yytext)-1);
	int i;
	for(i = strlen(sub)-1; i >= 0;i--){
		if(sub[i] >= '0' && sub[i] <= '7'){
			s += (sub[i] - '0') * base;
		}
		base *= 8;
	}
	yylval.type_int = s; 
	return INT;
}
{FLOAT}  {printf("(CONSTANTFLOAT, %s)\n", yytext);yylval.type_float=atof(yytext); return FLOAT;}
{CHAR}   {printf("(CONSTANTCHAR, %s)\n", yytext);strcpy(yylval.type_char,yytext);return CHAR;}
{STRING} {printf("(CONSTANTSTRING, %s)\n", yytext);strcpy(yylval.type_string,yytext);return STRING;}
"int"    {printf("(TYPEINT, %s)\n", yytext);strcpy(yylval.type_id, yytext);return TYPE;}
"float"  {printf("(TYPEFLOAT, %s)\n", yytext);strcpy(yylval.type_id, yytext);return TYPE;}
"char"   {printf("(TYPECHAR, %s)\n", yytext);strcpy(yylval.type_id, yytext);return TYPE;}
"string" {printf("(TYPESTRING, %s)\n", yytext);strcpy(yylval.type_id, yytext);return TYPE;}
"void"   {printf("(TYPEVOID, %s)\n", yytext);strcpy(yylval.type_id, yytext);return TYPE;}
"struct" {printf("(TYPESTRUCT, %s)\n", yytext);return STRUCT;}
"return" {printf("(RETURN, %s)\n", yytext);return RETURN;}
"if"     {printf("(IF, %s)\n", yytext);return IF;}
"else"   {printf("(ELSE, %s)\n", yytext);return ELSE;}
"while"  {printf("(WHILE, %s)\n", yytext);return WHILE;}
"for"    {printf("(FOR, %s)\n", yytext); return FOR;}
"continue" {printf("(CONTINUE, %s)\n", yytext);return CONTINUE;}
"break"  {printf("(BREAK, %s)\n", yytext); return BREAK;}
"switch" {printf("(SWITCH, %s)\n", yytext);return SWITCH;}
"case"   {printf("(CASE, %s)\n", yytext);return CASE;}
"default" {printf("(DEFAULT, %s)\n", yytext);return DEFAULT;}


{ID}     {printf("(INDENTIFIER, %s)\n", yytext);strcpy(yylval.type_id,  yytext); return ID;}
";"		 {printf("(SEMI, %s)\n", yytext);return SEMI;}
":"      {printf("(COLON, %s)\n", yytext);return COLON;}
","		 {printf("(COMMA, %s)\n", yytext);return COMMA;}
"."      {printf("(DOT, %s)\n", yytext);return DOT;}
">"|"<"|">="|"<="|"=="|"!=" {printf("(RELOP, %s)\n", yytext);strcpy(yylval.type_id, yytext);return RELOP;}
"="		 {printf("(ASSIGNOP, %s)\n", yytext);return ASSIGNOP;}
"+"		 {printf("(PLUSOP, %s)\n", yytext);return PLUS;}
"-"		 {printf("(MINUSOP, %s)\n", yytext);return MINUS;}
"*"		 {printf("(STAROP, %s)\n", yytext);return STAR;}
"/"		 {printf("(DIVOP, %s)\n", yytext);return DIV;}
"%"      {printf("(MODOP, %s)\n", yytext);return MOD;}
"&&"     {printf("(ANDOP, %s)\n", yytext);return AND;}
"||"     {printf("(OROP, %s)\n", yytext);return OR;}
"!"		 {printf("(NOTOP, %s)\n", yytext);return NOT;}
"++"     {printf("(AUTOPLUSOP, %s)\n", yytext); return AUTOPLUS;}
"--"     {printf("(AUTOMINUSOP, %s)\n", yytext); return AUTOMINUS;}
"+="     {printf("(PLUSASSIGNOP, %s)\n", yytext); return PLUSASSIGNOP;}
"-="     {printf("(MINUSASSIGNOP, %s)\n", yytext); return MINUSASSIGNOP;}
"*="     {printf("(STARASSIGNOP, %s)\n", yytext);return STARASSIGNOP;}
"/="     {printf("(DIVASSIGNOP, %s)\n)", yytext);return DIVASSIGNOP;}
"%="     {printf("(MODASSIGNOP, %s)\n", yytext);return MODASSIGNOP;}
"("		 {printf("(LP, %s)\n", yytext);return LP;}
")"		 {printf("(RP, %s)\n", yytext);return RP;}
"{"		 {printf("(LC, %s)\n", yytext);return LC;}
"}"		 {printf("(RC, %s)\n", yytext);return RC;}
"["      {printf("(LB, %s)\n", yytext);return LB;}
"]"      {printf("(RB, %s)\n", yytext);return RB;}
[\n]     {printf("(EOL, \\n)\n");yycolumn=1;}   
[ \r\t]  {}   
.		 {printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n",yytext,yylineno);}
[\/][\/].* {}//匹配行注释
[\/][\*][^\*]*[\*]+([^\*\/][^\*]*[\*]+)*[\/] {}//匹配块注释

%%

/* 和bison联用时，不需要这部分
void main()
{
yylex();
return 0;
}

*/
int yywrap()
{
    return 1;
}