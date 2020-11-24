#include <cstdio>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <queue>
using namespace std;

//产生式结构体，左部符号和右部符号串
struct Production {
    char left{};
    vector<char> rights;
    bool operator==(Production& rhs) const {
        if (left != rhs.left)
            return false;
        for (int i = 0; i < rights.size(); i++) {
            if (i >= rhs.rights.size())
                return false;
            if (rights[i] != rhs.rights[i])
                return false;
        }
        return true;
    }
};

//LR1项目
struct LR1Item {
    Production p;
    //点位置
    int location{};
    //向前看符号
    char next{};
};

//LR1项目集
struct LR1Items {
    vector<LR1Item> items;
};

//LR1项目集规范族
struct CanonicalCollection {
    vector<LR1Items> items;
    //DFA图
    vector< pair<int,char> > g[100];
}CC;

//文法结构体
struct Grammar {
    int num{};
    vector<char> T;
    vector<char> N;
    vector<Production> prods;
}grammar;

//FIRST集和FOLLOW集
map< char, set<char> > first;
map< char, set<char> > follow;

//DFA队列，用于存储待转移的有效项目集
queue< pair<LR1Items, int> > Q;

//action表和goto表
pair<int,int> action[100][100];
int goton[100][100];

string str;
stack< pair<int, char> >ST;

int isInT(char ch) {
    for (int i = 0; i < grammar.T.size(); i++) {
        if (grammar.T[i] == ch) {
            return i+1;
        }
    }
    return 0;
}

int isInN(char ch) {
    for (int i = 0; i < grammar.N.size(); i++) {
        if (grammar.N[i] == ch) {
            return i+1;
        }
    }
    return 0;
}

void getFirstSet() {
    for (char X : grammar.T) {
        set<char> tmp;
        tmp.insert(X);
        first[X] = tmp;
    }
    bool change = true;
    while (change) {
        change = false;
        for (auto & P : grammar.prods) {
            char X = P.left;
            set<char> &FX = first[X];
            if (isInT(P.rights[0]) || P.rights[0] == '&') {
                auto it = FX.find(P.rights[0]);
                if (it == FX.end()) {
                    change = true;
                    FX.insert(P.rights[0]);
                }
            } else {
                bool next = true;
                int idx = 0;
                while (next && idx < P.rights.size()) {
                    next = false;
                    char Y = P.rights[idx];
                    set<char> &FY = first[Y];
                    for (char it : FY) {
                        if (it != '&') {
                            auto itt = FX.find(it);
                            if (itt == FX.end()) {
                                change = true;
                                FX.insert(it);
                            }
                        }
                    }
                    auto it = FY.find('&');
                    if (it != FY.end()) {
                        next = true;
                        idx = idx + 1;
                    }
                }
            }
        }
    }
    printf("FIRST:\n");
    for (char X : grammar.N) {
        printf("%c: ",X);
        for (char it : first[X]) {
            printf("%c ",it);
        }
        printf("\n");
    }
}

void getFirstByAlphaSet(vector<char> &alpha, set<char> &FS)
{
    bool next = true;
    int idx = 0;
    while (idx < alpha.size() && next) {
        next = false;
        if (isInT(alpha[idx]) || alpha[idx] == '&') {
            auto itt = FS.find(alpha[idx]);
            if (itt == FS.end()) {
                FS.insert(alpha[idx]);
            }
        } else {
            char B = alpha[idx];
            set<char> &FB = first[B];
            for (char it : first[B]) {
                if (it == '&') {
                    next = true;
                    continue;
                }
                auto itt = FS.find(it);
                if (itt == FS.end()) {
                    FS.insert(it);
                }
            }
        }
        idx = idx + 1;
    }
    if (next) {
        FS.insert('&');
    }
}

bool isInLR1Items(LR1Items &I, LR1Item &t) {
    for (auto & item : I.items) {
        if (item.p == t.p && item.location == t.location && item.next == t.next)
            return true;
    }
    return false;
}

void printLR1Items(LR1Items &I) {
    for (auto & L : I.items) {
        printf("%c->", L.p.left);
        for (int i = 0; i < L.p.rights.size(); i++) {
            if (L.location == i)
                printf(".");
            printf("%c", L.p.rights[i]);
        }
        if (L.location == L.p.rights.size())
            printf(".");
        printf(",%c   ", L.next);
    }
    printf("\n");
}

