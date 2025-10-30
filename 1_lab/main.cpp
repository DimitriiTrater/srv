#include <chrono>
#include <cstddef>
#include <print>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>

void first()
{
   std::string s = "01234";

   for(unsigned int i = s.size() - 1; i >= 0; i-- )
   {
      std::cout<<s[i]<<std::endl;
   }
   
}


size_t fact(size_t n)
{
   size_t factorial = 1;

   for (size_t i = 2; i <= n; ++i)
   {
      factorial *= i;
   }
   return factorial;
}

void second()
{
   auto start = std::chrono::high_resolution_clock::now();

   fact(10'000'000);
   
   auto end = std::chrono::high_resolution_clock::now();

   std::println("Duration: {0}", (end - start).count());
}

int main()
{
   auto start = std::chrono::high_resolution_clock::now();
   fact(10'000'000);
   fact(10'000'000);
   auto end = std::chrono::high_resolution_clock::now();
   std::println("Duration: {0}", (end - start).count());
}
