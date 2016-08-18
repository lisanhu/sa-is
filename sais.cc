//
// Created by lsh on 16-8-15.
//

#include "sais.h"

void sa_is(const char *src, SA::size_t *sa, SA::size_t length, SA::size_t alphabet_sz, int mem_sz) {
    SA::SAIS(reinterpret_cast<const unsigned char *>(src), sa, length, alphabet_sz, mem_sz, 0);
}

namespace SA {
    const unsigned char mask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    const bool L = false, S = true;
    typedef long size_t;
    const size_t EMPTY = -1;

    /**
     * Set the type of the char at \b pos to the value of \b v
     * \Note This function will use one bit for the type of one character
     * @param t The type array
     * @param pos The position of the type element to be set
     * @param v The value to set, L- type to be false and S- type to be true
     */
    inline void tset(unsigned char *t, size_t pos, bool v) {
        size_t idx = pos / (sizeof(char) * 8);
        size_t offset = pos % (sizeof(char) * 8);
        t[idx] = v ? (t[idx] | mask[offset]) : ((~mask[offset]) & t[idx]);
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
        size_t idx = pos / (sizeof(char) * 8);
        size_t offset = pos % (sizeof(char) * 8);
        return (t[idx] & mask[offset]) != 0;
    }

    /**
     * Hash function to map the current char to an index in the buckets,
     * we can easily change this when numeric is not the best solution to identify index
     * @param c The character to be hashed
     * @return The index in the bucket of this char
     */
    inline unsigned hash_char(unsigned c) {
        switch (c) {
            case 0:
                return 0;
            case 'i':
                return 1;
            case 'm':
                return 2;
            case 'p':
                return 3;
            case 's':
                return 4;
            default:
                return 5;
        }
    }

    inline const size_t chr(const unsigned char *src, size_t pos, int mem_sz) {
        if (mem_sz == sizeof(char)) return hash_char(src[pos]);
        else return reinterpret_cast<const size_t *>(src)[pos];
    }

    inline bool isLMS(const unsigned char *src, unsigned char *t, size_t pos) {
//        return pos > 0 && tget(t, pos) == S && tget(t, pos - 1) == L;
        return pos > 0 && tget(t, pos) && !tget(t, pos - 1);
    }

    void getBuckets(const unsigned char *s, size_t *bkt, size_t n, size_t K, int cs, bool end) {
        size_t i, sum = 0;
        for (i = 0; i <= K; ++i) bkt[i] = 0; // clear all buckets
        for (i = 0; i < n; ++i)
            bkt[chr(s, i, cs)]++; // compute the size of each bucket
        for (i = 0; i <= K; ++i) {
            sum += bkt[i];
            bkt[i] = end ? sum - 1 : sum - bkt[i];
        }
    }

    class empty_str {
    };

    void induceSAl(unsigned char *t, size_t *SA, const unsigned char *s, size_t *bkt,
                   size_t n, size_t K, int cs, int level) {
        size_t i, j;
        getBuckets(s, bkt, n, K, cs, false); // find heads of buckets
        if (level == 0) bkt[0]++;
        for (i = 0; i < n; i++)
            if (SA[i] != EMPTY) {
                j = SA[i] - 1;
                if (j >= 0 && !tget(t, j))
                    SA[bkt[chr(s, j, cs)]++] = j;
            }
    }

    void induceSAs(unsigned char *t, size_t *SA, const unsigned char *s, size_t *bkt,
                   size_t n, size_t K, int cs) {
        size_t i, j;
        getBuckets(s, bkt, n, K, cs, true); // find ends of buckets
        for (i = n - 1; i >= 0; i--)
            if (SA[i] != EMPTY) {
                j = SA[i] - 1;
                if (j >= 0 && tget(t, j)) SA[bkt[chr(s, j, cs)]--] = j;
            }
    }

