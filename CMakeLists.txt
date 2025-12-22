cmake_minimum_required(VERSION 3.16)

project(ISPortfolio_Desktop VERSION 1.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport) # <-- PrintSupport

set(SOURCES
    main.cpp
    Database.cpp Database.h
    Security.cpp Security.h
    LoginWindow.h
    SpecialistWindow.h
    EmployerWindow.h
)

add_executable(ISPortfolio_Desktop ${SOURCES})

target_link_libraries(ISPortfolio_Desktop PRIVATE Qt6::Widgets Qt6::Sql Qt6::PrintSupport) # <-- Здесь тоже

if(WIN32)
    set_target_properties(ISPortfolio_Desktop PROPERTIES WIN32_EXECUTABLE ON)
endif()