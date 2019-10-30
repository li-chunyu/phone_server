#include "phone_server.h"
#include "phone_srv_type.h"
#include "synway.h"
#include "websocket_endpoint.h"

#include "pistache/http.h"
#include "pistache/router.h"
#include "pistache/endpoint.h"

#include <iostream>
#include <memory>

PhoneServer::PhoneServer(std::unique_ptr<SynwayAudioCard> up_card,
  Pistache::Address addr, size_t thr)
    :sp_http_endpoint(std::make_shared<Pistache::Http::Endpoint>(addr)) {
  up_audio_card = std::move(up_card);
  up_audio_card->set_send_callback(&PhoneServer::send, this);
  up_audio_card->set_close_callback(&PhoneServer::close, this);

  auto opts = Pistache::Http::Endpoint::options().threads(thr)  ;
  sp_http_endpoint->init(opts);
  setup_routes();
}

void PhoneServer::setup_routes() {
  using namespace Pistache::Rest;
  Routes::Get(router, "/call/:ch/:phone_num",
    Routes::bind(&PhoneServer::on_call, this));
    // TODO.
  //Routes::Get(router, "/hangup/:ch",
  //  Routes::bind(&PhoneServer::onHangup, this));
}

void PhoneServer::send(LPBYTE p_bytes, size_t len) {
  // TODO.
  // std::cout << "PhoneServer::send called" << std::endl;
  sp_ws->send(connection_id, reinterpret_cast<void*>(p_bytes), len);
}

int PhoneServer::call(int ch, std::string phone_num) {
  if (up_audio_card->call(6, "018043365315") != 0)
    return -1;
  return 0;
}

void PhoneServer::on_call(const Pistache::Rest::Request& request,
  Pistache::Http::ResponseWriter response) {
  auto phone_num = request.param(":phone_num").as<std::string>();
  auto ch = request.param(":ch").as<int>();
  std::string resp = "";
  resp = "oncall: phone number: " + phone_num +
      " channel:" + std::to_string(ch);
  int ret = call(ch, phone_num);

  sp_ws.reset(new websocket_endpoint());
  connection_id = sp_ws->connect("ws://127.0.0.1:8888/ws");

  if (ret == 0)
    response.send(Pistache::Http::Code::Ok, resp);
  else
    response.send(Pistache::Http::Code::Internal_Server_Error, "error");
}

void PhoneServer::close() {
  sp_ws->close(connection_id, 1000);
  connection_id = -1;
}

void PhoneServer::tear_down() {
  if (up_audio_card->tear_down() != 0) {
    std::cout << "audio card tear down failed";
  }
  // release audio card;
  up_audio_card.reset();
}

void PhoneServer::start() {
  sp_http_endpoint->setHandler(router.handler());
  //httpEndpoint->serve();
  sp_http_endpoint->serveThreaded();
}

void PhoneServer::stop() {
  if (connection_id != -1) {
    close();
  }
  sp_http_endpoint->shutdown();
}