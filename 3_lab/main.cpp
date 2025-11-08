#include <functional>
#include <print>
#include <string>
#include <thread>
#include <mutex>
#include <string>


int coins = 101;
std::mutex m;
std::string last_taken{};

void coin_sharing(std::string name, int & thief_coins, int const & comp_coins) 
{
   while (true)
   {
      m.lock();
      bool const coinsRanOut = coins == 0;
      if (coinsRanOut)
      {
         m.unlock();
         return;
      }
      bool const isOnlyOneCoin = (thief_coins == comp_coins) && (coins == 1);
      if (isOnlyOneCoin)
      {
         m.unlock();
         return;
      }
      if (last_taken == name)
      {
         m.unlock();
         continue;
      }
      if (thief_coins <= comp_coins)
      {
         std::println("thief: {}", name);
         coins -= 1;
         thief_coins += 1;
         last_taken = name;
      }
      m.unlock();
   }
}

int main()
{
   int bob_coins = 0;
   int tom_coins = 0;
   std::thread bob_takes(coin_sharing, std::string("bob"), std::ref(bob_coins), std::cref(tom_coins));
   std::thread tom_takes(coin_sharing, std::string("tom"), std::ref(tom_coins), std::cref(bob_coins));

   bob_takes.join();
   tom_takes.join();
   std::println("{} {} {}", bob_coins, tom_coins, coins);
}
