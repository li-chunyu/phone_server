#ifndef _H_PHONESERVER
#define _H_PHONESERVER

#include <cstddef>
#include <memory>

#include "phone_srv_type.h"

class SynwayAudioCard;
class Address;
class PhoneServer {
public:
  PhoneServer(std::unique_ptr<SynwayAudioCard>);
  void send(LPBYTE, size_t);
  void on_call();
  void close();
  void tear_down();
private:
  std::unique_ptr<SynwayAudioCard> up_audio_card;
};

#endif