    inline void build_t_array(const unsigned char *src, unsigned char *t, size_t length, int mem_sz) {
        if (length > 0) {
            tset(t, length - 1, S);
            for (long i = length - 2; i >= 0; --i) {
//                    if (chr(src, i, mem_sz) > chr(src, i + 1, mem_sz)) tset(t, i, S);
//                    else if (chr(src, i, mem_sz) < chr(src, i + 1, mem_sz)) tset(t, i, L);
//                    else tset(t, i, tget(t, i + 1));

                //!< The lines below are faster but not portable when we reverse the value of L and S
                if (chr(src, i, mem_sz) == chr(src, i + 1, mem_sz))
                    tset(t, i, tget(t, i + 1));
                else
                    tset(t, i, chr(src, i, mem_sz) < chr(src, i + 1, mem_sz));
            }
        } else {
            throw new empty_str;
        }
    }

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

        size_t *bkt = new size_t[alphabet_sz + 1];
        unsigned char *t = new unsigned char[(length + 1) / sizeof(char) + 1];
        build_t_array(src, t, length, mem_sz);

        getBuckets(src, bkt, length, alphabet_sz, mem_sz, true);
        for (size_t i = 0; i < length; ++i) sa[i] = EMPTY;
        for (size_t i = length - 2; i >= 0; --i)
            if (isLMS(src, t, i)) sa[bkt[chr(src, i, mem_sz)]--] = i;
        sa[0] = length - 1; // set the single sentinel LMS-substring


//        print_array(sa, length);
        induceSAl(t, sa, src, bkt, length, alphabet_sz, mem_sz, level);
//        print_array(sa, length);
        induceSAs(t, sa, src, bkt, length, alphabet_sz, mem_sz);
        delete[] bkt;


        size_t n1 = 0; ///< The length of S1
        for (size_t i = 0; i < length; i++) {
            ///< Because n1 and i are initialized by 0 and i is increased
            ///< on every iteration while n1 will not, so i >= n1, there
            ///< will be no data dependency problem here
            if (isLMS(src, t, sa[i]))
                sa[n1++] = sa[i];
        }

        // Init the name array buffer
        for (size_t i = n1; i < length; ++i) sa[i] = EMPTY;
        // find the lexicographic names of all substrings
        size_t name = 0, prev = -1;
        for (size_t i = 0; i < n1; i++) {
            size_t pos = sa[i];
            bool diff = false;
            for (size_t d = 0; d < length; d++)
                if (prev == -1 || pos + d == length - 1 || prev + d == length - 1 ||
                    chr(src, pos + d, mem_sz) != chr(src, prev + d, mem_sz) ||
                    tget(t, pos + d) != tget(t, prev + d)) {
                    diff = true;
                    break;
                }
                else if (d > 0 && (isLMS(src, t, pos + d) || isLMS(src, t, prev + d)))
                    break;

            if (diff) {
                name++;
                prev = pos;
            }
            pos = pos / 2; //(pos%2==0)?pos/2:(pos-1)/2;
            sa[n1 + pos] = name - 1;
        }
        for (size_t i = length - 1, j = length - 1; i >= n1; --i)
            if (sa[i] != EMPTY) sa[j--] = sa[i];

        // s1 is done now
        size_t *SA1 = sa, *s1 = sa + length - n1;
        if (name < n1) {
            SAIS((unsigned char *) s1, SA1, n1, name, sizeof(int), level + 1);
        } else { // generate the suffix array of s1 directly
            for (size_t i = 0; i < n1; ++i) SA1[s1[i]] = i;
        }



        bkt = new size_t[alphabet_sz + 1]; // bucket counters

        // put all left-most S characters into their buckets
        getBuckets(src, bkt, length, alphabet_sz, mem_sz, true); // find ends of buckets
        size_t j = 0;
        for(size_t i=1; i< length; ++i)
            if(isLMS(src, t, i)) s1[j++]=i; // get p1
        for(size_t i=0; i<n1; i++) SA1[i]=s1[SA1[i]]; // get index in s1
        for(size_t i=n1; i<length; i++) sa[i]=EMPTY; // init SA[n1..n-1]
        for(size_t i=n1-1; i>=0; i--) {
            j=sa[i]; sa[i]=EMPTY;
            if(level==0 && i==0)
                sa[0]=length-1;
            else
                sa[bkt[chr(src, j, mem_sz)]--]=j;
        }

        induceSAl(t, sa, src, bkt, length, alphabet_sz, mem_sz, level);
        induceSAs(t, sa, src, bkt, length, alphabet_sz, mem_sz);

        delete[] bkt;
        delete[] t;
    }
}
