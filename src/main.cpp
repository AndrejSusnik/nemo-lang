#include <editline/history.h>
#include <editline/readline.h>
#include <memory>

#include "grammar/grammar.h"
#include "interpreter/interpreter.h"
#include "ir/ir.h"
#include "mpc/mpc.h"
#include "nemo/common.hpp"
#include "nemo/verinfo.h"

int main(int argc, char **argv) {
  create_parsers();
  define_grammar();

  std::shared_ptr<ScopeContext> globalContext =
      std::make_shared<ScopeContext>();

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      mpc_result_t r;
      if (mpc_parse_contents(argv[i], Nemo, &r)) {
        const auto success =
            evaluate(static_cast<const mpc_ast_t *>(r.output), globalContext);
        if (success) {
        } else {
          std::cout << "Evaluating failed" << std::endl;
        }
        mpc_ast_delete(static_cast<mpc_ast_t *>(r.output));
      } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
      }
    }
    cleanup_parsers();
    return 0;
  }

  printf("Welcome to nemo shell version %d.%d.%d\n", major_version,
         minor_version, patch_version);

  while (1) {

    char *input = readline("nemo> ");
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Nemo, &r)) {
      const auto success =
          evaluate(static_cast<const mpc_ast_t *>(r.output), globalContext);
      if (success) {
      } else {
        std::cout << "Evaluating failed" << std::endl;
      }
      mpc_ast_delete(static_cast<mpc_ast_t *>(r.output));
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  cleanup_parsers();
  return 0;
}
