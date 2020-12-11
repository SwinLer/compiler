int c[3][3];
//3 Structure member name is repeated
struct testStruct
{
	int a;
	int b;
    char a;
};

int hello(int a,int b){
    a++;
    b++;
    return 1;
}

//3 function duplicate definition
int hello(){

}

//16 The function return value type does not match the return value 
//   type defined by the function
int hello2(){
    return 'c';
}

//17 Function has no return statement
int hello3(){
    int a;
}

int main()
{
    int b;
    struct testStruct ts;
    char b; //3 variable duplicate definition
    a=1; //1 undefined variable
    hell(); //2 undefined function
    b(); //4 Use function call form for non-function names
    b=hello; //5 Access the function name in the form of non-function call
    b=hello(1); //6 Too few parameters
    b=hello(1,2,3); //6 Too many parameters
    b=hello('1','2'); //7 Parameter type mismatch
    b[2];//8 Access non-array variables in the form of subscript variables
    c[0.1][0.2];//9 The subscript of the array variable is not an integer expression
    b.a;//10 Use member selection operator "." for non-structural variables
    ts.c;//11 Structure member does not exist
    b=2;
    1=b;//12 The left side of the assignment number is not an lvalue expression
    1++;//13 
    --1;//13 Increment and decrement non-lvalue expressions
    ++ts;//14 
    ts--;//14 Increment and decrement the structure variable
    b=1+'a';//15 Type mismatch
    if(b==1){
        break; //18 The break statement is not in a loop statement or switch statement
    }
    else{
        continue;//19 continue statement is not in loop statement
    }
}