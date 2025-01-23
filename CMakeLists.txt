cmake_minimum_required(VERSION 3.15)
project(MySystemCProject LANGUAGES CXX)

# C++17 Standard setzen
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SystemC finden
#set(CMAKE_MODULE_PATH /home/tinnwongsirikul/systemc-2.3.3/cmake ${CMAKE_MODULE_PATH})
#find_package(SystemC REQUIRED)
set(SYSTEMC_HOME /home/tinnwongsirikul/systemc-2.3.3)
#set(CMAKE_MODULE_PATH /home/tinnwongsirikul/systemc-2.3.3/rebuild ${CMAKE_MODULE_PATH})

include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib-linux64)


# Bibliothek erstellen
add_library(MySystemCProjectLib
    src/KGC.cpp
    src/NA.cpp
    src/TopLevel.cpp
)

# Bibliothek konfigurieren
target_include_directories(MySystemCProjectLib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

include_directories(/home/tinnwongsirikul/systemc-2.3.3/include)
link_directories(/home/tinnwongsirikul/systemc-2.3.3/lib-linux64)
target_link_libraries(MySystemCProjectLib
    PUBLIC
        #SystemC::systemc
        ${SYSTEMC_HOME}/lib-linux64/libsystemc.so
)

# Executable erstellen
add_executable(MySystemCProject src/main.cpp)

# Executable konfigurieren
target_include_directories(MySystemCProject
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MySystemCProject
    PUBLIC
        MySystemCProjectLib
)