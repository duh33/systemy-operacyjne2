#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>


#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int PORT = 12345;
const int BACKLOG = 16;

mutex clients_mutex;
vector<SOCKET> clients;

mutex history_mutex;
vector<string> chat_history;
const size_t MAX_HISTORY = 100;

void broadcast_message(const string& msg, SOCKET exclude = INVALID_SOCKET) {
    lock_guard<mutex> lock(clients_mutex);
    for (SOCKET client_sock : clients) {
        if (client_sock != exclude) {
            send(client_sock, msg.c_str(), (int)msg.size(), 0);
        }
    }
}

void handle_client(SOCKET client_sock) {
    // Send chat history to the new client
    {
        lock_guard<mutex> lock(history_mutex);
        for (const auto& line : chat_history) {
            send(client_sock, line.c_str(), (int)line.size(), 0);
        }
    }

    char buffer[512];
    while (true) {
        int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            cout << "Client disconnected.\n";
            break;
        }
        buffer[bytes_received] = '\0';
        string message = string(buffer);

        // Save message to history
        {
            lock_guard<mutex> lock(history_mutex);
            chat_history.push_back(message);
            if (chat_history.size() > MAX_HISTORY) {
                chat_history.erase(chat_history.begin()); // Remove oldest
            }
        }

        cout << "Received: " << message;
        // Broadcast to all other clients
        broadcast_message(message, client_sock);
    }

    // Remove client from list
    {
        lock_guard<mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), client_sock), clients.end());
    }
    closesocket(client_sock);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Bind failed\n";
        closesocket(server_sock);
        WSACleanup();
        return 1;
    }

    if (listen(server_sock, BACKLOG) == SOCKET_ERROR) {
        cerr << "Listen failed\n";
        closesocket(server_sock);
        WSACleanup();
        return 1;
    }

    cout << "Server listening on port " << PORT << endl;

    while (true) {
        SOCKET client_sock = accept(server_sock, nullptr, nullptr);
        if (client_sock == INVALID_SOCKET) {
            cerr << "Accept failed\n";
            continue;
        }

        {
            lock_guard<mutex> lock(clients_mutex);
            clients.push_back(client_sock);
        }
        cout << "New client connected\n";

        thread(handle_client, client_sock).detach();
    }

    closesocket(server_sock);
    WSACleanup();
    return 0;
}