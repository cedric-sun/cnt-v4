cmake_minimum_required(VERSION 3.17)
project(cnt5106_v4)

set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_compile_definitions(PANIC_BACKTRACE_FRAME_N=20)
add_compile_definitions(ACCEPT_QUEUE_SIZE=20)

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

target_link_libraries(peerProcess PRIVATE Threads::Threads)
