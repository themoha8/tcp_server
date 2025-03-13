#ifndef SLICE_H
#define SLICE_H

/*struct tm ;*/

uint64 put_c_string_in_slice(slice s, char *c_str);
uint64 put_string_in_slice(slice sl, string s);
uint64 put_int_in_slice(slice s, int x);
/*uint64 SlicePutTm(slice, struct tm);
uint64 SlicePutTmRFC822(slice, struct tm);*/

#endif
