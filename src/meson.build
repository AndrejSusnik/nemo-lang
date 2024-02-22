nemo_include = include_directories('include')
subdir('mpc')
subdir('grammar')
subdir('interpreter')
subdir('ir')

main_sources = ['main.cpp']

readline = dependency('libedit')

executable('nemo', main_sources, dependencies: [readline], link_with: [grammarlib, mpclib, interpreterlib, irlib], include_directories: [grammar_include, mpc_include, interpreter_include, nemo_include, ir_include])