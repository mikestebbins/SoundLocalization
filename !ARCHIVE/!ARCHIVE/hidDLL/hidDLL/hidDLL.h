// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HIDDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HIDDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.


#define HIDDLL_EXPORTS

#ifdef HIDDLL_EXPORTS
#define HIDDLL_API __declspec(dllexport)
#else
#define HIDDLL_API __declspec(dllimport)
#endif

//// This class is exported from the hidDLL.dll
//class HIDDLL_API ChidDLL {
//public:
//	ChidDLL(void);
//	// TODO: add your methods here.
//};

extern HIDDLL_API int nhidDLL;

HIDDLL_API int rawhid_open(int max, int vid, int pid, int usage_page, int usage);
HIDDLL_API int rawhid_recv(int num, void *buf, int len, int timeout); 
HIDDLL_API int rawhid_send(int num, void *buf, int len, int timeout);
HIDDLL_API void rawhid_close(int num);


HIDDLL_API int fnhidDLL(void);
