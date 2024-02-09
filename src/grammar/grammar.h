#ifndef NEMO_GRAMMAR
#define NEMO_GRAMMAR

#include "mpc.h"

extern mpc_parser_t* Nemo;

void create_parsers(void);
void define_grammar(void);
void cleanup_parsers(void);


#endif // !NEMO_GRAMMAR
