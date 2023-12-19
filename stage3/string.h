#ifndef STRING_H
#define STRING_H

#include "def.h"

// compares two strings by length and ASCII values. return value:
//   < 0 if s1 < s2
//   = 0 if s1 = s2
//   > 0 if s1 > s2
isize str_cmp(str s1, str s2);

// returns index of first of occurrence in s of any of the chars in tokens
// returns length of s if not found
usize str_find(str s, str tokens);

// parses a number in base base and returns number of chars processed
// resulting number is stored in *x
usize str_parse_num(str s, u8 base, u64 *x);

// this is a splitting function
// returns the next non-empty substring of *s that is delimited by the tokens in sep
// the returned string does not contain any of the separators
// returns an emtpy string when end is reached
// advances s to after the substring plus first delimiting token
str str_walk(str *s, str sep);

// advances the string while its first token matches any of the chars in tokens
// this can be used to consume whitespace, for example
str str_eat(str s, str tokens);

// advances the string s by x chars, increasing the data pointer and decreasing the length
// note: this is not bounds checked
str str_advance(str s, usize x);

// returns true if s starts with start
bool str_start(str s, str start);

// construct a str from a \0 terminated string at runtime
// avoid this for literals: use the S macro from def.h instead without a runtime cost
str str_intro(char *c);

// copy a string to the heap
str str_clone(str s);

#endif
