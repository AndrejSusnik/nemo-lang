ir_source = ['ir.cpp']
ir_include = include_directories('include')
irlib = shared_library('irlib',
            ir_source,
            include_directories : [ir_include, mpc_include],
            link_with : [mpclib],
            install : true)