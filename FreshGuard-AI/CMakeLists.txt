cmake_minimum_required(VERSION 3.10)
project(FreshGuardAI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Reference C++ backend build. Requires a C++17 compiler, Crow, SQLite3 dev headers,
# and OpenCV (optional). Not needed to run the project -- use backend/server.py (Python).

find_package(Threads REQUIRED)

add_executable(freshguard_server
    backend_cpp/main.cpp
    backend_cpp/User.cpp
    backend_cpp/Database.cpp
    backend_cpp/ImageProcessor.cpp
    backend_cpp/QualityDetector.cpp
    backend_cpp/Inspection.cpp
    backend_cpp/Report.cpp
    backend_cpp/AuthService.cpp
    backend_cpp/InspectionManager.cpp
    backend_cpp/Dashboard.cpp
    backend_cpp/ReportGenerator.cpp
    backend_cpp/ContactService.cpp
)

target_include_directories(freshguard_server PRIVATE backend_cpp)
target_link_libraries(freshguard_server PRIVATE Threads::Threads)

# Uncomment and adjust when Crow + SQLite are installed:
# target_link_libraries(freshguard_server PRIVATE crow sqlite3)
# if OpenCV is used:
# find_package(OpenCV REQUIRED)
# target_link_libraries(freshguard_server PRIVATE OpenCV::opencv_core OpenCV::opencv_imgproc OpenCV::opencv_imgcodecs)
