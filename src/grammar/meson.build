grammar_source = ['grammar.cpp']
grammar_include = include_directories('include')
install_data('grammar.mpc', install_dir : get_option('datadir') / 'nemo')
message(get_option('datadir') / 'nemo')
grammarlib = shared_library('grammarlib',
            grammar_source,
            include_directories : [grammar_include, mpc_include],
            link_with : [mpclib],
            install : true)