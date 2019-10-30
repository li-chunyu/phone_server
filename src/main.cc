#include "synway.h"
#include "phone_server.h"
#include <memory>
#include <iostream>

#include <signal.h>

using namespace std;

int main() {
  sigset_t signals;
  if (sigemptyset(&signals) != 0
          || sigaddset(&signals, SIGTERM) != 0
          || sigaddset(&signals, SIGINT) != 0
          || sigaddset(&signals, SIGHUP) != 0
          || pthread_sigmask(SIG_BLOCK, &signals, nullptr) != 0) {
    perror("install signal handler failed");
    return 1;
  }

  unique_ptr<SynwayAudioCard> up_card{new SynwayAudioCard()};
  if (up_card->init_card() != 0) {
    cout << "init_card false." << endl;
    return 0;
  }
  PhoneServer* p_phone_srv = new PhoneServer(std::move(up_card));
  p_phone_srv->on_call();

  int signal = 0;
  int status = sigwait(&signals, &signal);
  if (status == 0) {
    std::cout << "received signal " << signal << std::endl;
  } else {
    std::cout << "sigwait returns " << status << std::endl;
  }

  p_phone_srv->tear_down();
  delete p_phone_srv;

  cout << "finished." << endl;
  return 0;
}