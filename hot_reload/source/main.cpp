#include "core/console.hpp"
#include "server/server.hpp"
#include "client/client.hpp"
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace lightctrl;

constexpr u16 PORT = 1337;

void run_client() {
  Client client("127.0.0.1", PORT);
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  while (true) {
    client.ask();
    client.listen();
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(std::rand()%3000));
  }
}

void run_server() {
  Server server(PORT);

  while (true) {
    server.run();
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1));
  }
}

int main(int, char**) {
  Console::set_write_to_file(false);
  Console::println("Project Hot Reload");
  Console::println("(s)erver or (c)lient.");
  const std::string answer = Console::readln();

  Tcp_socket::win_init();

  if (answer == "s") {
    run_server();
  }

  else if (answer == "c") {
    run_client();
  }
  
  Tcp_socket::win_shutdown();

  return 0;
}