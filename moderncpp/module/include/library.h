#pragma once
// #include <queue>
// #include <string>
// #include <unordered_map>
// #include <unordered_set>
// #include <map>
// #include <set>
// #include <utility>
// #include <vector>
// #include <memory>
// #include <algorithm>

// // Book:
// // - author
// // - genre
// // - title
// // - review_score

// // Library:
// // - add new book
// // - update book's review score
// // - get top 10 books by review score
// using namespace std; 
// struct Book{
//     string author; 
//     string genre;  
//     string title; 
//     size_t review_score; 
    
// }; 


// struct Library
// {
//     void addBook(const Book& aBook){
//         if( auto it = titleVsBooks.find(aBook.title); it!= titleVsBooks.end())
//         {
//             return; 
//         }
//         books.push_back(aBook); 
//         titleVsBooks.insert(make_pair(aBook.title, &aBook));
//         reviewVsBooks[aBook.review_score].push_back(&aBook); 
//     }
    
//     bool updateReview(const string& title, size_t score)
//     {
//         if( auto it = titleVsBooks.find(title); it!= titleVsBooks.end())
//         {
//             size_t oldReviewScore = it->second->review_score; 
//             auto oldBooks = reviewVsBooks[oldReviewScore]; 
//             oldBooks.erase(remove_if(oldBooks.begin(), oldBooks.end(), [&title](auto bookPtr){
//                 return title == bookPtr->title; 
//             }), oldBooks.end()); 
            
//             const_cast<Book*>(it->second)->review_score = score;  
//             reviewVsBooks[score].push_back(it->second); 
//             return  true; 
//         }
        
//         return false;
//     }
//     vector<string> getTop10Books() const 
//     {
//         int count = 0; 
//         vector<string> titles; 
//         for(auto it = reviewVsBooks.crbegin() ; it!= reviewVsBooks.crend(); ++it)
//         {
//             auto books = it->second; 
//             for(auto bookPtr: books)
//             {
//                 ++count; 
//                 titles.push_back(bookPtr->title); 
//                 if(count == 10)
//                 {
//                     break; 
//                 }                
//             }
//             if(count == 10)
//             {
//                 break; 
//             }
//         }
//         if(count!= 10)
//         {
//             cout<<"library has less than 10 books"<<endl;
//         }
//         return titles;
//     }
//  deque<Book> books;
//  map<size_t, vector<const Book*>> reviewVsBooks;  
//  unordered_map<string, const Book*> titleVsBooks;  
// }; 