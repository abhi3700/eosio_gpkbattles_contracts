cmake_minimum_required(VERSION 3.10)
project(gpkbattlesco)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( gpkbattlesco gpkbattlesco gpkbattlesco.cpp )
target_include_directories( gpkbattlesco PUBLIC ${CMAKE_SOURCE_DIR}/ )



