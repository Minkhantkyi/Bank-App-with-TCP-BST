#include "Node.h"
#include <iostream>

using namespace std;

Node::Node() 
{
	Node* left = NULL;
	Node* right = NULL;
}

Node::Node(struct Info a)
{
	info._username = a._username;
	info._password = a._password;
	info._amount = a._amount;
}

Node* Node::insert(Node* root, struct Info info)
{
	if (root == NULL)
	{
		root = new Node;
		root->info._username = info._username;
		root->info._password = info._password;
		root->info._amount = info._amount;
		root->left = root->right = NULL;

	}
	else if (info._username <= root->info._username)
	{
		root->left = insert(root->left, info);
	}
	else
	{
		root->right = insert(root->right, info);
	}

	return root;
}

Node* Node::update(Node* root, struct Info up) 
{
	if (up._username == root->info._username) 
	{
		root->info._password = up._password;
		root->info._amount = up._amount;
	}
	else if (up._username < root->info._username) 
	{
		root->left = update(root->left, up);
	}
	else 
	{
		root->right = update(root->right, up);
	}

	return root;
}

Node* Node::constructBST(struct Info arr[], int left, int right) {

	if (left > right)return NULL;

	int mid = left + (right - left) / 2;
	Node* node = new Node(arr[mid]);
	node->left = constructBST(arr, left, mid - 1);
	node->right = constructBST(arr, mid + 1, right);

	return node;

}

bool Node::search(Node* root, string sUsername)
{
	if (root == NULL) return false;

	if (root->info._username == sUsername) return true;
	else if (sUsername < root->info._username)
	{
		return search(root->left, sUsername);
	}
	else
	{
		return search(root->right, sUsername);
	}
}

struct Info Node::searchAndExtract(Node* root, string sUsername)
{
	struct Info data;
	data._username = "NULL";
	data._password = "NULL";
	data._amount = "NULL";

	if (root == NULL) return data;

	if (root->info._username == sUsername)
	{
		data._username = root->info._username;
		data._password = root->info._password;
		data._amount = root->info._amount;
		return data;
	}
	else if (sUsername < root->info._username)
	{
		return searchAndExtract(root->left, sUsername);
	}
	else
	{
		return searchAndExtract(root->right, sUsername);
	}
}

void Node::inorder(Node* root)
{
	if (root == NULL) return;
	inorder(root->left);
	cout << " " << root->info._username << " " << root->info._password << " " << root->info._amount << endl;
	inorder(root->right);
}