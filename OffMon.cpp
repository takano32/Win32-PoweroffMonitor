
#include <windows.h>
#include <string>
using namespace std;




string ms2HMS(int i){
	char buffer[16];
	int ms=i%1000;
	int s=(i/1000)%60;
	int m=(i/1000)/60;
	int h=m/60;
	sprintf(buffer,"%02d:%02d",m,s);
	return buffer;
}



class DDE{
private:
	HSZ hszService;
	HSZ hszTopic;
	HCONV hConv;
	DWORD ddeInst;
	
	HDDEDATA hRet;
	HSZ hszMyTopic;
	char szBuf[2048];
public:
	DDE();
	HDDEDATA CALLBACK CallBack
		(UINT uType,UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2,
		HDDEDATA hdata, DWORD dwData1,DWORD dwData2);
	int Initialize();
	int Transaction(string cmd);
	string GetData();
	int Uninitialize();
};


DDE::DDE(){
	hConv=0;
	ddeInst=0;
}

HDDEDATA CALLBACK Callback
(UINT uType,UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2,
 HDDEDATA hdata, DWORD dwData1,DWORD dwData2){
    //MessageBox(NULL, "DDEコールバック関数が呼ばれました", "DDECALLBACK", MB_OK);
    return (HDDEDATA)NULL;
}

int DDE::Initialize(){
	//MessageBox(NULL,lpCmdLine,NULL,0);
	if(DdeInitialize(&ddeInst,Callback,APPCMD_CLIENTONLY,0)!=DMLERR_NO_ERROR){
		MessageBox(NULL,"DDE Initialize failure.",NULL,MB_OK);
		return -1;
	}else{
		//MessageBox(NULL,"DDE Initialize success",NULL,MB_OK);
	}
	hszService=DdeCreateStringHandle(ddeInst,"KbMedia Player",CP_WINANSI);
	hszTopic=DdeCreateStringHandle(ddeInst,"KbMedia Player",CP_WINANSI);
	hConv=DdeConnect(ddeInst,hszService,hszTopic,NULL);
	if(DdeGetLastError(ddeInst)!=DMLERR_NO_ERROR){
		MessageBox(NULL,"Connection failure.",NULL,MB_OK);
		return -1;
	}
	//MessageBox(NULL,"Connection success.",NULL,MB_OK);
	return 0;
}

int DDE::Transaction(string cmd){
	if(!hConv){
		MessageBox(NULL,"DDE isn't started.",NULL,MB_OK);
		return -1;
	}
	strcpy(szBuf,cmd.c_str());
	hszMyTopic=DdeCreateStringHandle(ddeInst,szBuf,CP_WINANSI);
	hRet=DdeClientTransaction(NULL,0,hConv,hszMyTopic,CF_TEXT,XTYP_REQUEST,1000,NULL);
	if(!hRet&&DdeGetLastError(ddeInst)!=DMLERR_NO_ERROR){
		MessageBox(NULL,"Transaction failure.",NULL,MB_OK);
		return -2;
	}
	return 0;
}

string DDE::GetData(){
	if(hRet){
		DdeGetData(hRet,(LPBYTE)szBuf,sizeof(szBuf),0);
		DdeFreeStringHandle(ddeInst,hszMyTopic);
		DdeFreeDataHandle(hRet);
		//MessageBox(NULL,szBuf,"data",MB_OK);
		//Command+=szBuf;
		return szBuf;
	}
	return 0;
}


int DDE::Uninitialize(){
	if(hConv){
		if(DdeFreeStringHandle(ddeInst,hszService)==0)
			MessageBox(NULL,"DdeFreeStringHandle failure.","Error",MB_OK);
		if(DdeFreeStringHandle(ddeInst,hszTopic)==0)
			MessageBox(NULL,"DdeFreeStringHandle failure.","Error",MB_OK);
		if(DdeDisconnect(hConv)!=0){
			//MessageBox(NULL,"DdeDisconnect success.","OK",MB_OK);
		}
		hConv=0;
	}else{
		MessageBox(NULL,"There isn't DDE to end.","OK",MB_OK);
	}
	
	if(DdeUninitialize(ddeInst)!=0){
		//MessageBox(NULL,"DdeUninitialize success.","OK",MB_OK);
	}
	return 0;
}


void TextOut(string Text){
	char buffer[512];
	string Command="\"clnch.exe\" /ex &TextOut;\"";
	Command+=Text;
	Command+="\"";
	if(WinExec(Command.c_str(),SW_SHOWDEFAULT)<32){
		HWND hWnd=GetForegroundWindow();
		if(hWnd){
			GetWindowText(hWnd,buffer,sizeof(buffer));
			SetWindowText(hWnd,Text.c_str());
			Sleep(2000);
			SetWindowText(hWnd,buffer);
		}else
			MessageBox(NULL,Text.c_str(),"info",MB_OK);
	}
}

int WINAPI WinMain
(HINSTANCE hInstance, // handle to current instance
 HINSTANCE hPrevInstance, // handle to previous instance
 LPSTR lpCmdLine, // pointer to command line
 int nCmdShow // show state of window
 ){
	//string Command="D:\\Tools\\clnch\\clnch.exe /ex &TextOut;";
	string Text;
	HWND KbMediaPlayer=FindWindow("TFrmMIDI",NULL);
	HWND CraftLaunch=FindWindow(NULL,"CraftLaunch");
	
	if(!KbMediaPlayer){	
		Text="KbMediaPlayerが起動されていません";
		TextOut(Text);
	}else{
		if(strcmp(lpCmdLine,"")==0){
			//MessageBox(NULL,"から",NULL,0);
			/*GetWindowText(KbMediaPlayer,buffer,sizeof(buffer));
			Command+=buffer;*/
			DDE* pDde;
			pDde=new DDE();
			pDde->Initialize();
			
			pDde->Transaction("Artist");
			Text+=pDde->GetData();
			Text+=" - ";
			pDde->Transaction("Title");
			Text+=pDde->GetData();

			pDde->Uninitialize();
		}else{
			string Cmd=lpCmdLine;
			DDE* pDde;
			pDde=new DDE();
			pDde->Initialize();
			
			if(stricmp(Cmd.c_str(),"now")==0){
				int ms;
				pDde->Transaction("Position");
				ms=atoi(pDde->GetData().c_str());
				Text+=ms2HMS(ms);
				Text+="/";
				pDde->Transaction("Length");
				ms=atoi(pDde->GetData().c_str());
				Text+=ms2HMS(ms);
				
			}else{
				pDde->Transaction(Cmd.c_str());
				Text+=pDde->GetData();
			}
			pDde->Uninitialize();
		}
		
		

		//MessageBox(NULL,buffer,"Error",0);
		//Command+="\"";
		//WinExec(Command.c_str(),SW_SHOWDEFAULT);
		TextOut(Text);
	}
	
	return 0;	
}
