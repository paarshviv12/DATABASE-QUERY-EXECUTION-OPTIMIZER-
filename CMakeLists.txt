cmake_minimum_required(VERSION 3.22)
project(data_optimiser_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Pull in FTXUI via FetchContent -----------------------------------------
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG        v5.0.0
)
FetchContent_MakeAvailable(ftxui)

# --- Homepage executable -----------------------------------------------------
find_package(Threads REQUIRED)

add_executable(homepage src/main.cpp)
target_link_libraries(homepage
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE Threads::Threads
)
