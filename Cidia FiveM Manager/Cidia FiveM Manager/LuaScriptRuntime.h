#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <functional>
namespace fx
{
	class Lua
	{
	private:
		__int64* m_state = 0;
#define LUAI_MAXSTACK		1000000
#define LUA_REGISTRYINDEX	(-LUAI_MAXSTACK - 1000)
		//offsets last update 2023-10-07
		typedef void(__fastcall* lua_pushvalue_t)(__int64* L, int idx);
		lua_pushvalue_t lua_pushvalueW = (lua_pushvalue_t)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2D160);
		typedef __int64* (__fastcall* luaL_newstate_t)();
		typedef void(__fastcall* lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);
		lua_Alloc lua_AllocW = (lua_Alloc)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2C160);
		luaL_newstate_t luaL_newstateW = (luaL_newstate_t)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x10917);
		typedef __int64* (__fastcall* lua_newstate)(void(__fastcall* a1)(void* a1, void* a2, size_t osize, size_t nsize), void* ud);//2CB30
		lua_newstate lua_newstateW = (lua_newstate)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2CB30);

		typedef int(__fastcall* luaL_ref)(__int64* L, int idxR);
		luaL_ref lua_refA = (luaL_ref)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2ABD0);

		typedef int(__fastcall* luaL_rawgeti)(__int64* L, int idxR, int ref);
		luaL_rawgeti lua_rawgetiW = (luaL_rawgeti)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2E780);

		typedef const char* (__fastcall* luaL_pushlstring)(__int64* L, const char* buff, size_t len);
		luaL_pushlstring lua_pushlstringW = (luaL_pushlstring)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2E180);

		typedef int(__fastcall* lua_pcallk)(__int64* L, int nargs, int nresults, int errfunc, void* ctx, void* k);
		lua_pcallk lua_pcallW = (lua_pcallk)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2EEE0);

		typedef __int64(__fastcall* lua_pop_t)(__int64* L, int idx);
		lua_pop_t lua_popW = (lua_pop_t)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2CEC0);

		typedef __int64(__fastcall* luaL_loadbufferT)(__int64* L, const char* buff, size_t size, const char* name, const char* mode);
		luaL_loadbufferT luaL_loadbufferA = (luaL_loadbufferT)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2B260);

		typedef __int64(__fastcall* luaL_loadfilex_t)(__int64* L, const char* script_name, const char* mode);
		luaL_loadfilex_t luaL_loadfilex = (luaL_loadfilex_t)((uintptr_t)GetModuleHandle(L"citizen-scripting-lua.dll") + 0x2AD30);

		std::vector<__int64*> threads;
		void create_state() {
			m_state = lua_newstateW(lua_AllocW, luaL_newstateW);
			threads.push_back(m_state);
		}

	public:
		void lua_pushvalue(int idx) {
			lua_pushvalueW(m_state, idx);
		}
		int lua_ref()
		{
			int ref = lua_refA(m_state, LUA_REGISTRYINDEX);
			return ref;
		}
		int lua_rawgeti(int ref)
		{
			return lua_rawgetiW(m_state, LUA_REGISTRYINDEX, ref);
		}
		const char* lua_pushlstring(const char* script, size_t script_size)
		{
			lua_pushlstringW(m_state, script, script_size);
			return script;
		}
		int lua_pcall(int nargs, int nresults, int errfunc, void* ctx, void* k)
		{
			if (lua_pcallW(m_state, 1, 0, 1, NULL, NULL) != 0)
			{
				MessageBoxA(0, "Error lua pcall", 0, 0);
			}
			return 0;
		}
		__int64 lua_pop(int idx)
		{
			lua_popW(m_state, idx);
			return 0;
		}

		void lua_load(const char* filename, const char* mode)
		{
			luaL_loadfilex(m_state, filename, NULL);
		}

		bool luaL_loadbuffer(const char* script, size_t size, const char* script_name, const char* mode)
		{
			std::string chunkName = script_name;
			const char* scriptname;
			if (chunkName.find("@") == std::string::npos) {
				chunkName = "@";
				chunkName += script_name;
				scriptname = chunkName.c_str();
			}
			else
			{
				scriptname = script_name;
			}

			if (luaL_loadbufferA(m_state, script, size, scriptname, mode /* SET MODE TO NULL */) != 0)
			{
				MessageBoxA(0, "Error executing script", 0, 0);
				return false;
			}
			else
			{
				return true;
			}
		}

		__int64* Get() {
			if (m_state == 0)
				create_state();

			return m_state;
		}

	};

	class LuaStateHolder
	{
	private:

	public:
		Lua LUA() {
			Lua class_t;
			return class_t;
		}
	};

	class Stream
	{
	private:
		std::string script;
	public:
		std::uint64_t GetLength(uint64_t* length)
		{
			*length = script.length() + 1;
			return 0;
		}
		std::uint64_t Read(std::string* data)
		{
			*data = script;// max script size [0x10000000000???1?]
			return 0;
		}
		std::uint64_t Write(std::string data)
		{
			script = data;
			return 0;
		}

		std::uint64_t ScriptState(Lua _this) {
			_this.Get();
			_this.lua_pop(1);
			_this.lua_pushlstring(script.c_str(), script.length() + 1);
			_this.luaL_loadbuffer(script.c_str(), script.length() + 1, "@@hardcap/cidiacheats.lua", NULL);//resource:@@hardcap/client.lua
			_this.lua_pcall(0, 0, 0, NULL, NULL);
			return 0;
		}
	};

	class LuaScriptRuntime : public LuaStateHolder
	{
	public:
		LuaStateHolder* m_state;
		Stream* m_stream;
		uint32_t RunFileInternal(const char* scriptName, std::function<uint32_t(char*)> loadFunction) {
			return m_state->LUA().lua_pcall(0, 0, 0, NULL, NULL);
		}

		uint32_t LoadSystemFileInternal(const char* scriptFile)
		{
			//return LoadSystemFileInternal_t(m_stream, scriptFile); ( mov[0x11*rdx] - 0xA4 = m_state [ 5 ] table. table size[0x80000] )
			return 0;
		}

		uint32_t LoadSystemFile(const char* scriptName) {
			return RunFileInternal(scriptName, std::bind(&LuaScriptRuntime::LoadSystemFileInternal, this, std::placeholders::_1));
		}

	};


}