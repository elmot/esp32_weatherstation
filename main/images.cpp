#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "inky_impression7_3.h"
#include "images.h"


constexpr const
#include "../../images-xpm/test1.xpm"
#include <string_view>
#include <map>
#include "inky_impression7_3.h"

template<const char *const *IMAGE>
constexpr std::array<uint8_t, InkyImpression7_3f::BUF_SIZE> fromXpm() {
    std::array<uint8_t, InkyImpression7_3f::BUF_SIZE> output = {};
    std::array<InkyImpression7_3f::COLOR, 256> colormap = {};
    static_assert(std::string_view(IMAGE[0]) == "800 480 7 1", "Invalid image format");
    static_assert(std::string_view(IMAGE[1]).ends_with("c #000000"), "Invalid image palette");
    colormap[IMAGE[1][0]] = InkyImpression7_3f::BLACK;
    static_assert(std::string_view(IMAGE[2]).ends_with("c #ECF8FF"), "Invalid image palette");
    colormap[IMAGE[2][0]] = InkyImpression7_3f::WHITE;
    static_assert(std::string_view(IMAGE[3]).ends_with("c #01BD26"), "Invalid image palette");
    colormap[IMAGE[3][0]] = InkyImpression7_3f::GREEN;
    static_assert(std::string_view(IMAGE[4]).ends_with("c #0D17E2"), "Invalid image palette");
    colormap[IMAGE[4][0]] = InkyImpression7_3f::BLUE;
    static_assert(std::string_view(IMAGE[5]).ends_with("c #FA2811"), "Invalid image palette");
    colormap[IMAGE[5][0]] = InkyImpression7_3f::RED;
    static_assert(std::string_view(IMAGE[6]).ends_with("c #FFFF22"), "Invalid image palette");
    colormap[IMAGE[6][0]] = InkyImpression7_3f::YELLOW;
    static_assert(std::string_view(IMAGE[7]).ends_with("c #F78216"), "Invalid image palette");
    colormap[IMAGE[7][0]] = InkyImpression7_3f::ORANGE;
    for (int y = 0, idx = 0; y < InkyImpression7_3f::HEIGHT; ++y) {
        for (int x = 0; x < InkyImpression7_3f::WIDTH; x += 2, ++idx) {
            InkyImpression7_3f::COLOR color0 = colormap[IMAGE[y + 8][x]];
            InkyImpression7_3f::COLOR color1 = colormap[IMAGE[y + 8][x + 1]];
            output[idx] = (color1 & 7) | ((color0 << 4) & 0x70);
        }
    }
    return output;
}

constexpr const std::array<uint8_t, InkyImpression7_3f::BUF_SIZE> image1 = fromXpm < test1_xpm > ();
