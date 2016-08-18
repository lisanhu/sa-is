#include <iostream>
#include "sais.h"

using namespace std;

int main() {
    string input = "mmiissiissiippii";
    SA::size_t *sa;
    sa = new SA::size_t[input.length() + 1];
    sa_is(input.c_str(), sa, input.length() + 1, 4, sizeof(char));
    print_array(sa, input.length() + 1);

    for (SA::size_t i = 0; i < input.length() + 1; ++i) {
        cout << input.c_str() + sa[i] << endl;
    }
    return 0;
}