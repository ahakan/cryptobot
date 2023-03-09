/**
 * @file elog.h
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 2.1.0
 * @date 2022-07-08
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

#define ERROR                       __FILE__ , __LINE__ , __FUNCTION__ , 0
#define WARNING                     __FILE__ , __LINE__ , __FUNCTION__ , 1
#define DEBUG                       __FILE__ , __LINE__ , __FUNCTION__ , 2
#define INFO                        __FILE__ , __LINE__ , __FUNCTION__ , 3

#define __FILENAME__                (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LogConsole                  std::cout

/**
 * @brief Elog namespace
 * 
 */
namespace Elog
{
    /**
     * @brief Enum for the log output
     * 
     */
    typedef enum
    {
        Console,
        File
    } LogOutput;

    /**
     * @brief Enum for byte size
     * 
     */
    typedef enum
    {
        KB1 = 1024,
        KB2 = 2048,
        KB10 = 10240,
        KB50 = 51200,
        KB100 = 102400,
        KB500 = 512000,
        MB1 = 1048576,
        MB5 = 5242880,
        MB10 = 10485760,
        MB20 = 20971520,
        MB50 = 52428800
    } ByteSize;

    /**
     * @brief Enum for the log level
     *        Error = Error, 
     *        Warning = Error, Warning, 
     *        Debug = Error, Warning, Debug, 
     *        Info = Error, Warning, Debug, Info
     * 
     */
    typedef enum
    {
        Error,
        Warning,
        Debug,
        Info
    }LogLevel;

    /**
     * @brief Enum for general size
     * 
     */
    typedef enum
    {
        Zero,
        One,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Fifteen=15,
        Twenty=20,
        TwentyFive=25,
        Thirty=30,
        ThirtyFive=35,
        Forty=40,
        FortyFive=45,
        Fifty=50
    }Size;

    /**
     * @brief The log class
     * 
     */
    class elog
    {
    private:

        /**
         * @brief The log file object
         * 
         */
        std::ofstream               mLogFile;

        /**
         * @brief The log file path
         * 
         */
        std::string                 mLogFilePath = "";

        /**
         * @brief The log file prefix
         * 
         */
        std::string                 mLogFileNamePrefix = "00";

        /**
         * @brief The log file infix
         *        Must be defined as an integer
         * 
         */
        std::string                 mLogFileNameInfix = "1";

        /**
         * @brief The log file suffix
         * 
         */
        std::string                 mLogFileNameSuffix = ".log";

        /**
         * @brief The log file max size
         * 
         */
        uint32_t                    mMaxFileSize = Elog::ByteSize::MB10;

        /**
         * @brief false => Console, true => File
         * 
         */
        bool                        mLogConsoleOrFile = Elog::LogOutput::Console;

        /**
         * @brief 0 = Error, 
         *        1 = Error, Warning, 
         *        2 = Error, Warning, Debug, 
         *        3 = Error, Warning, Debug, Info
         * 
         */
        uint8_t                     mMaxLogLevel = Elog::LogLevel::Info;

        /**
         * @brief TID column max size
         * 
         */
        uint8_t                     mMaxTIDSize = Elog::Size::Six;

        /**
         * @brief Line column max size
         * 
         */
        uint8_t                     mMaxLineSize = Elog::Size::Five;

        /**
         * @brief Level column max size
         * 
         */
        uint8_t                     mMaxLevelSize = Elog::Size::Seven;

        /**
         * @brief File name column max size
         * 
         */
        uint8_t                     mMaxFileNameSize = Elog::Size::Fifteen;

        /**
         * @brief Function name column max size
         * 
         */
        uint8_t                     mMaxFuncNameSize = Elog::Size::Twenty;

        /**
         * @brief Message column max length
         * 
         */
        uint32_t                    mMaxMessageLength = Elog::ByteSize::KB2;

        /**
         * @brief Write a head to log file
         * 
         */
        void                        addLogHeadToFile();
        
        /**
         * @brief Get current datetime
         * 
         * @return std::string 
         */
        std::string                 currentDateTime();
        
        /**
         * @brief Return log file full name
         * 
         * @return std::string 
         */
        std::string                 getLogFileFullName();
        
        /**
         * @brief Return log file size
         * 
         * @param fName 
         * @return std::ifstream::pos_type 
         */
        std::ifstream::pos_type     fileSize(const char* fName);

