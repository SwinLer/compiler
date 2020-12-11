int a,b,c; //INT
float m,n; //FLOAT
char d; //CHAR
string s; // STRING
int array0[2]; //ARRAY
int array1[2][3]; //ARRAY

struct testStruct //STRUCT
{
    int a;
    int b;
};
struct testStruct ts;

//FUNCTION
int fibo(int a){
    /* test IF 
    and ||
    */
    if(a==1 || a==2) return 1;
    return fibo(a-1)+fibo(a-2);
}

int main(){
    int i;
    //fuzhi
    //int
    a=1; //10
    b=0xabcd; //16
    c=015; //8
    //float
    m=1.2;
    n=1.2E+10;
    //char
    d='c';
    //string
    s="string test";
    //array
    array0[0]=1;
    array1[0][0]=2;
    //struct
    ts.a=2;

    m=read();
    i=1;
    while (i<=m) //bijiao
    {
        n=fibo(i);
        write(n);
        i=i+1;

    } 
    while(a<1 && b>=2){
        continue;
    }
    for(i=0;i<10;i++){
        if(a!=10 || i==1){
            break;
        }
    }
    //suanshu
    m=3; 
    n=2+m;
    m=a-i;
    m=a/i;
    m=a*i;
    //zizheng
    i++;
    ++i;
    //zijian
    i--;
    i++;
    //fuhefuzhi
    i+=1;
    i-=1;
    i*=2;
    i/=2;
    i%=3;
    //switch
    switch (a)
    {
    case 1:break;
    case 2:break;
    default:break;
    }
    return 1;
    
}