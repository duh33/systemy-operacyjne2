#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "ws2_32.lib")

using namespace std;

const char* SERVER_IP = "127.0.0.1";
const int PORT = 12345;

void receive_messages(SOCKET sock) {
    char buffer[512];
    while (true) {
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            cout << "\nDisconnected from server.\n";
            break;
        }
        buffer[bytes_received] = '\0';

        cout << "[New message]: " << buffer;
        if (buffer[bytes_received - 1] != '\n')
            cout << '\n';
    }
}


int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        cerr << "Failed to connect to server\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "Connected to chat server at " << SERVER_IP << ":" << PORT << "\n";

    thread recv_thread(receive_messages, sock);

    string line;
    while (true) {
        getline(cin, line);
        if (line.empty()) continue;
        line += '\n';  // newline to separate messages
        int send_result = send(sock, line.c_str(), (int)line.size(), 0);
        if (send_result == SOCKET_ERROR) {
            cerr << "Failed to send message\n";
            break;
        }
    }

    closesocket(sock);
    WSACleanup();

    if (recv_thread.joinable())
        recv_thread.join();

    return 0;
}
