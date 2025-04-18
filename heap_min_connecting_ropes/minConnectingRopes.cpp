#include <iostream>
#include <vector>

using namespace std;



//heapify at 
void heapify(vector<int>& arr, int index , int n)
{
    int lchild = 2 * index +1 ; 
    int rchild = 2 * index + 2 ; 

    int smallest = index;
    if (lchild >= n)
    {
        return;
    }
    else if(arr[lchild] < arr[smallest])
    {
        smallest = lchild;
    }
    
    if(rchild < n && arr[rchild] < arr[smallest])
    {
        smallest = rchild;
    }

    if (smallest == index)
    {
        return;
    }

    std::swap(arr[index], arr[smallest]);

    heapify(arr, smallest, n);

}

void createMinHeap(vector<int>& arr)
{
    for (int i = (arr.size())/2 ; i>=0; i--)
    {
        heapify(arr, i, arr.size());
    }
}


void printArray(const vector<int>& arr)
{
    cout<<"#####";
    for (const auto& i: arr)
    {
        cout<<i<<"-";
    }
}


// n is the index of the last element +1
void deleteMin(vector<int>& A)
{
    int n = A.size();
    if (n <= 1 )
    {
        return; 
    }
    std::swap(A[0], A[n - 1]);
    A.pop_back();
    heapify(A, 0, A.size());
}

int parent(int i)
{
    return (i-1) /2 ; 

}

void insert(vector<int>& A, int x)
{
    A.push_back(x);
    for (int i = A.size()-1 ; i && A[i] < A[parent(i)]; i = parent(i))
    {
        std::swap(A[i], A[parent(i)]);
    }
}



int solve(vector<int> &A) {

  cout<<"Print array before heapify*****";

  vector<int> B(A.begin(), A.end());

    printArray(B);

    if (0 == B.size())
    {
        return INT_MIN;
    }

    if (1 == B.size())
    {
        return B[0];
    }

    if (2 == B.size())
    {
        return B[0] + B[1];
    }

    createMinHeap(B);

     cout<<"Print array after heapify%%%%%";

   printArray(B);

    int cost  = 0; 

   //std::cout<<endl;


    
    while(B.size() != 1)
    {
        int firstMin = B[0];
        deleteMin(B); 
        cout<<"Print array after first delete*****";

       printArray(B);
        
        int secondMin = B[0]; 
        deleteMin(B);
         cout<<"Print array after second delete*****";

        printArray(B);
        cost += firstMin + secondMin;
        insert(B, cost);

       cout<<"Print array after insert######";

       printArray(B);

        //printArray(A);
    }

    //cout<<endl;
    return cost;
}


int main()
{  std::vector<int> array{1,2,3,4,5}; 
    std::cout<<"Ans is:"<<solve(array)<<endl;
    return 0; 
}
