///////////////////////////////////////////////////////////////////////////////
// 128-bit support when native support is absent
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>

namespace abieos {

#ifndef ABIEOS_NO_INT128

using uint128 = unsigned __int128;
using int128 = __int128;

#else

struct uint128 {
    std::array<uint8_t, 16> data = {};
};
EOSIO_REFLECT(uint128, data)

struct int128 {
    std::array<uint8_t, 16> data = {};
};
EOSIO_REFLECT(int128, data)

template <typename S>
void from_json(uint128& obj, S& stream) {
    decimal_to_binary(obj.data, stream.get_string());
}

template <typename S>
void from_json(int128& obj, S& stream) {
    auto s = stream.get_string();
    if (s.size() && s[0] == '-') {
        decimal_to_binary(obj.data, {s.data() + 1, s.size() - 1});
        negate(obj.data);
        eosio::check(is_negative(obj.data), eosio::convert_json_error(eosio::from_json_error::number_out_of_range));
    } else {
        decimal_to_binary(obj.data, s);
        eosio::check(!is_negative(obj.data), eosio::convert_json_error(eosio::from_json_error::number_out_of_range));
    }
}

template <typename S>
void to_json(const uint128& obj, S& stream) {
    auto s = "\"" + binary_to_decimal(obj.data) + "\"";
    stream.write(s.data(), s.size());
}

template <typename S>
void to_json(const int128& obj, S& stream) {
    if (is_negative(obj.data)) {
        auto n = obj;
        negate(n.data);
        auto s = "\"-" + binary_to_decimal(n.data) + "\"";
        stream.write(s.data(), s.size());
    } else {
        auto s = "\"" + binary_to_decimal(obj.data) + "\"";
        stream.write(s.data(), s.size());
    }
}

#endif // ABIEOS_NO_INT128

} // namespace abieos