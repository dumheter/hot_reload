#include "core/console.hpp"
#include "server/server.hpp"
#include "client/client.hpp"

using namespace lightctrl;

constexpr u16 PORT = 1338;

void run_client() {
  Client client("127.0.0.1", PORT);

  while (true) {
    client.ask();
    client.listen();
  }
}

void run_server() {
  Server server(PORT);

  while (true) {
    server.run();
  }
}

int main(int, char**) {
  Console::println("Project Hot Reload");

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