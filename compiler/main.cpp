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
struct word{//���ʶ���
    int type;//��������
    string name;//���ʵ�����
    string value;//���ʵ�ֵ
    int line;//�к�
};

struct symbol{//���ű����洢���ͺ����� count������b<a��b>a�����������ж�b=a
    string name;
    int type;//����Ϊ0������Ϊ1������Ϊ2
    int value;//�����ͱ���Ϊֵ������Ϊ�׵�ַ
    int addr;//�����ڸò��ƫ�Ƶ�ַ�������� ��Ϊ��������
    friend bool operator < (const symbol &a, const symbol &b){
        return  !(a.name == b.name);
    }
};

struct procable{
    int upper;//ֱ���ϲ�ֳ���
    set<symbol> s;//��ǰ����ű�

};

struct pcode{
    string ins;//ָ��
    int op1;//������1�������㼶
    int op2;//������2������ƫ�Ƶ�ַ
};

word *wordlist;//�������ĵ�����
int wordindex = -1;//��ǰ�������ʵ�ָ��
int tempindex;//���ݵĵ���ָ��
word currentword;
int level = 0;//����㼶��ÿ��һ�ηֳ���ͼ�һ
int currentlevel = 0;//��ǰ���в㼶
procable pt[20];
vector<int> stack;//�㼶ջ���ò㿪ʼʱ��ջ������ʱ��ջ����ջ���Ĳ㼶Ϊ�ò��ֱ�����
int pcodeindex = 0;//��һ��pcodeָ���ַ
pcode pc[1000];
int starting = 0;//��ʼָ���ַ

void getword();//���뵥��
void program();//����
void consystate();//����˵������
void variablestate(int &vn);//����˵������,����Ϊ���������ַ
void funcdec();//��������
void maindec();//������
void headofdec(int &vn);//����ͷ��,����Ϊ�ò��ַ
void consydec();//��������
void parameter(int &num);//����,���ز�������
void paramlist(int &num);//���������ز�������
void complstate(int &start);//�������,����Ϊ�ڶ��������ʼ��ַ
void statelist();//�������
void state();//���
void ifstate();//�������
void whilestate();//ѭ�����
int funccall();//�����������
int condition();//����
int expression();//���ʽ
int item();//��
int factor();//����
void valuelist(int &num);//ֵ������
void assignstate();//��ֵ���
int returnstate();//�������,����RET pcode�ĵ�ַ
void scanfstate();//�����
void printfstate();//д���
bool resettable(int l, symbol s);//��ֵ�����²�����ű�
bool settable(int l, symbol s);
bool checktable(int &le, int  &addr, int l, symbol s, int &type);//��ѯ�����ز㼶��ƫ�Ƶ�ַ
extern void error(int, int);//�������
extern word *word();//�ʷ�����

bool resettable(int l, symbol sym){//��ֵ����l�㼰�ϲ�����sym������
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
            return false;//����ʧ��
        }
        else
            l = pt[l].upper;
    }
    pt[l].s.erase(it);
    if(pt[l].s.insert(sym).second){
        //printf("���²���ɹ���\n");
    }

    return true;
}

