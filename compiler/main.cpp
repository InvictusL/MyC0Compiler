#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <stdlib.h>
using namespace std;
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
struct word{//单词定义
    int type;//单词类型
    string name;//单词的名字
    string value;//单词的值
    int line;//行号
};

struct symbol{//符号表表项，存储类型和名字 count函数以b<a和b>a都不成立来判断b=a
    string name;
    int type;//常量为0，变量为1，函数为2
    int value;//常量和变量为值，函数为首地址
    int addr;//符号在该层的偏移地址，函数名 则为参数个数
    friend bool operator < (const symbol &a, const symbol &b){
        return  !(a.name == b.name);
    }
};

struct procable{
    int upper;//直接上层分程序
    set<symbol> s;//当前层符号表

};

struct pcode{
    string ins;//指令
    int op1;//操作数1，变量层级
    int op2;//操作数2，变量偏移地址
};

word *wordlist;//输入程序的单词流
int wordindex = -1;//当前分析单词的指针
int tempindex;//回溯的单词指针
word currentword;
int level = 0;//定义层级，每有一次分程序就加一
int currentlevel = 0;//当前运行层级
procable pt[20];
vector<int> stack;//层级栈，该层开始时入栈，结束时出栈，次栈顶的层级为该层的直接外层
int pcodeindex = 0;//下一条pcode指令地址
pcode pc[1000];
int starting = 0;//开始指令地址

void getword();//读入单词
void program();//程序
void consystate();//常量说明部分
void variablestate(int &vn);//变量说明部分,参数为变量定义基址
void funcdec();//函数定义
void maindec();//主函数
void headofdec(int &vn);//声明头部,参数为该层基址
void consydec();//常量定义
void parameter(int &num);//参数,返回参数个数
void paramlist(int &num);//参数表，返回参数个数
void complstate(int &start);//复合语句,参数为内定义变量起始地址
void statelist();//语句序列
void state();//语句
void ifstate();//条件语句
void whilestate();//循环语句
int funccall();//函数调用语句
int condition();//条件
int expression();//表达式
int item();//项
int factor();//因子
void valuelist(int &num);//值参数表
void assignstate();//赋值语句
int returnstate();//返回语句,返回RET pcode的地址
void scanfstate();//读语句
void printfstate();//写语句
bool resettable(int l, symbol s);//赋值，重新插入符号表
bool settable(int l, symbol s);
bool checktable(int &le, int  &addr, int l, symbol s, int &type);//查询，返回层级和偏移地址
extern void error(int, int);//错误输出
extern word *word();//词法分析

bool resettable(int l, symbol sym){//赋值，在l层及上层查符号sym的声明
    bool isfind = false;
    set<symbol>::iterator it;
    while((!isfind)){
        for(it = pt[l].s.begin(); it != pt[l].s.end(); it ++){
            if(it->name == sym.name && it->type == sym.type){
                isfind = true;
                break;
            }
        }
        if(isfind)
            break;
        if(pt[l].upper == -1){
            error(40, currentword.line);
            return false;//查找失败
        }
        else
            l = pt[l].upper;
    }
    pt[l].s.erase(it);
    if(pt[l].s.insert(sym).second){
        //printf("重新插入成功！\n");
    }

    return true;
}

bool checkfunc(int &le, int &addr, symbol sym, int &type){//查找函数
    printf("%d", level);
    for(int l = 1; l <= level; l ++){
        if(pt[l].upper == 0){//如果该层的直接上层为程序
            for(set<symbol>::iterator it = pt[l].s.begin(); it != pt[l].s.end(); it ++){
                if(it->name == sym.name && it->type == 2){
                    le = l;
                    addr = it->addr;
                    type = it->type;
                    return true;
                }
            }
        }
    }
    return  false;
}

