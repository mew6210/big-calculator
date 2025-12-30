cmake_minimum_required(VERSION 3.14)
project(Big_calc)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES 
 "src/main.cpp"
 "src/lexer/lexer.cpp"
 "src/bigint/bigint.cpp" 
 "src/parser/parser.cpp"
 "src/core/core.cpp"
 "src/bigint/operations/comparsions.cpp" 
 "src/bigint/operations/add.cpp" 
 "src/bigint/operations/multiply.cpp" 
 "src/bigint/helpers.cpp" 
 "src/bigint/operations/subtract.cpp"
 "src/astnodes/nodeprints.cpp"
 "src/logging/logging.cpp" 
 "src/bigint/operations/divide.cpp" 
  
 "src/eval/eval.cpp"
 "src/astnodes/astoper.cpp" "src/eval/evalException.hpp"  "src/eval/evalctx/evalctx.cpp" "src/eval/stl/stl.cpp" )

add_executable(${PROJECT_NAME} ${SOURCES})
