#ifndef NEMO_GRAMMAR
#define NEMO_GRAMMAR

#include "mpc.h"

extern mpc_parser_t* Identifier;
extern mpc_parser_t* Integer;
extern mpc_parser_t* Char;
extern mpc_parser_t* String;
extern mpc_parser_t* Lambda;
extern mpc_parser_t* Statement;
extern mpc_parser_t* Expression;
extern mpc_parser_t* Assignment;
extern mpc_parser_t* Nemo;

void create_parsers(void);
void define_grammar(void);
void cleanup_parsers(void);


#endif // !NEMO_GRAMMAR
