#include <iostream>
#include <map>
#include <vector>
#include <cstring>
using namespace std;

/*
 *全局变量定义
 */
string s;//规则字符串缓冲
string sss;//规约字符串
vector<string> v;//用vector存储规则（一条规则为一个单位）
vector<char> stack;//为firstvt和lastvt设一个栈,奇数为vn，偶数为vt
vector<char> symbolStack;//符号栈
vector<char>::iterator iterOfChar;
vector<string>::iterator iterOfVector;//返回指针
map<char, char> vt;
map<char, char> vn;
map<char, char>::iterator iter;
map<char, char>::iterator iter2;
multimap<char, char> F;//一个key对应多个value，存储firstvt集
multimap<char, char> L;//存储lastvt集
multimap<char, char> vtOfStack;//算符优先矩阵,存在则first大于second
multimap<char, char> vtOfWork;//算符优先矩阵,存在即first等于second
typedef multimap<char, char>::iterator Iter;
pair<Iter, Iter> It;
int father[256];//并查集,值是下标的父结点

/*
 * 寻找c的根结点
*/
char findFather(char c){
    if(c == v.front()[0] && father[c] != -1)
        c = father[c];
    while(father[c] != -1 && c != v.front()[0]){
        c = father[c];
    }
    return c;
}

/*
 * 判断map中是否有键值对<a,b>
*/
bool findFromMultimap(multimap<char, char> map, char a, char b){
    Iter it;
    It = map.equal_range(a);
    for(it = It.first; it != It.second; it ++){
        if(it->second == b)
            return  true;
    }
    return false;
}

/*
 * 求firstvt集
*/
void firstvt(){
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        if(vt.count(s[3])){// U:=b
            F.insert(pair<char, char>(s[0], s[3]));
            stack.push_back(s[0]);
            stack.push_back(s[3]);
        }
        else if(s.length() > 4 && vn.count(s[3]) && vt.count(s[4])){// or U:=Vb
            F.insert(pair<char, char>(s[0], s[4]));
            stack.push_back(s[0]);
            stack.push_back(s[4]);
        }
    }
        while(!stack.empty()){
            char svt = stack.back();//最后一个元素，为vt
            stack.pop_back();
            char svn = stack.back();//倒数第二个元素，为vn
            stack.pop_back();
            for(iterOfVector = v.begin();iterOfVector != v.end(); iterOfVector ++){
                s = *iterOfVector;
                if(s[3] == svn && (!findFromMultimap(F, s[0], svt))){
                    F.insert(pair<char, char>(s[0], svt));
                    stack.push_back(s[0]);//新加入的F[u,b]=true 也需要入栈
                    stack.push_back(svt);
                }
            }
        }
}

/*
 * 求lastvt集
 */
void lastvt(){
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        if(vt.count(s[s.length()-1])){// U:=...b
            L.insert(pair<char, char>(s[0], s[s.length()-1]));
            stack.push_back(s[0]);
            stack.push_back(s[s.length()-1]);
        }
        else if(s.length() > 4 && vn.count(s[s.length()-1]) && vt.count(s[s.length()-2])){// or U:=...bV
            L.insert(pair<char, char>(s[s.length()-1], s[s.length()-2]));
            stack.push_back(s[s.length()-1]);
            stack.push_back(s[s.length()-2]);
        }
    }
        while(!stack.empty()){
            char svt = stack.back();//最后一个元素，为vt
            stack.pop_back();
            char svn = stack.back();//倒数第二个元素，为vn
            stack.pop_back();
            for(iterOfVector = v.begin();iterOfVector != v.end(); iterOfVector ++){
                s = *iterOfVector;
                if(s[s.length()-1] == svn  && (!findFromMultimap(L, s[0], svt))){
                    L.insert(pair<char, char>(s[0], svt));
                    stack.push_back(s[0]);
                    stack.push_back(svt);
                }
            }
        }
}

