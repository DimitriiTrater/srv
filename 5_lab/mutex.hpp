#include <semaphore>
class Mutex
{
public:
  Mutex() = default;
  Mutex(Mutex const &) = delete;
  Mutex & operator=(Mutex const &) = delete;

  void lock()
  {
    semaphore_.acquire();
  }

  void unlock()
  {
    semaphore_.release();
  }

  
private:
  std::binary_semaphore semaphore_ {1};
};
