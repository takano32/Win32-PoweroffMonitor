
#include <windows.h>
#include <string>

using namespace std;





int WINAPI WinMain
(HINSTANCE hInstance, // handle to current instance
 HINSTANCE hPrevInstance, // handle to previous instance
 LPSTR lpCmdLine, // pointer to command line
 int nCmdShow // show state of window
 ){
	//SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 0);

	Sleep(3000);
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 1);
  	//Sleep(5000);
	//SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
	return 0;	
}
