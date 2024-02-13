nemo_include = include_directories('include')
subdir('mpc')
subdir('grammar')
subdir('interpreter')

main_sources = ['main.cpp']

readline = dependency('libedit')

executable('nemo', main_sources, dependencies: [readline], link_with: [grammarlib, mpclib, interpreterlib], include_directories: [grammar_include, mpc_include, interpreter_include, nemo_include])