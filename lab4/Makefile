result: grammarTree.l grammarTree.h grammarTree.y
	bison -d parser.y
	flex lex.l
	gcc -g -o parser ast.c parser.tab.c lex.yy.c semantic_analysis.c semantic_case.c code.c exp.c object_code.c