/**
*
*/

#include "Application.hpp"

#include <Windows.h>


int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow) {

    return Application{}.run();
}