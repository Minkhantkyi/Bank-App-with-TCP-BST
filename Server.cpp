#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include "BST.h"
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

#pragma comment(lib,"Ws2_32.lib")

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "27015"

using namespace std;

BST x;

void main() {

	string userArr[5] = { "mkk" };
	int userSize = sizeof(userArr);
	int userID = 0;

    WSADATA wsaData;
    WORD ver = MAKEWORD(2, 2);
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(ver, &wsaData);
    if (iResult != 0) 
    {
        cout << " WSAStartup fail with error : " << iResult << endl;
        return;
    }
    else {
        cout << " WSAStartup Complete! " << endl;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) 
    {
        cout << " getaddrinfo fail with error : " << iResult << endl;
        WSACleanup();
        return;
    }
    else {
        cout << " getaddrinfo Complete! " << endl;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) 
    {
        cout << " Socket fail with error : " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return;
    }
    else {
        cout << " Socket Complete! " << endl;
    }

    // Setup TCP Listening Socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) 
    {
        cout << " Binding fail with error : " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    else {
        cout << " Binding Complete! " << endl;
    }

    freeaddrinfo(result);

    // Tell Winsock the socket is for listening 
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) 
    {
        cout << " Listening fail with error : " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    else 
    {
        cout << " Listen Complete! " << endl;
    }

    // Accept a client socket
    sockaddr_in client;
    int clientSize = sizeof(client);

    ClientSocket = accept(ListenSocket, (sockaddr*)&client, &clientSize);
    if (ClientSocket == INVALID_SOCKET) {
        cout << " Accept client socket fail with error : " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    else
    {
        cout << " Accept client socket Complete! " << endl;
    }

    char host[NI_MAXHOST];          // Client's remote name
    char service[NI_MAXSERV];       // Service (i.e. port) the client is connect on

    ZeroMemory(host, NI_MAXHOST);           // same as memset(host, 0, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) 
    {
        cout << " " << host << " connected on port " << service << endl;
    }
    else 
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << " " << host << " connected on port " << ntohs(client.sin_port) << endl;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    int loop = 0;
    string username, password, deposit, amount;
    do {

        ZeroMemory(recvbuf, recvbuflen);

        // Wait for client to send data
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        string optionNum = string(recvbuf, 0, iResult);

        if (iResult > 0) 
        {
            
            if (optionNum == "1") 
            {
                int lc = 0;
                x.loadData();

                cout << "\n\n Registering New Account.\n";

                while (lc == 0) 
                {
                    ZeroMemory(recvbuf, recvbuflen);

                    int bytes = recv(ClientSocket, recvbuf, recvbuflen, 0);
                    username = string(recvbuf, 0, bytes);
                    cout << " " << username << endl;
                    int result = x.search(username);

                    if (result == true) 
                    {
                        send(ClientSocket, "0", bytes, 0);
                        cout << " Invalid username " << endl;
                    }
                    else 
                    {
                        send(ClientSocket, "1", bytes, 0);
                        cout << " Valid username " << endl;
                        lc++;
                    }
                }

                int bytesPassword = recv(ClientSocket, recvbuf, recvbuflen, 0);
                password = string(recvbuf, 0, bytesPassword);
                cout << " Received Password : " << password << endl;
                int bytesDeposit = recv(ClientSocket, recvbuf, recvbuflen, 0);
                deposit = string(recvbuf, 0, bytesDeposit);
                cout << " Received Deposit : " << deposit << endl;
                x.insert(username, password, deposit);
                x.saveData();
            }
            else if (optionNum == "2") 
            {
                int lOne = 0;
                x.loadData();

                cout << "\n\n log In \n";

                while (lOne == 0) 
                {
                    ZeroMemory(recvbuf, recvbuflen);

                    int bytes = recv(ClientSocket, recvbuf, recvbuflen, 0);
                    username = string(recvbuf, 0, bytes);

                    if (username == "0") 
                    {
                        send(ClientSocket, "2", bytes, 0);
                        cout << " Exit from Log in " << endl;
                        lOne++;
                    }
                    else 
                    {
                        cout << " Log in user : " << username << endl;
                        int result = x.search(username);
                        
                        if (result == true)
                        {
                            send(ClientSocket, "1", bytes, 0);
                            cout << " Valid User " << endl;
                            
                            struct Info data = x.searchForExtract(username);

                            password = data._password;
                            send(ClientSocket, password.c_str(), password.size() + 1, 0);
                            cout << " Password was sent. " << endl;

                            amount = data._amount;
                            send(ClientSocket, amount.c_str(), amount.size() + 1, 0);
                            cout << " Amount in acc was sent. " << endl;

                            int funLoop = 0, funByte;
                            string fun;
                            do 
                            {
                                x.loadData();
                                data = x.searchForExtract(username);

                                ZeroMemory(recvbuf, recvbuflen);
                                funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                fun = string(recvbuf, 0, funByte);

                                if (fun == "CI") 
                                {
                                    cout << "\n Cash In \n";

                                    string oriCash = data._amount;
                                    
                                    ZeroMemory(recvbuf, recvbuflen);
                                    funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                    amount = string(recvbuf, 0, funByte);
                                    cout << " New amount is received. " << endl;
                                    data._amount = amount;
                                    cout << " Amount in the Acc is updated. New Value : " << data._amount << endl;

                                    // Store cash in record
                                    float inCash = stof(data._amount) - stof(oriCash);
                                    string filename = data._username + "_Record.txt";
                                    ofstream cashIn(filename, ios::app);
                                    
                                    if (cashIn.is_open()) 
                                    {
                                        cashIn << "Cash In : " << setprecision(2) << fixed << inCash << " $ " << endl;
                                    }
                                    else
                                    {
                                        cout << " Unable to open! \n";
                                    }
                                    cout << " Cash In record is stored." << endl;

                                    x.update(data);
                                    x.saveData();
                                }
                                else if(fun == "CO") 
                                {
                                    cout << "\n Cash Out \n";

                                    string oriCash = data._amount;
                                    
                                    ZeroMemory(recvbuf, recvbuflen);
                                    funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                    amount = string(recvbuf, 0, funByte);
                                    cout << " Left amount is received. " << endl;
                                    data._amount = amount;
                                    cout << " Amount in the Acc is updated. New Value : " << data._amount << endl;

                                    // Store cash out record
                                    float outCash = stof(data._amount) - stof(oriCash);
                                    string fileName = data._username + "_Record.txt";
                                    ofstream cashOut(fileName, ios::app);

                                    if (cashOut.is_open()) 
                                    {
                                        cashOut << "Cash Out : " << setprecision(2) << fixed << outCash << " $ " << endl;
                                    }
                                    else 
                                    {
                                        cout << " Unable to open! \n";
                                    }
                                    cout << " Cash Out record is stored." << endl;

                                    x.update(data);
                                    x.saveData();
                                }
                                else if (fun == "TM") 
                                {
                                    cout << "\n Transfer Money \n";

                                    int ltm = 0;
                                    string recAccName;
                                    do 
                                    {
                                        ZeroMemory(recvbuf, recvbuflen);
                                        funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                        recAccName = string(recvbuf, 0, funByte);

                                        int result = x.search(recAccName);

                                        if (result == true)
                                        {
                                            send(ClientSocket, "1", funByte, 0);
                                            cout << " Valid User " << endl;
                                            ltm++;
                                        }
                                        else
                                        {
                                            send(ClientSocket, "0", funByte, 0);
                                            cout << " Invalid User " << endl;
                                        }

                                    } while (ltm < 1);

                                    struct Info recAcc = x.searchForExtract(recAccName);
                                    string amount = recAcc._amount;

                                    send(ClientSocket, amount.c_str(), amount.size() + 1, 0);
                                    cout << " Receiver Acc amount is sent. " << endl;

                                    // Received recAcc money
                                    ZeroMemory(recvbuf, recvbuflen);
                                    funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                    amount = string(recvbuf, 0, funByte);
                                    recAcc._amount = amount;
                                    cout << " Receiver's new amount : " << amount << endl;

                                    // Treansfered amount
                                    ZeroMemory(recvbuf, recvbuflen);
                                    funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                    string transMoney = string(recvbuf, 0, funByte);
                                    float tm = stof(transMoney);

                                    // Store record for receiver acc
                                    string recvFilename = recAccName + "_Record.txt";
                                    ofstream recvRecord(recvFilename, ios::app);

                                    if (recvRecord.is_open()) 
                                    {
                                        recvRecord << "Received From " << data._username 
                                            << " amount : " << setprecision(2) << fixed << tm << endl;
                                    }
                                    else
                                    {
                                        cout << " Unable to open! \n";
                                    }
                                    cout << " Record is stored for Receiver." << endl;
                                   
                                    // Received Sender money
                                    ZeroMemory(recvbuf, recvbuflen);
                                    funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                    amount = string(recvbuf, 0, funByte);
                                    data._amount = amount;
                                    cout << " Sender's left amount : " << amount << endl;

                                    // Store record for sender acc
                                    string senderFilename = data._username + "_Record.txt";
                                    ofstream senderRecord(senderFilename, ios::app);

                                    if (senderRecord.is_open()) 
                                    {
                                        senderRecord << "Sent to " << recAccName 
                                            << " amount : " << setprecision(2) << fixed << tm << endl;
                                    }
                                    else
                                    {
                                        cout << " Unable to open! \n";
                                    }
                                    cout << " Record is stored for Sender." << endl;

                                    x.update(recAcc);
                                    x.update(data);
                                    x.saveData();
                                }
                                else if (fun == "CP") 
                                {
                                    ZeroMemory(recvbuf, recvbuflen);
                                    funByte = recv(ClientSocket, recvbuf, recvbuflen, 0);
                                    password = string(recvbuf, 0, funByte);
                                    cout << " New password : " << password << " is received. " << endl;
                                    data._password = password;
                                    cout << " Acc's password is updated." << endl;

                                    x.update(data);
                                    x.saveData();
                                }
                                else if (fun == "R") 
                                {
                                    cout << "\n Record \n";

                                    string record;
                                    string filename = data._username + "_Record.txt";
                                    ifstream read(filename);
                                    stringstream ss;

                                    if (read.is_open())
                                    {
                                        ss << read.rdbuf();
                                        record = ss.str();
                                    }
                                    else 
                                    {
                                        cout << " Unable to open! \n";
                                    }
                                    send(ClientSocket, record.c_str(), record.size() + 1, 0);
                                    cout << " All records are sent." << endl;
                                }
                                else if (fun == "Q") 
                                {
                                    funLoop++;
                                }

                            } while (funLoop < 1);

                        }
                        else
                        {
                            send(ClientSocket, "0", bytes, 0);
                            cout << " Invalid User " << endl;
                        }
                    }
                }
            }
            else if (optionNum == "3") 
            {
                loop++;
            }
        }
        else if (iResult == 0) 
        {
            cout << " Connection closing..." << endl;
        }
        else 
        {
            cout << " Recv fail with error : " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

    } while (loop == 0);

    // Shut down the connection sice we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        cout << " Shutdown fail with error : " << WSAGetLastError() << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return;
    }
    else 
    {
        cout << " Shutdown connection with Client... " << endl;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

}
