
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.70.0 REQUIRED
	COMPONENTS filesystem system regex)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(SEND_ERROR "Boost is required")
endif()