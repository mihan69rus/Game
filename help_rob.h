#ifndef SENTRY2
#define SENTRY2

#include "structs_const.h"

struct player *push(struct player *p);
struct player *delete_list(struct player *p);
void write_list(struct player *p);
void write_me(struct player *p);
void write_market(struct information *p);

#endif