/*
 * 获得算符优先矩阵
*/
void getOpm(){
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        for(int i = 3; i < s.length()-1; i ++){
            if(vn.count(s[i]) && vt.count(s[i+1])){//存储栈内终结符大于栈外终结符
                Iter it;
                It = L.equal_range(s[i]);//key为s[i]的所有映射，first为第一个迭代器，second为最后一个迭代器
                for(it = It.first; it != It.second; it ++){//将s[i]的lastvt集中的所有终结符 设为大于s[i+1]
                    vtOfStack.insert(pair<char, char>(it->second, s[i+1]));
                }
            }
            else if(vt.count(s[i]) && vt.count(s[i+1])){
                vtOfWork.insert(pair<char, char>(s[i], s[i+1]));//存储 s[i]=s[i+1]
            }
            else if(i < s.length()-2 && vt.count(s[i]) && vt.count(s[i+2])){
                vtOfWork.insert(pair<char, char>(s[i], s[i+2]));//存储 s[i]=s[i+2]
            }
        }
    }
}

/*
 * 获得最左素短语
*/
string getLPP(){
    iterOfChar = symbolStack.end()-1;
    while(vn.count(*iterOfChar))
        iterOfChar --;
    char s1 = *iterOfChar;//找到栈中最上面的终结符
    vector<char>::iterator copyOfiterOfChar;
    copyOfiterOfChar = iterOfChar;//保存最左素短语的起始
    iterOfChar --;
    bool isequal = true;//判断当前是否连等
    while(isequal){
        while(vn.count(*iterOfChar))
            iterOfChar --;
        char s2= *iterOfChar;
        if(findFromMultimap(vtOfWork, s2, s1)){
            isequal = true;
            s1 = s2;
            copyOfiterOfChar = iterOfChar;
            iterOfChar --;
        }
        else {
            isequal = false;
        }
    }
    if(vn.count(*(copyOfiterOfChar-1)))//当前待规约的最左终结符的左边是一个非终结符，则该非终结符也需规约
        copyOfiterOfChar --;
    string s3;
    for(;copyOfiterOfChar != symbolStack.end();){
        char c = *copyOfiterOfChar;
        s3 += c;
        symbolStack.erase(copyOfiterOfChar);//清栈
    }
    return  s3;
}

/*
 * 进行规约
*/
bool Statute(string st){
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        s = s.substr(3);
        if(s.length() != st.length())
            continue;
        bool issuccess = true;
        for(int i = 0; i < s.length(); i ++){
            if(vt.count(s[i]) && vt.count(st[i]) && s[i] == st[i])//都为终结符且相等
                continue;
            else if(vn.count(s[i]) && vn.count(st[i]) && findFather(s[i]) == findFather(st[i])){//都为非终结符并且父结点相同
                continue;
            }
            else {
                issuccess = false;
                break;
            }
        }
        if(issuccess){//规约成功
            symbolStack.push_back((*iterOfVector)[0]);
            return true;
        }
    }
    return false;//规约失败
}

/*
 * 获取当前符号栈上的内容
*/
string getStack(){
    string s;
    for(iterOfChar = symbolStack.begin(); iterOfChar != symbolStack.end(); iterOfChar ++){
        char c = *iterOfChar;
        s += c;
    }
    return s;
}

/*
 * 分析规约输入的句子
*/
void work(string ss){
    symbolStack.clear();
    symbolStack.push_back('#');//栈初始化
    int p = 0;//分析字符的指针
    printf("%-20s%-20s%-20s%-20s%-20s%-20s\n","步骤", "栈", "优先关系", "当前符号", "剩余符号", "动作");
    int i =1;//步骤数
    bool iserror =false;//是否出现错误
    while(p < ss.length()){
        iterOfChar = symbolStack.end()-1;//end（）指向最后一个元素的下一个
        while(vn.count(*iterOfChar))//找到栈中最上面的终结符
            iterOfChar --;
        char top = *iterOfChar;
        while(findFromMultimap(vtOfStack, top, ss[p])){// 大于 规约
            string sa = getStack();
            string sb = ss.substr(p+1);
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), '>', ss[p], sb.c_str(), "规约");
            sss = getLPP();
            if(!Statute(sss)){//规约失败
                iserror = true;
                break;
            }
            iterOfChar = symbolStack.end()-1;
            while(vn.count(*iterOfChar))//找到栈中最上面的终结符
                iterOfChar --;
            top = *iterOfChar;
        }
        if(iserror){
            printf("\n");
            printf("句子不合法，规约失败！\n");
            break;
        }
        if(findFromMultimap(vtOfWork, top, ss[p])){// 等于 入栈
            string sa = getStack();
            string sb = ss.substr(p+1);
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), '=', ss[p], sb.c_str(), "入栈");
            symbolStack.push_back(ss[p]);
        }
        else{// 小于 入栈
            string sa = getStack();
            string sb = ss.substr(p+1);
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), '<', ss[p], sb.c_str(), "入栈");
            symbolStack.push_back(ss[p]);
        }
        p ++;
    }
    if(p == ss.length()){//字符全部输入，但是栈中的非终结符不一定为开始符号
        string sa = getStack();
        char c = findFather(sa[1]);
        sa[1] = c;
        if(c == v.front()[0]){//如果栈中的非终结符是开始符号的子节点，即其能规约到开始符号
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), ' ', ' ', " ", "结束");
            printf("\n");
            printf("规约成功！\n");
        }
        else{
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), ' ', ' ', " ", "结束");
            printf("\n");
            printf("规约失败！\n");
        }
    }
}


