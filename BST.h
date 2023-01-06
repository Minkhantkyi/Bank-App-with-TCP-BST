#pragma once
#include "Node.h"
#include <iostream>
#include <vector>

using namespace std;

class BST
{

private:
	// Vector for use in writing data to File
	vector<struct Info> vec;

	// To store registered user information
	string userInfoFile = "User_Info.txt";

public:
	BST();

	// Insert new registered user's data to BST
	void insert(string username, string password, string amount);

	// Update user's data
	void update(struct Info up);

	// Load users' data from File
	void loadData();

	// Create BST with loaded data from File
	void createBST(vector<struct Info> vc, int size);

	// Put BST's struct Info datt into vector to use in writing to File
	void putIntoVec(Node* root);

	// Write data to File
	void saveData();

	// Search user for checking register and login
	bool search(string sUsername);

	// Serach data that match with user's input username and return data
	struct Info searchForExtract(string sUsername);

	// Print Data. It is only used in testing code. 
	void print();

};