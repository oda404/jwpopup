
cmake_minimum_required(VERSION 3.20)

project(jwpopup C)

set(INCLUDE_DIR include/)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror=incompatible-pointer-types")

# libjwpopup
find_package(X11 REQUIRED)
find_package(Freetype REQUIRED)

set(LIB_SRC core/jwpopup.c core/x11.c core/drw.c)
set(LIB_HEADERS ${INCLUDE_DIR}/jw/popup/popup.h ${INCLUDE_DIR}/jw/popup/x11.h ${INCLUDE_DIR}/jw/popup/drw.h)

add_library(${PROJECT_NAME} SHARED ${LIB_SRC} ${LIB_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} Xft fontconfig)

# jwpopupd
set(DAEMON_SRC daemon/main.c)
set(DAEMON_TARGET ${PROJECT_NAME}d)

add_executable(${DAEMON_TARGET} ${DAEMON_SRC})
target_include_directories(${DAEMON_TARGET} PRIVATE ${INCLUDE_DIR})
target_link_libraries(${DAEMON_TARGET} ${PROJECT_NAME})