int main(){

    /*
     * 输入文法
    */
    printf("请输入文法行数：\n");
    int n;
    scanf("%d", &n);
    printf("请输入文法（不含#）：\n");//默认格式： A->...
    for(int i = 0; i < n; i ++){
        cin>>s;
        v.push_back(s);
    }

    /*
     * 构造vn，vt
    */
    for(int i = 0; i < n; i ++){
        vn.insert(pair<char, char>(v[i][0], v[i][0]));
    }
    for(int i = 0; i < n; i ++){
        for(int j = 3; j < v[i].length(); j ++){
            if(!vn.count(v[i][j])){
                vt.insert(pair<char, char>(v[i][j], v[i][j]));
            }
        }
    }

    /*
     *vn，vt，产生式，并设置非终结符的并查集
    */
    memset(father, -1, 256);
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        if(s.length() == 4 && vn.count(s[0]) && vn.count(s[3])){// U->B
            father[(s[3])] =s[0];//设置非终结符的父结点
        }
    }
    father[v.front()[0]] = v.front()[0];//开始符号的父结点为其自己

    printf("VN集为：\n");
    for(iter = vn.begin(); iter != vn.end(); iter ++){
        printf("%c ", iter->first);
    }
    printf("\n");

    printf("VT集为：\n");
    for(iter = vt.begin(); iter != vt.end(); iter ++){
        printf("%c ", iter->first);
    }
    printf("\n");

    /*
     * 构造firstvt和lastvt集并输出
    */
    firstvt();
    printf("firstvt集为：\n");
    for(iter = vn.begin(); iter != vn.end(); iter ++){
        printf("%c: ", iter->first);
        It = F.equal_range(iter->first);
        Iter it;
        it = It.first;
        while(it != It.second){
            printf("%c ", it->second);
            it ++;
        }
        printf("\n");
    }

    lastvt();
    printf("lastvt集为：\n");
    for(iter = vn.begin(); iter != vn.end(); iter ++){
        printf("%c: ", iter->first);
        It = L.equal_range(iter->first);
        Iter it;
        it = It.first;
        while(it != It.second){
            printf("%c ", it->second);
            it ++;
        }
        printf("\n");
    }

    /*
     * 构造算符优先矩阵并输出
    */
    getOpm();
    printf("%-20s","算符优先矩阵为:");
    for(iter = vt.begin(); iter != vt.end(); iter ++){
        printf("%-20c", iter->first);
    }
    printf("\n");
    for(iter = vt.begin(); iter != vt.end(); iter ++){
        printf("%-20c", iter->first);
        iter2 = vt.begin();
        while (iter2 != vt.end()) {
            if(findFromMultimap(vtOfStack, iter->first, iter2->first)){//大于
                printf("%-20c", '>');
            }
            else if (findFromMultimap(vtOfWork, iter->first, iter2->first)) {
                printf("%-20c", '=');
            }
            else{
                printf("%-20c", '<');
            }
            iter2 ++;
        }
        printf("\n");
    }

    for(iter = vt.begin(); iter != vt.end(); iter ++){//栈上终结符都大于栈外#
        vtOfStack.insert(pair<char, char>(iter->first, '#'));
    }
    vtOfWork.insert(pair<char, char>('#', '#'));//栈内#等于栈外#

    /*
     * 输入句子，输出分析过程
    */
    while(1){
        printf("请输入句子（#结束）：\n");
        cin>>s;
        work(s);
    }
}
