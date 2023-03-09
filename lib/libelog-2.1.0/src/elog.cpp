/**
 * @file elog.cpp
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 2.1.0
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../inc/elog.h"

/**
 * @brief Set the Log File Name Prefix object
 * 
 * @param name 
 */
void Elog::elog::setLogFileNamePrefix(std::string name)
{
    mLogFileNamePrefix = name;
}

/**
 * @brief Get the Log File Name Prefix object
 * 
 * @return std::string 
 */
std::string Elog::elog::getLogFileNamePrefix()
{
    return mLogFileNamePrefix;
}

/**
 * @brief Set the Log File Name Suffix object
 * 
 * @param name 
 */
void Elog::elog::setLogFileNameSuffix(std::string name)
{
    mLogFileNameSuffix = name;
}

/**
 * @brief Get the Log File Name Suffix object
 * 
 * @return std::string 
 */
std::string Elog::elog::getLogFileNameSuffix()
{
    return mLogFileNameSuffix;
}

/**
 * @brief Set the Max File Size object
 * 
 * @param size 
 */
void Elog::elog::setMaxFileSize(Elog::ByteSize size)
{
    mMaxFileSize = size;
}

/**
 * @brief Get the Max File Size object
 * 
 * @return uint32_t 
 */
uint32_t Elog::elog::getMaxFileSize()
{
    return mMaxFileSize;
}

/**
 * @brief Set the Log Console Or File object
 * 
 * @param selection 
 */
void Elog::elog::setLogConsoleOrFile(Elog::LogOutput selection)
{
    mLogConsoleOrFile = selection;
}

/**
 * @brief Get the Log Console Or File object
 * 
 * @return true File
 * @return false Console
 */
bool Elog::elog::getLogConsoleOrFile()
{
    return mLogConsoleOrFile;
}

/**
 * @brief Set the Max Log Level object
 * 
 * @param level 
 */
void Elog::elog::setMaxLogLevel(Elog::LogLevel level)
{
    mMaxLogLevel = level;
}

/**
 * @brief Get the Max Log Level object
 * 
 * @return uint8_t 
 */
uint8_t Elog::elog::getMaxLogLevel()
{
    return mMaxLogLevel;
}

/**
 * @brief Set the Max TID Size object
 * 
 * @param size 
 */
void Elog::elog::setMaxTIDSize(Elog::Size size)
{
    mMaxTIDSize = size;
}

/**
 * @brief Get the Max TID Size object
 * 
 * @return uint8_t 
 */
uint8_t Elog::elog::getMaxTIDSize()
{
    return mMaxTIDSize;
}

/**
 * @brief Set the Max Line Size object
 * 
 * @param size 
 */
void Elog::elog::setMaxLineSize(Elog::Size size)
{
    mMaxLineSize = size;
}

/**
 * @brief Get the Max Line Size object
 * 
 * @return uint8_t 
 */
uint8_t Elog::elog::getMaxLineSize()
{
    return mMaxLineSize;
}

/**
 * @brief Set the Max Level Size object
 * 
 * @param size 
 */
void Elog::elog::setMaxLevelSize(Elog::Size size)
{
    mMaxLevelSize = size;
}

/**
 * @brief Get the Max Level Size object
 * 
 * @return uint8_t 
 */
uint8_t Elog::elog::getMaxLevelSize()
{
    return mMaxLevelSize;
}

/**
 * @brief Set the Max File Name Size object
 * 
 * @param size 
 */
void Elog::elog::setMaxFileNameSize(Elog::Size size)
{
    mMaxFileNameSize = size;
}

/**
 * @brief Get the Max File Name Size object
 * 
 * @return uint8_t 
 */
uint8_t Elog::elog::getMaxFileNameSize()
{
    return mMaxFileNameSize;
}

/**
 * @brief Set the Max Func Name Size object
 * 
 * @param size 
 */
void Elog::elog::setMaxFuncNameSize(Elog::Size size)
{
    mMaxFuncNameSize = size;
}

/**
 * @brief Get the Max Func Name Size object
 * 
 * @return uint8_t 
 */
uint8_t Elog::elog::getMaxFuncNameSize()
{
    return mMaxFuncNameSize;
}

/**
 * @brief Set the Max Message Length object
 * 
 * @param length 
 */
void Elog::elog::setMaxMessageLength(Elog::ByteSize size)
{
    mMaxMessageLength = size;
}

/**
 * @brief Get the Max Message Length object
 * 
 * @return uint16_t 
 */
uint16_t Elog::elog::getMaxMessageLength()
{
    return mMaxMessageLength;
}

/**
 * @brief Write a head to log file
 * 
 */
void Elog::elog::addLogHeadToFile()
{
    if (mLogFile.is_open())
    {
        mLogFile << "[" << "            Date / Time"<< "]";
        mLogFile << "[" << addSpacesToConstChar("TID", mMaxTIDSize) << "]";
        mLogFile << "[" << addSpacesToConstChar("File", mMaxFileNameSize)<< "]";
        mLogFile << "[" << addSpacesToConstChar("Function", mMaxFuncNameSize) << "]";
        mLogFile << "[" << addSpacesToConstChar("Line", mMaxLineSize) << "]";
        mLogFile << "[" << addSpacesToConstChar("Level", mMaxLevelSize) << "]" << ": ";
        mLogFile << "[" << "Message" << "]" << std::endl;
        mLogFile.flush();
    }
}


