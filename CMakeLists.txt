cmake_minimum_required(VERSION 3.14)
project(Big_calc)

set(SOURCES 
"src/main.cpp"
"src/lexer/lexer.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