bool checktable(int &le, int &addr,int l, symbol sym, int &type){//查询，返回层级和偏移地址和类型；函数名返回函数参数个数，变量返回偏移地址
    bool isfind = false;
    set<symbol>::iterator it;
    while((!isfind)){
        for(it = pt[l].s.begin(); it != pt[l].s.end(); it ++){
            if(it->name == sym.name){
                isfind = true;
                break;
            }
        }
        if(isfind)
            break;
        if(pt[l].upper == -1){
            error(40, currentword.line);
            le = -1;
            addr = -1;
            type = -1;
            return false;//查找失败
        }
        else
            l = pt[l].upper;
    }
    le = l;
    type = it->type;
    if(it->type == 0)
        addr = it->value;
    else
        addr = it->addr;
    return true;
}

bool settable(int l, symbol sym){//声明标识符，填表或出错,仅需在当前层查看是否重复
    for(set<symbol>::iterator it = pt[l].s.begin(); it != pt[l].s.end(); it ++){
        if(it->name == sym.name && it->type == sym.type){
            error(10, currentword.line);
            return false;
        }
    }
    if(pt[l].s.insert(sym).second)
        printf("到达这里\n");
    return true;
}

void getword(){
    currentword = wordlist[++wordindex];
    if(currentword.type == 37)
        currentword = wordlist[++wordindex];
    printf("%d %s\n", wordindex, currentword.name.c_str());
}

void getheadofword(){//回溯到非终结符的开始,保证进入某个非终结符子程序时，当前单词为其起始
    wordindex = tempindex;
    currentword = wordlist[wordindex];
}

void nextline(){
    int l = currentword.line;
    while (currentword.line == l) {
        getword();
    }
}

void program(){//在进入子程序前需要读入单词
    stack.push_back(-1);//-1层为0层的上层
    stack.push_back(currentlevel);//当前层级入栈
    pt[currentlevel].upper = *(stack.cend()-2);//次栈顶
    if(currentword.type == 1){//const
        consystate();
    }
    if(currentword.type == 2){//int
        tempindex = wordindex;
        getword();
        if(currentword.type == 14){//标识符
            getword();
            if(currentword.type != 21){//非左括号，变量说明
                getheadofword();
                int r = 0;
                variablestate(r);
            }
            else
                getheadofword();
        }
    }

    while(currentword.type !=2 && currentword.type != 4){
        error(5, currentword.line);
        nextline();
    }

    while(currentword.type == 2 || currentword.type == 4){//int或void
        tempindex = wordindex;
        getword();
        if(currentword.type == 5){//主函数
            getheadofword();
            break;
        }
        else if(currentword.type == 14){//标识符
            getword();
            if(currentword.type == 21){//左括号，函数定义
                getheadofword();
                funcdec();
            }
        }
    }
    maindec();
}

void consystate(){//常量说明
    if(currentword.type == 1){
        getword();
        consydec();
        while(currentword.type == 19){//当前单词为,
            getword();
            consydec();
        }
    }

}

void consydec(){//常量定义
    symbol sym;
    sym.type = 0;//类型为 const
    if(currentword.type == 14){
        sym.name = currentword.name;
        sym.type = 0;
        if(settable(currentlevel, sym)){//插表成功
            getword();
            if(currentword.type == 27){
                getword();
                if(currentword.type == 15){
                    getword();
                    if(currentword.type == 34){
                        printf("正整数");
                        sym.value =atoi(currentword.value.c_str());//转化为int
                        sym.addr = -1;//地址为-1表示常量
                        if(resettable(currentlevel, sym))
                            printf("插入符号表成功！\n");
                        printf("到达这里\n");
                        getword();
                    }
                }
                else if(currentword.type == 16){
                    getword();
                    if(currentword.type == 34){                        
                        printf("负整数");
                        sym.value =-atoi(currentword.value.c_str());//转化为int,负数
                        sym.addr = -1;//地址为-1表示常量
                        if(resettable(currentlevel, sym))
                            printf("插入符号表成功！\n");
                        getword();
                    }
                }
                else if(currentword.type == 34){                    
                    sym.value =atoi(currentword.value.c_str());//转化为int,负数
                    sym.addr = -1;//地址为-1表示常量
                    if(resettable(currentlevel, sym))
                        printf("插入符号表成功！\n");
                    getword();
                }
            }
        }
    }
}

