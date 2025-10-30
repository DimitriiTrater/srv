
#include <chrono>
#include <cstddef>
#include <print>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>

void Func(std::string name)
{
   long double i = 0;
   long double increment = 1e-9L;
   auto start = std::chrono::high_resolution_clock::now();
   auto end = start + std::chrono::seconds(1);
   while (std::chrono::high_resolution_clock::now() < end)
   {
      i += increment;
   }

   std::cout << name << ":" <<i << std::endl;
}

int main()
{
    std::thread thread1(Func, "t1");
    std::thread thread2(Func, "t2");
    std::thread thread3(Func, "t3");
    thread1.join();
    thread2.join();
    thread3.join();
}
