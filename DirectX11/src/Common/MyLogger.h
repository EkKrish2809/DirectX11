#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <stdlib.h>

class MyLogger
{
    public:
        // Common() {}

        MyLogger()
        {
            if (!CreateLogFile())
            {
                MessageBox(NULL, TEXT("Log File Not Created"), TEXT("Failure !!!"), MB_OK);
            }
        }

        ~MyLogger() 
        {
            CloseLog();
        }

        BOOL CreateLogFile(void)
        {
            //code
            if (LogPtr != NULL)
                return(FALSE);

            if (fopen_s(&LogPtr, gszLogFilePathName, "w") != 0)
            {
                MessageBox(NULL, TEXT("Failed To Create Log File \"D3DLog.txt\" !!! Exitting Now ..."), TEXT("LOG FILE ERROR"), MB_ICONERROR | MB_OK);
                return(FALSE);
            }

            else
            {
                fprintf(LogPtr, "============================================================\n");
                fprintf(LogPtr, "Log File \"D3DLog.txt\" Has Been Created Successfully !!!\n");
                fprintf(LogPtr, "============================================================\n\n");
            }
            return(TRUE);
        }

        void Logger(const char *statement, ...)
        {
            va_list arg;
            int ret;

            //code
            if (LogPtr == NULL)
                return;
            
            fopen_s(&LogPtr, gszLogFilePathName, "a+");
            va_start(arg, statement);
            ret = vfprintf(LogPtr, statement, arg);
            va_end(arg);
            fclose(LogPtr);
        }

        void CloseLog(void)
        {
            //code
            if (LogPtr == NULL)
                return;

            fopen_s(&LogPtr, gszLogFilePathName, "a+");
            fprintf(LogPtr, "============================================================\n");
            fprintf(LogPtr, "Log File \"D3DLog.txt\" Has Been Closed Successfully !!!\n");
            fprintf(LogPtr, "============================================================\n");

            fclose(LogPtr);
            LogPtr = NULL;
        }

    private:
        FILE *LogPtr = NULL;
        char gszLogFilePathName[11] = "D3DLog.txt";
};

#endif