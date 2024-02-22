#include "grammar/grammar.h"
#include "mpc/mpc.h"
#include <stdio.h>

mpc_parser_t *Identifier;
mpc_parser_t *Integer;
mpc_parser_t *Char;
mpc_parser_t *String;
mpc_parser_t *Collection;
mpc_parser_t *Lambda;
mpc_parser_t *Operator;
mpc_parser_t *TypeDefinition;
mpc_parser_t *Statement;
mpc_parser_t *Expression;
mpc_parser_t *Pipeline;
mpc_parser_t *Assignment;
mpc_parser_t *Comment;
mpc_parser_t *Nemo;

void create_parsers(void) {
  Identifier = mpc_new("ident");
  Integer = mpc_new("number");
  Char = mpc_new("character");
  String = mpc_new("str");
  Collection = mpc_new("collection");
  Lambda = mpc_new("lambda");
  Operator = mpc_new("operator");
  TypeDefinition = mpc_new("type_definition");
  Statement = mpc_new("statement");
  Expression = mpc_new("expression");
  Pipeline = mpc_new("pipeline");
  Assignment = mpc_new("assignment");
  Comment = mpc_new("comment");
  Nemo = mpc_new("nemo");
}

void define_grammar(void) {
  FILE *grammar =
      fopen("/home/x/projects/nemo-lang/src/grammar/grammar.mpc", "r");
  if (grammar == NULL) {
    fprintf(stderr, "Could not find grammar file\n");
    exit(1);
  }

  mpc_err_t *error =
      mpca_lang_file(MPCA_LANG_DEFAULT, grammar, Identifier, Integer, Char,
                     String, Collection, Lambda, Operator, TypeDefinition, Statement,
                     Expression, Pipeline, Assignment, Comment, Nemo, NULL);

  if (error != NULL) {
    mpc_err_print(error);
    mpc_err_delete(error);
    exit(1);
  }

  fclose(grammar);
}

void cleanup_parsers(void) {
  mpc_cleanup(14, Identifier, Integer, Char, String, Collection, Lambda,
              Statement, Operator, TypeDefinition, Expression, Pipeline, Comment, Assignment,
              Nemo);
}
