#include<iostream>
using namespace std;
#include<string>
#include<sapi.h>
#pragma comment(lib,"sapi.lib")

//配置
void MSSSpeak(LPCTSTR speakContent) {//LPCTSTR
    //初始化COM接口
    ISpVoice* pVoice = NULL;
    if (FAILED(::CoInitialize(NULL))) {
        MessageBox(NULL, (LPCWSTR)L"COM接口初始化失败！", (LPCWSTR)L"提示",
        MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);
    }

    //获取SpVoice接口
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL,
        CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);

        if (SUCCEEDED(hr)) {
            pVoice->SetVolume((USHORT)100); //设置音量，范围是 0 -100
            pVoice->SetRate(0); //设置速度，范围是 -10 - 10
            hr = pVoice->Speak(speakContent, 0, NULL);
            pVoice->Release();
            pVoice = NULL;
        }
    //释放com资源
    ::CoUninitialize();
}

//string转换成wstring  
std::wstring  StringToWString(const std::string& s) {
    std::wstring wszStr;
    int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
    wszStr.resize(nLength);
    LPWSTR lpwszStr = new wchar_t[nLength];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
    wszStr = lpwszStr;
    delete[] lpwszStr;
    return wszStr;
}


//朗读
void read(string temp) {

    wstring a = StringToWString(temp);
    LPCWSTR str = a.c_str();
    /*不知道为什么Cstr不行*/
    MSSSpeak(str);
    cout << "朗读结束\n";
}

int main() {
    read("你好，世界");
    return 0;
}