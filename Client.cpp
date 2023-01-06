#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "27015"
char buff[DEFAULT_BUFLEN];

using namespace std;

void addUnderscore(string* str);
void addSpace(string* str);
void cashIn(float* cash);
void cashOut(float* cash);
void changePass(string* str, string oriPassword);
void transferMoney(float* tm, SOCKET ts);
void showRecord(SOCKET ts);

void main() {

	int iResult = 0;
	// Initialize Winsock
	WSADATA wsaData;
	WORD ver = MAKEWORD(2, 2);
	iResult = WSAStartup(ver, &wsaData);
	if (iResult != 0) {
		cout << " WSAStartup fail with error : " << iResult << endl;
		return;
	}
	else 
	{
		cout << " WSAStartup Complete! " << endl;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) 
	{
		cout << " getaddrinfo fail with error : " << iResult << endl;
		WSACleanup();
		return;
	}
	else 
	{
		cout << " getadrrinfo Complete! " << endl;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) 
		{
			cout << " Create Socket fail with error : " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}else
		{
			cout << " Create Socket Complete! " << endl;
		}

		// Connect to serve
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;

	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cout << " Unable to connect to Server with error : " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}
	else 
	{
		cout << " Connected to Server " << endl;
	}

	string userInput;
	string username;
	string password;
	string deposit;
	int control = 0;

	cout << "\n TCP BANKING SYSTEM - WELCOME! \n\n";
	cout << " MAIN MENU \n";

	do {

		cout << "\n 1) Register new Account " << endl;
		cout << " 2) Login " << endl;
		cout << " 3) Exit " << endl;
		cout << " >  ";
		getline(cin, userInput);

		if (userInput == "1") {

			int controlOne = 0;

			// Send data to server for creating new acc
			iResult = send(ConnectSocket, userInput.c_str(), userInput.size() + 1, 0);

			cout << "\n\n REGISTER NEW ACCOUNT " << endl;

			do {

				cout << "\n Enter username : ";
				getline(cin, username);
				string* str = &username;
				addUnderscore(str);

				// Send username to server for checking
				iResult = send(ConnectSocket, username.c_str(), username.size() + 1, 0);
				if (iResult == SOCKET_ERROR) 
				{
					cout << " Error : " << WSAGetLastError() << endl;
					closesocket(ConnectSocket);
					WSACleanup();
					return;
				}
				else 
				{
					// Wait for response
					ZeroMemory(buff, DEFAULT_BUFLEN);
					int bytesReceived = recv(ConnectSocket, buff, DEFAULT_BUFLEN, 0);
					if (string(buff, 0, bytesReceived) == "1")
					{
						int confirm = 0;
						string confirmPassword;
						cout << " Enter password : ";
						getline(cin, password);

						cout << " Confirm password : ";
						getline(cin, confirmPassword);

						do 
						{
							if (confirmPassword != password) 
							{
								cout << "\n Invalid password!";
								cout << "\n Confirm password : ";
								getline(cin, confirmPassword);
							}
							else 
							{
								confirm++;
							}

						} while (confirm == 0);

						confirm = 0;
						cout << endl;

						do 
						{
							cout << " Enter deposit amount (at least 10$) : ";
							getline(cin, deposit);

							float checkDeposit = stof(deposit);
							if (checkDeposit >= 10) {
								confirm++;
							}

						} while (confirm == 0);

						send(ConnectSocket, password.c_str(), password.size() + 1, 0);
						send(ConnectSocket, deposit.c_str(), deposit.size() + 1, 0);
						controlOne++;

						cout << " The Acc with username " << username << " has been created! \n\n";
					}
					else
					{
						cout << " Invalid username. Plz.. try again." << endl;
					}
				}

			} while (controlOne == 0);

		}
		else if (userInput == "2") 
		{
			int controlTwo = 0;

			// Send data to server for creating new acc
			iResult = send(ConnectSocket, userInput.c_str(), userInput.size() + 1, 0);

			cout << "\n\n LOG IN " << endl;

			do 
			{
				cout << "\n Enter username (or) 0 to Exit : ";
				getline(cin, username);
				string* str = &username;
				addUnderscore(str);
				
				// Send username to server for checking
				iResult = send(ConnectSocket, username.c_str(), username.size() + 1, 0);
				if (iResult == SOCKET_ERROR)
				{
					cout << " Error : " << WSAGetLastError() << endl;
					closesocket(ConnectSocket);
					WSACleanup();
					return;
				}
				else 
				{
					// Wait for response
					ZeroMemory(buff, DEFAULT_BUFLEN);
					int bytesReceived = recv(ConnectSocket, buff, DEFAULT_BUFLEN, 0);
					
					if (string(buff, 0, bytesReceived) == "1") 
					{
						bytesReceived = recv(ConnectSocket, buff, DEFAULT_BUFLEN, 0);
						string checkPassword = string(buff, 0, bytesReceived);

						int lp = 0;
						cout << " Enter password : ";
						getline(cin, password);

						do 
						{
							if (password != checkPassword) 
							{
								cout << "\n Invalid password!";
								cout << "\n Enter password : ";
								getline(cin, password);
							}
							else 
							{
								lp++;
							}

						} while (lp == 0);

						bytesReceived = recv(ConnectSocket, buff, DEFAULT_BUFLEN, 0);
						string amount = string(buff, 0, bytesReceived);
						float fAmount = stof(amount);
						float* pFAmount = &fAmount;

						string fun;
						int lf = 0;
						string withSpace = username;
						string* pWS = &withSpace;
						addSpace(pWS);
						cout << "\n\n USER INFO & FUNCTIONS " << endl;

						while (lf < 1) 
						{
							string changePassword;
							string* pCP = &changePassword;

							cout << "\n USER INFO : " << " " << withSpace <<
								"    Cash: " << setprecision(2) << fixed << fAmount << "$" << "\n\n";

							cout << " 1) Cash In \n";
							cout << " 2) Cash Out \n";
							cout << " 3) Transfer Money \n";
							cout << " 4) Transfer Record \n";
							cout << " 5) Change Password \n";
							cout << " 6) Quit \n";
							cout << " >  ";
							cin >> fun;

							switch (stoi(fun))
							{
							case 1:
								send(ConnectSocket, "CI", fun.size() + 1, 0);
								cashIn(pFAmount);
								amount = to_string(fAmount);
								send(ConnectSocket, amount.c_str(), amount.size() + 1, 0);
								break;

							case 2:
								send(ConnectSocket, "CO", fun.size() + 1, 0);
								cashOut(pFAmount);
								amount = to_string(fAmount);
								send(ConnectSocket, amount.c_str(), amount.size() + 1, 0);
								break;

							case 3:
								send(ConnectSocket, "TM", fun.size() + 1, 0);
								cin.ignore();
								transferMoney(pFAmount, ConnectSocket);
								amount = to_string(fAmount);
								send(ConnectSocket, amount.c_str(), amount.size() + 1, 0);
								break;

							case 4:
								send(ConnectSocket, "R", fun.size() + 1, 0);
								showRecord(ConnectSocket);
								break;
							
							case 5:
								send(ConnectSocket, "CP", fun.size() + 1, 0);
								changePass(pCP, checkPassword);
								send(ConnectSocket, changePassword.c_str(), changePassword.size() + 1, 0);
								break;
							
							case 6:
								send(ConnectSocket, "Q", fun.size() + 1, 0);
								lf++;
								cin.ignore();
								break;

							default:
								cout << "\n Plz enter the number we mentioned!\n";
								cout << " > ";
								cin >> fun;
								continue;
							}
						}
					}
					else if(string(buff, 0, bytesReceived) == "0")
					{
						cout << " Invalid username. Plz.. try again." << endl;
					}
					else if (string(buff, 0, bytesReceived) == "2") 
					{
						cout << "\n Back To Main Menu... " << endl;
						controlTwo++;
					}
				}
				cin.clear();

			} while (controlTwo == 0);

		}
		else if (userInput == "3") 
		{
			iResult = send(ConnectSocket, userInput.c_str(), userInput.size() + 1, 0);
			control++;
		}
		else 
		{
			continue;
		}

	} while (control == 0);

	// Shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		cout << " Shutdown fail with error : " << WSAGetLastError() << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	else
	{
		cout << "\n Shut down connection to Server... " << endl;
	}

	// Cleanup
	closesocket(ConnectSocket);
	WSACleanup();

};

