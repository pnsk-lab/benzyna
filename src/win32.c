#include <ba_runtime.h>

#include <windows.h>
#include <GL/gl.h>

static char* argv1 = NULL;
static HDC   dc;
static HGLRC glrc;
static HWND  window;
static BOOL(APIENTRY* wglSwapIntervalEXT)(int);
static BOOL loop = TRUE;

static void make_current(ba_runtime_t* rt) {
	wglMakeCurrent(dc, glrc);
}

static void swap_buffer(ba_runtime_t* rt) {
	SwapBuffers(dc);
}

static void swap_interval(ba_runtime_t* rt, int interval) {
	if(wglSwapIntervalEXT != NULL) wglSwapIntervalEXT(interval);
}

LRESULT CALLBACK wndproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	if(msg == WM_PAINT) {
		PAINTSTRUCT ps;

		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	} else if(msg == WM_CLOSE) {
		loop = FALSE;
	} else if(msg == WM_DESTROY) {
		PostQuitMessage(0);
	} else {
		return DefWindowProc(hWnd, msg, wp, lp);
	}

	return 0;
}

int main(int argc, char** argv) {
	ba_runtime_t	      ba;
	int		      i;
	double		      scale = 1;
	WNDCLASSEX	      wc;
	PIXELFORMATDESCRIPTOR desc;
	int		      fmt;
	DWORD		      style;
	RECT		      rc;
	MSG		      msg;

	ba.param.turbo = ba_false;
	ba.param.fps   = 30;

	for(i = 1; i < argc; i++) {
		if(strcmp(argv[i], "--turbo") == 0) {
			ba.param.turbo = ba_true;
		} else if(strcmp(argv[i], "--vsync") == 0) {
			ba.param.fps = 0;
		} else if(strcmp(argv[i], "--fps") == 0) {
			if(argv[i + 1] == NULL) {
				fprintf(stderr, "%s: --fps needs argument\n", argv[0]);
				return 1;
			}
			ba.param.fps = atof(argv[++i]);
		} else if(strcmp(argv[i], "--scale") == 0) {
			if(argv[i + 1] == NULL) {
				fprintf(stderr, "%s: --scale needs argument\n", argv[0]);
				return 1;
			}
			scale = atof(argv[++i]);
		} else {
			argv1 = argv[i];
		}
	}

	if(argv1 == NULL) return 1;

	ba.param.make_current  = make_current;
	ba.param.swap_buffer   = swap_buffer;
	ba.param.swap_interval = swap_interval;

	wglSwapIntervalEXT = NULL;

	wc.cbSize	 = sizeof(wc);
	wc.style	 = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	 = wndproc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = GetModuleHandle(NULL);
	wc.hCursor	 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName	 = NULL;
	wc.lpszClassName = "benzyna";
	wc.hIcon	 = LoadIcon(wc.hInstance, "BENZYNA");
	wc.hIconSm	 = NULL;
	if(!RegisterClassEx(&wc)) return 1;

	window = CreateWindow("benzyna", "Benzyna", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME), CW_USEDEFAULT, CW_USEDEFAULT, BA_WIDTH * scale, BA_HEIGHT * scale, NULL, 0, wc.hInstance, NULL);
	if(window == NULL) {
		return 1;
	}

	dc = GetDC(window);

	memset(&desc, 0, sizeof(desc));
	desc.nSize	= sizeof(desc);
	desc.nVersion	= 1;
	desc.dwFlags	= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	desc.iPixelType = PFD_TYPE_RGBA;
	desc.cColorBits = 32;
	desc.cDepthBits = 32;
	fmt		= ChoosePixelFormat(dc, &desc);

	SetPixelFormat(dc, fmt, &desc);

	glrc = wglCreateContext(dc);
	if(glrc == NULL) {
		return 1;
	}

	wglMakeCurrent(dc, glrc);

	wglSwapIntervalEXT = wglGetProcAddress("wglSwapIntervalEXT");

	SetRect(&rc, 0, 0, BA_WIDTH * scale, BA_HEIGHT * scale);
	style = (DWORD)GetWindowLongPtr(window, GWL_STYLE);
	AdjustWindowRect(&rc, style, FALSE);
	SetWindowPos(window, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);

	ShowWindow(window, SW_NORMAL);
	UpdateWindow(window);

	if(!ba_runtime_init(&ba)) {
		return 1;
	}

	if(!ba_runtime_load_path(&ba, argv1)) {
		ba_runtime_uninit(&ba);
		return 1;
	}

	loop = TRUE;
	do {
		while(PeekMessage(&msg, window, 0, 0, PM_NOREMOVE)) {
			if(GetMessage(&msg, window, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else {
				loop = FALSE;
			}
		}
		ba_runtime_step(&ba);
	} while(loop);

	ba_runtime_uninit(&ba);

	wglMakeCurrent(NULL, NULL);
	ReleaseDC(window, dc);
	wglDeleteContext(glrc);
	DestroyWindow(window);
}
