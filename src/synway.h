#ifndef _H_SYNWAY
#define _H_SYNWAY

#include <string>
#include <thread>
#include <functional>

#include "phone_srv_type.h"

class PhoneServer;
class SynwayAudioCard {
public:
  SynwayAudioCard();

  int init_card();
  void set_send_callback(std::function<void(PhoneServer*, LPBYTE, size_t)>, PhoneServer*);
  void set_close_callback(std::function<void(PhoneServer*)>, PhoneServer*);
  int call(int ch, std::string phone_num);
  void hangup(int ch);
  int channel_states(int ch);
  int buffer_size();
  ~SynwayAudioCard();

private:
  void connection_handle(int);
  void ch_state_change_handler(int, int);
  void ch_talking_state_handler(int);
  static int recordMemBlockHandler(int, int, LPBYTE, DWORD dwStopOffset, void*);

  std::shared_ptr<std::thread> sp_connection_handle_thread;
  int ch;
  LPBYTE buf1; // buf1 for pingpong
  LPBYTE buf2; // buf2 for pingpong
  size_t buf_size; // buf_size of buf1 and buf2

  bool event_loop_flag;

  std::function<void(LPBYTE, size_t)> send_callback; // call back got from user.
  std::function<void()> close_callback; // call back for close websocket connection.
};
#endif