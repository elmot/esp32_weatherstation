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
constexpr std::array<uint8_t, DISP_BUF_SIZE> fromXpm() {
    std::array<uint8_t, DISP_BUF_SIZE> output = {};
    std::array<COLOR, 256> colormap = {};
    static_assert(std::string_view(IMAGE[0]) == "800 480 7 1", "Invalid image format");
    static_assert(std::string_view(IMAGE[1]).ends_with("c #000000"), "Invalid image palette");
    colormap[IMAGE[1][0]] = BLACK;
    static_assert(std::string_view(IMAGE[2]).ends_with("c #ECF8FF"), "Invalid image palette");
    colormap[IMAGE[2][0]] = WHITE;
    static_assert(std::string_view(IMAGE[3]).ends_with("c #01BD26"), "Invalid image palette");
    colormap[IMAGE[3][0]] = GREEN;
    static_assert(std::string_view(IMAGE[4]).ends_with("c #0D17E2"), "Invalid image palette");
    colormap[IMAGE[4][0]] = BLUE;
    static_assert(std::string_view(IMAGE[5]).ends_with("c #FA2811"), "Invalid image palette");
    colormap[IMAGE[5][0]] = RED;
    static_assert(std::string_view(IMAGE[6]).ends_with("c #FFFF22"), "Invalid image palette");
    colormap[IMAGE[6][0]] = YELLOW;
    static_assert(std::string_view(IMAGE[7]).ends_with("c #F78216"), "Invalid image palette");
    colormap[IMAGE[7][0]] = ORANGE;
    for (int y = 0, idx = 0; y < DISP_HEIGHT; ++y) {
        for (int x = 0; x < DISP_WIDTH; x += 2, ++idx) {
            COLOR color0 = colormap[IMAGE[y + 8][x]];
            COLOR color1 = colormap[IMAGE[y + 8][x + 1]];
            output[idx] = (color1 & 7) | ((color0 << 4) & 0x70);
        }
    }
    return output;
}

constexpr const std::array<uint8_t, DISP_BUF_SIZE> image1 = fromXpm < test1_xpm > ();