void variablestate(int &vn){//变量说明
    headofdec(vn);
    while (currentword.type == 19) {
        symbol sym;
        sym.type = 1;
        getword();
        if(currentword.type == 14){
            sym.name = currentword.name;
            sym.addr = vn;
            vn ++;
            if(settable(currentlevel, sym)){
                printf("%s 插入成功！\n", currentword.name.c_str());
            }
            getword();
        }
    }
}

void headofdec(int &vn){//声明头部
    printf("到达这里\n");
    if(currentword.type == 2){
        symbol sym;
        sym.type = 1;
        getword();
        if(currentword.type == 14){
            sym.name = currentword.name;
            sym.addr = vn;
            vn ++;
            printf("到达这里\n");
            if(settable(currentlevel, sym)){
                printf("%s 插入成功！\n", currentword.name.c_str());
            }
            getword();
        }
    } 
}

void funcdec(){//函数定义,层级增加
    level ++;//层级增加
    currentlevel = level;
    stack.push_back(currentlevel);
    pt[currentlevel].upper = *(stack.cend()-2);
    if(currentword.type == 2 || currentword.type == 4){
        symbol sym;
        sym.type = 2;
        getword();
        if(currentword.type == 14){
            sym.name = currentword.name;
            sym.value = pcodeindex;//如果符号为函数名，则地址为下一条首指令地址
            if(settable(currentlevel, sym)){
                getword();
                printf("当前层级为 %d \n", currentlevel);
                int t = 0;//参数个数
                parameter(t);
                int start = t;
                sym.addr = t;
                resettable(currentlevel, sym);
                complstate(start);
            }

        }
    }
    stack.pop_back();
    currentlevel = stack.back();//返回上一层级
}

void maindec(){
    level ++;//层级增加
    currentlevel = level;
    stack.push_back(currentlevel);
    pt[currentlevel].upper = *(stack.cend()-2);
    if(currentword.type == 2 || currentword.type == 4){
        symbol sym;
        sym.type = 2;
        getword();
        if(currentword.type == 5){
            sym.name = currentword.name;
            sym.type = 2;
            starting = pcodeindex;
            getword();
            if(settable(currentlevel, sym)){
                printf("当前层级为 %d \n", currentlevel);
                int t = 0;
                parameter(t);
                sym.addr = t;
                int start = t;
                resettable(currentlevel, sym);
                complstate(start);
            }

        }
    }
    stack.pop_back();
    currentlevel = stack.back();//返回上一层级
}

void parameter(int &num){//参数
    if(currentword.type == 21){
        getword();
        if(currentword.type == 22){//参数表为空
            printf("参数表为空\n");
            num = 0;
            getword();
        }
        else{
            paramlist(num);
            if(currentword.type == 22){
                printf("参数个数正确\n");
                getword();
            }
        }
    }
}

void paramlist(int &num){//参数列表
    int i = 0;
    symbol sym;
    if(currentword.type == 2){
        getword();
        if(currentword.type == 14){
            sym.name = currentword.name;
            sym.type = 1;
            sym.addr = i;
            i ++;
            if(settable(currentlevel, sym)){
                getword();
                while (currentword.type == 19) {
                    getword();
                    if(currentword.type == 2){
                        getword();
                        if(currentword.type == 14){
                            sym.name = currentword.name;
                            sym.addr = i;
                            i ++;
                            if(settable(currentlevel, sym))
                                getword();
                        }
                    }
                }
            }
        }
    }
    else {
        error(40, currentword.line);
        nextline();
    }
    num = i;//返回参数个数
}

