#include <Windows.h>
#include <stdio.h>

//Needs to be global. Hooked function must have identical number and type of parameters.
char originalFuncPreamble[6] = {0};
SIZE_T bytes = 0;

int __stdcall HookedMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
	
	lpText = (LPCWSTR)L"HOOKED!";
	lpCaption = (LPCWSTR)L"HOOKED!";
	//unhook();
	WriteProcessMemory(GetCurrentProcess(), &MessageBoxW, originalFuncPreamble, 6, &bytes);

	return MessageBoxW(hWnd, lpText , lpCaption, uType);
}

int main() {
	LPCTSTR text = (LPCTSTR)L"I\'m not hooked\n";
	LPCTSTR caption = (LPCTSTR)L"Not hooked\n";

	//Get the original function preamble to restore when unhooking
	ReadProcessMemory(GetCurrentProcess(), &MessageBoxW, originalFuncPreamble, 6, &bytes);

	printf("Pointer to MessageBoxW function @ 0x%p\n",MessageBoxW);
	printf("First 6 bytes read: ");
	for (size_t i = 0; i < bytes; i++) {
		printf("%02hhx ", originalFuncPreamble[i]);
	}
	printf("\n");

	//Get hook address
	void *HookedFuncAddress = &HookedMessageBoxW;

	//Hook MessageBoxW by overwriting its first bytes
	char buffer[6] = { 0 };
	memcpy_s(buffer, sizeof(buffer), "\x68", 1);
	memcpy_s(buffer + 1, sizeof(buffer), &HookedFuncAddress, 4);
	memcpy_s(buffer + 5, sizeof(buffer), "\xc3", 1);

	printf("Pointer to HookedMessageBoxW function @ 0x%p\n", HookedMessageBoxW);
	printf("Will be overwritten with: ");
	for (size_t i = 0; i < bytes; i++) {
		printf("%02hhx ", buffer[i]);
	}
	printf("\n");

	WriteProcessMemory(GetCurrentProcess(), &MessageBoxW, buffer, 6, &bytes);

	//printf("MessageBoxW GetProcAddress pointer @ %p\n", messageBoxAddress);

	MessageBoxW(NULL, text, caption, MB_OK);


	return 0;
}