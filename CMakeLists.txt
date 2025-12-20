cmake_minimum_required(VERSION 3.31)
project(DNS_Client C)

set(CMAKE_C_STANDARD 17)

add_executable(DNS_Client main.c
        Network/dns.c
        Network/dns.h
        Network/NetworkManager.c
        Network/NetworkManager.h
        Utils/Serializor.c
        Utils/Serializor.h
)

target_link_libraries(DNS_Client ws2_32)