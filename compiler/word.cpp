#include<stdio.h>
#include <iostream>
#include<string.h>
#include <stdlib.h>
#include<ctype.h>
using  namespace std;
//保留字、运算符、分界符一符一类
#define CONSY 1                   //const
#define INTSY 2                   //int
#define ELSESY 3                  //else
#define VOIDSY 4                  //void
#define MAINSY 5                  //main
#define DOSY 6                    //do
#define WHISY 7                   //while
#define SWISY 8                   //switch
#define CASESY 9                  //case
#define IFSY 10                   //if
#define SCANSY 11                 //scanf
#define PRISY 12                  //printf
#define RTSY 13                   //return
#define IDSY 14                   //普通标识符
#define PLUSSY 15                 //+
#define MINUSSY 16                //-
#define STARSY 17                 //*
#define DIVSY 18                  //'/'
#define COMMASY 19                //,
#define COLONSY 20                //:
#define LPARSY 21                 //(
#define RPARSY 22                 //)
#define LBRASY 23                 //[
#define RBRASY 24                 //]
#define LBRASSY 25                //{
#define RBRASSY 26                //}
#define ASSIGNSY 27               //=
#define EQUSY 28                  //==
#define GTRSY 29                  //>
#define GTESY 30                  //>=
#define LTSY 31                   //<
#define LESY 32                   //<=
#define NEQSY 33                  //！=
#define DIGITSY 34                //无符号整数
#define CHAR2SY 35                //字符
#define STRINGSY 36               //字符串
#define SEMISY 37                 //;

//全局变量
const char CON[] = "const";
const char INT[] = "int";
const char ELSE[] = "else";
const char VOID[] = "void";
const char MAIN[] = "main";
const char DO[] = "do";
const char WHILE[] = "while";
const char SWITCH[] = "switch";
const char CASE[] = "case";
const char IF[] = "if";
const char SCANF[] = "scanf";
const char PRINTF[] = "printf";
const char RETURN[] = "return";
const int imax = 2147483647;
struct word{//单词定义
    int type;//单词类型
    string name;//单词的名字
    string value;//单词的值
    int line;//行号
};

char CHAR;//当前读入的字符
char token[50];//当前单词已读入字符的缓冲数组
char read[1000];//从文件读入字符的缓冲数组
int read_length = 0;//读入字符数组的长度
int read_index = 0;//待读入字符的指针
FILE* input_file;//读入的文件
int lineindex = 1;//line
word w[1000];//word list in order
int wordpoint = 0;
extern void error(int, int);

char get_ch(){//读入字符
    if(read_index == read_length){//该缓冲数组中的字符已经读完
        if(fgets(read,1000,input_file) != NULL){
            read_length = strlen(read);
            read_index = 0;
            return('\n');//读入行末尾的回车
        }
        else
            return(EOF);//文件中字符已经读取结束
    }
    else{
        read_index ++;
        return(read[read_index-1]);
    }
}

void reset_token(){//为下一次读入单词做准备
    int i;
    for(i=0;i<50;i++)
        token[i] = '\0';
    return;
}

long long trans_to_bina(){//将得到的字符串数字转化为二进制数
    int len,i;
    long long result;
    len = strlen(token);
    result = 0;
    for(i=0;i<len;i++){
        result = result*10 + token[i] - '0';
        if(result > imax){
            printf("%-20s%-20s\n", token, "单词错误");
            return 0;
        }
    }
    long long p=1,y=0,yushu;
    while(1){
        yushu = result%2;
        result /= 2;
        y += yushu*p;
        p *= 10;
        if( result < 2 ){
            y += result*p;
            break;
        }
    }
        return y;

}

void cat(char c){//将字符c加入单词已读入缓冲数组中
    int i=0;
    while(token[i] != '\0'){
        i++;
    }
    token[i] = c;
}

int is_letter(){//判断字符是否为字母（包括 _ ）
    return (isalpha(CHAR) || (CHAR == '_'));
}

int is_digit(){//判断字符是否为数字
    return isdigit(CHAR);
}

int is_colon(){//判断字符是否为 ：
    if(CHAR == ':')
        return 1;
    return 0;
}

int is_semi(){//判断字符是否为 ;
    if(CHAR == ';')
        return 1;
    return 0;
}

int is_comma(){//判断字符是否为 ,
    if(CHAR == ',')
        return 1;
    return 0;
}

int is_plus(){ //判断字符是否为 +
    if(CHAR == '+')
        return 1;
    return 0;
}