void complstate(int &start){//复合语句
    if(currentword.type == 25){
        getword();
        if(currentword.type == 1){//const
            consystate();
        }
        if(currentword.type == 2){//int
            tempindex = wordindex;
            getword();
            if(currentword.type == 14){//标识符
                getword();
                if(currentword.type != 21){//非左括号，变量说明
                    getheadofword();
                    variablestate(start);
                }
            }
        }
        printf("这里是statement前的word：%s\n", currentword.name.c_str());
        statelist();
        if(currentword.type == 26){
            getword();
        }
        else{
            error(18, currentword.line);
        }
    }
}

void statelist(){//语句序列
    state();
    while (currentword.type != 26) {
        printf("%s %d\n", currentword.name.c_str(), currentword.type);
        state();
    }
}

void state(){
    if(currentword.type == 10){
        ifstate();
    }
    else if(currentword.type == 7) {
        whilestate();
    }
    else if(currentword.type == 25){
        getword();
        statelist();
        if(currentword.type == 26){
            printf("正确");
        }
        getword();
    }
    else if(currentword.type == 14){
        getword();
        if(currentword.type == 27){
            wordindex -= 2;
            getword();
            assignstate();
        }
        else if(currentword.type == 21){
            wordindex -= 2;
            getword();
            funccall();
        }
    }
    else if(currentword.type == 13){
        returnstate();
    }
    else if(currentword.type == 10){
        ifstate();
    }
    else if(currentword.type == 11){
        scanfstate();
    }
    else if(currentword.type == 12){
        printfstate();
    }
}

void ifstate(){
    if(currentword.type == 10){
        getword();
        if(currentword.type == 21){
            getword();
            condition();
            int temp1 = pcodeindex;
            int temp2 = 0;
            if(currentword.type == 22){
                getword();
                pc[pcodeindex++] = {"JPC", 0, -1};
                state();
                pc[temp1].op2 = pcodeindex + 1;
                temp2 = pcodeindex;
                pc[pcodeindex++] = {"JMP", 0, -1};//如果条件为假，跳转到这之后
                if(currentword.type == 3){
                    getword();
                    state();
                }
                pc[temp2].op2 = pcodeindex;//如果条件为真，则真语句执行完后跳转到这

            }

        }
    }
}

void whilestate() {
    if(currentword.type == 7){
        getword();
        if(currentword.type == 21){
            getword();
            int temp2 = pcodeindex;//保存循环体首地址
            condition();
            int temp = pcodeindex;
            pc[pcodeindex++] = {"JPC", 0, -1};
            if(currentword.type == 22){
                getword();
                state();
                pc[pcodeindex++] = {"JMP", 0, temp2};
                pc[temp].op2 = pcodeindex;
            }
            else
                error(17, currentword.line);//缺少右括号
        }
    }
}

int condition(){
    int temp;
    temp = expression();
    if(currentword.type == 22){
        return temp;
    }
    else if(currentword.type == 28){
        getword();
        int temp1 = expression();
        pc[pcodeindex++] = {"EQL", 0, -1};//pcode生成应该遵循后缀表达式
        return temp == temp1;
    }
    else if(currentword.type == 29){
        getword();
        int temp1 = expression();
         pc[pcodeindex++] = {"GRT", 0, -1};
        return temp > temp1;
    }
    else if(currentword.type == 30){
        getword();
        int temp1 = expression();
         pc[pcodeindex++] = {"GEQ", 0, -1};
        return temp >= temp1;
    }
    else if(currentword.type == 31){
        getword();
        int temp1 = expression();
         pc[pcodeindex++] = {"LSS", 0, -1};
        return temp < temp1;
    }
    else if(currentword.type == 32){
        getword();
        int temp1 = expression();
         pc[pcodeindex++] = {"LER", 0, -1};
        return temp <= temp1;
    }
    else if(currentword.type == 33){
        getword();
        int temp1 = expression();
         pc[pcodeindex++] = {"NEQ", 0, -1};

        return temp != temp1;
    }
    else
        error(27, currentword.line);
    return 0;
}

