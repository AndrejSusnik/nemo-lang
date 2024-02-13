interpreter_source = ['interpreter.cpp']
interpreter_include = include_directories('include')
interpreterlib = shared_library('interpreterlib',
            interpreter_source,
            include_directories : [interpreter_include, mpc_include, nemo_include],
            link_with: [mpclib],
            install : true)