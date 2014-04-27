#include "lmobject.h"
#ifndef LM_EVAL_H
#define LM_EVAL_H

lmobject*  lm_eval(lmstate *lmsgi, char *input);

#define            lm_assert_exists(Var,Name) do{                                         \
                       if(Var == NULL ){                                                  \
                           fprintf(stderr, "error: <unbound symbol '%s'>\n",Name);\
                           return NULL;                                                   \
                       }}while(0)


#define            lm_assert_not_null(Var) do{                                  \
                       if(Var == NULL ){                                        \
                           fprintf(stderr,                                      \
                                  "error: <null pointer on line %d in %s>\n",   \
                                  __LINE__, __FILE__);\                         \
                           return NULL;                                         \
                       }}while(0)


#define            lm_bad_syntax(Line) do{                                     \
                        fprintf(stderr, "error: <bad syntax at '%s'>\n", Line);\
                        return NULL;                                           \
                   }while(0)


#define            lm_bad_msg(Msg) do{                                         \
                        fprintf(stderr, "error: <bad message at '%s'>\n", Msg);\
                        return NULL;                                           \
                   }while(0)

#endif
