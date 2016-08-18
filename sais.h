//
// Created by lsh on 16-8-15.
//

#ifndef SA_IS_SAIS_H
#define SA_IS_SAIS_H

#include <iostream>

template <typename T>
void print_array(T * array, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

namespace SA {
    typedef long size_t;
    void SAIS(const unsigned char *src, size_t *sa, size_t length, size_t alphabet_sz, int mem_sz, int level);
}

void sa_is(const char *src, SA::size_t *sa, SA::size_t length, SA::size_t alphabet_sz, int mem_sz);

#endif //SA_IS_SAIS_H
