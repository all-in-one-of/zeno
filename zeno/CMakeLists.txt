file(GLOB ZENO_SOURCE */*.cpp */*/*.cpp */*/*/*.cpp */*/*/*/*.cpp */*.h */*/*.h */*/*/*.h */*/*/*/*.h)

add_library(zeno SHARED ${ZENO_SOURCE})

if (ZENO_ENABLE_OPENMP)
    find_package(OpenMP)
    if (TARGET OpenMP::OpenMP_CXX)
        message("found package: OpenMP::OpenMP_CXX")
        target_link_libraries(zeno PRIVATE OpenMP::OpenMP_CXX)
    endif()
endif()
if (ZENO_VISUALIZATION)
    target_compile_definitions(zeno PUBLIC -DZENO_VISUALIZATION)
    if (UNIX OR APPLE)
		target_link_libraries(zeno PRIVATE stdc++fs)  # for <experimental/filesystem>
	endif()
endif()
if (ZENO_GLOBALSTATE)
    target_compile_definitions(zeno PUBLIC -DZENO_GLOBALSTATE)
endif()
if (ZENO_BENCHMARKING)
    target_compile_definitions(zeno PRIVATE -DZENO_BENCHMARKING)
endif()
if (ZENO_FAULTHANDLER)
    target_compile_definitions(zeno PRIVATE -DZENO_FAULTHANDLER)
endif()
if (ZENO_FAIL_SILENTLY)
    target_compile_definitions(zeno PRIVATE -DZENO_FAIL_SILENTLY)
endif()

target_compile_definitions(zeno PRIVATE -DDLL_ZENO)
target_include_directories(zeno PUBLIC .)

target_link_libraries(zeno PUBLIC spdlog)
