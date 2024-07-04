#include <Geode/Geode.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <random>

using namespace geode::prelude;

struct MyHardStreak : Modify<MyHardStreak, HardStreak> {
  void addPoint(CCPoint p0) {
    float max_wave_pulse_size = Mod::get()->getSettingValue<double>("max-wave-pulse-size");
    float min_wave_pulse_size = Mod::get()->getSettingValue<double>("min-wave-pulse-size");
    float max_wave_trail_size = Mod::get()->getSettingValue<double>("max-wave-trail-size");
    float min_wave_trail_size = Mod::get()->getSettingValue<double>("min-wave-trail-size");
    float min_y_pos_offset = Mod::get()->getSettingValue<double>("min-y-pos-offset");
    float max_y_pos_offset = Mod::get()->getSettingValue<double>("max-y-pos-offset");
    float min_x_pos_offset = Mod::get()->getSettingValue<double>("min-x-pos-offset");
    float max_x_pos_offset = Mod::get()->getSettingValue<double>("max-x-pos-offset");
    float skip_chance = Mod::get()->getSettingValue<double>("skip-chance");

    bool chaos = Mod::get()->getSettingValue<bool>("chaos");

    if (chaos) {
      std::mt19937 rng(std::time(nullptr));
      std::uniform_real_distribution<float> dist_pulse(min_wave_pulse_size, max_wave_pulse_size);
      std::uniform_real_distribution<float> dist_trail(min_wave_trail_size, max_wave_trail_size);

      auto random_float_pulse = [&]() { return dist_pulse(rng); };
      auto random_float_trail = [&]() { return dist_trail(rng); };

      std::vector<std::function<void()>> operations = {
          [&]() { p0.x += random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.x -= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.x *= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.x /= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y += random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y -= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y *= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { p0.y /= random_float(min_x_pos_offset, max_y_pos_offset); },
          [&]() { m_waveSize += random_float_trail(); },
          [&]() { m_waveSize -= random_float_trail(); },
          [&]() { m_waveSize *= random_float_trail(); },
          [&]() { m_waveSize /= random_float_trail(); },
          [&]() { m_pulseSize += random_float_trail(); },
          [&]() { m_pulseSize -= random_float_trail(); },
          [&]() { m_pulseSize *= random_float_trail(); },
          [&]() { m_pulseSize /= random_float_trail(); }};

      std::bernoulli_distribution enable_op(skip_chance);

      for (auto &op: operations) {
        if (enable_op(rng)) {
          op();
        }
      }
    }

    HardStreak::addPoint(p0);
  }

  float random_float(float min = -25.f, float max = 25.f) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
  }
};
