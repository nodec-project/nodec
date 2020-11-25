#include <nodec/nodec_exception.hpp>

#include <Windows.h>

int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    try
    {

    }
    catch (const nodec::NodecException& e)
    {
        //MessageBox(nullptr, e.what(), )
    }
    
}
