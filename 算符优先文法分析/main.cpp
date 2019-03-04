#include <iostream>
#include <map>
#include <vector>
#include <cstring>
using namespace std;

/*
 *ȫ�ֱ�������
 */
string s;//�����ַ�������
string sss;//��Լ�ַ���
vector<string> v;//��vector�洢����һ������Ϊһ����λ��
vector<char> stack;//Ϊfirstvt��lastvt��һ��ջ,����Ϊvn��ż��Ϊvt
vector<char> symbolStack;//����ջ
vector<char>::iterator iterOfChar;
vector<string>::iterator iterOfVector;//����ָ��
map<char, char> vt;
map<char, char> vn;
map<char, char>::iterator iter;
map<char, char>::iterator iter2;
multimap<char, char> F;//һ��key��Ӧ���value���洢firstvt��
multimap<char, char> L;//�洢lastvt��
multimap<char, char> vtOfStack;//������Ⱦ���,������first����second
multimap<char, char> vtOfWork;//������Ⱦ���,���ڼ�first����second
typedef multimap<char, char>::iterator Iter;
pair<Iter, Iter> It;
int father[256];//���鼯,ֵ���±�ĸ����

/*
 * Ѱ��c�ĸ����
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
 * �ж�map���Ƿ��м�ֵ��<a,b>
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
 * ��firstvt��
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
            char svt = stack.back();//���һ��Ԫ�أ�Ϊvt
            stack.pop_back();
            char svn = stack.back();//�����ڶ���Ԫ�أ�Ϊvn
            stack.pop_back();
            for(iterOfVector = v.begin();iterOfVector != v.end(); iterOfVector ++){
                s = *iterOfVector;
                if(s[3] == svn && (!findFromMultimap(F, s[0], svt))){
                    F.insert(pair<char, char>(s[0], svt));
                    stack.push_back(s[0]);//�¼����F[u,b]=true Ҳ��Ҫ��ջ
                    stack.push_back(svt);
                }
            }
        }
}

/*
 * ��lastvt��
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
            char svt = stack.back();//���һ��Ԫ�أ�Ϊvt
            stack.pop_back();
            char svn = stack.back();//�����ڶ���Ԫ�أ�Ϊvn
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
 * ���������Ⱦ���
*/
void getOpm(){
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        for(int i = 3; i < s.length()-1; i ++){
            if(vn.count(s[i]) && vt.count(s[i+1])){//�洢ջ���ս������ջ���ս��
                Iter it;
                It = L.equal_range(s[i]);//keyΪs[i]������ӳ�䣬firstΪ��һ����������secondΪ���һ��������
                for(it = It.first; it != It.second; it ++){//��s[i]��lastvt���е������ս�� ��Ϊ����s[i+1]
                    vtOfStack.insert(pair<char, char>(it->second, s[i+1]));
                }
            }
            else if(vt.count(s[i]) && vt.count(s[i+1])){
                vtOfWork.insert(pair<char, char>(s[i], s[i+1]));//�洢 s[i]=s[i+1]
            }
            else if(i < s.length()-2 && vt.count(s[i]) && vt.count(s[i+2])){
                vtOfWork.insert(pair<char, char>(s[i], s[i+2]));//�洢 s[i]=s[i+2]
            }
        }
    }
}

/*
 * ��������ض���
*/
string getLPP(){
    iterOfChar = symbolStack.end()-1;
    while(vn.count(*iterOfChar))
        iterOfChar --;
    char s1 = *iterOfChar;//�ҵ�ջ����������ս��
    vector<char>::iterator copyOfiterOfChar;
    copyOfiterOfChar = iterOfChar;//���������ض������ʼ
    iterOfChar --;
    bool isequal = true;//�жϵ�ǰ�Ƿ�����
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
    if(vn.count(*(copyOfiterOfChar-1)))//��ǰ����Լ�������ս���������һ�����ս������÷��ս��Ҳ���Լ
        copyOfiterOfChar --;
    string s3;
    for(;copyOfiterOfChar != symbolStack.end();){
        char c = *copyOfiterOfChar;
        s3 += c;
        symbolStack.erase(copyOfiterOfChar);//��ջ
    }
    return  s3;
}

/*
 * ���й�Լ
*/
bool Statute(string st){
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        s = s.substr(3);
        if(s.length() != st.length())
            continue;
        bool issuccess = true;
        for(int i = 0; i < s.length(); i ++){
            if(vt.count(s[i]) && vt.count(st[i]) && s[i] == st[i])//��Ϊ�ս�������
                continue;
            else if(vn.count(s[i]) && vn.count(st[i]) && findFather(s[i]) == findFather(st[i])){//��Ϊ���ս�����Ҹ������ͬ
                continue;
            }
            else {
                issuccess = false;
                break;
            }
        }
        if(issuccess){//��Լ�ɹ�
            symbolStack.push_back((*iterOfVector)[0]);
            return true;
        }
    }
    return false;//��Լʧ��
}

