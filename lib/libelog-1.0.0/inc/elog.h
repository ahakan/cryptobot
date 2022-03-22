/**
 * @file elog.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-03-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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
#define  MAX_FILE_SIZE              10485760            // 10MB

#define  MAX_LINE_SIZE              5
#define  MAX_TID_SIZE               6
#define  MAX_LEVEL_SIZE             7
#define  MAX_FILE_NAME_SIZE         18
#define  MAX_FUNC_NAME_SIZE         22
#define  MAX_MESSAGE_LENGTH         2048           

#define  ELOG                       getLog

#define  LogConsole                 std::cout

class elog
{
    private:
        std::ofstream               LogFile;
        std::string                 LogFilePath = "";
        std::string                 LogFileNamePrefix = "0000";
        std::string                 LogFileNameInfix = "1";         // Must be defined as an integer
        std::string                 LogFileNameSuffix = ".log";

        void                        addLogHeadToFile();
        std::string                 currentDateTime();
        std::string                 getLogFileFullName();
        std::ifstream::pos_type     fileSize(const char* fName);

    public:
        std::mutex                  MutexLock;
        char const*                 LevelNames[ 4 ] = { "ERROR", "WARNING", "DEBUG", "INFO" };


        inline                      elog();
        inline                      ~elog();

        void                        changeFile();
        void                        writeLogToFile(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message);
        void                        writeLogToConsole(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message);
        std::string                 addSpacesToConstChar(const char* getChar, uint8_t maxSize);
        std::string                 addSpacesToUnsignedInt(unsigned int getInt, uint8_t maxSize);
};


/**
 * @brief Construct a new elog::elog object
 * 
 */
inline elog::elog()
{
    char            *_Message       = (char *)malloc(MAX_MESSAGE_LENGTH);
    const char*     _FileName       = __FILENAME__;
    const char*     _FunctionName   = __FUNCTION__;

    if (_Message)
    {
        #if LOG_CONSOLE_OR_FILE == 0

            snprintf (_Message, 255, "Logging has been successfully started.");

            writeLogToConsole(_FileName,
                                    std::to_string(gettid()),
                                    _FunctionName,
                                    std::to_string(__LINE__),
                                    LevelNames[ 0 ],
                                    _Message);

        #else

            if (!LogFile.is_open())
            {
                LogFile.open(getLogFileFullName());

                addLogHeadToFile();

                snprintf (_Message, MAX_MESSAGE_LENGTH-1, "Logging has been successfully started. Max log file size: %dKB", MAX_FILE_SIZE);

                writeLogToFile(addSpacesToConstChar(_FileName, MAX_FILE_NAME_SIZE),
                                addSpacesToUnsignedInt(gettid(), MAX_TID_SIZE),
                                addSpacesToConstChar(_FunctionName, MAX_FUNC_NAME_SIZE),
                                addSpacesToUnsignedInt(__LINE__, MAX_LINE_SIZE),
                                addSpacesToConstChar(LevelNames[ 3 ], MAX_LEVEL_SIZE),
                                _Message);
            }

        #endif

        free(_Message);
    }
}


/**
 * @brief Destroy the elog::elog object
 * 
 */
inline elog::~elog()
{
    char            *_Message       = (char *)malloc(MAX_MESSAGE_LENGTH);
    const char*     _FileName       = __FILENAME__;
    const char*     _FunctionName   = __FUNCTION__;

    if (_Message)
    {
        #if LOG_CONSOLE_OR_FILE == 0
            snprintf (_Message, 255, "Logging has been successfully terminated.");

            writeLogToConsole(_FileName,
                                    std::to_string(gettid()),
                                    _FunctionName,
                                    std::to_string(__LINE__),
                                    LevelNames[ 0 ],
                                    _Message);
        #else
            snprintf (_Message, MAX_MESSAGE_LENGTH-1, "Logging has been successfully terminated. Total log file: %s", LogFileNameInfix.c_str());

            writeLogToFile(addSpacesToConstChar(_FileName, MAX_FILE_NAME_SIZE),
                            addSpacesToUnsignedInt(gettid(), MAX_TID_SIZE),
                            addSpacesToConstChar(_FunctionName, MAX_FUNC_NAME_SIZE),
                            addSpacesToUnsignedInt(__LINE__, MAX_LINE_SIZE),
                            addSpacesToConstChar(LevelNames[ 3 ], MAX_LEVEL_SIZE),
                            _Message);

            LogFile.close();
        #endif

        free(_Message);
    }
}
  

extern elog _elog;


/**
 * @brief Get the Log object
 * 
 * @tparam Args 
 * @param file 
 * @param line 
 * @param function 
 * @param lvl 
 * @param f 
 * @param args 
 */
template<typename... Args>
void getLog(char const *file, unsigned int line, char const * function, unsigned char lvl, const char * f, Args... args)
{
    
    if( static_cast<int>(lvl) < MAX_LEVEL )
    {
        char    *_Message = (char *)malloc(MAX_MESSAGE_LENGTH);

        if (_Message)
        {
            _elog.MutexLock.lock();

            // /full/path/to/file.c to file.c
            file = (strrchr(file, '/') ? strrchr(file, '/') + 1 : file);

            // get all args
            snprintf (_Message, MAX_MESSAGE_LENGTH-1, f, args...);

            #if LOG_CONSOLE_OR_FILE == 0
                _elog.writeLogToConsole(file,
                                    std::to_string(gettid()),
                                    function,
                                    std::to_string(line),
                                    _elog.LevelNames[ lvl ],
                                    _Message);
            #else
                _elog.writeLogToFile(_elog.addSpacesToConstChar(file, MAX_FILE_NAME_SIZE),
                                    _elog.addSpacesToUnsignedInt(gettid(), MAX_TID_SIZE),
                                    _elog.addSpacesToConstChar(function, MAX_FUNC_NAME_SIZE),
                                    _elog.addSpacesToUnsignedInt(line, MAX_LINE_SIZE),
                                    _elog.addSpacesToConstChar(_elog.LevelNames[ lvl ], MAX_LEVEL_SIZE),
                                    _Message);

                _elog.changeFile();
            #endif

            _elog.MutexLock.unlock();

            free(_Message);
        }
    }
}


#endif