#pragma once

//#include "esphome.h"

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"


namespace esphome {
namespace camp_chef_smoker {

inline void seconds_to_hms(char* buffer, int seconds) {
  int h = seconds / 3600;
  int m = (seconds % 3600) / 60;
  int s = seconds % 60;
  sprintf(buffer, "%02d:%02d:%02d", h, m, s);
}

class CampChefSmoker : public PollingComponent, public uart::UARTDevice { //public sensor::Sensor

 public:
  CampChefSmoker() {}

  void setup() override;
  void update() override;
  void loop() override;

  void set_pit_temp_sensor(sensor::Sensor *s) { pit_temp_ = s; }
  void set_set_temp_sensor(sensor::Sensor *s) { set_temp_ = s; }
  void set_probe1_sensor(sensor::Sensor *s) { probe1_ = s; }
  void set_probe2_sensor(sensor::Sensor *s) { probe2_ = s; }
  void set_smoke_sensor(sensor::Sensor *s) { smoke_ = s; }

  void set_startup_timer_sensor(sensor::Sensor *s) { startup_timer_ = s; }
  void set_feed_timer_sensor(sensor::Sensor *s) { feed_timer_ = s; }
  void set_shutdown_timer_sensor(sensor::Sensor *s) { shutdown_timer_ = s; }
  void set_active_countdown_timer_seconds_sensor(sensor::Sensor *s) { active_countdown_timer_seconds_ = s; }
  void set_active_countdown_timer_sensor(text_sensor::TextSensor *t) { active_countdown_timer_text_ = t; }
  void set_cook_time_seconds_sensor(sensor::Sensor *s) { cook_time_seconds_ = s; }
  void set_cook_time_sensor(text_sensor::TextSensor *t) { cook_time_text_ = t; }

  void set_status_sensor(text_sensor::TextSensor *t) { status_ = t; }

  void set_flame_out_sensor(binary_sensor::BinarySensor *b) { flame_out_ = b; }
  void set_over_temp_sensor(binary_sensor::BinarySensor *b) { over_temp_ = b; }
  void set_rtd_error_sensor(binary_sensor::BinarySensor *b) { rtd_error_ = b; }

  void set_probe1_valid_sensor(binary_sensor::BinarySensor *b) { probe1_valid_ = b; }
  void set_probe2_valid_sensor(binary_sensor::BinarySensor *b) { probe2_valid_ = b; }
  
  void set_auger_sensor(binary_sensor::BinarySensor *b) { auger_ = b; }
  void set_fan_sensor(binary_sensor::BinarySensor *b) { fan_ = b; }
  void set_igniter_sensor(binary_sensor::BinarySensor *b) { igniter_ = b; }

 protected:

  void send_opcode(uint8_t opcode);
  void decode_frame(std::vector<uint8_t> &frame);
  int digits_to_int(uint8_t *data, int len);
  int raw_state_value = -1;

  std::vector<uint8_t> rx_buffer_;

  sensor::Sensor *pit_temp_{nullptr};
  sensor::Sensor *set_temp_{nullptr};
  sensor::Sensor *probe1_{nullptr};
  sensor::Sensor *probe2_{nullptr};
  sensor::Sensor *smoke_{nullptr};

  sensor::Sensor *startup_timer_{nullptr};
  sensor::Sensor *feed_timer_{nullptr};
  sensor::Sensor *shutdown_timer_{nullptr};
  sensor::Sensor *active_countdown_timer_seconds_{nullptr};
  text_sensor::TextSensor *active_countdown_timer_text_{nullptr};
  sensor::Sensor *cook_time_seconds_{nullptr};
  text_sensor::TextSensor *cook_time_text_{nullptr};

  text_sensor::TextSensor *status_{nullptr};

  binary_sensor::BinarySensor *flame_out_{nullptr};
  binary_sensor::BinarySensor *over_temp_{nullptr};
  binary_sensor::BinarySensor *rtd_error_{nullptr};

  binary_sensor::BinarySensor *probe1_valid_{nullptr};
  binary_sensor::BinarySensor *probe2_valid_{nullptr};

  binary_sensor::BinarySensor *auger_{nullptr};
  binary_sensor::BinarySensor *fan_{nullptr};
  binary_sensor::BinarySensor *igniter_{nullptr};
};

}
}