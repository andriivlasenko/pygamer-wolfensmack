#include <cassert>
#include "map.h"

static const char map[] = "0120012013201250"\
                          "0              0"\
                          "2              2"\
                          "0     00300    0"\
                          "1     5        4"\
                          "2     0     0020"\
                          "0   0300       1"\
                          "2   2  0       2"\
                          "5   2  100300  3"\
                          "1   0   0      1"\
                          "2       4      4"\
                          "3       0  20210"\
                          "2     020      0"\
                          "0 020          2"\
                          "2              0"\
                          "0020123512030210";

Map::Map() : w(16), h(16) {
    //assert(sizeof(map) == w*h+1); // +1 for the null terminated string
}

int Map::get(const size_t i, const size_t j) const {
    //assert(i<w && j<h && sizeof(map) == w*h+1);
    return map[i+j*w] - '0';
}

bool Map::is_empty(const size_t i, const size_t j) const {
    //assert(i<w && j<h && sizeof(map) == w*h+1);
    return map[i+j*w] == ' ';
}

