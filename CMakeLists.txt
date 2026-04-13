cmake_minimum_required(VERSION 3.16)

# ------------------------------------------------------------
# Project setup
# ------------------------------------------------------------
project(SmartQuestionPaperGenerator LANGUAGES CXX)

# Export compile commands for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------
# Qt setup
# ------------------------------------------------------------
# Tell CMake exactly where Qt 6.10.0 with MinGW is installed
# (note the extra /Qt6 at the end)
set(Qt6_DIR "C:/Qt/6.10.0/mingw_64/lib/cmake/Qt6")

# Optional: explicitly set compiler (to avoid mismatched MinGW)
set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1310_64/bin/g++.exe")

# Find Qt Widgets
find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)


# Enable automatic MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ------------------------------------------------------------
# Source and UI files
# ------------------------------------------------------------
set(SOURCES
    main.cpp
    MainWindow.cpp
    AddQuestionWindow.cpp
    GeneratePaperWindow.cpp
    questionbank.cpp
    papergenerator.cpp
)

set(HEADERS
    MainWindow.h
    AddQuestionWindow.h
    GeneratePaperWindow.h
    questionbank.h
    papergenerator.h
)

set(UIS
    MainWindow.ui
    AddQuestionWindow.ui
    GeneratePaperWindow.ui
)

# ------------------------------------------------------------
# Build the executable
# ------------------------------------------------------------
qt_add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UIS}
)

# ------------------------------------------------------------
# Link with Qt Widgets
# ------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::PrintSupport)


# ------------------------------------------------------------
# Fix IntelliSense include paths for Qt headers
# ------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
)


# ------------------------------------------------------------
# Set output directory for build artifacts
# ------------------------------------------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)