/**
 * @brief Get current datetime
 * 
 * @return std::string 
 */
std::string Elog::elog::currentDateTime()
{
    const auto              p1 = std::chrono::system_clock::now();
    uint16_t                milliseconds = (std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count())%1000;
    time_t                  now = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
    struct tm               tstruct = *localtime(&now);
    char                    buf[30];

    strftime(buf, sizeof(buf), "%Y.%m.%d-%X", &tstruct);

    if (milliseconds < 10)
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), ".00%d", milliseconds);
    else if( 10 <= milliseconds && milliseconds < 100)
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), ".0%d", milliseconds);
    else
        snprintf(buf + strlen(buf), sizeof buf - strlen(buf), ".%d", milliseconds);

    return buf;
}


/**
 * @brief Return log file full name
 * 
 * @return std::string 
 */
std::string Elog::elog::getLogFileFullName()
{
    return mLogFilePath + mLogFileNamePrefix + mLogFileNameInfix + mLogFileNameSuffix;
}


/**
 * @brief Return log file size
 * 
 * @param fName 
 * @return std::ifstream::pos_type 
 */
std::ifstream::pos_type Elog::elog::fileSize(const char* fName)
{
    std::ifstream file(fName, std::ifstream::ate | std::ifstream::binary);

    return file.tellg();
}


/**
 * @brief Create a new log file
 * 
 */
void Elog::elog::changeFile()
{
    uint32_t _FileSize = fileSize((getLogFileFullName()).c_str());

    if (_FileSize >= mMaxFileSize)
    {
        mLogFile.close();

        mLogFileNameInfix = std::to_string(std::stoi(mLogFileNameInfix)+1);

        mLogFile.open(getLogFileFullName());

        addLogHeadToFile();
    }
}


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
void Elog::elog::writeLogToFile(std::string _TID, std::string _FileName, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message)
{
    if (mLogFile.is_open())
    {
        mLogFile << "[" << currentDateTime() << "]";
        mLogFile << "[" << _TID << "]";
        mLogFile << "[" << _FileName << "]";
        mLogFile << "[" << _FunctionName << "]";
        mLogFile << "[" << _Line << "]";
        mLogFile << "[" << _LevelNames  << "]" << ": ";
        mLogFile << _Message << std::endl;
        mLogFile.flush();
    }
    else
    {
        mLogFile.open(getLogFileFullName());

        addLogHeadToFile();

        snprintf (_Message, mMaxMessageLength-1, "Logging has been successfully started. Max log file size: %dKB", mMaxFileSize);

        writeLogToFile( addSpacesToUnsignedInt(gettid(), mMaxTIDSize),
                        addSpacesToConstChar("elog.cpp", mMaxFileNameSize),
                        addSpacesToConstChar("elog", mMaxFuncNameSize),
                        addSpacesToUnsignedInt(__LINE__, mMaxLineSize),
                        addSpacesToConstChar(LevelNames[ 3 ], mMaxLevelSize),
                        _Message);
    }
}


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
void Elog::elog::writeLogToConsole(std::string _TID, std::string _FileName, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message)
{
    LogConsole << "[" << currentDateTime() << "]";
    LogConsole << "[" << _TID << "]";
    LogConsole << "[" << _FileName << "]";
    LogConsole << "[" << _FunctionName << "]";
    LogConsole << "[" << _Line << "]";
    LogConsole << "[" << _LevelNames << "]" << ": ";
    LogConsole << _Message << std::endl;
}


/**
 * @brief Add spaces to const char*. It is used to change the function name and file name.
 *
 * @param getChar 
 * @param maxSize 
 * @return std::string 
 */
std::string Elog::elog::addSpacesToConstChar(const char* getChar, uint8_t maxSize)
{
    std::string     _StringWithSpace;

    if (strlen(getChar) < maxSize)
        for (size_t i = 0; i < maxSize - strlen(getChar); i++)
            _StringWithSpace = _StringWithSpace + " ";

    _StringWithSpace = _StringWithSpace + getChar;

    return _StringWithSpace;
}


/**
 * @brief Add spaces to unsigned int
 * 
 * @param getInt 
 * @param maxSize 
 * @return std::string 
 */
std::string Elog::elog::addSpacesToUnsignedInt(unsigned int getInt, uint8_t maxSize)
{
    std::string     _StringWithSpace;
    size_t          _IntLength = std::to_string(getInt).length();

    if (_IntLength < maxSize)
        for (size_t i = 0; i < maxSize - _IntLength; i++)
            _StringWithSpace = _StringWithSpace + " ";

    _StringWithSpace = _StringWithSpace + std::to_string(getInt);

    return _StringWithSpace;
}

Elog::elog _elog;