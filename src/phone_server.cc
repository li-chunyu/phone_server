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
    con_state = true;
  } else {
    con_state = false;
  }
}

void PhoneServer::close() {
  con_state = false;
  std::cout << "phone server close." << std::endl;
}