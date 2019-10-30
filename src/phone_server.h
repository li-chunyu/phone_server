#ifndef _H_PHONESERVER
#define _H_PHONESERVER

#include <cstddef>
#include <memory>

#include "pistache/http.h"
#include "pistache/router.h"
#include "pistache/endpoint.h"

#include "phone_srv_type.h"

class websocket_endpoint;
class SynwayAudioCard;
class Address;
class PhoneServer {
public:
  PhoneServer(std::unique_ptr<SynwayAudioCard>, Pistache::Address, size_t);
  void send(LPBYTE, size_t);
  int call(int, std::string);
  void close();
  void tear_down();

  void start();
  void stop();
private:
  void setup_routes();
  void on_call(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter);

  std::unique_ptr<SynwayAudioCard> up_audio_card;

  std::shared_ptr<Pistache::Http::Endpoint> sp_http_endpoint;
  Pistache::Rest::Router router;
  std::shared_ptr<websocket_endpoint> sp_ws;
  int connection_id; // websocket connection_id, used by websocketpp

};

#endif