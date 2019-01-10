#ifndef SENTRY3
#define SENTRY3

#include "structs_const.h"

int check_space(int ch);
int check_arith(int ch);
int check_letter(int ch);
int check_num(int ch);
int check_special_sym(int ch);
void write_lexemes(lexeme *p);
void delete_lexemes(lexeme *p);
int check_list(lexeme *p);

#endif
