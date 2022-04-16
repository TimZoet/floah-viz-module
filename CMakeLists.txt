find_package(math REQUIRED)
find_package(sol REQUIRED COMPONENTS core luna)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../floah-layout")
include(floahVersionString)

set(NAME floah-viz)
set(TYPE module)
set(INCLUDE_DIR "include/floah-viz")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/vertex.h

    ${INCLUDE_DIR}/generators/circle_generator.h
    ${INCLUDE_DIR}/generators/generator.h
    ${INCLUDE_DIR}/generators/rectangle_generator.h
)

set(SOURCES
    ${SRC_DIR}/generators/circle_generator.cpp
    ${SRC_DIR}/generators/rectangle_generator.cpp
)

set(DEPS_PUBLIC
    floah-layout
    math::math
    sol::core
    sol::luna
)

set(DEPS_PRIVATE

)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${FLOAH_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${FLOAH_VERSION}
    HEADERS "${HEADERS}"
)

target_compile_definitions(
    ${NAME}
    PRIVATE
        FLOAH_VERSION_MAJOR=${FLOAH_VERSION_MAJOR}
        FLOAH_VERSION_MINOR=${FLOAH_VERSION_MINOR}
        FLOAH_VERSION_PATCH=${FLOAH_VERSION_PATCH}
)
