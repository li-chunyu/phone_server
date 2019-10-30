#include "synway.h"
#include "phone_server.h"
#include <memory>
#include <iostream>
using namespace std;

int main() {
  unique_ptr<SynwayAudioCard> up_card{new SynwayAudioCard()};
  if (up_card->init_card() != 0) {
    cout << "init_card false." << endl;
    return 0;
  }
  PhoneServer phone_srv(std::move(up_card));
  phone_srv.on_call();
  while(phone_srv.con_state);
  cout << "finished." << endl;
  return 0;
}