void addUnderscore(string* str)
{
	string x = *str;
	string newX;

	for (auto& ch : x) {

		if (ch == ' ')
		{
			ch = '_';
			string rs(1, ch);
			newX = newX.append(rs);
		}
		else
		{
			string rs(1, ch);
			newX = newX.append(rs);
		}
	}

	*str = newX;
};

void addSpace(string* str)
{
	string x = *str;
	string newX;

	for (auto& ch : x)
	{
		if (ch == ' ')
		{
			ch = '_';
			string rs(1, ch);
			newX = newX.append(rs);
		}
		else
		{
			string rs(1, ch);
			newX = newX.append(rs);
		}
	}

	*str = newX;
};

void cashIn(float* cash)
{
	cout << "\n\n CASH IN " << endl;

	float cashIn;
	float oriCash = *cash;
	cout << "\n Enter amount : ";
	cin >> cashIn;

	oriCash += cashIn;
	*cash = oriCash;
	cout << "\n Cash In complete... \n\n";
};

void cashOut(float* cash) 
{
	cout << "\n\n CASH OUT " << endl;

	int lco = 0;
	float cashOut;
	float oriCash = *cash;
	cout << "\n Enter amount : ";
	cin >> cashOut;
	
	do 
	{
		float allow = oriCash - cashOut;
		if (allow >= 10) 
		{
			oriCash -= cashOut;
			lco++;
		}
		else 
		{
			cout << "\n Cash in the Acc must have at least 10$.";
			cout << "\n Enter amount : ";
			cin >> cashOut;
		}

	} while (lco < 1);

	*cash = oriCash;
	cout << "\n Cash Out complete... \n\n";
};

