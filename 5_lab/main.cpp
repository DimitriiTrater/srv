
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <print>
#include <thread>
#include <utility>

#include "mutex.hpp"

namespace
{
constexpr int kMaxAmountOfNuggets = 10'000;
constexpr int kDaysToWork = 5;
} // namespace

Mutex m;
int fat_men_eaten_count = 0;

enum class ProgramState : uint8_t
{
  Running,
  CookFired,
  CookQuit,
  CookNoSalary,
  Finished
};

std::string ToString(ProgramState state)
{
  switch (state)
  {
  case ProgramState::Running: return "Running";
  case ProgramState::CookFired: return "CookFired";
  case ProgramState::CookQuit: return "CookQuit";
  case ProgramState::CookNoSalary: return "CookNoSalary";
  case ProgramState::Finished: return "Finished";
  }
}

enum class Turn : uint8_t
{
  Cook,
  FatMen
};

ProgramState program_state = ProgramState::Running;

Turn current_turn = Turn::Cook;

class FatMan
{
public:
  FatMan() = default;
  FatMan(int const gluttony): gluttony_(gluttony) {} 

  void Eat(int & dish)
  {
    while (true)
    {
      m.lock();
      // std::println("Eat");
      if (program_state != ProgramState::Running || IsBlown())
      {
        m.unlock();
        break;
      }

      if (current_turn != Turn::FatMen || has_eaten_this_round_)
      {
        // std::println("{}", has_eaten_this_round_);
        m.unlock();
        std::this_thread::yield();
        continue;
      }

      if (dish >= gluttony_)
      {
        // std::println("Eat");
        amount_of_nuggets_ += gluttony_;
        dish -= gluttony_;
        has_eaten_this_round_ = true;
        fat_men_eaten_count += 1;
        
        if (IsBlown())
        {
          m.unlock();
          break;
        }
      }
      else
      {
        amount_of_nuggets_ += gluttony_ - dish;
        dish -= gluttony_;
        program_state = ProgramState::CookFired;
        has_eaten_this_round_ = true;
        fat_men_eaten_count += 1;
      }

      m.unlock();

      std::this_thread::yield();

      if (program_state != ProgramState::Running)
      {
        break;
      }
    }
  }

  bool IsBlown() const { return amount_of_nuggets_ > kMaxAmountOfNuggets; }
  int GetEaten() const { return amount_of_nuggets_; }
  void ResetRound() { has_eaten_this_round_ = false; }

private:
  int amount_of_nuggets_ = 0;
  int gluttony_ = 0;
  bool has_eaten_this_round_ = false;
};

class Cook
{
public:
  Cook() = default;
  Cook(int const efficiency_factor) : efficiency_factor_(efficiency_factor) {} 

  void Serve(int & dish1, int & dish2, int & dish3)
  {
    auto start = std::chrono::high_resolution_clock::now();
    while (program_state == ProgramState::Running)
    {
      m.lock();
      
      if (current_turn != Turn::Cook) {
        // std::println("Non cook turn");
        // std::println("{}", Print(program_state));
        m.unlock();
        std::this_thread::yield();
        continue;
      }

      auto current_time = std::chrono::high_resolution_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start).count();
      if (elapsed >= kDaysToWork)
      {
        program_state = ProgramState::CookQuit;
        m.unlock();
        break;
      }

      // std::println("Serve");
      dish1 += efficiency_factor_;
      dish2 += efficiency_factor_;
      dish3 += efficiency_factor_;

      current_turn = Turn::FatMen;
      m.unlock();

      std::this_thread::yield();
      if (program_state != ProgramState::Running) {
        break;
      }
    }
  }

private:
  int efficiency_factor_ = 0;
};


ProgramState CheckState(
  FatMan & fat_man1,
  FatMan & fat_man2,
  FatMan & fat_man3,
  int dish1,
  int dish2,
  int dish3
)
{
  m.lock();
  // std::println("CheckState, {}", Print(program_state));
  if (program_state != ProgramState::Running) {
    m.unlock();
    return program_state;
  }

  // std::println("{}, {}, {}", fat_man1.IsBlown() , fat_man2.IsBlown() , fat_man3.IsBlown());
  // std::println("{}, {}, {}", dish1 , dish2 , dish3);
  // std::println("{}, {}", std::to_underlying(current_turn) , fat_men_eaten_count);
  // std::println("{}, {}, {}", fat_man1.GetEaten(), fat_man2.GetEaten(), fat_man3.GetEaten());
  if (fat_man1.IsBlown() && fat_man2.IsBlown() && fat_man3.IsBlown())
  {
    // std::println("IsBlown");
    program_state = ProgramState::CookNoSalary;
    current_turn = Turn::Cook;
  }
  else if (dish1 < 0 || dish2 < 0 || dish3 < 0)
  {
    // std::println("IsFired");
    program_state = ProgramState::CookFired;
    current_turn = Turn::Cook;
  }
  else if (current_turn == Turn::FatMen && fat_men_eaten_count >= 3)
  {
    // std::println("Update Round");
    current_turn = Turn::Cook;
    fat_men_eaten_count = 0;
    fat_man1.ResetRound();
    fat_man2.ResetRound();
    fat_man3.ResetRound();
  }
  
  ProgramState current_state = program_state;
  // std::println("Current State {}", Print(current_state));
  m.unlock();
  return current_state;
}


void RunScenario(std::string const & scenario_name, int gluttony1, int gluttony2, int gluttony3, int efficiency_factor)
{
  std::println("\n=== {} ===", scenario_name);
  std::println("gluttony1: {}, gluttony2: {}, gluttony3: {} efficiency_factor {}", gluttony1, gluttony2, gluttony3, efficiency_factor);
  int dish1 = 3000;
  int dish2 = 3000;
  int dish3 = 3000;

  program_state = ProgramState::Running; 
  current_turn = Turn::Cook;

  FatMan fat_man1(gluttony1);
  FatMan fat_man2(gluttony2);
  FatMan fat_man3(gluttony3);

  Cook cook(efficiency_factor);

  std::thread fat_man1_t(&FatMan::Eat, &fat_man1, std::ref(dish1));
  std::thread fat_man2_t(&FatMan::Eat, &fat_man2, std::ref(dish2));
  std::thread fat_man3_t(&FatMan::Eat, &fat_man3, std::ref(dish3));
  std::thread cook_t(&Cook::Serve, &cook, std::ref(dish1), std::ref(dish2), std::ref(dish3));


  ProgramState current_state = ProgramState::Running;
  while (current_state == ProgramState::Running)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    current_state = CheckState(fat_man1, fat_man2, fat_man3, dish1, dish2, dish3);
  }

  fat_man1_t.join();
  fat_man2_t.join();
  fat_man3_t.join();
  cook_t.join();

  std::println("dish1: {}, dish2: {}, dish3: {}", dish1, dish2, dish3);
  std::println("Eaten:");
  std::println("fat_man1: {}, fat_man2: {}, fat_man3: {}", fat_man1.GetEaten(), fat_man2.GetEaten(), fat_man3.GetEaten());

  std::println("{}", ToString(current_state));
}

int main()
{
  RunScenario("CookQuit", 50, 50, 50, 100);
  RunScenario("CookNoSalary", 1000, 1000, 1000, 1000);
  RunScenario("CookIsFired", 50, 50, 5000, 1);
  
  return 0;
}
