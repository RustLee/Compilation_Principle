//
// Created by Rust Lee on 4/11/2020.
//
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>

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
"return","break","main","void",_KEY_WORDEND};

int p_input;
int p_token;

WORD * scanner();

int main(){
    int over = 1;
    WORD* word = new WORD;
    cout << "reading code from code.txt" << endl;
    FILE *fp;
    ofstream file_out;
    file_out.open(R"(D:\Clion\Compilation_Principle\Ex01\token_out.txt)",ios::out | ios::app);
    if(!file_out.is_open())
        return 0;
    if((fp = freopen(R"(D:\Clion\Compilation_Principle\Ex01\code.txt)","r",stdin)) == nullptr){
        cout << "File doesn't exist!" << endl;
        return 0;
    } else{
        while ((scanf("%[^#]s",&input)) != EOF){
            p_input = 0;
            cout << "your words:" << "\n" << input << "\n";
            while (over < 1000 && over != -1 && over != 200){
                word = scanner();
                if (word->type < 1000){
                    if (word->type != 999){
                        cout << "[ " << word->type << "\t" << word->word << " ]" << endl;
                        file_out << "[ " << word->type << "\t" << word->word << " ]" << endl;
                    }
                }
                over = word->type;
            }
            scanf("%[^#]s",&input);
        }
    }
    file_out.close();
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

void concat(){
    token[p_token] = ch;
    p_token++;
    token[p_token] = '\0';
}

bool letter(){
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool digit(){
    return (ch >= '0' && ch <= '9');
}

int reserve(){
    int i = 0;
    while (strcmp(keywords[i],_KEY_WORDEND) != 0){
        if(!strcmp(keywords[i],token)){
            return i + 1;
        }
        i++;
    }
    return 100;
}

void retract(){
    p_input--;
}

WORD * scanner(){
    WORD * inner_word = new WORD ;
    inner_word->type = 0;
    inner_word->word = "";
    p_token = 0;
    m_getch();
    get_blank();

    if(letter()){
        while (letter() || digit()){
            concat();
            m_getch();
        }
        retract();
        inner_word->type = reserve();
        inner_word->word = token;
        return inner_word;
    } else if (digit()){
        while (digit()){
            concat();
            m_getch();
        }
        retract();
        inner_word->type = 101;
        inner_word->word = token;
        return(inner_word);
    }else switch (ch){
            case '(':
                inner_word->type = 15;
                inner_word->word = "(";
                return(inner_word);
            case ')':
                inner_word->type = 16;
                inner_word->word = ")";
                return(inner_word);
            case '[':
                inner_word->type = 17;
                inner_word->word = "[";
                return(inner_word);
            case ']':
                inner_word->type = 18;
                inner_word->word = "]";
                return(inner_word);
            case '=':
                m_getch();
                if(ch == '='){
                    inner_word->type = 37;
                    inner_word->word = "==";
                    return(inner_word);
                }
                retract();
                inner_word->type = 21;
                inner_word->word = "=";
                return(inner_word);
            case '.':
                inner_word->type = 22;
                inner_word->word = ".";
                return(inner_word);
            case '!':
                m_getch();
                if(ch == '='){
                    inner_word->type = 38;
                    inner_word->word = "!=";
                    return(inner_word);
                }
                retract();
                inner_word->type = -1;
                inner_word->word = "ERROR";
                return(inner_word);
            case '~':
                inner_word->type = 24;
                inner_word->word = "~";
                return(inner_word);
            case '+':
                m_getch();
                if(ch == '+'){
                    inner_word->type = 25;
                    inner_word->word = "++";
                    return(inner_word);
                } else if (ch == '='){
                    inner_word->type = 42;
                    inner_word->word = "+=";
                    return(inner_word);
                } else{
                    retract();
                    inner_word->type = 32;
                    inner_word->word = "+";
                    return(inner_word);
                }
            case '-':
                m_getch();
                if(ch == '-'){
                    inner_word->type = 26;
                    inner_word->word = "--";
                    return(inner_word);
                } else if (ch == '='){
                    inner_word->type = 44;
                    inner_word->word = "-=";
                    return(inner_word);
                } else if (ch == '>'){
                    inner_word->type = 43;
                    inner_word->word = "->";
                    return(inner_word);
                } else{
                    retract();
                    inner_word->type = 32;
                    inner_word->word = "+";
                    return(inner_word);
                }
            case ',':
                inner_word->type = 28;
                inner_word->word = ",";
                return(inner_word);
            case '*':
                inner_word->type = 29;
                inner_word->word = "*";
            case '/':
                m_getch();
                if (ch == '*'){
                    m_getch();
                    while (ch != '*'){
                        m_getch();
                        if (ch == '/'){
                            inner_word->type = 107;
                            inner_word->word = "/*/";
                            return(inner_word);
                        }
                    }
                } else{
                    retract();
                    inner_word->type = 30;
                    inner_word->word = "/";
                    return(inner_word);
                }
            case '>':
                m_getch();
                if(ch == '='){
                    inner_word->type = 35;
                    inner_word->word = ">=";
                    return(inner_word);
                }
                retract();
                inner_word->type = 33;
                inner_word->word = ">";
                return(inner_word);
            case '<':
                m_getch();
                if(ch == '='){
                    inner_word->type = 36;
                    inner_word->word = "<=";
                    return(inner_word);
                }
                retract();
                inner_word->type = 34;
                inner_word->word = "<";
                return(inner_word);
            case '|':
                m_getch();
                if(ch == '|'){
                    inner_word->type = 39;
                    inner_word->word = "||";
                    return(inner_word);
                }
                retract();
                inner_word->type = -1;
                inner_word->word = "ERROR";
                return(inner_word);
            case '&':
                m_getch();
                if(ch == '&'){
                    inner_word->type = 40;
                    inner_word->word = "&&";
                    return(inner_word);
                }
                retract();
                inner_word->type = -1;
                inner_word->word = "ERROR";
                return(inner_word);
            case '?':
                m_getch();
                if(ch == ':'){
                    inner_word->type = 41;
                    inner_word->word = "?:";
                    return(inner_word);
                }
                retract();
                inner_word->type = -1;
                inner_word->word = "ERROR";
                return(inner_word);
            case '{':
                inner_word->type = 109;
                inner_word->word = "{";
                return(inner_word);
            case '}':
                inner_word->type = 110;
                inner_word->word = "}";
                return(inner_word);
            case ':':
                inner_word->type = 108;
                inner_word->word = ":";
                return(inner_word);
            case ';':
                inner_word->type = 105;
                inner_word->word = ";";
                return(inner_word);
            case '\0':
                inner_word->type = 200;
                inner_word->word = "OVER";
                return(inner_word);
            case '#':
                inner_word->type = 0;
                inner_word->word = "#";
            default:
                inner_word->type = -1;
                inner_word->word = "ERROR";
                return(inner_word);
    }
}

