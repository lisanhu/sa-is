//
// Created by lsh on 16-8-15.
//

#include "sais.h"

namespace SA {
    const unsigned char mask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    const bool L = false, S = true;
    typedef long size_t;

    /**
     * Set the type of the char at \b pos to the value of \b v
     * \Note This function will use one bit for the type of one character
     * @param t The type array
     * @param pos The position of the type element to be set
     * @param v The value to set, L- type to be false and S- type to be true
     */
    inline void tset(unsigned char *t, size_t pos, bool v) {
        size_t idx = pos / sizeof(char);
        size_t offset = pos % sizeof(char);
        t[idx] = v ? t[idx] | mask[offset] : (~mask[offset]) & t[idx];
    }

    /**
     * Get the type of the char at \b pos
     * \Note We need this because the type information are stored in binary stream in
     * order to save memory
     * @param t The type array
     * @param pos The position of the type to be read
     * @return The type of the char at \b pos, L- type is false and S- type is true
     */
    inline bool tget(unsigned char *t, size_t pos) {
        size_t idx = pos / sizeof(char);
        size_t offset = pos % sizeof(char);
        return (t[idx] & mask[offset]) != 0;
    }

    inline unsigned chr(const unsigned char *src, size_t pos, int mem_sz) {
        if (mem_sz == sizeof(char)) return src[pos];
        else return static_cast<unsigned *>(src)[pos];
    }

    /**
     * Hash function to map the current char to an index in the buckets,
     * we can easily change this when numeric is not the best solution to identify index
     * @param c The character to be hashed
     * @return The index in the bucket of this char
     */
    inline size_t hash_char(unsigned c) {
        return c;
    }

    /**
     * This class is used to force calling build_t_array and other functions
     * when building a task like this <br/>
     * The initial steps are forced to be acomplished in this way.
     */
    class SA_IS {

    public:
        SA_IS(const unsigned char *src, size_t length, size_t alphabet_sz, int mem_sz, int level) :
                src(src), length(length), alphabet_sz(alphabet_sz),
                mem_sz(mem_sz), level(level) {
            t = new unsigned char[(length + 1) / sizeof(char) + 1];
            build_t_array();
            buckets = build_buckets();
        }

        class empty_str {
        };

    private:
        const unsigned char *src;
        unsigned char *t;
        size_t length, alphabet_sz;
        int mem_sz, level;
        const size_t * buckets;

        size_t * build_buckets() {
            size_t * bkt = new size_t[alphabet_sz + 1];
            for (size_t i = 0; i <= length; ++i) {
                bkt[hash_char(chr(src, i, mem_sz))]++;
            }
            return bkt;
        }

        void build_t_array() {
            if (length > 0) {
                tset(t, length, S);
                for (long i = length - 1; i >= 0; --i) {
//                    if (chr(src, i, mem_sz) > chr(src, i + 1, mem_sz)) tset(t, i, S);
//                    else if (chr(src, i, mem_sz) < chr(src, i + 1, mem_sz)) tset(t, i, L);
//                    else tset(t, i, tget(t, i + 1));

                    //!< The lines below are faster but not portable when we reverse the value of L and S
                    if (chr(src, i, mem_sz) == chr(src, i + 1, mem_sz))
                        tset(t, i, tget(t, i + 1));
                    else
                        tset(t, i, chr(src, i, mem_sz) > chr(src, i + 1, mem_sz));
                }
            } else {
                throw new empty_str;
            }
        }
    };

    /**
     * Calculate the suffix array of \b src into \b sa <br/>
     * with the length of the string \b is length, <br/>
     * the size of the alphabet in \b src is \b alphabet_sz, <br/>
     * the size of each member in the string \b src is \b mem_sz, <br/>
     * the recursion level of the current process is \b level <br/>
     *
     * \Note
     * The string src is ending with '\0', do not append other symbol to end it.
     *
     * @param src The C-style string to be calculated
     * @param sa The suffix array to store the results
     * @param length The length of the C-style string (the ending '\0' does not count as one character)
     * @param alphabet_sz The size of the alphabet (the tailer does not count)
     * @param mem_sz The size of each member in the array (in order to distinguish with recursion call for
     * \b S_1 because \b S_1 is a integer array instead of char array.
     * @param level Recursion level used as a indicator.
     */
    void SAIS(const unsigned char *src, size_t *sa, size_t length, size_t alphabet_sz, int mem_sz, int level) {
        if (length == 0) return; //!< return when empty string

        SA_IS is = SA_IS(src, length, alphabet_sz, mem_sz, level);
    }
}
