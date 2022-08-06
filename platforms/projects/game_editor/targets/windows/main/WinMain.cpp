/**
 *
 */

// Thank you, Microsoft, for file WinDef.h with min/max redefinition.
#define NOMINMAX

#include "Application.hpp"

#include <Windows.h>

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow) {
    return Application{}.run();
}