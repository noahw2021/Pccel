# Pccel
PLASM2 Compiled C Emulation Layer for Win32/MSVC CRT

## What is this
This is a C library that exports function as instrinsics to be used for the PLASM2 operating system, moth. I plan on developing this in Visual Studio, and this library acts as a translation library that can have Win32 code simulating a PLASM2 cpu instance. This does not execute any PLASM2 code, but just implements the entire standard. This is a fast developed project, and just designed to "work", and the quality of code is subpar to my standards. I will fix this later, however I'd like to start working on current projects.

## How to use
Link your program against this library, and use the included header file but change "__declspec(dllexport)" to "__declspec(dllimport)". An example implementation can be found [here](https://github.com/noahw2021/moth).

## Future Support
I plan on supporting macOS and Linux later, however that is not a priority right now.
