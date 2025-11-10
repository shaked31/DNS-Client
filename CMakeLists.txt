cmake_minimum_required(VERSION 3.31)
project(DNS_Client C)

set(CMAKE_C_STANDARD 17)

add_executable(DNS_Client main.c
        Network/dns.c
        Network/dns.h
        Network/NetworkManager.cpp
        Network/NetworkManager.h)
