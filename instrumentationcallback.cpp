#include "stdafx.h"
#include <Windows.h>
#include <psapi.h>
#include <DbgHelp.h>

#pragma comment (lib, "imagehlp.lib")

#include "Header.h"
#include <thread>

extern "C" VOID medium(VOID);
extern "C" VOID hook(DWORD64 R10, DWORD64 RAX/* ... */);
pNtSetInformationProcess NtSetInformationProcess = (pNtSetInformationProcess)GetProcAddress(GetModuleHandle(L"ntdll"),
	"NtSetInformationProcess");

DWORD64 counter = 0;
bool flag = false;

VOID hook(DWORD64 R10, DWORD64 RAX/* ... */) {
	
	//printf("[Something spooky was called]");
	//return;

	// This flag is there for prevent recursion
	if (!flag) {
		flag = true;

		counter++;

		CHAR buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = { 0 };
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;
		DWORD64 Displacement;

		// MSDN: Retrieves symbol information for the specified address.
		BOOLEAN result = SymFromAddr(GetCurrentProcess(), R10, &Displacement, pSymbol);

		if (result) {
			printf("[Syscall ->] Function: %s Return value: 0x%llx\n", pSymbol->Name, RAX);  //RAX holds our return value. 
		}

		flag = false;
	}


}



int main()
{	

	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout); //sets cout to be used with our newly created console	

	// TODO: setup for get symbols
	SymSetOptions(SYMOPT_UNDNAME);
	SymInitialize(GetCurrentProcess(), NULL, TRUE);

	PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION nirvana;
	nirvana.Callback = (PVOID)(ULONG_PTR)medium;
	nirvana.Reserved = 0; // always 0
	nirvana.Version = 0; // 0 for x64, 1 for x86

	NtSetInformationProcess(
		GetCurrentProcess(),
		(PROCESS_INFORMATION_CLASS)ProcessInstrumentationCallback,
		&nirvana,
		sizeof(nirvana) /*0x8*/ );

	



    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:						
		std::thread([&hModule]() {
			// Call your function here with hModule
			// For example:
			main();
			}).detach();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}