/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
using namespace std;

struc TransactioData 
{
  int id; 
  string dateStr;
  double price;
  int quantity;
};

class Parser
{
    public:
    ~virtual Parser() =0;
    virtual TransactioData Parse(const string& );
    virtual void visit(DataWrapper& dw)
    {
        dw.Parser()
    }
};
 Parser::~Parser()
 {
     
 }

class DataWrapper
{
   shared_ptr<Parser> _parser; 
   std::vector<TransactioData> vecTransactions ;
   mutex m; 
   conition cond;
   
   
   
//   void setParser(shared_ptr<Parser> parser)
//   {
//       _parser = parser;
//   }
   
   void parse(const string& str)
   {
       vecTransactions.push_back(_parser.parse(str));
   }
   virtual accept(shared_ptr<Parser>& v)
   {
       v.visit(*this);
   }
   
   
   
}; 

class XMLDateWrapper : public DataWrapper

{
    
};
class XMLParser : public Parser
{
    
    public: 
    
    TransactioData Parse(const string& )
    {
        //parsing logic here 
        return TransactioData();
    }
    
    
}; 

class CSVParser : public Parser
{
    
    public: 
    
    TransactioData Parse(const string& )
    {
        //parsing logic here 
        return TransactioData();
    }
    
}; 


int main()
{
    
    DataWrapper  dataWrapper;
    
    dataWrapper.setParser(std::move(make_shared<Parser>(XMLParser)); 
    // String str , from xmlFile 
    dataWrapper.parse(str);
    
    mutex m; 
    atomic<bool> bVal;
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
}