    public:
        std::mutex                  MutexLock;

        char const*                 LevelNames[ 4 ] = { "ERROR", "WARNING", "DEBUG", "INFO" };

        /**
         * @brief Construct a new elog object
         * 
         */
        inline                      elog();

        /**
         * @brief Destroy the elog object
         * 
         */
        inline                      ~elog();

        /**
         * @brief Set the Log File Name Prefix object
         * 
         * @param name 
         */
        void                        setLogFileNamePrefix(std::string name);

        /**
         * @brief Get the Log File Name Prefix object
         * 
         * @return std::string 
         */
        std::string                 getLogFileNamePrefix();

        /**
         * @brief Set the Log File Name Suffix object
         * 
         * @param name 
         */
        void                        setLogFileNameSuffix(std::string name);

        /**
         * @brief Get the Log File Name Suffix object
         * 
         * @return std::string 
         */
        std::string                 getLogFileNameSuffix();

        /**
         * @brief Set the Log Console Or File object
         * 
         * @param selection 
         */
        void                        setLogConsoleOrFile(Elog::LogOutput selection);

        /**
         * @brief Get the Log Console Or File object
         * 
         * @return bool 
         */
        bool                        getLogConsoleOrFile();

        /**
         * @brief Set the Max File Size object
         * 
         * @param size 
         */
        void                        setMaxFileSize(Elog::ByteSize size);

        /**
         * @brief Get the Max File Size object
         * 
         * @return uint32_t 
         */
        uint32_t                    getMaxFileSize();

        /**
         * @brief Set the Max Log Level object
         * 
         * @param level 
         */
        void                        setMaxLogLevel(Elog::LogLevel level);

        /**
         * @brief Get the Max Log Level object
         * 
         * @return uint8_t 
         */
        uint8_t                     getMaxLogLevel();

        /**
         * @brief Set the Max TID Size object
         * 
         * @param size 
         */
        void                        setMaxTIDSize(Elog::Size size);

        /**
         * @brief Get the Max TID Size object
         * 
         * @return uint8_t 
         */
        uint8_t                     getMaxTIDSize();

        /**
         * @brief Set the Max Line Size object
         * 
         * @param size 
         */
        void                        setMaxLineSize(Elog::Size size);

        /**
         * @brief Get the Max Line Size object
         * 
         * @return uint8_t 
         */
        uint8_t                     getMaxLineSize();

        /**
         * @brief Set the Max Level Size object
         * 
         * @param size 
         */
        void                        setMaxLevelSize(Elog::Size size);

        /**
         * @brief Get the Max Level Size object
         * 
         * @return uint8_t 
         */
        uint8_t                     getMaxLevelSize();

        /**
         * @brief Set the Max File Name Size object
         * 
         * @param size 
         */
        void                        setMaxFileNameSize(Elog::Size size);

        /**
         * @brief Get the Max File Name Size object
         * 
         * @return uint8_t 
         */
        uint8_t                     getMaxFileNameSize();

        /**
         * @brief Set the Max Func Name Size object
         * 
         * @param size 
         */
        void                        setMaxFuncNameSize(Elog::Size size);

        /**
         * @brief Get the Max Func Name Size object
         * 
         * @return uint8_t 
         */
        uint8_t                     getMaxFuncNameSize();

        /**
         * @brief Set the Max Message Length object
         * 
         * @param length 
         */
        void                        setMaxMessageLength(Elog::ByteSize size);

        /**
         * @brief Get the Max Message Length object
         * 
         * @return uint16_t 
         */
        uint16_t                    getMaxMessageLength();


        /**
         * @brief Create a new log file
         * 
         */
        void                        changeFile();

        /**
         * @brief Write a log message to log file
         * 
         * @param _FileName 
         * @param _TID 
         * @param _FunctionName 
         * @param _Line 
         * @param _LevelNames 
         * @param _Message 
         */
        void                        writeLogToFile(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message);
        
        /**
         * @brief Write a log message to console
         * 
         * @param _FileName 
         * @param _TID 
         * @param _FunctionName 
         * @param _Line 
         * @param _LevelNames 
         * @param _Message 
         */
        void                        writeLogToConsole(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message);
        
