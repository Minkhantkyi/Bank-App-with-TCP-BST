#include <iostream>
#include "BST.h"
#include "Node.h"
#include <vector>
#include <iomanip>
#include <fstream>

using namespace std;

Node* root;

BST::BST() {
	root = NULL;
}

void BST::insert(string username, string password, string amount)
{
	struct Info in;
	in._username = username;
	in._password = password;
	in._amount = amount;
	root = root->insert(root, in);
}

void BST::update(struct Info up) 
{
	root = root->update(root, up);
}

void BST::loadData()
{
	vec.clear();
	struct Info l;
	ifstream load(userInfoFile);

	if (load.is_open())
	{
		while (load >> l._username >> l._password >> l._amount)
		{
			vec.push_back(l);
		}
	}
	else
	{
		cout << " Unable to open! \n";
	}

	int size = vec.size();
	createBST(vec, size);

	load.close();
}

void BST::createBST(vector<struct Info> vc, int size)
{
	struct Info* arr = new struct Info[size];
	for (int i = 0; i < size; i++)
	{
		arr[i] = vec[i];
	}

	root = root->constructBST(arr, 0, size - 1);
}

void BST::putIntoVec(Node* root)
{
	if (root == NULL) return;
	putIntoVec(root->left);
	struct Info v;
	v._username = root->info._username;
	v._password = root->info._password;
	v._amount = root->info._amount;
	vec.push_back(v);
	putIntoVec(root->right);
}

void BST::saveData()
{
	vec.clear();
	putIntoVec(root);
	ofstream save(userInfoFile, ios::out);

	if (save.is_open())
	{
		for (int i = 0; i < vec.size(); i++)
		{
			float amount = stof(vec[i]._amount);
			save << vec[i]._username << " " << vec[i]._password << " "
				<< setprecision(2) << fixed << amount << endl;
		}

	}
	else
	{
		cout << " Unable to open! \n";
	}

	save.close();
}

bool BST::search(string sUsername)
{
	return root->search(root, sUsername);
}

struct Info BST::searchForExtract(string sUsername)
{
	return root->searchAndExtract(root, sUsername);
}

void BST::print()
{
	root->inorder(root);
	cout << endl;
}