#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

using namespace std;

int main()
{
    string ipAddress = "127.0.0.1";
    int port = 54003;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cerr << "Can't create socket" << endl;
        return 1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);

    if (inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr) <= 0)
    {
        cerr << "Invalid address/Address not supported" << endl;
        close(sock);
        return 1;
    }

    if (connect(sock, (sockaddr*)&hint, sizeof(hint)) == -1)
    {
        cerr << "Can't connect to server" << endl;
        close(sock);
        return 1;
    }

    char buf[4096];
    string userInput;

    cout << "Connected to server. Type messages (empty line to quit):" << endl;

    do
    {
        cout << "> ";
        getline(cin, userInput);

        if (userInput.size() > 0)
        {
            int sendResult = send(sock, userInput.c_str(), userInput.size(), 0);
            if (sendResult == -1)
            {
                cerr << "Send failed" << endl;
                break;
            }

            memset(buf, 0, 4096);

            int bytesReceived = recv(sock, buf, 4096, 0);
            if (bytesReceived == -1)
            {
                cerr << "Error in recv()" << endl;
                break;
            }
            else if (bytesReceived == 0)
            {
                cout << "Server disconnected" << endl;
                break;
            }
            else if (bytesReceived > 0)
            {
                cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
            }
        }

    } while (userInput.size() > 0);

    close(sock);

    return 0;
}
