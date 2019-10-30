#include "synway.h"
#include "phone_srv_type.h"

#include <shpa3api.h> // synway driver.

#include <thread>
#include <memory>
#include <functional>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

class PhoneServer;

std::string audio_card_error() {
    char msg[200];
    SsmGetLastErrMsg(msg);
    return std::string(msg);
}

SynwayAudioCard::SynwayAudioCard() {
  buf_size = 1600;
  buf1 = (LPBYTE)malloc(buf_size);
  buf2 = (LPBYTE)malloc(buf_size);
}

SynwayAudioCard::~SynwayAudioCard() {
  free(buf1);
  free(buf2);
  event_loop_flag = false;
  sp_connection_handle_thread->join();
  SsmCloseCti();
}

int SynwayAudioCard::init_card() {
  if (SsmStartCti("conf/ShConfig.ini", "conf/ShIndex.ini") != 0) {
    std::cout << audio_card_error() << std::endl;
    return -1;
  }
  return 0;
}

int SynwayAudioCard::buffer_size(){
  return buf_size;
}

void SynwayAudioCard::set_send_callback(std::function<void(PhoneServer*, LPBYTE, size_t)> func, PhoneServer* p) {
  send_callback = std::bind(func, p, std::placeholders::_1, std::placeholders::_2);
}

void SynwayAudioCard::set_close_callback(std::function<void(PhoneServer*)> func, PhoneServer* p) {
  close_callback = std::bind(func, p);
}


int SynwayAudioCard::call(int ch, std::string phone_num) {
  //send_callback(NULL, 0);
  if (channel_states(ch) != 0)
    return -1;
  
  SsmPickup(ch);
  if (SsmStartPickupAnalyze(ch) == -1) {
    SsmHangup(ch);
    std::cout << "SsmStartPickupAnalyze faild" << std::endl;
    return -1;
  }

  if (SsmAutoDial(ch, phone_num.c_str()) == -1) {
    std::cout << audio_card_error() << std::endl;
    SsmHangup(ch);
    return -1;
  }

  // connection handle;
  sp_connection_handle_thread.reset(
    new std::thread(&SynwayAudioCard::connection_handle,
    this,
    ch)
  );
  return 0;
}

void SynwayAudioCard::hangup(int ch) {
  return;
}

int SynwayAudioCard::channel_states(int ch) {
  return SsmGetChState(ch);
}

void SynwayAudioCard::connection_handle(int ch) {
    // establish websocket connection.
    MESSAGE_INFO event;
    event_loop_flag = true;
    printf("on connection_handle\n");
    while (event_loop_flag) {
        std::memset(&event, 0, sizeof(PMESSAGE_INFO));
        if (SsmWaitForEvent(50, &event) == 0) {
            switch(event.wEventCode) {
                case E_CHG_ChState:
                    ch_state_change_handler(ch ,SsmGetChState(ch));
                    break;
            }
        }
    }
    printf("exit connection_handle\n");
}

void SynwayAudioCard::ch_state_change_handler(int ch, int ch_state) {
    switch (ch_state) {
        case S_CALL_STANDBY:
        case S_CALL_PICKUPED:
        case S_CALL_ANALOG_WAITDIALTONE:
        case S_CALL_ANALOG_WAITDIALRESULT:
        case S_CALL_WAIT_REMOTE_PICKUP:
        case S_CALL_ANALOG_TXPHONUM: // 去话呼叫，拨号
            break;
        case S_CALL_TALKING:
            printf("ch: %d, talking\n", ch);
            ch_talking_state_handler(ch);
            break;
        case S_CALL_PENDING: // 通道挂起状态
            //ch_pending_handler(ch);
            // TODO:加一个 close ws callback
            printf("ch: %d, pending\n", ch);
            SsmStopRecordMemBlock(ch);
            SsmHangup(ch);
            close_callback();
            event_loop_flag = false;
            break;
        default:
            printf("Unkown state number: %d\n", ch_state);
            event_loop_flag = false;
            break;
    }
}

void SynwayAudioCard::ch_talking_state_handler(int ch) {
  if (SsmRecordMemBlock(ch, -2, buf1, buf_size, SynwayAudioCard::recordMemBlockHandler, this) != 0) {
    std::cout << audio_card_error() << std::endl;
  }
  if (SsmRecordMemBlock(ch, -2, buf2, buf_size, SynwayAudioCard::recordMemBlockHandler, this) != 0) {
    std::cout << audio_card_error() << std::endl;
  }
}

int SynwayAudioCard::recordMemBlockHandler(int ch, int nEndReason, LPBYTE pucBuf, DWORD dwStopOffset, void *pV) {
    int resualt = 0;
    SynwayAudioCard *p_card = (SynwayAudioCard*)pV;
    if (nEndReason == 1 || nEndReason == 3) {
        return 0;
    }
    if (pucBuf == p_card->buf1) {
        // pobj->spws->send(pucBuf, 1600);
        //p_card->spws->send(pobj->get_connection_id(), reinterpret_cast<void *>(pucBuf), 1600);
        p_card->send_callback(p_card->buf1, p_card->buf_size);
        resualt = SsmRecordMemBlock(ch, -2, p_card->buf1, p_card->buf_size, SynwayAudioCard::recordMemBlockHandler, p_card);
        printf("send buf1\n");
    }
    if (pucBuf == p_card->buf2) {
        p_card->send_callback(p_card->buf2, p_card->buf_size);
        resualt = SsmRecordMemBlock(ch, -2, p_card->buf2, p_card->buf_size, SynwayAudioCard::recordMemBlockHandler, p_card);
        printf("send buf2\n");
    }
    return resualt;
}