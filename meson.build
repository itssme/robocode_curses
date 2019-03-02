# uncomment as necessary
project('robocode', 'cpp',
        license : ['Boost'],
        version : '0.1',
        default_options : ['cpp_std=c++17', 'warning_level=3', 'werror=false'])

thread_dep = dependency('threads')

# # begin asio: https://think-async.com/Asio/
# add_global_arguments('-I' + get_option('asio_include_dir'), language : 'cpp')
# add_global_arguments('-DASIO_STANDALONE', language : 'cpp')
# add_global_arguments('-DASIO_NO_DEPRECATED', language : 'cpp')
# # end asio

# add_global_arguments('-I' + get_option('clipp_include_dir'), language : 'cpp')

# add_global_arguments('-I' + get_option('cli11_include_dir'), language : 'cpp')

add_global_arguments('-I' + get_option('spdlog_include_dir'), language : 'cpp')

# add_global_arguments('-I' + get_option('json_include_dir'), language : 'cpp')

# begin protobuf: https://developers.google.com/protocol-buffers/
# protobuf: must be installed independently and has to be found...
protoc = find_program('protoc', required : true)
grpc_cpp_plugin = find_program('grpc_cpp_plugin', required : true)
protobuf_dep = dependency('protobuf', required : true)
grpc_dep = dependency('grpc', required : true)
grpcpp_dep = dependency('grpc++', required : true)

gen = generator(protoc, \
  output    : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h', '@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h'],
  arguments : ['--proto_path=@SOURCE_DIR@/src/proto',
                '--plugin=protoc-gen-grpc=' + grpc_cpp_plugin.path(),
               '--grpc_out=@BUILD_DIR@',
               '--cpp_out=@BUILD_DIR@',
               '../src/proto/@PLAINNAME@'])

internal_proto = gen.process('src/proto/messages.proto')

inc_dir = include_directories('include')
game = ['src/main.cpp', 'src/curses_drawable_objects.cpp', 'src/communication/client.cpp', 'src/communication/server.cpp', 'src/game_objects/bullet.cpp', 'src/game_objects/game.cpp', 'src/game_objects/robot.cpp', internal_proto]

executable('robocode',
           sources : game,
           link_args: '-lncurses',
           include_directories : inc_dir,
           dependencies : [thread_dep, protobuf_dep, grpc_dep, grpcpp_dep]
          )