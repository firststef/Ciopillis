#ifdef WIN32
#include <Windows.h>
void SleepFunc(unsigned long time) {
    Sleep(time);
}
#endif