int is_minus(){//判断字符是否为 -
    if(CHAR == '-')
        return 1;
    return 0;
}

int is_star(){//判断字符是否为 *
    if(CHAR == '*')
        return 1;
    return 0;
}

int is_div(){//判断字符是否为 /
    if(CHAR == '/')
        return 1;
    return 0;
}

int is_squo(){//判断字符是否为 '
    if(CHAR == '\'')
        return 1;
    return 0;
}

int is_dquo(){//判断字符是否为 "
    if(CHAR == '\"')
        return 1;
    return 0;
}

int is_lparent(){//判断字符是否为 (
    if(CHAR == '\(')
        return 1;
    return 0;
}

int is_rparent(){//判断字符是否为 )
    if(CHAR == ')')
        return 1;
    return 0;
}

int is_lbracket(){ //判断字符是否为 [
    if(CHAR == '[')
        return 1;
    return 0;
}

int is_rbracket(){//判断字符是否为 ]
    if(CHAR == ']')
        return 1;
    return 0;
}

int is_lbrace(){ //判断字符是否为 {
    if(CHAR == '{')
        return 1;
    return 0;
}

int is_rbrace(){//判断字符是否为 }
    if(CHAR == '}')
        return 1;
    return 0;
}

int is_gtr(){// 判断字符是否为 >
    if(CHAR == '>')
        return 1;
    return 0;
}

int is_lt(){// 判断字符是否为 <
    if(CHAR == '<')
        return 1;
    return 0;
}

int is_equ(){// 判断字符是否为 =
    if(CHAR == '=')
        return 1;
    return 0;
}

int is_exc(){// 判断字符是否为 !
    if(CHAR == '!')
        return 1;
    return 0;
}

int is_zero(){ // 判断字符是否为 0
    if(CHAR == '0')
        return 1;
    return 0;
}

int is_valid_character(){ // 判断字符是否合法
    if(CHAR == 32)
        return 1;
    else if(CHAR == 33)
        return 1;
    else if((CHAR >= 35) && (CHAR <= 126))
        return 1;
    else
        return 0;
}

int is_key(){// 判断单词是否为保留字
    word newword;
    if(strcmp(token,CON) == 0)
        return CONSY;
    else if(strcmp(token,INT) == 0)
        return INTSY;
    else if(strcmp(token,VOID) == 0)
        return VOIDSY;
    else if(strcmp(token,MAIN) == 0)
        return MAINSY;
    else if(strcmp(token,DO) == 0)
        return DOSY;
    else if(strcmp(token,WHILE) == 0)
        return WHISY;
    else if(strcmp(token,SWITCH) == 0)
        return SWISY;
    else if(strcmp(token,CASE) == 0)
        return CASESY;
    else if(strcmp(token,IF) == 0)
        return IFSY;
    else if(strcmp(token,ELSE) == 0)
        return  ELSESY;
    else if(strcmp(token,SCANF) == 0)
        return SCANSY;
    else if(strcmp(token,PRINTF) == 0)
        return PRISY;
    else if(strcmp(token,RETURN) == 0)
        return RTSY;
    else
        return IDSY;//不是保留字，则是标识符
}

