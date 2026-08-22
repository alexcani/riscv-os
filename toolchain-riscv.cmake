# Toolchain file for RISC-V cross-compilation
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER riscv64-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv64-unknown-elf-g++)
set(CMAKE_ASM_COMPILER riscv64-unknown-elf-gcc)

set(RISCV_COMMON_FLAGS "-march=rv64g -mabi=lp64d -mcmodel=medany")
set(CMAKE_C_FLAGS_INIT "${RISCV_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${RISCV_COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${RISCV_COMMON_FLAGS}")

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
