#pragma once
#include"../IMGUI.H"
#include"../IMGUI_IMPL_WIN32.H"
#include"../IMGUI_IMPL_DX11.H"
#include <d3d11.h>
#include <dwmapi.h>
#pragma comment(lib, "d3d11.lib")//创建窗口
inline ImFont* g_font = NULL;
inline IDXGISwapChain* pSwapChain;
inline ID3D11Device* pDevice;
inline ID3D11DeviceContext* pContext;
inline ID3D11RenderTargetView* MainRenderTargetView;
inline HWND hwnd = NULL;
inline ImColor BgColor{ 0, 0, 0 };

LRESULT WINAPI WndProc_External(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	///* if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	//	 return true;*/
	//	 switch (msg)
	//	 {
	//	 case WM_CREATE:
	//	 {
	//	     MARGINS     Margin = { -1 };
	//	     DwmExtendFrameIntoClientArea(hWnd, &Margin);
	//	     break;
	//	 }
	//	 case WM_SIZE:
	//	     if (g_Device.g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
	//	     {
	//	         g_Device.CleanupRenderTarget();
	//	         g_Device.g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
	//	         g_Device.CreateRenderTarget();
	//	     }
	//	     return 0;
	//	 case WM_SYSCOMMAND:
	//	     if ((wParam & 0xfff0) == SC_KEYMENU)
	//	         return 0;
	//	     break;
	//	 case WM_DESTROY:
	//	     ::PostQuitMessage(0);
	//	     return 0;
	//	 }
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
HWND ToolCreateWindow(PVOID Call)
{
	int random2 = 100 + rand() % 200;
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc_External, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"STATIC", NULL };
	RegisterClassExW(&wc);
	hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED, L"STATIC", L"DeltaForce", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, random2, random2, NULL, NULL, GetModuleHandle(NULL), NULL);
	//SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLongA(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), NULL, LWA_COLORKEY);


	//创建设备
	//设置窗口背景透明

	POINT Point{};
	RECT Rect{};
	HWND deskhWnd = GetDesktopWindow();
	GetClientRect(deskhWnd, &Rect);
	ClientToScreen(deskhWnd, &Point);
	GameData::Info::GameCenterX = Rect.right / 2;
	GameData::Info::GameCenterY = Rect.bottom / 2;

	int random = rand() % 20;
	SetWindowPos(hwnd, HWND_TOPMOST, (int)Point.x + 1, (int)Point.y + 1, (int)Rect.right - random, (int)Rect.bottom- random, SWP_SHOWWINDOW);
	MARGINS     Margin = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &Margin);

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	D3D_FEATURE_LEVEL featureLevel;
	UINT createDeviceFlags = 0;
	HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &featureLevel, &pContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &featureLevel, &pContext);
	if (res != S_OK)
		return 0;

	BgColor = ImColor(0, 0, 0,0);

	//初始化IMGUI
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer == nullptr)
		return 0;
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &MainRenderTargetView);
	pBackBuffer->Release();
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	//IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simhei.ttf", 15.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
	SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

	//消息循环
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		POINT Point{};
		RECT Rect{};
		// 控制窗口状态切换
		POINT MousePos;
		GetCursorPos(&MousePos);
		ScreenToClient(hwnd, &MousePos);
		ImGui::GetIO().MousePos.x = static_cast<float>(MousePos.x);
		ImGui::GetIO().MousePos.y = static_cast<float>(MousePos.y);

		if (GetAsyncKeyState(0x1))
		{
			ImGui::GetIO().MouseDown[0] = true;

		}
		else
		{
			ImGui::GetIO().MouseDown[0] = false;
		}
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Hello, world!");
		ImGui::Text("This is some useful text.");
		((void(*)(void))Call)();

		ImGui::End();

		ImGui::EndFrame();
		ImGui::Render();
//调用call

		const float clear_color_with_alpha[4] = { BgColor.Value.x, BgColor.Value.y , BgColor.Value.z, BgColor.Value.w };
		pContext->OMSetRenderTargets(1, &MainRenderTargetView, NULL);

		pContext->ClearRenderTargetView(MainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		pSwapChain->Present(1, 0);
	}
}
namespace draw
{
	VOID FilledRect(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, ImVec4 Color)
	{
		ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), 0, 0);
	}

	// 绘制直线
	VOID Line(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, ImVec4 Color, FLOAT Width)
	{
		ImGui::GetForegroundDrawList()->AddLine(ImVec2(X1, Y1), ImVec2(X2, Y2), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Width);
	}
	VOID CircleFilled(FLOAT X, FLOAT Y, FLOAT Radius, ImVec4 Color, INT Segments)
	{
		ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(X, Y), Radius, ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Segments);
	}

	VOID Text(FLOAT X, FLOAT Y, ImVec4 Color, FLOAT Size, CONST CHAR* Str, ...)
	{
		CHAR Buffer[1024] = { 0 };
		va_list va_alist;
		va_start(va_alist, Str);
		vsprintf_s(Buffer, Str, va_alist);
		va_end(va_alist);
		ImGui::GetForegroundDrawList()->AddText(g_font, Size, ImVec2(X, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Buffer);
	}

	FLOAT TextCenter(FLOAT X, FLOAT Y, ImVec4 Color, FLOAT Size, CONST CHAR* Str, ...)
	{
		CHAR Buffer[1024] = { 0 };
		va_list va_alist;
		va_start(va_alist, Str);
		vsprintf_s(Buffer, Str, va_alist);
		va_end(va_alist);
		ImVec2 TextSize = g_font->CalcTextSizeA(g_font->FontSize, D3D11_FLOAT32_MAX, 0.0f, Buffer);
		ImGui::GetForegroundDrawList()->AddText(NULL, Size, ImVec2(X - TextSize.x / 2, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(Color)), Buffer);
		return X - TextSize.x / 2;
	}

}