#include "lmobject.h"
#ifndef LMSGI_H
#define LMSGI_H

void       print_usage(lmsgi_state *lmsgi);
void       lm_init(lmsgi_state *lmsgi, int argc, char **argv);
void       lm_terminate(lmsgi_state *lmsgi, int cleanht);
lmobject*  lm_new_procvar(lmsgi_state *lmsgi, char *Name);
lmobject*  lm_new_funvar(lmsgi_state *lmsgi, char *Name, char **Instructions);
lmobject*  lm_get_var(lmsgi_state *lmsgi, char *name);
void       connect_to_pig(lmsgi_state *lmsgi);

int        msg_pig(lmsgi_state *lmsgi, const char *msg);
void       register_node(const char *msg, char *buffer);
void       message_node(const char *msg, char *buffer);
void       hey_node(const char *msg, char *buffer);

#endif
