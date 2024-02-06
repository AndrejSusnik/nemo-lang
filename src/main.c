#include "mpc/mpc.h"

#include <editline/readline.h>
#include <editline/history.h>

#include "grammar/grammar.h"
#include "verinfo.h"

int main(int argc, char** argv) {
  create_parsers();
  define_grammar();

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      mpc_result_t r;
      if (mpc_parse_contents(argv[i], Nemo, &r)) {
        mpc_ast_print(r.output);
        mpc_ast_delete(r.output);
      } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
      }
    }
    cleanup_parsers();
    return 0;
  }

  printf("Welcome to nemo shell version %d.%d.%d\n", major_version, minor_version, patch_version);

  while (1) {
  
    char* input = readline("nemo> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Nemo, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
  }
  
  cleanup_parsers(); 
  return 0;
}
