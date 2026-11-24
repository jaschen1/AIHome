#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace ac {

enum class Mode : uint8_t { Auto, Cool, Dry, Fan, Heat };

struct State {
    Mode mode = Mode::Cool;
    uint8_t temperature = 25;
    uint8_t fan = 0;
    bool power = false;
    bool economy = false;
    bool verticalSwing = false;
    bool horizontalSwing = false;
    bool swingAuto = false;
    bool quiet = false;
    bool turbo = false;
    bool timer = false;
    bool light = true;
    bool dry = false;
    bool auxiliaryHeat = false;
    bool sleep = false;
};

struct Packet {
    std::array<uint8_t, 24> bytes{};
    size_t size;
};

constexpr uint8_t kMinimumTemperature = 16;
constexpr uint8_t kMaximumTemperature = 30;
constexpr uint8_t kMaximumFan = 5;
constexpr size_t kMaxPulseCount = 419;
using Timings = std::array<uint16_t, kMaxPulseCount>;

uint8_t effectiveTemperature(const State &state);
uint8_t effectiveFan(const State &state);
Packet encode(const State &state);
size_t makeTimings(const Packet &packet, Timings &timings);

}  // namespace ac
