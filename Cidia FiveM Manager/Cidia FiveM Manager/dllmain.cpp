#include "pch.h"
#include "LuaScriptRuntime.h"

void Execute()
{
    std::string script = R"(print("Hello world i am from cidiacheats.com"))";
    fx::LuaScriptRuntime* Runtime = new fx::LuaScriptRuntime();
    Runtime->m_stream->Write(script);
    Runtime->m_stream->ScriptState(Runtime->LUA());
    Runtime->LoadSystemFile("@@hardcap/cidiacheats.lua");
}
DWORD WINAPI manager(LPVOID lr)
{
    Execute();
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, manager, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

