#include <string>
#include <Windows.h>
namespace g2
{
    namespace utils
    {
        namespace convert
        {
            namespace string
            {

                inline std::string gbk_to_utf8(const std::string &src)
                {
                    std::string result;
                    WCHAR *strSrc;
                    LPSTR szRes;
                    int i = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
                    strSrc = new WCHAR[i + 1];
                    MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, strSrc, i);
                    i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
                    szRes = new CHAR[i + 1];
                    WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);

                    result = szRes;
                    delete[] strSrc;
                    delete[] szRes;

                    return result;
                }

                inline std::string utf8_to_gbk(const std::string &src)
                {
                    std::string result;
                    WCHAR *strSrc;
                    LPSTR szRes;
                    int i = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, NULL, 0);
                    strSrc = new WCHAR[i + 1];
                    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, strSrc, i);
                    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
                    szRes = new CHAR[i + 1];
                    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

                    result = szRes;
                    delete[] strSrc;
                    delete[] szRes;

                    return result;
                }

            }
        }
    }
}