int expression(){
    int temp;
    temp = item();
    while(currentword.type == 15 || currentword.type == 16){
        if(currentword.type == 15){
            getword();
            int temp1;
            temp1 = item();
            pc[pcodeindex ++] = {"ADD", 0, -1};
        }
        else{
            getword();
            int temp1;
            temp1 = item();
            pc[pcodeindex ++] = {"SUB", 0, -1};
        }
    }
    return temp;
}

int item(){
    int temp = factor();
    while(currentword.type == 17 || currentword.type == 18){
        if(currentword.type == 17){
            getword();
            int temp1;
            temp1 = factor();
            pc[pcodeindex ++] = {"MUL", 0, -1};
        }
        else{
            getword();
            int temp1;
            temp1 = factor();
            pc[pcodeindex ++] = {"DIV", 0, -1};
        }
    }
    return temp;
}

int factor(){
    int temp = 0;
    if(currentword.type == 14){
        symbol sym;
        sym.name = currentword.name;
        sym.type = 1;
        int le = 0, addr = 0, type = 0;
        if(checkfunc(le, addr, sym, type)){
            funccall();
        }
        else if(checktable(le, addr, currentlevel, sym, type)){//查找到标识符，并且返回其层级和偏移地址
            if(type == 1){
                pc[pcodeindex++] = {"LOD", le, addr};
                getword();
            }
            else if(type == 0){
                pc[pcodeindex++] = {"LIT", le, addr};
                getword();
            }
            else if(type == 2)
                funccall();
        }
        else{
            error(40, currentword.line);
            getword();
        }
    }
    else if(currentword.type == 21){
        getword();
        temp = expression();
        if(currentword.type == 22)
            getword();
        else {
            error(17, currentword.line);
        }
    }
    else if(currentword.type == 15){
        getword();
        if(currentword.type == 34){
            pc[pcodeindex ++] = {"LDC", 0, atoi(currentword.value.c_str())};
            getword();
        }

    }
    else if(currentword.type == 16){
        getword();
        if(currentword.type == 34){
            pc[pcodeindex ++] = {"LDC", 0, -atoi(currentword.value.c_str())};
            getword();
        }

    }
    else if(currentword.type == 34){ 
        pc[pcodeindex ++] = {"LDC", 0, atoi(currentword.value.c_str())};
        getword();
    }
    return temp;
}

int funccall(){//还没有跳转到相应的函数体
    if(currentword.type == 14){
        symbol sym;
        sym.name = currentword.name;
        sym.type = 2;
        int le = 0, addr = 0, type = 0;

        if(checkfunc(le, addr, sym, type)){
            if(type == 2){//标识符为函数名
                getword();
                if(currentword.type == 21){
                    getword();
                    if(currentword.type == 22){
                        if(addr != 0)
                            error(8, currentword.line);
                        getword();
                    }
                    else{
                        int t = 0;
                        valuelist(t);
                        printf("形参个数：%d, 实参个数：%d\n", addr, t);
                        if(t != addr)
                            error(8, currentword.line);
                        if(currentword.type == 22){
                            getword();
                        }
                    }
                    int t = pcodeindex + 1;
                    pc[pcodeindex++] = {"RETADDR", 0, t};
                    pc[pcodeindex++] = {"CAL", le, addr};//等参数计算完成再call

                    printf("JSR addrofreturn\n");
                }
            }
            else{
                error(40, currentword.line);
                getword();
            }

        }
        else{
            error(40, currentword.line);
            getword();
        }
    }
    return  0;
}

void valuelist(int &num){
    int i = 1;
    expression();
    while (currentword.type == 19) {
        i ++;
        getword();
        expression();
    }
    num = i;
}

void assignstate(){
    if(currentword.type == 14){
        symbol sym;
        sym.name = currentword.name;
        sym.type = 1;
        int le = 0, addr = 0, type = 0;
        if(checktable(le, addr, currentlevel, sym, type)){
            if(type == 0 || type == 2){
                error(40, currentword.line);
            }
            pc[pcodeindex++] = {"LDA", le, addr};
            getword();
            if(currentword.type == 27){
                getword();
                expression();
                pc[pcodeindex ++] = {"STN", le, addr};
            }
        }
        else{
            error(40, currentword.line);
            nextline();
        }
    }
}

