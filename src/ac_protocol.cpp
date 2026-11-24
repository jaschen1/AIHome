#include "ac_protocol.h"

#include <algorithm>

namespace ac {
namespace {
constexpr size_t kSectionSize = 8;
constexpr uint16_t kHeaderMarkUs = 9010;
constexpr uint16_t kHeaderGapUs = 4505;
constexpr uint16_t kBitMarkUs = 680;
constexpr uint16_t kOneGapUs = 1530;
constexpr uint16_t kZeroGapUs = 510;
constexpr uint16_t kBlockGapUs = 19975;
constexpr uint16_t kSectionGapUs = 39950;

void setChecksum(uint8_t *section) {
    uint8_t sum = 10;
    for (size_t i = 0; i < 4; ++i) sum += section[i] & 0x0F;
    for (size_t i = 4; i < 7; ++i) sum += section[i] >> 4;
    section[7] = (section[7] & 0x0F) | ((sum & 0x0F) << 4);
}
}  // namespace

uint8_t effectiveTemperature(const State &state) {
    if (state.mode == Mode::Auto || state.mode == Mode::Dry) return 25;
    return std::clamp(state.temperature, kMinimumTemperature, kMaximumTemperature);
}

uint8_t effectiveFan(const State &state) {
    return state.economy ? 0 : std::min(state.fan, kMaximumFan);
}

Packet encode(const State &state) {
    Packet packet;
    packet.size = state.timer ? 24 : 16;
    const uint8_t fan = effectiveFan(state);
    const uint8_t command = static_cast<uint8_t>(state.mode) |
                            (state.power ? 0x08 : 0) |
                            (std::min<uint8_t>(fan, 3) << 4) |
                            (state.swingAuto ? 0x40 : 0) |
                            (state.sleep ? 0x80 : 0);
    const uint8_t temperature = (effectiveTemperature(state) - kMinimumTemperature) |
                                (state.timer ? 0x90 : 0);
    const uint8_t options = (state.turbo ? 0x10 : 0) |
                            (state.light ? 0x20 : 0) |
                            (state.dry ? 0x80 : 0);

    auto &bytes = packet.bytes;
    bytes[0] = command;
    bytes[1] = temperature;
    bytes[2] = options;
    bytes[3] = 0x50;
    bytes[4] = (state.verticalSwing ? 0x01 : 0) | (state.horizontalSwing ? 0x10 : 0);
    bytes[5] = 0x40;
    bytes[7] = (state.economy ? 0x04 : 0) | (state.auxiliaryHeat ? 0x08 : 0);

    if (state.timer) {
        // The original remote inserts this middle section when the 30-minute timer is set.
        bytes[8] = command;
        bytes[9] = temperature;
        bytes[10] = options;
        bytes[11] = 0x60;
        bytes[13] = 0xE0;
        bytes[14] = 0x01;
        bytes[15] = 0x01;
    }

    const size_t last = packet.size - kSectionSize;
    bytes[last] = command;
    bytes[last + 1] = temperature;
    bytes[last + 2] = options;
    bytes[last + 3] = 0x70;
    bytes[last + 4] = state.quiet ? 0x80 : 0;
    bytes[last + 6] = fan << 4;

    for (size_t offset = 0; offset < packet.size; offset += kSectionSize) {
        setChecksum(bytes.data() + offset);
    }
    return packet;
}

size_t makeTimings(const Packet &packet, Timings &timings) {
    size_t count = 0;
    auto addBits = [&](uint8_t value, unsigned bits) {
        for (unsigned bit = 0; bit < bits; ++bit) {
            timings[count++] = kBitMarkUs;
            timings[count++] = (value & (1U << bit)) ? kOneGapUs : kZeroGapUs;
        }
    };
    for (size_t offset = 0; offset < packet.size; offset += kSectionSize) {
        timings[count++] = kHeaderMarkUs;
        timings[count++] = kHeaderGapUs;
        for (size_t i = 0; i < 4; ++i) addBits(packet.bytes[offset + i], 8);
        addBits(0b010, 3);
        timings[count++] = kBitMarkUs;
        timings[count++] = kBlockGapUs;
        for (size_t i = 4; i < kSectionSize; ++i) addBits(packet.bytes[offset + i], 8);
        timings[count++] = kBitMarkUs;
        if (offset + kSectionSize < packet.size) timings[count++] = kSectionGapUs;
    }
    return count;
}

}  // namespace ac
