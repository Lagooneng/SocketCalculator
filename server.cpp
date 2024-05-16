#include<string>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<stack>
#include<cctype>
#include "server.h"
using namespace std;
#define MAXBUFFER 100


int main() {
    int ss, sa;
    char recvBuf[100], sendBuf[100];
    struct sockaddr_in ssa;

    memset(&ssa, 0, sizeof(ssa));
    ssa.sin_family = AF_INET;
    ssa.sin_addr.s_addr = htonl(INADDR_ANY);
    ssa.sin_port = htons(12345);

    ss = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if ( ss < 0 )
    {
        cout << "서버 오픈 실패" << endl;
        return -1;
    }
    
    bind(ss, (struct sockaddr *)&ssa, sizeof(ssa));
    int n = listen(ss, 10);
    if ( n < 0 )
    {
        cout << "listen() 실패" << endl;
        return -1;
    }

    cout << "Calculator Server Start" << endl;

    while (1)
    {
        sa = accept(ss, 0, 0);
        cout << "Client is detected-" << endl;
        recv(sa, recvBuf, 100, 0);
        
        string str(recvBuf);

        if (!checkString(str))
        {
            cout << "잘못된 입력 발생" << endl;
            strcpy(sendBuf, "Error");
            send(sa, sendBuf, MAXBUFFER, 0);
            close(sa);
            continue;
        }
        
        
        str = handler(str);

        float num = cal(str);

        strcpy(sendBuf, to_string(num).c_str());
        send(sa, sendBuf, MAXBUFFER, 0);
        close(sa);
    }
    
    return 0;
}

int priority(char c) {
    if( c == '+' || c == '-' ) return 1;
    if( c == '*' || c == '/' ) return 2;
    else return 0;
}

float cal(string str) {
    int size = (int)str.size();
    string temp = "";
    stack<float> stack;
    char c;
    
    for (int i = 0; i < size; ++i)
    {
        c = str[i];

        if ( isdigit(c) || c == '.' )
        {
            temp += c;
        }
        else {
            if( stack.empty() ) {
                stack.push( stof(temp) );
                temp = "";
                continue;
            }
            
            if ( c == ',' )
            {
                if( temp == "" ) continue;
                stack.push( stof(temp) );
                temp = "";
                continue;
            }
            
            if ( temp != "" )
            {
                stack.push( stof(temp));
            }
            
            float num1 = stack.top();
            stack.pop();
            float num2 = stack.top();
            stack.pop();
            temp = "";

            float result;

            if ( c == '+' )
            {
                result = num2 + num1;
            }
            else if( c == '-' ) {
                result = num2 - num1;
            }
            else if( c == '*' ) {
                result = num2 * num1;
            }
            else if( c == '/' ) {
                result = num2 / num1;
            }

            stack.push(result);
        }
    }


    return stack.top();
}

string handler(string str) {
    int size = str.size();
    int dotFlag = 0;
    std::stack<char> stack;
    string temp = "";
    
    for (int i = 0; i < size; i++)
    {
        char c = str[i];

        if( isspace(c)  ) continue;
        else if( isdigit(c) || c == '.' ) {
            if(dotFlag == 1) temp += ',';
            dotFlag = 0;
            temp += c;
            continue;
        }
        else if( !isdigit(c) && c != '(' ) {
            dotFlag = 1;
        }

        if( stack.empty() || c == '(' ) {
            stack.push(c);
            continue;
        }

        if( c == ')' ) {
            while ( stack.top() != '(' )
            {
                temp += stack.top();
                stack.pop();
            }
            stack.pop();
            continue;
        }

        if( priority(stack.top()) < priority(c) ) {
            stack.push(c);
        }
        else {
            while (!stack.empty() && priority(stack.top()) >= priority(c)) 
            {
                temp += stack.top();
                stack.pop();
            }
            stack.push(c);
        }
    }
    
    while (!stack.empty())
    {
        temp += stack.top();
        stack.pop();
    }
    
    return temp;
}

bool checkString(string str) {
    int bracketCount = 0;
    int decimalPointCount = 0;
    char prev = '\0';
    char c;


    int length = (int)str.size();

    for (int i = 0; i < length; ++i)
    {
        if( str[i] == ' ' ) continue;
        prev = c;
        c = str[i];
        
        if( c != '(' && c != ')' && c != '+' && 
            c != '-' && c != '/' && c != '*' &&
            c != '.' && c != ',' && !isdigit(c) ) {
                return false;
        }


        if ( !isdigit(c) )  //  특수문자
        {
            if ( c == '.' )
            {
                decimalPointCount++;
                if(decimalPointCount > 1) return false;
            }
            else {
                decimalPointCount = 0;
            }

            if( prev != '\0' ) {
                if ( ( prev == '+' ||  prev == '-' || prev == '*' || prev == '/') &&
                     ( c == '+' ||  c == '-' || c == '*' || c == '/') )
                {
                    return false;
                }
            }
        }
        

        if( c == '(' ) {
            bracketCount++;
        }
        else if ( c == ')' )
        {
            if (bracketCount <= 0)
            {
                return false;
            }
            bracketCount--;
        }
 
    }
    
    if (bracketCount != 0)
    {
        return false;
    }
    

    return true;
}