int returnstate(){
    if(currentword.type == 13){
        int temp = 0;
        //getword();
        printf("这里是return\n");
        getword();
        if(currentword.type == 21){
            getword();

            temp = expression();
            if(currentword.type == 22){
                printf("返回语句 return （表达式）\n");
                getword();
            }
        }
        pc[pcodeindex++] = {"RET", 0, -1};
    }
}

void scanfstate(){
    if(currentword.type == 11){
        getword();
        if(currentword.type == 21){
            getword();
            if(currentword.type == 14){
                symbol sym;
                sym.name = currentword.name;
                sym.type = 1;
                int le = 0, addr = 0, type = 0;
                if(checktable(le, addr, currentlevel, sym, type)){
                    if(type == 2){
                        error(40, currentword.line);
                    }
                    pc[pcodeindex++] = {"SCANF", le, addr};
                }
                else {
                    error(42, currentword.line);//赋值错误
                }
                getword();
                if(currentword.type == 22){
                    getword();
                }
            }
        }
    }
}

void printfstate(){
    if(currentword.type == 12){
        getword();
        if(currentword.type == 21){
            getword();
            if(currentword.type != 22){
                if(currentword.type == 36){
                    pc[pcodeindex++] = {currentword.name, 0, 0};
                    getword();
                    if(currentword.type == 19){
                        getword();
                        if(currentword.type != 22){
                            expression();
                            pc[pcodeindex++] = {"PRINTF", 0, -1};//打印当前栈顶
                        }
                        else {
                            printf("printf(""字符串""，)出错\n");
                        }
                    }
                }
                else{
                    expression();
                    pc[pcodeindex++] = {"PRINTF", 0, -1};
                }
                if(currentword.type == 22){
                    printf("%d行printf成功\n", currentword.line);
                    getword();
                }
            }
            else
                error(17, currentword.line);
        }
    }
}

