/**
 * @file elog.cpp
 * @author ahc (ahmethakan@pm.me)
 * @brief 
 * @version 0.1
 * @date 2022-03-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../inc/elog.h"


/**
 * @brief Write a head to log file
 * 
 */
void elog::addLogHeadToFile()
{
    if (LogFile.is_open())
    {
        LogFile << "[" << "            Date / Time"<< "]";
        LogFile << "[" << addSpacesToConstChar("File", MAX_FILE_NAME_SIZE)<< "]";
        LogFile << "[" << addSpacesToConstChar("TID", MAX_TID_SIZE) << "]";
        LogFile << "[" << addSpacesToConstChar("Function", MAX_FUNC_NAME_SIZE) << "]";
        LogFile << "[" << addSpacesToConstChar("Line", MAX_LINE_SIZE) << "]";
        LogFile << "[" << addSpacesToConstChar("Level", MAX_LEVEL_SIZE) << "]" << ": ";
        LogFile << "[" << "Message" << "]" << std::endl;
        LogFile.flush();
    }
}


/**
 * @brief Get current datetime
 * 
 * @return std::string 
 */
std::string elog::currentDateTime()
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
std::string elog::getLogFileFullName()
{
    return LogFilePath + LogFileNamePrefix + LogFileNameInfix + LogFileNameSuffix;
}


/**
 * @brief Return log file size
 * 
 * @param fName 
 * @return std::ifstream::pos_type 
 */
std::ifstream::pos_type elog::fileSize(const char* fName)
{
    std::ifstream file(fName, std::ifstream::ate | std::ifstream::binary);

    return file.tellg();
}


/**
 * @brief Create a new log file
 * 
 */
void elog::changeFile()
{
    uint32_t _FileSize = fileSize((getLogFileFullName()).c_str());

    if (_FileSize >= MAX_FILE_SIZE)
    {
        LogFile.close();

        LogFileNameInfix = std::to_string(std::stoi(LogFileNameInfix)+1);

        LogFile.open(getLogFileFullName());

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
void elog::writeLogToFile(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message)
{
    if (LogFile.is_open())
    {
        LogFile << "[" << currentDateTime() << "]";
        LogFile << "[" << _FileName << "]";
        LogFile << "[" << _TID << "]";
        LogFile << "[" << _FunctionName << "]";
        LogFile << "[" << _Line << "]";
        LogFile << "[" << _LevelNames  << "]" << ": ";
        LogFile << _Message << std::endl;
        LogFile.flush();
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
void elog::writeLogToConsole(std::string _FileName, std::string _TID, std::string _FunctionName, std::string _Line, std::string _LevelNames, char* _Message)
{
    LogConsole << "[" << currentDateTime() << "]";
    LogConsole << "[" << _FileName << "]";
    LogConsole << "[" << _TID << "]";
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
std::string elog::addSpacesToConstChar(const char* getChar, uint8_t maxSize)
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
std::string elog::addSpacesToUnsignedInt(unsigned int getInt, uint8_t maxSize)
{
    std::string     _StringWithSpace;
    size_t          _IntLength = std::to_string(getInt).length();

    if (_IntLength < maxSize)
        for (size_t i = 0; i < maxSize - _IntLength; i++)
            _StringWithSpace = _StringWithSpace + " ";

    _StringWithSpace = _StringWithSpace + std::to_string(getInt);

    return _StringWithSpace;
}

elog _elog;