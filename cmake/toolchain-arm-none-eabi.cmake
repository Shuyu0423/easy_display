set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(ARM_NONE_EABI_ROOT "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/12.2 mpacbti-rel1/bin" CACHE PATH "Arm GNU toolchain bin directory")

set(CMAKE_C_COMPILER "${ARM_NONE_EABI_ROOT}/arm-none-eabi-gcc.exe")
set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_ROOT}/arm-none-eabi-gcc.exe")
set(CMAKE_OBJCOPY "${ARM_NONE_EABI_ROOT}/arm-none-eabi-objcopy.exe" CACHE FILEPATH "objcopy")
set(CMAKE_SIZE "${ARM_NONE_EABI_ROOT}/arm-none-eabi-size.exe" CACHE FILEPATH "size")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