void exec(){
    vector<symbol> opstack;
    for(int i = starting; i < pcodeindex; i ++){
        if(pc[i].ins == "JMP" || pc[i].ins == "JPC"){
            i = pc[i].op2 - 1;
        }
        else if(pc[i].ins == "LDA"){
            set<symbol>::iterator it;
            for(it = pt[pc[i].op1].s.begin(); it != pt[pc[i].op1].s.end(); it ++){
                if(it->addr == pc[i].op2){
                    opstack.push_back(*it);
                    break;
                }
            }
        }
        else if(pc[i].ins == "LDC"){
            symbol sym;
            sym.name = to_string(i);
            sym.value = pc[i].op2;
            opstack.push_back(sym);
        }
        else if(pc[i].ins == "EQL"){//相等
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            if(temp2.value == temp1.value){
                i ++;
            }
            else
                continue;
        }
        else if(pc[i].ins == "GRT"){//大于
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            if(temp2.value > temp1.value){
                i ++;
            }
            else
                continue;
        }
        else if(pc[i].ins == "GEQ"){//大于等于
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            if(temp2.value >= temp1.value){
                i ++;
            }
            else
                continue;
        }
        else if(pc[i].ins == "LSS"){//小于
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            if(temp2.value < temp1.value){
                i ++;
            }
            else
                continue;
        }
        else if(pc[i].ins == "LER"){//小于等于
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            if(temp2.value <= temp1.value){
                i ++;
            }
            else
                continue;
        }
        else if(pc[i].ins == "NEQ"){//不等
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            if(temp2.value != temp1.value){
                i ++;
            }
            else
                continue;
        }
        else if (pc[i].ins == "ADD") {
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            symbol sym;
            sym.value = temp2.value + temp1.value;
            sym.name = to_string(i);
            opstack.push_back(sym);
        }
        else if (pc[i].ins == "SUB") {
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            symbol sym;
            sym.value = temp2.value - temp1.value;
            sym.name = to_string(i);
            opstack.push_back(sym);
        }
        else if (pc[i].ins == "MUL") {
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            symbol sym;
            sym.value = temp2.value * temp1.value;
            sym.name = to_string(i);
            opstack.push_back(sym);
        }
        else if (pc[i].ins == "DIV") {
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            symbol sym;
            sym.value = temp2.value / temp1.value;
            sym.name = to_string(i);
            opstack.push_back(sym);
        }
        else if (pc[i].ins == "LOD") {
            set<symbol>::iterator it;
            for(it = pt[pc[i].op1].s.begin(); it != pt[pc[i].op1].s.end(); it ++){
                if(it->addr == pc[i].op2){
                    opstack.push_back(*it);
                    break;
                }
            }
        }
        else if(pc[i].ins == "LIT"){
            symbol sym;
            sym.name = to_string(i);
            sym.value = pc[i].op2;
            opstack.push_back(sym);
        }
        else if (pc[i].ins == "STN") {
            symbol temp1 = opstack.back();
            opstack.pop_back();
            symbol temp2 = opstack.back();
            opstack.pop_back();
            temp2.value = temp1.value;
            set<symbol>::iterator it;
            resettable(pc[i].op1, temp2);//赋值

        }
        else if (pc[i].ins == "CAL") {
            set<symbol>::iterator it, it1;
            for(it = pt[pc[i].op1].s.begin(); it != pt[pc[i].op1].s.end(); it ++){
                if(it->addr == pc[i].op2 && it->type == 2){
                    break;
                }
            }
            int j = it->addr - 1;
            int t = it->value - 1;
            symbol tem = opstack.back();
            opstack.pop_back();
            for(; j >= 0; j --){//实参传入形参中
                symbol temp = opstack.back();
                opstack.pop_back();
                for(it = pt[pc[i].op1].s.begin(); it != pt[pc[i].op1].s.end(); it ++){
                    if(it->type == 1 && it->addr == j){
                        temp.name = it->name;
                        temp.type = it->type;
                        temp.addr = it->addr;
                        resettable(pc[i].op1, temp);
                        break;
                    }
                }
            }
            i = t;//函数的value存储其首地址
            opstack.push_back(tem);
        }
        else if(pc[i].ins == "RETADDR"){
            symbol sym;
            sym.name = to_string(i);
            sym.value = pc[i].op2;
            opstack.push_back(sym);
        }
        else if(pc[i].ins == "RET"){
            symbol sym = opstack.back();
            opstack.pop_back();
            symbol sym1 = opstack.back();
            opstack.pop_back();
            opstack.push_back(sym);
            i = sym1.value;//第二个操作数为返回地址
        }
        else if(pc[i].ins == "SCANF"){
            int t;
         scanf("%d", &t);
            symbol sym;
            set<symbol>::iterator it;
            for(it = pt[pc[i].op1].s.begin(); it != pt[pc[i].op1].s.end(); it ++){
                if(it->addr == pc[i].op2){
                    break;
                }
            }
            sym.name = it->name;
            sym.type = it->type;
            sym.value = t;
            sym.addr = it->addr;
            pt[pc[i].op1].s.erase(it);
            pt[pc[i].op1].s.insert(sym);
        }
        else if (pc[i].ins == "PRINTF") {
            printf("%d\n", opstack.back().value);
        }
        else{
            printf("%s\n", pc[i].ins.c_str());
        }
    }
}

int main()
{
    wordlist = word();
    getword();
    program();
    printf("p-code序列为：\n");
    for(int i = 0; i < pcodeindex; i ++){
        printf("%d %s %d %d\n", i, pc[i].ins.c_str(), pc[i].op1, pc[i].op2);
    }
    printf("是否解释该p-code序列？(1/0)\n");
    int i ;
    scanf("%d", &i);
    if(i){
        exec();
    }

      return 0;
}
