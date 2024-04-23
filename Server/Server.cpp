#include <winsock2.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

#define MAX_CLIENTS 10
#define DEFAULT_BUFLEN 4096

#pragma comment(lib, "ws2_32.lib") // Winsock library
#pragma warning(disable:4996)

SOCKET server_socket;

vector<string> history;

struct Order
{
    string dishName;
    int cookTime;
};

mutex mtx;

void addOrder(const Order& order)
{
    mtx.lock();
    history.push_back(order.dishName);
    mtx.unlock();
}

void processOrders()
{
    while (true)
    {
        mtx.lock();
        if (history.empty())
        {
            mtx.unlock();
            continue;
        }

        string order = history.front();
        history.erase(history.begin());
        mtx.unlock();

        cout << "Received order: " << order << endl;

        // Process the order
        // In this example, we're just printing out the order information
        // In a real application, you would perform the necessary processing
        // Here, we're just sleeping for a short time to simulate processing
        this_thread::sleep_for(chrono::seconds(3));
        cout << "Order processed: " << order << endl;
    }
}

int main()
{
    cout << "Start server... DONE." << endl;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Failed. Error Code: " << WSAGetLastError() << endl;
        return 1;
    }

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "Could not create socket: " << WSAGetLastError() << endl;
        return 2;
    }

    // Prepare the sockaddr_in structure
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind socket
    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cerr << "Bind failed with error code: " << WSAGetLastError() << endl;
        return 3;
    }

    // Listen to incoming connections
    listen(server_socket, MAX_CLIENTS);

    cout << "Server is waiting for incoming connections..." << endl;

    // Start processing thread
    thread orderThread(processOrders);

    while (true) {
        SOCKET new_socket; // New client socket
        sockaddr_in address;
        int addrlen = sizeof(sockaddr_in);
        if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) {
            cerr << "Accept function error" << endl;
            return 5;
        }

        cout << "New connection, socket fd is " << new_socket << ", ip is: " << inet_ntoa(address.sin_addr) << ", port: " << ntohs(address.sin_port) << endl;

        // Add new socket to array of sockets
        mtx.lock();
        history.push_back("New connection established");
        mtx.unlock();
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
