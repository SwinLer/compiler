int a,b,c;
float m,n;
int fibo(int a){
    if(a==1 || a==2) return 1;
    return fibo(a-1)+fibo(a-2);
}
int main(){
    int m,n,i;
    char q;
    int w[2];
    w[0]=2;
    m=read();
    i=1;
    while (i<=m)
    {
        n=fibo(i);
        write(n);
        i=i+1;

    }
    i++;
    i+=1;
    ++i;
    return 1;
    
}