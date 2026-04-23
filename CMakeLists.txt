cmake_minimum_required(VERSION 3.21)

set(APP_NAME "Cous Patcher")
set(APP_EXECUTABLE "cous-patcher")
set(APP_VERSION "1.0")

set(SLINT_STYLE "fluent" CACHE STRING "Slint Widget Style")

project(${APP_EXECUTABLE} CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# Slint UI framework
include(FetchContent)

FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    GIT_TAG        release/1
    SOURCE_SUBDIR  api/cpp
)
FetchContent_MakeAvailable(Slint)

# C backend
add_library(creator-backend STATIC
    src/bundle.c
    src/plist.c
)

# App
add_executable(${APP_EXECUTABLE} MACOSX_BUNDLE
    src/main.cpp
    src/file_picker.mm
)

# Bundle metadata
set_target_properties(${APP_EXECUTABLE} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "${APP_NAME}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.cousdev.cous-patcher"
    MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION}"
)

# Compile settings
slint_target_sources(${APP_EXECUTABLE} ui/app.slint)
target_link_libraries(${APP_EXECUTABLE} PRIVATE
    Slint::Slint
    creator-backend
    "-framework Cocoa"
)
target_include_directories(creator-backend PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${APP_EXECUTABLE} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_source_files_properties(src/file_picker.mm
    PROPERTIES COMPILE_FLAGS "-x objective-c++"
)

# Install app bundle
install(TARGETS ${APP_EXECUTABLE}
    BUNDLE DESTINATION .
)

# Fix macOS bundle dependencies
install(CODE "
    include(BundleUtilities)

    # Construct full app path
    set(app \"\${CMAKE_INSTALL_PREFIX}/${APP_EXECUTABLE}.app\")

    # Force absolute path (this is the critical fix)
    get_filename_component(app \"\${app}\" ABSOLUTE)

    message(STATUS \"Fixing bundle: \${app}\")

    fixup_bundle(
        \"\${app}\"
        \"\"
        \"\${CMAKE_INSTALL_PREFIX}/lib\"
    )
")