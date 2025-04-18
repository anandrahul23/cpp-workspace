// C++ program to print top
// view of binary tree

#include <iostream> 
#include <map>
#include <vector>
#include <queue>
using namespace std;

// Structure of binary tree
struct Node {
	Node* left;
	Node* right;
	int hd;
	int data;
};

// function to create a new node
Node* newNode(int key)
{
	Node* node = new Node();
	node->left = node->right = NULL;
	node->data = key;
	return node;
}
void fillMap(Node* A, map<int, vector<int> >& verticalMap, int vIndex)
 {
     //Base Condition 
     if (nullptr == A)
     {
         return; 
     }
      fillMap(A->left, verticalMap, vIndex-1);
    fillMap(A->right, verticalMap, vIndex+1);
     auto it = verticalMap.find(vIndex);
    if (it == verticalMap.end())
    {
  verticalMap.emplace(vIndex, vector<int>{A->data});

    }
    else
    {
        verticalMap[vIndex].push_back(A->data);
    }


 }


vector<int> solve(Node* A) 
{

    map<int, vector<int> > mapVerticalIndexVsNodes; 

    int vIndex = 0; 

    if (A == nullptr)
    {
        return vector<int> {};
    }

    fillMap(A, mapVerticalIndexVsNodes, vIndex);
    vector<int> sol; 


    for (const auto& kv : mapVerticalIndexVsNodes)
   {
       if( !kv.second.empty())
       {
           sol.push_back(kv.second[kv.second.size()-1]);
       }
   }

   for (const auto& kv : mapVerticalIndexVsNodes)
   {
       cout<<"key:"<<kv.first<<"->";
      for (const auto & val : kv.second)
      {
          cout<<val<<",";
      }
      cout<<"#";
   }
    cout<<"now printing the vector:****";

for (auto& val: sol)
{
    cout<<val<<",";
}
cout<<endl;

   return sol;

    

}

// function should print the topView of
// the binary tree
void topview(Node* root)
{
	if (root == NULL)
		return;
	queue<Node*> q;
	map<int, int> m;
	int hd = 0;
	root->hd = hd;

	// push node and horizontal distance to queue
	q.push(root);

	cout << "The top view of the tree is : \n";

	while (q.size()) {
		hd = root->hd;

		// count function returns 1 if the container
		// contains an element whose key is equivalent
		// to hd, or returns zero otherwise.
		if (m.count(hd) == 0)
			m[hd] = root->data;
		if (root->left) {
			root->left->hd = hd - 1;
			q.push(root->left);
		}
		if (root->right) {
			root->right->hd = hd + 1;
			q.push(root->right);
		}
		q.pop();
		root = q.front();
	}

	for (auto i = m.begin(); i != m.end(); i++) {
		cout << i->second << " ";
	}
  //solve(root);
}

// Driver code
int main()
{
	/* Create following Binary Tree
		1
		/ \
	2 3
		\
		4
		\
			5
			\
			6
	*/
	Node* root = newNode(1);
	root->left = newNode(2);
	root->right = newNode(3);
  root->left->left = newNode(4);
  //
  root->left->right = newNode(5);
  //
  root->right->left = newNode(6);
  //
  root->right->right = newNode(7);
  //
  root->left->left->left = newNode(8);
  //
  root->left->left->right = newNode(9);
  //
  root->left->right->left = newNode(10);
  //
  root->left->right->right = newNode(11);
  //
  root->right->left->left = newNode(12);
  //
  root->right->left->right = newNode(13);
    
    //
     root->right->right->left = newNode(13);
  //
   root->right->right->right = newNode(15);
  
  root->left->left->left->left = newNode(16);
    root->left->left->left->right = newNode(17);
  
   root->left->left->right->left = newNode(18);
  root->left->left->right->right = newNode(19);
  root->left->right->left->left = newNode(20);
   root->left->right->left->right = newNode(21);
  
  root->left->right->right->left = newNode(22);
   root->left->right->right->right = newNode(23);
  
   root->right->left->left->left = newNode(24);
   root->right->left->left->right = newNode(25);
  
   root->right->left->right->left = newNode(26);
   root->right->left->right->right = newNode(27);
  
   root->right->right->left->left = newNode(28);
   root->right->right->left->right = newNode(29);

  root->right->right->right->left = newNode(30); 
  
  root->right->right->right->right = newNode(31);
  
  
  
  
  
 
	
  
	topview(root);
  solve(root);
	return 0;
}
/* This code is contributed by Niteesh Kumar */
