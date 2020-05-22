#ifndef UTILS_H
#define UTILS_H


#define MAX_STUFF 500

#include <vector>
#include <cstdint>
#include <string>

uint16_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a=255);
void unpack_color(const uint16_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a);

#endif // UTILS_H