bool checkfunc(int &le, int &addr, symbol sym, int &type){//���Һ���
    printf("%d", level);
    for(int l = 1; l <= level; l ++){
        if(pt[l].upper == 0){//����ò��ֱ���ϲ�Ϊ����
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

bool checktable(int &le, int &addr,int l, symbol sym, int &type){//��ѯ�����ز㼶��ƫ�Ƶ�ַ�����ͣ����������غ���������������������ƫ�Ƶ�ַ
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
            return false;//����ʧ��
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

bool settable(int l, symbol sym){//������ʶ�����������,�����ڵ�ǰ��鿴�Ƿ��ظ�
    for(set<symbol>::iterator it = pt[l].s.begin(); it != pt[l].s.end(); it ++){
        if(it->name == sym.name && it->type == sym.type){
            error(10, currentword.line);
            return false;
        }
    }
    if(pt[l].s.insert(sym).second)
        printf("��������\n");
    return true;
}

void getword(){
    currentword = wordlist[++wordindex];
    if(currentword.type == 37)
        currentword = wordlist[++wordindex];
    printf("%d %s\n", wordindex, currentword.name.c_str());
}

void getheadofword(){//���ݵ����ս���Ŀ�ʼ,��֤����ĳ�����ս���ӳ���ʱ����ǰ����Ϊ����ʼ
    wordindex = tempindex;
    currentword = wordlist[wordindex];
}

void nextline(){
    int l = currentword.line;
    while (currentword.line == l) {
        getword();
    }
}

void program(){//�ڽ����ӳ���ǰ��Ҫ���뵥��
    stack.push_back(-1);//-1��Ϊ0����ϲ�
    stack.push_back(currentlevel);//��ǰ�㼶��ջ
    pt[currentlevel].upper = *(stack.cend()-2);//��ջ��
    if(currentword.type == 1){//const
        consystate();
    }
    if(currentword.type == 2){//int
        tempindex = wordindex;
        getword();
        if(currentword.type == 14){//��ʶ��
            getword();
            if(currentword.type != 21){//�������ţ�����˵��
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

    while(currentword.type == 2 || currentword.type == 4){//int��void
        tempindex = wordindex;
        getword();
        if(currentword.type == 5){//������
            getheadofword();
            break;
        }
        else if(currentword.type == 14){//��ʶ��
            getword();
            if(currentword.type == 21){//�����ţ���������
                getheadofword();
                funcdec();
            }
        }
    }
    maindec();
}

void consystate(){//����˵��
    if(currentword.type == 1){
        getword();
        consydec();
        while(currentword.type == 19){//��ǰ����Ϊ,
            getword();
            consydec();
        }
    }

}

void consydec(){//��������
    symbol sym;
    sym.type = 0;//����Ϊ const
    if(currentword.type == 14){
        sym.name = currentword.name;
        sym.type = 0;
        if(settable(currentlevel, sym)){//���ɹ�
            getword();
            if(currentword.type == 27){
                getword();
                if(currentword.type == 15){
                    getword();
                    if(currentword.type == 34){
                        printf("������");
                        sym.value =atoi(currentword.value.c_str());//ת��Ϊint
                        sym.addr = -1;//��ַΪ-1��ʾ����
                        if(resettable(currentlevel, sym))
                            printf("������ű�ɹ���\n");
                        printf("��������\n");
                        getword();
                    }
                }
                else if(currentword.type == 16){
                    getword();
                    if(currentword.type == 34){                        
                        printf("������");
                        sym.value =-atoi(currentword.value.c_str());//ת��Ϊint,����
                        sym.addr = -1;//��ַΪ-1��ʾ����
                        if(resettable(currentlevel, sym))
                            printf("������ű�ɹ���\n");
                        getword();
                    }
                }
                else if(currentword.type == 34){                    
                    sym.value =atoi(currentword.value.c_str());//ת��Ϊint,����
                    sym.addr = -1;//��ַΪ-1��ʾ����
                    if(resettable(currentlevel, sym))
                        printf("������ű�ɹ���\n");
                    getword();
                }
            }
        }
    }
}

void variablestate(int &vn){//����˵��
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
                printf("%s ����ɹ���\n", currentword.name.c_str());
            }
            getword();
        }
    }
}

void headofdec(int &vn){//����ͷ��
    printf("��������\n");
    if(currentword.type == 2){
        symbol sym;
        sym.type = 1;
        getword();
        if(currentword.type == 14){
            sym.name = currentword.name;
            sym.addr = vn;
            vn ++;
            printf("��������\n");
            if(settable(currentlevel, sym)){
                printf("%s ����ɹ���\n", currentword.name.c_str());
            }
            getword();
        }
    } 
}

void funcdec(){//��������,�㼶����
    level ++;//�㼶����
    currentlevel = level;
    stack.push_back(currentlevel);
    pt[currentlevel].upper = *(stack.cend()-2);
    if(currentword.type == 2 || currentword.type == 4){
        symbol sym;
        sym.type = 2;
        getword();
        if(currentword.type == 14){
            sym.name = currentword.name;
            sym.value = pcodeindex;//�������Ϊ�����������ַΪ��һ����ָ���ַ
            if(settable(currentlevel, sym)){
                getword();
                printf("��ǰ�㼶Ϊ %d \n", currentlevel);
                int t = 0;//��������
                parameter(t);
                int start = t;
                sym.addr = t;
                resettable(currentlevel, sym);
                complstate(start);
            }

        }
    }
    stack.pop_back();
    currentlevel = stack.back();//������һ�㼶
}

void maindec(){
    level ++;//�㼶����
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
                printf("��ǰ�㼶Ϊ %d \n", currentlevel);
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
    currentlevel = stack.back();//������һ�㼶
}

void parameter(int &num){//����
    if(currentword.type == 21){
        getword();
        if(currentword.type == 22){//������Ϊ��
            printf("������Ϊ��\n");
            num = 0;
            getword();
        }
        else{
            paramlist(num);
            if(currentword.type == 22){
                printf("����������ȷ\n");
                getword();
            }
        }
    }
}

void paramlist(int &num){//�����б�
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
    num = i;//���ز�������
}

void complstate(int &start){//�������
    if(currentword.type == 25){
        getword();
        if(currentword.type == 1){//const
            consystate();
        }
        if(currentword.type == 2){//int
            tempindex = wordindex;
            getword();
            if(currentword.type == 14){//��ʶ��
                getword();
                if(currentword.type != 21){//�������ţ�����˵��
                    getheadofword();
                    variablestate(start);
                }
            }
        }
        printf("������statementǰ��word��%s\n", currentword.name.c_str());
        statelist();
        if(currentword.type == 26){
            getword();
        }
        else{
            error(18, currentword.line);
        }
    }
}

void statelist(){//�������
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
            printf("��ȷ");
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
                pc[pcodeindex++] = {"JMP", 0, -1};//�������Ϊ�٣���ת����֮��
                if(currentword.type == 3){
                    getword();
                    state();
                }
                pc[temp2].op2 = pcodeindex;//�������Ϊ�棬�������ִ�������ת����

            }

        }
    }
}

void whilestate() {
    if(currentword.type == 7){
        getword();
        if(currentword.type == 21){
            getword();
            int temp2 = pcodeindex;//����ѭ�����׵�ַ
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
                error(17, currentword.line);//ȱ��������
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
        pc[pcodeindex++] = {"EQL", 0, -1};//pcode����Ӧ����ѭ��׺���ʽ
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
        else if(checktable(le, addr, currentlevel, sym, type)){//���ҵ���ʶ�������ҷ�����㼶��ƫ�Ƶ�ַ
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

int funccall(){//��û����ת����Ӧ�ĺ�����
    if(currentword.type == 14){
        symbol sym;
        sym.name = currentword.name;
        sym.type = 2;
        int le = 0, addr = 0, type = 0;

        if(checkfunc(le, addr, sym, type)){
            if(type == 2){//��ʶ��Ϊ������
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
                        printf("�βθ�����%d, ʵ�θ�����%d\n", addr, t);
                        if(t != addr)
                            error(8, currentword.line);
                        if(currentword.type == 22){
                            getword();
                        }
                    }
                    int t = pcodeindex + 1;
                    pc[pcodeindex++] = {"RETADDR", 0, t};
                    pc[pcodeindex++] = {"CAL", le, addr};//�Ȳ������������call

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
        printf("������return\n");
        getword();
        if(currentword.type == 21){
            getword();

            temp = expression();
            if(currentword.type == 22){
                printf("������� return �����ʽ��\n");
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
                    error(42, currentword.line);//��ֵ����
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
                            pc[pcodeindex++] = {"PRINTF", 0, -1};//��ӡ��ǰջ��
                        }
                        else {
                            printf("printf(""�ַ���""��)����\n");
                        }
                    }
                }
                else{
                    expression();
                    pc[pcodeindex++] = {"PRINTF", 0, -1};
                }
                if(currentword.type == 22){
                    printf("%d��printf�ɹ�\n", currentword.line);
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
        else if(pc[i].ins == "EQL"){//���
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
        else if(pc[i].ins == "GRT"){//����
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
        else if(pc[i].ins == "GEQ"){//���ڵ���
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
        else if(pc[i].ins == "LSS"){//С��
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
        else if(pc[i].ins == "LER"){//С�ڵ���
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
        else if(pc[i].ins == "NEQ"){//����
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
            resettable(pc[i].op1, temp2);//��ֵ

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
            for(; j >= 0; j --){//ʵ�δ����β���
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
            i = t;//������value�洢���׵�ַ
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
            i = sym1.value;//�ڶ���������Ϊ���ص�ַ
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
    printf("p-code����Ϊ��\n");
    for(int i = 0; i < pcodeindex; i ++){
        printf("%d %s %d %d\n", i, pc[i].ins.c_str(), pc[i].op1, pc[i].op2);
    }
    printf("�Ƿ���͸�p-code���У�(1/0)\n");
    int i ;
    scanf("%d", &i);
    if(i){
        exec();
    }

      return 0;
}
