#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#define MAXBUFFER 100

int main() {
    int cs;
    char recvBuf[100], sendBuf[100];
    struct sockaddr_in csa;

    memset(&csa, 0, sizeof(csa));
    csa.sin_family = AF_INET;
    csa.sin_addr.s_addr = inet_addr("127.0.0.1");
    csa.sin_port = htons(12345);

    cs = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (cs < 0)
    {
        std::cout << "소켓 생성 실패" << std::endl;
        close(cs);
        return -1;
    }
    
    int n = connect(cs, (struct sockaddr *) &csa, sizeof(csa));
    if (n < 0)
    {
        std::cout << "연결 실패" << std::endl;
        close(cs);
        return -1;
    }

    printf("식 입력: ");
    std::string str;
    getline(std::cin, str);
    strcpy(sendBuf, str.c_str());
    
    send(cs, sendBuf, 100, 0);

    recv(cs, recvBuf, 100, 0);
    
    std::string result(recvBuf);

    if( result.compare("Error") == 0 ) {
        std::cout << "입력이 잘못되었습니다." << std::endl;
        close(cs);
        return 0;
    }

    int case1 = std::stoi(result);
    float case2 = std::stof(result);
    

    if ( (float)case1 == case2 )
    {
        printf("계산 결과: [%d]", case1);
    }
    else {
        printf("계산 결과: [%.4f]", case2);
    }
    
    printf("\n");

    close(cs);

    return 0;
}