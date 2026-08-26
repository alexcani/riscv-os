cmake_minimum_required(VERSION 3.28)

project(risotto
    DESCRIPTION "RISC-V Toy Kernel"
    LANGUAGES C CXX ASM
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMMON_FLAGS "-ffreestanding -Wall -Wextra -nolibc -nostdlib")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -fno-exceptions -fno-rtti")

add_executable(risotto
    main.cpp
    platform/trap.S
    platform/entry.S
    mm/allocator.cpp
)

target_include_directories(risotto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(risotto PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
set_target_properties(risotto PROPERTIES SUFFIX ".elf")
target_link_options(risotto PRIVATE "-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld" -nostdlib -Wl,-Map=${CMAKE_BINARY_DIR}/risotto.map)

# Run the kernel in QEMU
add_custom_target(run
    COMMAND qemu-system-riscv64 -machine virt -m 512M -nographic -kernel $<TARGET_FILE:risotto>
    DEPENDS risotto
)
