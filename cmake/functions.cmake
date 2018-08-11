# В текущей версии CMake не может включить режим C++17 в некоторых компиляторах.
# Функция использует обходной манёвр.
function(custom_enable_cxx17 TARGET)
    # Включаем C++17 везде, где CMake может.
    target_compile_features(${TARGET} PUBLIC cxx_std_17)

    # Включаем режим C++latest в Visual Studio
    #if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #    set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "/std:c++latest")

    # Включаем компоновку с libc++, libc++experimental и pthread для Clang
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-stdlib=libc++ -pthread")
        target_link_libraries(${TARGET} PRIVATE  c++experimental pthread)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-pthread")
        target_link_libraries(${TARGET} PRIVATE pthread)
    endif()
endfunction(custom_enable_cxx17)
