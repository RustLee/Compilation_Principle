//
// Created by Rust Lee on 4/11/2020.
//
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#define _KEY_WORDEND "haven't expanding"
using namespace std;

typedef struct {
    int type;
    char * word;
}WORD;

char ch;
char input[255];
char token[255];
char * keywords[] = {"int","short","long","double","float","char","if","else","do","while",
"return","break","main","void"};

int p_input;
int p_token;

WORD * scanner();

int main(){
    int over = 1;
    WORD* word = new WORD;
    cout << "reading code from code.txt" << endl;
    FILE *fp;
    if((fp = freopen("code.txt","r",stdin)) == nullptr){
        cout << "File doesn't exist!" << endl;
        return 0;
    } else{
        while ((scanf("%[^#]s",&input)) != EOF){
            p_input = 0;
            cout << "your words:" << "\n" << input << "\n";
            while (over < 1000 && over != -1){
                word = scanner();
                if (word->type < 1000){
                    if (word->type != 990)
                        cout << "[ " << word->type << "\t" << word->word << " ]" << endl;
                }
                over = word->type;
            }
            scanf("%[^#]s",input);
        }
    }
    return 0;
}

char m_getch(){
    ch = input[p_input];
    p_input++;
    return ch;
}

void get_blank(){
    while (ch == ' ' || ch == 10)
    {
        ch = input[p_input];
        p_input++;
    }
}

bool letter(){
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool digit(){
    return (ch >= '0' && ch <= '9');
}

int reserce(){
    int i = 1;
    while (strcmp(keywords[i],_KEY_WORDEND) != 0){
        if(!strcmp(keywords[i],token)){
            return i + 1;
        }
        i++;
    }
    return 10;
}

void retract(){
    p_input--;
}