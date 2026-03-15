#include "camp_chef_smoker.h"

namespace esphome {
namespace camp_chef_smoker {

static const char *TAG = "camp_chef_smoker";
uint loop_count = 0;

void CampChefSmoker::setup() {

}

void CampChefSmoker::update() {

  if(loop_count < 2) {
    loop_count++;
    ESP_LOGI(TAG, "Reading boot information");
    send_opcode(0x07);
    send_opcode(0x08);
    send_opcode(0x34);
  }

  send_opcode(0x11);
  send_opcode(0x50);
  send_opcode(0x51);
  send_opcode(0x52);
  send_opcode(0x53);
}

void CampChefSmoker::loop() {

  while (available()) {
    uint8_t c = read();
    rx_buffer_.push_back(c);
  }

  while (true) {

    auto start = std::find(rx_buffer_.begin(), rx_buffer_.end(), 0xFE);
    if (start == rx_buffer_.end())
      return;

    auto end = std::find(start, rx_buffer_.end(), 0xFF);
    if (end == rx_buffer_.end())
      return;

    std::vector<uint8_t> frame(start, end + 1);

    decode_frame(frame);

    rx_buffer_.erase(rx_buffer_.begin(), end + 1);
  }
}

void CampChefSmoker::send_opcode(uint8_t opcode) {

  uint8_t pkt[4] = {0xFE, opcode, 0x00, 0xFF};
  write_array(pkt, 4);
  delay(100);
}

int CampChefSmoker::digits_to_int(uint8_t *data, int len) {
  int value = 0;
  for (int i = 0; i < len; i++)
    value = value * 10 + data[i];
  return value;
}

void CampChefSmoker::decode_frame(std::vector<uint8_t> &frame) {

  uint8_t opcode = frame[1];
  uint8_t *payload = &frame[2];
  char timer_buf[16];

  switch (opcode) {

    case 0x07:
      ESP_LOGI(TAG, "Model Code: %d", payload[0]);
      break;

    case 0x08: {

      int min_pit_temp = digits_to_int(payload, 3);
      int max_pit_temp = digits_to_int(payload + 3, 3);
      int feed_time = digits_to_int(payload + 6, 3);
      int unknown_value_210 = digits_to_int(payload + 9, 3);
      int unknown_value_238 = digits_to_int(payload + 12, 4);
      int shutdown_time = digits_to_int(payload + 16, 4);

      ESP_LOGI(TAG, "Capabilities and Limits Block (0x08):");
      ESP_LOGI(TAG, "  Min Pit Temp: %d", min_pit_temp);
      ESP_LOGI(TAG, "  Max Pit Temp: %d", max_pit_temp);
      ESP_LOGI(TAG, "  Feed Time: %d", feed_time);
      ESP_LOGI(TAG, "  Unknown Value (210): %d", unknown_value_210);
      ESP_LOGI(TAG, "  Unknown Value (238): %d", unknown_value_238);
      ESP_LOGI(TAG, "  Shutdown Time: %d", shutdown_time);

      break;
    }

    case 0x11: {
      int pit = digits_to_int(payload,3);
      int set = digits_to_int(payload+3,3);
      int probe1 = digits_to_int(payload+12,3);
      int probe2 = digits_to_int(payload+15,3);
      int smoke = payload[25];

      uint8_t state = payload[26];
      raw_state_value = state;

      if (status_) {

        const char *text = "UNKNOWN";

        if (state == 0x01)
        {
          text = "IDLE";
          if(active_countdown_timer_seconds_)
            active_countdown_timer_seconds_->publish_state(0);
          if(active_countdown_timer_text_)
            active_countdown_timer_text_->publish_state("00:00:00");
        }else if (state == 0x02)
        {
          text = "RUNNING";
          if(active_countdown_timer_seconds_)
            active_countdown_timer_seconds_->publish_state(0);
          if(active_countdown_timer_text_)
            active_countdown_timer_text_->publish_state("00:00:00");
        }else if (state == 0x00)
        {
          text = "STARTUP";
        }else if (state == 0x04)
        {
          text = "SHUTDOWN";
        }
        else if (state == 0x03)
        {
          text = "FEED";
        }else{
          ESP_LOGW(TAG, "Unknown state value: 0x%02X", state);
          if(active_countdown_timer_seconds_)
            active_countdown_timer_seconds_->publish_state(0);
          if(active_countdown_timer_text_)
            active_countdown_timer_text_->publish_state("00:00:00");
        }

        status_->publish_state(text);
      }

      if (pit_temp_) pit_temp_->publish_state(pit);
      if (set_temp_) set_temp_->publish_state(set);
      if (probe1_) probe1_->publish_state(probe1);
      if (probe2_) probe2_->publish_state(probe2);
      if (smoke_) smoke_->publish_state(smoke);

      if (rtd_error_) rtd_error_->publish_state(payload[28]);
      if (over_temp_) over_temp_->publish_state(payload[29]);
      if (flame_out_) flame_out_->publish_state(payload[30]);

      if (probe1_valid_) probe1_valid_->publish_state(payload[34]);
      if (probe2_valid_) probe2_valid_->publish_state(payload[35]);

      if(auger_) auger_->publish_state(payload[38]);
      if(fan_) fan_->publish_state(payload[40]);
      if(igniter_) igniter_->publish_state(payload[39]);

      break;
    }

    case 0x34: {
      std::string fw;

      for (size_t i = 0; i < frame.size() - 2; i++) {
        uint8_t c = payload[i];
        if (c == 0xFF)
          break;
        fw += static_cast<char>(c);
      }

      ESP_LOGI(TAG, "Firmware: %s", fw.c_str());
      break;
    }

    case 0x50:
      if (startup_timer_)
        startup_timer_->publish_state(digits_to_int(payload,4));
        if(raw_state_value == 0x00)
        {
          if(active_countdown_timer_seconds_)
            active_countdown_timer_seconds_->publish_state(digits_to_int(payload,4));
          if(active_countdown_timer_text_)
          {
            seconds_to_hms(timer_buf, digits_to_int(payload,4));
            active_countdown_timer_text_->publish_state(timer_buf);
          }
        }
      break;

    case 0x51:
      if (feed_timer_)
        feed_timer_->publish_state(digits_to_int(payload,4));
        if(raw_state_value == 0x03)
        {
          if(active_countdown_timer_seconds_)
            active_countdown_timer_seconds_->publish_state(digits_to_int(payload,4));
          if(active_countdown_timer_text_)
          {
            seconds_to_hms(timer_buf, digits_to_int(payload,4));
            active_countdown_timer_text_->publish_state(timer_buf);
          }
        }
      break;

    case 0x52:
      if (shutdown_timer_)
        shutdown_timer_->publish_state(digits_to_int(payload,4));
      if(raw_state_value == 0x04)
      {
        if(active_countdown_timer_seconds_)
        {
          active_countdown_timer_seconds_->publish_state(digits_to_int(payload,4));
        }
        if(active_countdown_timer_text_)
        {
          seconds_to_hms(timer_buf, digits_to_int(payload,4));
          active_countdown_timer_text_->publish_state(timer_buf);
        }
      }
      break;

    case 0x53: {
      // This format takes the format of [hundreds of hours] [tens of hours] [hours] [tens of minutes] [minutes] [tens of seconds] [seconds]
      int h = digits_to_int(payload,3);
      int m = digits_to_int(payload+3,2);
      int s = digits_to_int(payload+5,2);

      int total = h*3600 + m*60 + s;

      if (cook_time_seconds_)
        cook_time_seconds_->publish_state(total);
      if (cook_time_text_)
      {
        seconds_to_hms(timer_buf, total);
        cook_time_text_->publish_state(timer_buf);
      }

      break;
    }

    default: {
      std::string hex;
      hex.reserve(frame.size() * 3);

      char buf[4];
      for (uint8_t b : frame) {
        sprintf(buf, "%02X ", b);
        hex += buf;
      }

      ESP_LOGW(TAG, "Unknown opcode 0x%02X Frame: %s", opcode, hex.c_str());
      break;
    }

  }
}

}
}