void closure(LR1Items &I) {
    bool change =  true;
    while (change) {
        change = false;
        LR1Items J;
        J.items.assign(I.items.begin(), I.items.end());
        for (auto & L : J.items) {
            if (L.location < L.p.rights.size()) {
                char B = L.p.rights[L.location];
                if (isInN(B)) {
                    set<char> FS;
                    vector<char> alpha;
                    alpha.assign(L.p.rights.begin() + L.location + 1, L.p.rights.end());
                    alpha.push_back(L.next);
                    getFirstByAlphaSet(alpha, FS);
                    for (auto & P : grammar.prods) {
                        if (P.left == B) {
                            for (char b : FS) {
                                LR1Item t;
                                t.location = 0;
                                t.next = b;
                                t.p.left = P.left;
                                t.p.rights.assign(P.rights.begin(), P.rights.end());
                                if (!isInLR1Items(I, t)) {
                                    change = true;
                                    I.items.push_back(t);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int isInCanonicalCollection(LR1Items &I) {
    for (int i = 0; i < CC.items.size(); i++) {
        LR1Items &J = CC.items[i];
        bool flag = true;
        if (J.items.size() != I.items.size()) {
            flag = false;
            continue;
        }
        for (auto & t : I.items) {
            if (!isInLR1Items(J, t)) {
                flag = false;
                break;
            }
        }
        if (flag) {
            return i + 1;
        }
    }
    return 0;
}

void go(LR1Items &I, char X, LR1Items &J) {
    for (auto & L : I.items) {
        /* 非规约项目 */
        if (L.location < L.p.rights.size()) {
            char B = L.p.rights[L.location];
            /* 如果点后面是非终结符，且非终结符为X，点位置加1, 加入到转移项目集中*/
            if (B == X) {
                LR1Item t;
                t.location = L.location + 1;
                t.next = L.next;
                t.p.left = L.p.left;
                t.p.rights.assign(L.p.rights.begin(), L.p.rights.end());
                J.items.push_back(t);
            }
        }
    }
    if (!J.items.empty()) {
        closure(J);
    }
}

void DFA() {
    LR1Item t;
    t.location = 0;
    t.next = '$';
    t.p.left = grammar.prods[0].left;
    t.p.rights.assign(grammar.prods[0].rights.begin(), grammar.prods[0].rights.end());
    LR1Items I;
    I.items.push_back(t);
    closure(I);
    CC.items.push_back(I);
    Q.push(pair<LR1Items, int>(I, 0));
    while (!Q.empty()) {
        LR1Items &S = Q.front().first;
        int sidx = Q.front().second;
        for (char & i : grammar.T) {
            LR1Items D;
            go(S, i, D);
            int idx;
            if (!D.items.empty()) {
                idx = isInCanonicalCollection(D);
                if (idx > 0) {
                    idx = idx - 1;
                } else {
                    idx = CC.items.size();
                    CC.items.push_back(D);
                    Q.push(pair<LR1Items, int>(D, idx));
                }
                CC.g[sidx].emplace_back(pair<char, int>(i, idx));
            }
        }
        for (char & i : grammar.N) {
            LR1Items D;
            go(S, i, D);
            int idx;
            if (!D.items.empty()) {
                idx = isInCanonicalCollection(D);
                if (idx != 0) {
                    idx = idx - 1;
                } else {
                    idx = CC.items.size();
                    CC.items.push_back(D);
                    Q.push(pair<LR1Items, int>(D, idx));
                }
                CC.g[sidx].emplace_back(pair<char, int>(i, idx));
            }
        }
        Q.pop();
    }

    printf("CC size: %d\n", CC.items.size());
    for (int i = 0; i < CC.items.size(); i++) {
        printf("LR1Items %d:\n", i);
        printLR1Items(CC.items[i]);
        for (const auto & j : CC.g[i]) {
            pair<char, int> p= j;
            printf("to %d using %c\n", p.second, p.first);
        }
    }
}

void productLR1AnalysisTable() {
    for (int i = 0; i < CC.items.size(); i++) {
        LR1Items &LIt= CC.items[i];
        for (auto & L : LIt.items) {
            if (L.location < L.p.rights.size()) {
                char a = L.p.rights[L.location];
                int j = isInT(a);
                if (j > 0) {
                    j = j - 1;
                    for (int k = 0; k < CC.g[i].size(); k++) {
                        pair<char, int> p = CC.g[i][k];
                        if (p.first == a) {
                            action[i][j].first = 1;
                            action[i][j].second = p.second;
                            break;
                        }
                    }
                }
            } else {
                if (L.p.left == grammar.prods[0].left) {
                    if (L.next == '$')
                        action[i][grammar.T.size() - 1].first = 3;
                } else {
                    int  j = isInT(L.next) - 1;
                    for (int k = 0; k < grammar.prods.size(); k++) {
                        if (L.p == grammar.prods[k]) {
                            action[i][j].first = 2;
                            action[i][j].second = k;
                            break;
                        }
                    }

                }
            }
        }
        for (int k = 0; k < CC.g[i].size(); k++) {
            pair<char, int> p = CC.g[i][k];
            char A = p.first;
            int j = isInN(A);
            if (j > 0) {
                j = j - 1;
                goton[i][j] = p.second;
            }
        }
    }
    for (int i = 0; i < grammar.T.size() / 2; i++)
        printf("\t");
    printf("action");
    for (int i = 0; i < grammar.N.size() / 2 + grammar.T.size() / 2 + 1; i++)
        printf("\t");
    printf("goto\n");
    printf("\t");
    for (char i : grammar.T) {
        printf("%c\t", i);
    }
    printf("|\t");
    for (int i = 1; i  < grammar.N.size(); i++) {
        printf("%c\t", grammar.N[i]);
    }
    printf("\n");
    for (int i = 0; i < CC.items.size(); i++) {
        printf("%d\t", i);
        for (int j = 0; j < grammar.T.size(); j++) {
            if (action[i][j].first == 1) {
                printf("%c%d\t", 'S', action[i][j].second);
            } else if (action[i][j].first == 2) {
                printf("%c%d\t", 'R', action[i][j].second);
            } else if (action[i][j].first == 3) {
                printf("ACC\t");
            } else {
                printf("\t");
            }
        }
        printf("|\t");
        for (int j = 1; j < grammar.N.size(); j++) {
            if (goton[i][j]) {
                printf("%d\t", goton[i][j]);
            } else {
                printf("\t");
            }

        }
        printf("\n");
    }
}

void initGrammar() {
    printf("Please enter the num of production:\n");
    cin >> grammar.num;
    string s;
    printf("Please enter the production:\n");
    for (int i = 0; i < grammar.num; i++) {
        cin >> s;
        Production tmp;
        tmp.left = s[0];
        for (int j = 3; j < s.size(); j++) {
            tmp.rights.push_back(s[j]);
        }
        grammar.prods.push_back(tmp);
    }
    printf("Please enter the non-terminators(end with #):\n");
    char ch;
    cin >> ch;
    while (ch != '#') {
        grammar.N.push_back(ch);
        cin >> ch;
    }
    printf("Please enter the terminators(end with #):\n");
    cin >> ch;
    while (ch != '#') {
        grammar.T.push_back(ch);
        cin >> ch;
    }
    /* 把$当作终结符 */
    grammar.T.push_back('$');
    /* 求FIRST集 */
    getFirstSet();
    /* 构建DFA和SLR1预测分析表 */
    DFA();
    productLR1AnalysisTable();
    /* 读入待分析串并初始化分析栈 */
    printf("Please enter the String to be analyzed:\n");
    cin >> str;
    str += '$';
    ST.push(pair<int, char>(0, '-'));
}

void process() {
    int ip = 0;
    printf("The ans:\n");
    do {
        int s = ST.top().first;
        char a = str[ip];
        int j = isInT(a) - 1;
        //移进
        if (action[s][j].first == 1) {
            ST.push(pair<int, char>(action[s][j].second, a));
            ip = ip + 1;
        }
        //规约
        else if (action[s][j].first == 2) {
            Production &P = grammar.prods[action[s][j].second];
            printf("%c->", P.left);
            for (char right : P.rights) {
                ST.pop();
                printf("%c", right);
            }
            printf("\n");
            s = ST.top().first;
            char A = P.left;
            j = isInN(A) - 1;
            ST.push(pair<int, char>(goton[s][j], A));
        }
        //接受
        else if (action[s][j].first == 3) {
            printf("ACC\n");
            return;
        } else {
            printf("error\n");
        }
    } while(true);
}

int main() {
    initGrammar();
    process();
    return 0;
}

