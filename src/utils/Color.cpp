#include "utils/Color.hpp"
#include "utils/other.hpp"

string Color::as_html() {
    string result = "#";
    
    uint8_t high = (red >> 4) & 0xF;
    uint8_t low  = (red) & 0xF;
    result += nibble_to_hex(high);
    result += nibble_to_hex(low);

    high = (green >> 4) & 0xF;
    low  = (green) & 0xF;
    result += nibble_to_hex(high);
    result += nibble_to_hex(low);

    high = (blue >> 4) & 0xF;
    low  = (blue) & 0xF;
    result += nibble_to_hex(high);
    result += nibble_to_hex(low);

    return result;
}

void Color::from_html(const char* html) {
    if (! html || strlen(html) != 7 || html[0] != '#') return;

    red = (hex_to_nibble(html[1]) << 4) | hex_to_nibble(html[2]);
    green = (hex_to_nibble(html[3]) << 4) | hex_to_nibble(html[4]);
    blue = (hex_to_nibble(html[5]) << 4) | hex_to_nibble(html[6]);
}
