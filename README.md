# Phone Service

A toy phone service. Only support one connection(for both telephone connection and websocket connection).

## Source file

- synway.h, support Synway audio card.
- phone_server.h, support RESTful API.
- websocket_endpoint.h, a wrapper of websocketpp.

## Dependencies

1. Pistache
2. Websocketpp
3. Boost (for Pistache)

## How to use

check src/main.cc, src/call.sh

## TODO

- interface "hangup".