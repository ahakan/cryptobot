#ifndef ELOG_H
#define ELOG_H

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <mutex>
#include <unistd.h>
#include <sys/types.h>

#include <chrono>
#include <time.h>

#define  ERROR                      __FILE__ , __LINE__ , __FUNCTION__ , 0
#define  WARNING                    __FILE__ , __LINE__ , __FUNCTION__ , 1
#define  DEBUG                      __FILE__ , __LINE__ , __FUNCTION__ , 2
#define  INFO                       __FILE__ , __LINE__ , __FUNCTION__ , 3


#define  __FILENAME__               (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define  LOG_CONSOLE_OR_FILE        1                   // 0=Console , 1=File
#define  MAX_LEVEL                  4                   // 1 = Error, 
                                                        // 2 = Error, Warning, 
                                                        // 3 = Error, Warning, Debug, 
                                                        // 4 = Error, Warning, Debug, Info
#define  MAX_FILE_SIZE              26214400            // 25MB

#define  MAX_LINE_SIZE              5
#define  MAX_TID_SIZE               6
#define  MAX_LEVEL_SIZE             7
#define  MAX_FILE_NAME_SIZE         20
#define  MAX_FUNC_NAME_SIZE         20

#define  ELOG                       getLog

#define  LogConsole                 std::cout

class eLog
{
    private:
        std::ofstream               LogFile;
        std::string                 LogFilePath = "";
        std::string                 LogFileNamePrefix = "0000";
        std::string                 LogFileNameInfix = "1";         // Must be defined as an integer
        std::string                 LogFileNameSuffix = ".log";

    public:
                                    eLog();
                                    ~eLog();
        void                        writeLogToFile(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message);
        void                        writeLogToConsole(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message);
        void                        addLogHeadToFile();
        void                        changeFile();
        std::string                 currentDateTime();
        std::string                 getLogFileFullName();
        std::ifstream::pos_type     fileSize(const char* fName);
        std::string                 addSpacesToConstChar(const char* getChar, uint8_t maxSize);
        std::string                 addSpacesToUnsignedInt(unsigned int getInt, uint8_t maxSize);

        char const*                 LevelNames[ 4 ] = { "ERROR", "WARNING", "DEBUG", "INFO" };
        std::mutex                  MutexLock;
};

extern eLog _eLog;

template<typename... Args>
void getLog(char const *file, unsigned int line, char const * function, unsigned char lvl, const char * f, Args... args)
{
    
    if( static_cast<int>(lvl) < MAX_LEVEL )
    {
        char    _Message[256];

        _eLog.MutexLock.lock();

        // /full/path/to/file.c to file.c
        file = (strrchr(file, '/') ? strrchr(file, '/') + 1 : file);

        // get all args
        snprintf (_Message, 255, f, args...);


        #if LOG_CONSOLE_OR_FILE == 0
            _eLog.writeLogToConsole(file,
                                std::to_string(gettid()),
                                function,
                                std::to_string(line),
                                _eLog.LevelNames[ lvl ],
                                _Message);
        #else
            _eLog.writeLogToFile(_eLog.addSpacesToConstChar(file, MAX_FILE_NAME_SIZE),
                                _eLog.addSpacesToUnsignedInt(gettid(), MAX_TID_SIZE),
                                _eLog.addSpacesToConstChar(function, MAX_FUNC_NAME_SIZE),
                                _eLog.addSpacesToUnsignedInt(line, MAX_LINE_SIZE),
                                _eLog.addSpacesToConstChar(_eLog.LevelNames[ lvl ], MAX_LEVEL_SIZE),
                                _Message);

            _eLog.changeFile();
        #endif

        _eLog.MutexLock.unlock();
    }
}


#endif