void changePass(string* str, string oriPassword)
{
	cout << "\n\n CHANGE PASSWORD " << endl;

	int lcp = 0;
	string changePass, oldPassword, confirmPass;
	cout << "\n Enter OLD password : ";
	cin >> oldPassword;
	
	do 
	{
		if (oldPassword == oriPassword) 
		{
			int lcpOne = 0;

			cout << "\n Enter new password : ";
			cin >> changePass;
			cout << "\n Confirm new password : ";
			cin >> confirmPass;

			do 
			{
				if (confirmPass == changePass) 
				{
					lcpOne++;
				}
				else 
				{
					cout << "\n Passwords didn't match! Plz enter the same password.";
					cout << "\n Enter new password : ";
					cin >> changePass;
					cout << "\n Confirm new password : ";
					cin >> confirmPass;
				}

			} while (lcpOne < 1);

			lcp++;
		}
		else 
		{
			cout << "\n Invalid OLD password! ";
			cout << "\n Enter OLD password : ";
			cin >> oldPassword;
		}

	} while (lcp < 1);

	*str = confirmPass;
	cout << "\n Setting new password complete... \n\n";
};

void transferMoney(float* tm, SOCKET ts)
{
	cout << "\n\n TRANSFER MONEY " << endl;

	int ltm = 0;
	string accName;
	float tAmount, oriCash = *tm;

	cout << "\n Enter Acc name (or) 0 to quit : ";
	getline(cin, accName);

	string* str = &accName;
	addUnderscore(str);

	do 
	{
		if (accName == "0")
		{
			ltm++;
		}else
		{
			int tmByte = send(ts, accName.c_str(), accName.size() + 1, 0);

			// Wait for response
			ZeroMemory(buff, DEFAULT_BUFLEN);
			tmByte = recv(ts, buff, DEFAULT_BUFLEN, 0);

			if (string(buff, 0, tmByte) == "1")
			{
				// Wait for response
				ZeroMemory(buff, DEFAULT_BUFLEN);
				tmByte = recv(ts, buff, DEFAULT_BUFLEN, 0);
				string receivedAccAmount = string(buff, 0, tmByte);

				int cl = 0;
				cout << "\n Enter amount : ";
				cin >> tAmount;

				do
				{
					if ((oriCash - tAmount) >= 10)
					{
						float raAmount = stof(receivedAccAmount);
						raAmount += tAmount;
						oriCash -= tAmount;
						receivedAccAmount = to_string(raAmount);
						send(ts, receivedAccAmount.c_str(), receivedAccAmount.size() + 1, 0);

						// Send transfered amount
						string transMoney = to_string(tAmount);
						send(ts, transMoney.c_str(), transMoney.size() + 1, 0);

						cout << "\n Transfering complete... \n\n";
						cl++;
						ltm++;
					}
					else
					{
						cout << "\n Cash in the Acc must have at least 10$.";
						cout << "\n Enter amount : ";
						cin >> tAmount;
					}

				} while (cl < 1);
			}
			else
			{
				cout << "\n Invalid username! ";
				cout << "\n Enter Acc name (or) 0 to quit : ";
				cin >> accName;
			}
		}

	} while (ltm < 1);

	*tm = oriCash;
};

void showRecord(SOCKET ts)
{
	cout << "\n\n TRANSFER RECORD \n\n";

	int lr = 0;
	vector<string> vRecord;

	ZeroMemory(buff, DEFAULT_BUFLEN);
	int rByte = recv(ts, buff, DEFAULT_BUFLEN, 0);
	string record = string(buff, 0, rByte);

	cout << record << endl;

};
