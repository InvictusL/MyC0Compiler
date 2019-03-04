#include<stdio.h>
#include <iostream>
#include<string.h>
#include <stdlib.h>
#include<ctype.h>
using  namespace std;
//�����֡���������ֽ��һ��һ��
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
#define IDSY 14                   //��ͨ��ʶ��
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
#define NEQSY 33                  //��=
#define DIGITSY 34                //�޷�������
#define CHAR2SY 35                //�ַ�
#define STRINGSY 36               //�ַ���
#define SEMISY 37                 //;

//ȫ�ֱ���
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
struct word{//���ʶ���
    int type;//��������
    string name;//���ʵ�����
    string value;//���ʵ�ֵ
    int line;//�к�
};

char CHAR;//��ǰ������ַ�
char token[50];//��ǰ�����Ѷ����ַ��Ļ�������
char read[1000];//���ļ������ַ��Ļ�������
int read_length = 0;//�����ַ�����ĳ���
int read_index = 0;//�������ַ���ָ��
FILE* input_file;//������ļ�
int lineindex = 1;//line
word w[1000];//word list in order
int wordpoint = 0;
extern void error(int, int);

char get_ch(){//�����ַ�
    if(read_index == read_length){//�û��������е��ַ��Ѿ�����
        if(fgets(read,1000,input_file) != NULL){
            read_length = strlen(read);
            read_index = 0;
            return('\n');//������ĩβ�Ļس�
        }
        else
            return(EOF);//�ļ����ַ��Ѿ���ȡ����
    }
    else{
        read_index ++;
        return(read[read_index-1]);
    }
}

void reset_token(){//Ϊ��һ�ζ��뵥����׼��
    int i;
    for(i=0;i<50;i++)
        token[i] = '\0';
    return;
}

