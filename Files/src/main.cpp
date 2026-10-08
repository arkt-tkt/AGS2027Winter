#include <crtdbg.h>
#include <Windows.h>
#include "Application.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

	// メモリリーク検出用
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application::CreateInstance();

	auto& app = Application::GetInstance();

	if (!app.Init()) return -1;

	app.GameLoop();

	if (!app.Release()) return -1;

	Application::DeleteInstance();

	return 0;
}
