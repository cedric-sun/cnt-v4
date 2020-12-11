cmake_minimum_required(VERSION 3.17)
project(cnt5106_v4)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_compile_definitions(PANIC_BACKTRACE_FRAME_N=20)
add_compile_definitions(ACCEPT_QUEUE_SIZE=20)
add_compile_definitions(BUFFERED_READER_BUFSZ=65536) #64 KiB
add_compile_definitions(BUFFERED_WRITER_BUFSZ=65536) #64 KiB
add_compile_definitions(PIECE_CACHE_SIZE=16777216) # 16 MiB

set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -ggdb")
set(CMAKE_LINKER_FLAGS_DEBUG
        "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# choose the config suite to use
set(CNT_CONFIG_SUITE "testconfig-20MiB-3peers")

set(CNT_PROJECT_DIR "cnt_project")

add_custom_target(cnt_clean
        COMMAND ${CMAKE_COMMAND} -E rm -r ${CNT_PROJECT_DIR})

add_custom_target(cnt_project_copy_config
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/${CNT_CONFIG_SUITE} ${CNT_PROJECT_DIR})

add_dependencies(cnt_project_copy_config cnt_clean)

FILE(COPY script/demo.sh DESTINATION .)
add_executable(peerProcess
        piecebitfield/PieceBitfield.cpp
        piecebitfield/AbstractPieceBitfield.cpp
        Logger.cpp
        Config.cpp
        main.cpp
        io/BufferedReader.cpp
        utils/MathUtils.cpp
        utils/err_utils.cpp
        tcp/Connection.cpp
        tcp/Acceptor.cpp
        msg/ActualMsg.cpp
        msg/HandshakeMsg.cpp
        storage/PieceRepository.cpp
        session/AsyncMsgScanner.cpp
        session/SessionCollection.cpp
        session/Session.cpp
        )

set_target_properties(peerProcess PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CNT_PROJECT_DIR})

add_dependencies(peerProcess cnt_project_copy_config)

include(FindOpenSSL)
target_link_libraries(peerProcess PRIVATE Threads::Threads OpenSSL::SSL)
