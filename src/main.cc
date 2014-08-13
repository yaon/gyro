#include <iostream>
#include <thread>

void cb(void)
{
  std::cout << "IN CALLBACK" << std::endl;
}

int main()
{
  std::cout << "IN MAIN" << std::endl;

  std::thread t(cb);
  t.join();
  return 0;
}
