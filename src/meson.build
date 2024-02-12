subdir('mpc')
subdir('grammar')

main_sources = ['main.cpp']

readline = dependency('libedit')

executable('nemo', main_sources, dependencies: [readline], link_with: [grammarlib, mpclib], include_directories: [grammar_include, mpc_include])