/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <vector>
#include <atomic>
using namespace std;

struct TransactioData 
{
  int id; 
  string dateStr;
  double price;
  int quantity;
  TransactioData(int ii=0 ):id(ii)
  {
      
  }
  
  bool operator<( const TransactioData& rhs)
  {
      return this->id <rhs.id;
  }
};


int main()
{
    cout<<"Hello World";
    
    std::vector<TransactioData> vecTransctions ;
    
    for (int i=0; i <5; ++i)
    {
        vecTransctions.push_back(TransactioData(i));
    }
    
    std::sort(vecTransctions.begin(), vecTransctions.end());
    auto it = find_if(vecTransctions.begin(), vecTransctions.end(), [&](const TransactioData& data){ return data.id ==2;});
    cout<<"searched id:"<<(*it).id7
    
    mutex m; 
    atomic<bool> bVal(true);
    cout<<"Hello World";
    
     int i =0; 
    
    auto evenPrinter = [&] () mutable{
        while(true)
        {
            
            if (bVal && i%2 ==0)
            {
                scoped_lock lock(m); 
                cout<<i<<endl;
                i++;
                bVal = false;
                
            }
        }
    };
        
        
        auto oddPrinter = [&] ()mutable{
            
            while(true)
            {
                if (!bVal && i%2 ==1)
                {
                    scoped_lock lock(m); 
                    cout<<i<<endl;
                    i++;
                    bVal = true;
                    
                }
            }

        };
        
        thread t1 (evenPrinter);
        thread t2(oddPrinter);
        
        t1.join();
        t2.join();

    return 0;

    return 0;
}
