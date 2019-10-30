#include "phone_server.h"
#include "phone_srv_type.h"
#include "synway.h"

#include <iostream>
#include <memory>

PhoneServer::PhoneServer(std::unique_ptr<SynwayAudioCard> up_card) {
  up_audio_card = std::move(up_card);
  up_audio_card->set_send_callback(&PhoneServer::send, this);
  up_audio_card->set_close_callback(&PhoneServer::close, this);
}

void PhoneServer::send(LPBYTE p_bytes, size_t len) {
  std::cout << "PhoneServer::send called" << std::endl;
}

void PhoneServer::on_call() {
  if (up_audio_card->call(6, "018043365315") == 0) {
    std::cout << "PhoneServer::on_call:  succ" << std::endl;
  } else {
    std::cout << "PhoneServer::on_call:  fail" << std::endl;
  }
}

void PhoneServer::close() {
  std::cout << "phone server close." << std::endl;
}

void PhoneServer::tear_down() {
  if (up_audio_card->tear_down() != 0) {
    std::cout << "audio card tear down failed";
  }
  // release audio card;
  up_audio_card.reset();
}