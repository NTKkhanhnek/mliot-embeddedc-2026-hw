@echo off
setlocal

echo ============================================================
echo STEP 1: CLEANING BUILD DIRECTORY
echo ============================================================

if exist build (
    echo Deleting existing build folder...
    rmdir /s /q build

    if exist build (
        echo ERROR: Failed to delete build directory.
        goto :error
    )
) else (
    echo Build folder does not exist. Nothing to clean.
)

echo.
echo ============================================================
echo STEP 2: CONFIGURING PROJECT WITH CMAKE
echo ============================================================

cmake -S . -B build -G Ninja
if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    goto :error
)

echo.
echo ============================================================
echo STEP 3: COMPILING FIRMWARE WITH NINJA
echo ============================================================

ninja -C build
if errorlevel 1 (
    echo ERROR: Firmware build failed.
    goto :error
)

if not exist "build\app_firmware.bin" (
    echo ERROR: build\app_firmware.bin was not generated.
    goto :error
)

echo.
echo ============================================================
echo STEP 4: FLASHING FIRMWARE TO TARGET MCU
echo ============================================================

where STM32_Programmer_CLI >nul 2>&1
if errorlevel 1 (
    echo ERROR: STM32_Programmer_CLI was not found in PATH.
    echo Please install STM32CubeProgrammer or add its bin folder to PATH.
    goto :error
)

STM32_Programmer_CLI -c port=SWD -w "build\app_firmware.bin" 0x08000000 -rst
if errorlevel 1 (
    echo ERROR: Flashing firmware failed.
    goto :error
)

echo.
echo ============================================================
echo BUILD AND FLASH COMPLETED SUCCESSFULLY
echo ============================================================
goto :end

:error
echo.
echo ============================================================
echo BUILD AND FLASH FAILED
echo ============================================================
exit /b 1

:end
endlocal
exit /b 0