        /**
         * @brief Add spaces to const char*. It is used to change the function name and file name.
         *
         * @param getChar 
         * @param maxSize 
         * @return std::string 
         */
        std::string                 addSpacesToConstChar(const char* getChar, uint8_t maxSize);
        
        /**
         * @brief Add spaces to unsigned int
         * 
         * @param getInt 
         * @param maxSize 
         * @return std::string 
         */
        std::string                 addSpacesToUnsignedInt(unsigned int getInt, uint8_t maxSize);
    };

}


/**
 * @brief Construct a new Elog::elog::elog object
 * 
 */
inline Elog::elog::elog()
{
}


/**
 * @brief Destroy the Elog::elog::elog object
 * 
 */
inline Elog::elog::~elog()
{
    char            *_Message       = (char *)malloc(mMaxMessageLength);
    const char*     _FileName       = __FILENAME__;
    const char*     _FunctionName   = __FUNCTION__;

    if (_Message)
    {
        if (getLogConsoleOrFile() == 0)
        {
            snprintf (_Message, 255, "Logging has been successfully terminated.");

            writeLogToConsole(  std::to_string(gettid()),
                                _FileName,
                                _FunctionName,
                                std::to_string(__LINE__),
                                LevelNames[ 3 ],
                                _Message);
        }
        else
        {
            snprintf (_Message, mMaxMessageLength-1, "Logging has been successfully terminated. Total log file: %s", mLogFileNameInfix.c_str());

            writeLogToFile( addSpacesToUnsignedInt(gettid(), mMaxTIDSize),
                            addSpacesToConstChar(_FileName, mMaxFileNameSize),
                            addSpacesToConstChar(_FunctionName, mMaxFuncNameSize),
                            addSpacesToUnsignedInt(__LINE__, mMaxLineSize),
                            addSpacesToConstChar(LevelNames[ 3 ], mMaxLevelSize),
                            _Message);

            mLogFile.close();
        }

        free(_Message);
    }
}
  

extern Elog::elog _elog;


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
    if( static_cast<uint8_t>(lvl) <= _elog.getMaxLogLevel() )
    {
        char    *_Message = (char *)malloc(_elog.getMaxMessageLength());

        if (_Message)
        {
            _elog.MutexLock.lock();

            // /full/path/to/file.c to file.c
            file = (strrchr(file, '/') ? strrchr(file, '/') + 1 : file);

            // get all args
            snprintf (_Message, _elog.getMaxMessageLength()-1, f, args...);

            if (_elog.getLogConsoleOrFile() == 0)
            {
                _elog.writeLogToConsole(file,
                                        std::to_string(gettid()),
                                        function,
                                        std::to_string(line),
                                        _elog.LevelNames[ lvl ],
                                        _Message);
            }
            else
            {
                _elog.writeLogToFile(_elog.addSpacesToUnsignedInt(gettid(), _elog.getMaxTIDSize()),
                                     _elog.addSpacesToConstChar(file, _elog.getMaxFileNameSize()),
                                     _elog.addSpacesToConstChar(function, _elog.getMaxFuncNameSize()),
                                     _elog.addSpacesToUnsignedInt(line, _elog.getMaxLineSize()),
                                     _elog.addSpacesToConstChar(_elog.LevelNames[ lvl ], _elog.getMaxLevelSize()),
                                     _Message);

                _elog.changeFile();
            }

            _elog.MutexLock.unlock();

            free(_Message);
        }
    }
}


#define ELOG getLog
#define ELOG_LEVEL _elog.setMaxLogLevel
#define ELOG_OUTPUT _elog.setLogConsoleOrFile
#define ELOG_FILESIZE _elog.setMaxFileSize
#define ELOG_FILEPREFIX _elog.setLogFileNamePrefix
#define ELOG_FILESUFFIX _elog.setLogFileNameSuffix
#define ELOG_TIDCOLSIZE _elog.setMaxTIDSize
#define ELOG_LINECOLSIZE _elog.setMaxLineSize
#define ELOG_LEVELCOLSIZE _elog.setMaxLevelSize
#define ELOG_FILENAMECOLSIZE _elog.setMaxFileNameSize
#define ELOG_FUNCNAMECOLSIZE _elog.setMaxFuncNameSize
#define ELOG_MESSAGELENGTH _elog.setMaxMessageLength

#endif