/*
 * ��ȡ��ǰ����ջ�ϵ�����
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
 * ������Լ����ľ���
*/
void work(string ss){
    symbolStack.clear();
    symbolStack.push_back('#');//ջ��ʼ��
    int p = 0;//�����ַ���ָ��
    printf("%-20s%-20s%-20s%-20s%-20s%-20s\n","����", "ջ", "���ȹ�ϵ", "��ǰ����", "ʣ�����", "����");
    int i =1;//������
    bool iserror =false;//�Ƿ���ִ���
    while(p < ss.length()){
        iterOfChar = symbolStack.end()-1;//end����ָ�����һ��Ԫ�ص���һ��
        while(vn.count(*iterOfChar))//�ҵ�ջ����������ս��
            iterOfChar --;
        char top = *iterOfChar;
        while(findFromMultimap(vtOfStack, top, ss[p])){// ���� ��Լ
            string sa = getStack();
            string sb = ss.substr(p+1);
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), '>', ss[p], sb.c_str(), "��Լ");
            sss = getLPP();
            if(!Statute(sss)){//��Լʧ��
                iserror = true;
                break;
            }
            iterOfChar = symbolStack.end()-1;
            while(vn.count(*iterOfChar))//�ҵ�ջ����������ս��
                iterOfChar --;
            top = *iterOfChar;
        }
        if(iserror){
            printf("\n");
            printf("���Ӳ��Ϸ�����Լʧ�ܣ�\n");
            break;
        }
        if(findFromMultimap(vtOfWork, top, ss[p])){// ���� ��ջ
            string sa = getStack();
            string sb = ss.substr(p+1);
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), '=', ss[p], sb.c_str(), "��ջ");
            symbolStack.push_back(ss[p]);
        }
        else{// С�� ��ջ
            string sa = getStack();
            string sb = ss.substr(p+1);
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), '<', ss[p], sb.c_str(), "��ջ");
            symbolStack.push_back(ss[p]);
        }
        p ++;
    }
    if(p == ss.length()){//�ַ�ȫ�����룬����ջ�еķ��ս����һ��Ϊ��ʼ����
        string sa = getStack();
        char c = findFather(sa[1]);
        sa[1] = c;
        if(c == v.front()[0]){//���ջ�еķ��ս���ǿ�ʼ���ŵ��ӽڵ㣬�����ܹ�Լ����ʼ����
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), ' ', ' ', " ", "����");
            printf("\n");
            printf("��Լ�ɹ���\n");
        }
        else{
            printf("%-20d%-20s%-20c%-20c%-20s%-20s", i++, sa.c_str(), ' ', ' ', " ", "����");
            printf("\n");
            printf("��Լʧ�ܣ�\n");
        }
    }
}


int main(){

    /*
     * �����ķ�
    */
    printf("�������ķ�������\n");
    int n;
    scanf("%d", &n);
    printf("�������ķ�������#����\n");//Ĭ�ϸ�ʽ�� A->...
    for(int i = 0; i < n; i ++){
        cin>>s;
        v.push_back(s);
    }

    /*
     * ����vn��vt
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
     *vn��vt������ʽ�������÷��ս���Ĳ��鼯
    */
    memset(father, -1, 256);
    for(iterOfVector = v.begin(); iterOfVector != v.end(); iterOfVector ++){
        s = *iterOfVector;
        if(s.length() == 4 && vn.count(s[0]) && vn.count(s[3])){// U->B
            father[(s[3])] =s[0];//���÷��ս���ĸ����
        }
    }
    father[v.front()[0]] = v.front()[0];//��ʼ���ŵĸ����Ϊ���Լ�

    printf("VN��Ϊ��\n");
    for(iter = vn.begin(); iter != vn.end(); iter ++){
        printf("%c ", iter->first);
    }
    printf("\n");

    printf("VT��Ϊ��\n");
    for(iter = vt.begin(); iter != vt.end(); iter ++){
        printf("%c ", iter->first);
    }
    printf("\n");

    /*
     * ����firstvt��lastvt�������
    */
    firstvt();
    printf("firstvt��Ϊ��\n");
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
    printf("lastvt��Ϊ��\n");
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
     * ����������Ⱦ������
    */
    getOpm();
    printf("%-20s","������Ⱦ���Ϊ:");
    for(iter = vt.begin(); iter != vt.end(); iter ++){
        printf("%-20c", iter->first);
    }
    printf("\n");
    for(iter = vt.begin(); iter != vt.end(); iter ++){
        printf("%-20c", iter->first);
        iter2 = vt.begin();
        while (iter2 != vt.end()) {
            if(findFromMultimap(vtOfStack, iter->first, iter2->first)){//����
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

    for(iter = vt.begin(); iter != vt.end(); iter ++){//ջ���ս��������ջ��#
        vtOfStack.insert(pair<char, char>(iter->first, '#'));
    }
    vtOfWork.insert(pair<char, char>('#', '#'));//ջ��#����ջ��#

    /*
     * ������ӣ������������
    */
    while(1){
        printf("��������ӣ�#��������\n");
        cin>>s;
        work(s);
    }
}