long long trans_to_bina(){//���õ����ַ�������ת��Ϊ��������
    int len,i;
    long long result;
    len = strlen(token);
    result = 0;
    for(i=0;i<len;i++){
        result = result*10 + token[i] - '0';
        if(result > imax){
            printf("%-20s%-20s\n", token, "���ʴ���");
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

void cat(char c){//���ַ�c���뵥���Ѷ��뻺��������
    int i=0;
    while(token[i] != '\0'){
        i++;
    }
    token[i] = c;
}

int is_letter(){//�ж��ַ��Ƿ�Ϊ��ĸ������ _ ��
    return (isalpha(CHAR) || (CHAR == '_'));
}

int is_digit(){//�ж��ַ��Ƿ�Ϊ����
    return isdigit(CHAR);
}

int is_colon(){//�ж��ַ��Ƿ�Ϊ ��
    if(CHAR == ':')
        return 1;
    return 0;
}

int is_semi(){//�ж��ַ��Ƿ�Ϊ ;
    if(CHAR == ';')
        return 1;
    return 0;
}

int is_comma(){//�ж��ַ��Ƿ�Ϊ ,
    if(CHAR == ',')
        return 1;
    return 0;
}

int is_plus(){ //�ж��ַ��Ƿ�Ϊ +
    if(CHAR == '+')
        return 1;
    return 0;
}

int is_minus(){//�ж��ַ��Ƿ�Ϊ -
    if(CHAR == '-')
        return 1;
    return 0;
}

int is_star(){//�ж��ַ��Ƿ�Ϊ *
    if(CHAR == '*')
        return 1;
    return 0;
}

int is_div(){//�ж��ַ��Ƿ�Ϊ /
    if(CHAR == '/')
        return 1;
    return 0;
}

int is_squo(){//�ж��ַ��Ƿ�Ϊ '
    if(CHAR == '\'')
        return 1;
    return 0;
}

int is_dquo(){//�ж��ַ��Ƿ�Ϊ "
    if(CHAR == '\"')
        return 1;
    return 0;
}

int is_lparent(){//�ж��ַ��Ƿ�Ϊ (
    if(CHAR == '\(')
        return 1;
    return 0;
}

int is_rparent(){//�ж��ַ��Ƿ�Ϊ )
    if(CHAR == ')')
        return 1;
    return 0;
}

int is_lbracket(){ //�ж��ַ��Ƿ�Ϊ [
    if(CHAR == '[')
        return 1;
    return 0;
}

int is_rbracket(){//�ж��ַ��Ƿ�Ϊ ]
    if(CHAR == ']')
        return 1;
    return 0;
}

int is_lbrace(){ //�ж��ַ��Ƿ�Ϊ {
    if(CHAR == '{')
        return 1;
    return 0;
}

int is_rbrace(){//�ж��ַ��Ƿ�Ϊ }
    if(CHAR == '}')
        return 1;
    return 0;
}

int is_gtr(){// �ж��ַ��Ƿ�Ϊ >
    if(CHAR == '>')
        return 1;
    return 0;
}

int is_lt(){// �ж��ַ��Ƿ�Ϊ <
    if(CHAR == '<')
        return 1;
    return 0;
}

int is_equ(){// �ж��ַ��Ƿ�Ϊ =
    if(CHAR == '=')
        return 1;
    return 0;
}

int is_exc(){// �ж��ַ��Ƿ�Ϊ !
    if(CHAR == '!')
        return 1;
    return 0;
}

int is_zero(){ // �ж��ַ��Ƿ�Ϊ 0
    if(CHAR == '0')
        return 1;
    return 0;
}

int is_valid_character(){ // �ж��ַ��Ƿ�Ϸ�
    if(CHAR == 32)
        return 1;
    else if(CHAR == 33)
        return 1;
    else if((CHAR >= 35) && (CHAR <= 126))
        return 1;
    else
        return 0;
}

int is_key(){// �жϵ����Ƿ�Ϊ������
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
        return IDSY;//���Ǳ����֣����Ǳ�ʶ��
}

int lexanal(){//���ʵķ������򣬽���֮ǰCHAR�Ѿ��������ַ�
    reset_token();//��յ��ʻ���
    int type = 0;//�������ʵ�����,0��ʾ����
    word newword;
    while((CHAR == ' ') || (CHAR == '\t') || (CHAR == '\n')){//���˵���ǰ���������÷���
        if(CHAR == '\n')
            lineindex ++;
        CHAR = get_ch();
        if(CHAR == EOF)
        {
            printf("end of the file\n");
            return 0;
        }
    }
    if(is_letter()) {//������ʵ����ַ�Ϊ��ĸ
        do{
            cat(CHAR);
            CHAR = get_ch();
        }while(is_letter() || is_digit());//�øõ���ȫ�����浽token�У����ж����Ǳ�ʶ�����Ǳ�����
        type = is_key();
    }
    else if(is_digit()){//������ַ�Ϊ���֣�����������ͷ���
        int iszero = 0;//�ж��Ƿ�Ϊ��������
        if(is_zero()){
            iszero = 1;
        }
        do{
            cat(CHAR);
            CHAR = get_ch();
        }while(is_digit());//����ַ�Ϊ���֣���һֱ����
        if((iszero == 1) && (strlen(token) > 1)){//��λΪ0�ķ�������
            error(1, lineindex);//����������0��ͷ
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
            //printf("%-20s%-20s\n", token, "���ʴ���");
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
        int ischar = 1;//�ж�''���Ƿ�Ϊ�Ϸ��ַ�
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
            if(ischar){//����ǺϷ������� '�Ϸ��ַ�'
                cat(CHAR);
                CHAR = get_ch();
                type = CHAR2SY;
                //printf("%-20s%-20d%-20s\n", token, type, token);
            }
            else {//����Ϊ '�Ƿ��ַ�' �����屨��
                cat(CHAR);
                CHAR = get_ch();
                error(2, lineindex);
            }

        }
        else{
            if(ischar){//��ʽΪ '�ַ��ַ�
                read_index -=2;
                //printf("%-20s%-20s\n", token[0],"���ʴ���");
                error(3, lineindex);
                CHAR = get_ch();
            }
            else {//��ʽΪ '�Ƿ��ַ� �ַ�
                 read_index --;
                //printf("%-20s%-20s\n", token,"���ʴ���");
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
            //printf("%-20s%-20s\n", token, "���ʴ���");
        }
    }
    else{
        error(5, lineindex);
        printf("%-20s%-20s\n", CHAR, "���ʴ���");
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
    input_file = fopen("C:\\Users\\��Х��\\Desktop\\����ԭ��ʵ��\\�ʷ�����\\input.txt", "r");
    if(fgets(read,256,input_file) != NULL){//�״δ��ļ�����
        printf("%-20s%-20s%-20s\n","����","���","ֵ");
        read_length = strlen(read);
        read_index = 0;
        CHAR = get_ch();
        while(CHAR != EOF){//ÿ���жϵ��ʵ����ַ��Ƿ�Ϊ��
            lexanal();
    }
    }
    return w;
}