int lexanal(){//单词的分析程序，进入之前CHAR已经读入了字符
    reset_token();//清空单词缓存
    int type = 0;//保留单词的类型,0表示出错
    word newword;
    while((CHAR == ' ') || (CHAR == '\t') || (CHAR == '\n')){//过滤单词前的所有无用符号
        if(CHAR == '\n')
            lineindex ++;
        CHAR = get_ch();
        if(CHAR == EOF)
        {
            printf("end of the file\n");
            return 0;
        }
    }
    if(is_letter()) {//如果单词的首字符为字母
        do{
            cat(CHAR);
            CHAR = get_ch();
        }while(is_letter() || is_digit());//让该单词全部缓存到token中，再判断其是标识符还是保留字
        type = is_key();
    }
    else if(is_digit()){//如果首字符为数字，则需区分零和非零
        int iszero = 0;//判断是否为非零数字
        if(is_zero()){
            iszero = 1;
        }
        do{
            cat(CHAR);
            CHAR = get_ch();
        }while(is_digit());//如果字符为数字，则一直输入
        if((iszero == 1) && (strlen(token) > 1)){//首位为0的非零数字
            error(1, lineindex);//非零整数以0开头
        }
        else{
           type = 34;
        }
    }

else if(is_plus()){
        cat(CHAR);
        CHAR = get_ch();
       type = PLUSSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_minus()){
        cat(CHAR);
        CHAR = get_ch();
        type = MINUSSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_star()){
        cat(CHAR);
        CHAR = get_ch();
        type = STARSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_div()){
        cat(CHAR);
        CHAR = get_ch();
        type = DIVSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_gtr()){
        cat(CHAR);
        CHAR = get_ch();
        if(is_equ()){
            cat(CHAR);
            type = GTESY;
            CHAR = get_ch();
        }
        else
            type = GTRSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_lt()){
        cat(CHAR);
        CHAR = get_ch();
        if(is_equ()){
            cat(CHAR);
            type = LESY;
            CHAR = get_ch();
        }
        else
            type = LTSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_equ()){
        cat(CHAR);
        CHAR = get_ch();
        if(is_equ()){
            cat(CHAR);
            type = EQUSY;
            CHAR = get_ch();
        }
        else
            type = ASSIGNSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_exc()){
        cat(CHAR);
        CHAR = get_ch();
        if(is_equ()){
            cat(CHAR);
            type = NEQSY;
            CHAR = get_ch();
            //printf("%-20s%-20d%-20s\n", token, type, token);
        }
        else{
            //printf("%-20s%-20s\n", token, "单词错误");
        }
    }
    else if(is_lparent()){
        cat(CHAR);
        CHAR = get_ch();
        type = LPARSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_rparent()){
        cat(CHAR);
        CHAR = get_ch();
        type = RPARSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_lbracket()){
        cat(CHAR);
        CHAR = get_ch();
        type = LBRASY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_rbracket()){
        cat(CHAR);
        CHAR = get_ch();
        type = RBRASY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_lbrace()){
        cat(CHAR);
        CHAR = get_ch();
        type =LBRASSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_rbrace()){
        cat(CHAR);
        CHAR = get_ch();
        type =RBRASSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_comma()){
        cat(CHAR);
        CHAR = get_ch();
        type =COMMASY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_colon()){
        cat(CHAR);
        CHAR = get_ch();
        type =COLONSY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_semi()){
        cat(CHAR);
        CHAR = get_ch();
        type =SEMISY;
        //printf("%-20s%-20d%-20s\n", token, type, token);
    }
    else if(is_squo()){
        int ischar = 1;//判断''中是否为合法字符
        cat(CHAR);
        CHAR = get_ch();
        if(is_valid_character()){
            cat(CHAR);
        }
        else{
            ischar = 0;
        }
        CHAR = get_ch();
        if(is_squo()){
            if(ischar){//如果是合法符，即 '合法字符'
                cat(CHAR);
                CHAR = get_ch();
                type = CHAR2SY;
                //printf("%-20s%-20d%-20s\n", token, type, token);
            }
            else {//否则为 '非法字符' ，整体报错
                cat(CHAR);
                CHAR = get_ch();
                error(2, lineindex);
            }

        }
        else{
            if(ischar){//形式为 '字符字符
                read_index -=2;
                //printf("%-20s%-20s\n", token[0],"单词错误");
                error(3, lineindex);
                CHAR = get_ch();
            }
            else {//形式为 '非法字符 字符
                 read_index --;
                //printf("%-20s%-20s\n", token,"单词错误");
                 error(3, lineindex);
                CHAR = get_ch();
        }
    }
}
    else if(is_dquo()){
        cat(CHAR);
        CHAR = get_ch();
        while(is_valid_character()){
            if(CHAR == '\\')
                cat(CHAR);
            cat(CHAR);
            CHAR = get_ch();
        }
        if(is_dquo()){
            cat(CHAR);
            CHAR = get_ch();
            type = STRINGSY;
            //printf("%-20s%-20d%-20s\n", token, type, token);
        }
        else{
            error(4, lineindex);
            //printf("%-20s%-20s\n", token, "单词错误");
        }
    }
    else{
        error(5, lineindex);
        printf("%-20s%-20s\n", CHAR, "单词错误");
    }
    if(type != 0){
        newword.type = type;
        newword.line = lineindex%2 + lineindex/2;
        newword.value = token;
        newword.name = token;
        w[wordpoint++] = newword;
    }
}

word *word(){
    input_file = fopen("C:\\Users\\柳啸峰\\Desktop\\编译原理实验\\词法分析\\input.txt", "r");
    if(fgets(read,256,input_file) != NULL){//首次从文件输入
        printf("%-20s%-20s%-20s\n","单词","类别","值");
        read_length = strlen(read);
        read_index = 0;
        CHAR = get_ch();
        while(CHAR != EOF){//每次判断单词的首字符是否为空
            lexanal();
    }
    }
    return w;
}
