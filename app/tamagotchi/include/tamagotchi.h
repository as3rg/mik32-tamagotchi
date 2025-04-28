#pragma once

#include <algorithm>
#include <cstddef>
namespace tamagotchi {
struct tamagotchi {
public:
  static constexpr size_t max_health = 255;
  static constexpr size_t max_energy = 255;

  static constexpr size_t min_temperature = 5;
  static constexpr size_t max_temperature = 45;
  static constexpr size_t min_comfort_temperature = 20;
  static constexpr size_t max_comfort_temperature = 25;
  static constexpr size_t temperature_damage_period = 10;
  static constexpr size_t temperature_damage = 1;

  static constexpr size_t max_satiety = 255;
  static constexpr size_t satiety_feed_portion = 10;
  static constexpr size_t satiety_hunger_portion = 1;
  static constexpr size_t satiety_hunger_period = 10;
  static constexpr size_t satiety_damage_period = 10;
  static constexpr size_t satiety_damage = 1;

  static constexpr size_t max_brightness = 255;
  static constexpr size_t max_comfort_sleep_brightness = 100;
  static constexpr size_t min_comfort_awake_brightness = 190;
  static constexpr size_t energy_portion = 3;
  static constexpr size_t energy_period = 10;
  static constexpr size_t energy_damage = 1;

  static constexpr size_t max_happiness = 255;
  static constexpr size_t happiness_portion = 3;
  static constexpr size_t happiness_period = 10;
  static constexpr size_t happiness_healing = 1;

  size_t health = max_health;
  size_t temperature;
  size_t time = 12 * 60 * 60 + 30 * 60;
  size_t happiness = 0;
  size_t satiety = max_satiety;
  size_t energy = max_energy;
  size_t brightness;

  constexpr size_t clock_seconds() const { return total_seconds() % 60; }

  constexpr size_t clock_minutes() const { return total_seconds() / 60 % 60; }

  constexpr size_t clock_hours() const {
    return total_seconds() / 60 / 60 % 24;
  }

  constexpr size_t total_seconds() const { return time; }

  constexpr void tick() { time++; }

  constexpr void damage(size_t dmg) {
    if (health > dmg) {
      health -= dmg;
    } else {
      health = 0;
    }
  }

  constexpr void heal(size_t delta) {
    if (max_health - health > delta) {
      health += delta;
    } else {
      health = max_health;
    }
  }

  constexpr void use_energy(size_t delta, size_t dmg) {
    if (energy > delta) {
      energy -= delta;
    } else if (energy) {
      energy = 0;
    } else {
      damage(dmg);
    }
  }

  constexpr void recover_energy(size_t delta) {
    if (max_energy - energy > delta) {
      energy += delta;
    } else {
      energy = max_energy;
    }
  }

  constexpr bool is_dead() const { return !health; }

  // satiety
  constexpr void feed() {
    satiety = std::min(satiety + satiety_feed_portion, max_satiety);
  }

  constexpr bool is_hungry() const { return !satiety; }

  constexpr void damage_hungry() {
    if (total_seconds() % satiety_damage_period == 0 && is_hungry()) {
      damage(satiety_damage);
    }
  }

  constexpr void hunger() {
    if (total_seconds() % satiety_hunger_period == 0) {
      if (satiety >= satiety_hunger_portion) {
        satiety -= satiety_hunger_portion;
      } else {
        satiety = 0;
      }
    }
  }

  // temperature
  constexpr bool is_hot() const {
    return temperature > max_comfort_temperature;
  }

  constexpr bool is_cold() const {
    return temperature < min_comfort_temperature;
  }

  constexpr void damage_temperature() {
    if (total_seconds() % temperature_damage_period == 0 &&
        (is_hot() || is_cold())) {
      damage(temperature_damage);
    }
  }

  // brightness
  constexpr bool is_awake_time() const {
    return 12 <= clock_hours() && clock_hours() < 18;
  }

  constexpr bool is_sleep_time() const {
    return 0 <= clock_hours() && clock_hours() < 6;
  }

  constexpr bool is_too_bright() const {
    return is_sleep_time() && brightness > max_comfort_sleep_brightness;
  }

  constexpr bool is_too_dark() const {
    return is_awake_time() && brightness < min_comfort_awake_brightness;
  }

  constexpr void calc_energy() {
    if (total_seconds() % energy_period == 0) {
      if (is_too_bright() || is_too_dark()) {
        use_energy(energy_portion, energy_damage);
      } else if (is_sleep_time()) {
        recover_energy(energy_portion);
      }
    }
  }

  // happiness
  constexpr void happiness_to_healing() {
    if (total_seconds() % happiness_period == 0) {
      if (happiness > happiness_portion) {
        happiness -= happiness_portion;
        heal(happiness_healing);
      }
    }
  }
};
} // namespace tamagotchi