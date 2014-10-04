#ifndef PASSWD_H
#define PASSWD_H
#include <stdbool.h>
#define MAX_PASSWD_LEN 32
#define HASH_STR_LEN 128

enum passwd_event {
  NOTHING, INSERT, CLEAR, OVERFLOW
};
// get passwd event
enum passwd_event passwd_get_event(void);
// unset update flag 
void passwd_clear_event(void);
// return passwd len
int passwd_len(void);
// store passwd's hash str to hashstr
void passwd_get_hashstr(char *hashstr);
// insert a char into passwd
int passwd_insert(char c);
// clean passwd content
int passwd_clear(void);
#endif
