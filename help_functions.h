#ifndef NEWMOD_H_SENTRY
#define NEWMOD_H_SENTRY

#include "structs_const.h"

int length(const char *s);
int cmp_s(const char *s1,const  char *s2);
int count_users(gamers *p);
char *num_in_str(int num);
int str_in_num(const char *s);
char *take_str(char *s, int count);
int check_for_work(char *s, int *count);
char *cut_str(char *s, int *count);
int words_in_str(char *s);

#endif
