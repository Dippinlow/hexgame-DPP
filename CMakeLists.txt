	# Minimum CMake version required, we'll just use the latest version.
    cmake_minimum_required(VERSION 3.25)
    # Project name, version and description
    project(hexgame-dpp VERSION 1.0 DESCRIPTION "A hex bot")
     
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
     
    # Create an executable
    add_executable(${PROJECT_NAME}
        src/main.cpp
        src/graphicsHandler.cpp
        src/gameManager.cpp
        src/game.cpp
        src/dataHandler.cpp
        src/commands.cpp
    )
     
    # Find our pre-installed DPP package (using FindDPP.cmake).
    find_package(DPP REQUIRED)
    find_package(nlohmann_json REQUIRED)

    # Link the pre-installed DPP package.
    target_link_libraries(${PROJECT_NAME} 
        ${DPP_LIBRARIES}
        nlohmann_json::nlohmann_json
        png
    )
     
    # Include the DPP directories.
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${DPP_INCLUDE_DIR}
    )
     
    # Set C++ version
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )

    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)