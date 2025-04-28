#pragma once

#include "assets.h"
#include "control_button.h"
#include "dht11.h"
#include "elements.h"
#include "indicator_bar.h"
#include "logical_button.h"
#include "mik32_hal_gpio.h"
#include "progress_bar.h"
#include "screen.h"
#include "sensor.h"
#include "tamagotchi.h"

#include "ssd1315.h"

namespace tamagotchi {
struct interface {
protected:
  dht11 dht{GPIO_0, GPIO_PIN_0};
  control_button btn{GPIO_0, GPIO_PIN_10};
  sensor brightness{ANALOG_REG, 1};

  info_panel info_panel_{0, 0};
  ui::screen scr;

  ssd1315 ssd1315_;

  tamagotchi tamchi;

public:
  interface(SPI_TypeDef *spi)
      : ssd1315_(spi, GPIO_1, GPIO_PIN_8, GPIO_1, GPIO_PIN_9) {
    scr.drawables().push_back(info_panel_);
  }

  ~interface() = default;

  interface(const interface &) = default;

  interface(interface &&) = default;

  interface &operator=(const interface &) = default;

  interface &operator=(interface &&) = default;

  void init() {
    btn.init();
    {
      ssd1315_.init();
      uint8_t data_init[] = {
          0xA8,
          0x3F, // Set MUX Ratio // 0x3F - 128x64; 0x1F - 128x32
          0xD3,
          0x00, // Set Display Offset
          0x40, // Set Display Start Line
          0xA1, // Set Segment re-map
          0x20,
          0x01, // Set vertical addressing
          0xC8, // Set COM Output Scan Direction
          0xDA,
          0x12, // Set COM Pins hardware configuration. 0x12 - 128x64; 0x02 -
                // 128x32
          0x81,
          0xFF, // Set Contrast Control
          0xA4, // Disable Entire Display On
          0xA6, // Set Normal Display
          0xD5,
          0x80, // Set Osc Frequency
          0x8D,
          0x14, // Enable charge pump regulator
          0xAF  // Display On
      };
      ssd1315_.command(data_init, sizeof(data_init));
    }
    {
      info_panel_.temperature.bar.lvl_1() =
          (tamagotchi::min_comfort_temperature - tamagotchi::min_temperature) *
          ui::indicator_bar::max_value /
          (tamagotchi::max_temperature - tamagotchi::min_temperature);
      info_panel_.temperature.bar.lvl_2() =
          (tamagotchi::max_comfort_temperature - tamagotchi::min_temperature) *
          ui::indicator_bar::max_value /
          (tamagotchi::max_temperature - tamagotchi::min_temperature);
    }
    {
      brightness.init();
      info_panel_.brightness.bar.lvl_1() =
          tamagotchi::max_comfort_sleep_brightness *
          ui::indicator_bar::max_value / tamagotchi::max_brightness;
      info_panel_.brightness.bar.lvl_2() =
          tamagotchi::min_comfort_awake_brightness *
          ui::indicator_bar::max_value / tamagotchi::max_brightness;
    }
  }

  void tick() {
    if (tamchi.is_dead())
      return;

    tamchi.tick();

    auto btn_event = btn.tick();

    // satiety
    {
      if (btn_event == logical_button::button_event::CLICK) {
        tamchi.feed();
      }
      tamchi.damage_hungry();
      tamchi.hunger();
    }

    // temperature
    {
      tamchi.damage_temperature();
    }

    // brightness
    {
      tamchi.calc_energy();
    }

    // happiness
    {
      if (btn_event == logical_button::button_event::HOLD) {
        tamchi.happiness = tamagotchi::max_happiness;
      }
      tamchi.happiness_to_healing();
    }
  }

  void update() {
    if (tamchi.is_dead())
      return;
    tamchi.temperature = dht.receive().temp_h;
    tamchi.brightness =
        brightness.get() * tamagotchi::max_brightness / sensor::max_value;
  }

  void draw() {
    info_panel_.clock.set(tamchi.clock_hours(), tamchi.clock_minutes());
    info_panel_.temperature.bar.indicator() =
        (tamchi.temperature - tamagotchi::min_temperature) *
        ui::indicator_bar::max_value /
        (tamagotchi::max_temperature - tamagotchi::min_temperature);
    info_panel_.satiety.bar.progress() = tamchi.satiety *
                                         ui::progress_bar::max_progress /
                                         tamagotchi::max_satiety;
    info_panel_.brightness.bar.indicator() = tamchi.brightness *
                                             ui::progress_bar::max_progress /
                                             tamagotchi::max_brightness;
    info_panel_.happiness.bar.progress() = tamchi.happiness *
                                           ui::progress_bar::max_progress /
                                           tamagotchi::max_happiness;

    info_panel_.energy.set_progress(tamchi.energy * energy_bar::max_progress /
                                    tamagotchi::max_energy);
    info_panel_.health.set_progress(tamchi.health * heart_bar::max_progress /
                                    tamagotchi::max_health);

    if (tamchi.is_dead()) {
      info_panel_.eyes.icon() = assets::eyes_dead;
    } else if (tamchi.is_hungry()) {
      info_panel_.eyes.icon() = assets::eyes_hungry;
    } else if (tamchi.is_cold()) {
      info_panel_.eyes.icon() = assets::eyes_cold;
    } else if (tamchi.is_hot()) {
      info_panel_.eyes.icon() = assets::eyes_hot;
    } else if (tamchi.is_too_bright()) {
      info_panel_.eyes.icon() = assets::eyes_tired;
    } else if (tamchi.is_too_dark()) {
      info_panel_.eyes.icon() = assets::eyes_sleepy;
    } else if (tamchi.is_sleep_time()) {
      info_panel_.eyes.icon() = assets::eyes_closed;
    } else {
      static size_t eyes_pos = 0;
      switch (eyes_pos++ % 6) {
      case 0:
      case 4:
        info_panel_.eyes.icon() = assets::eyes_left;
        break;
      case 1:
      case 3:
        info_panel_.eyes.icon() = assets::eyes_right;
        break;
      case 2:
      case 5:
        info_panel_.eyes.icon() = assets::eyes_center;
        break;
      }
    }

    scr.redraw();
    ssd1315_.data(scr.raw_data(), scr.raw_size());
  }
};
} // namespace tamagotchi