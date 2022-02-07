#include "../inc/utilities.h"


/**
 * @brief Construct a new Utilities::Utilities object
 * 
 */
Utilities::Utilities()
{
    // Log file init
    mConfigFile.open("config.json", std::ifstream::binary);

    mConfigFile >> mConfigJson;

    if (mConfigJson.size() == 0)
    {
        ELOG(ERROR, "Failed to initialize Utilities constructor.");
        exit(0);
    }

    // Get preferred exchange name
    mExchangeName   = mConfigJson["user"]["exchange"]["name"].asString();

    // Get exchange config
    mExchangeJson   = mConfigJson[mExchangeName];

    ELOG(INFO, "Utilities constructor initialized.");

    
}


/**
 * @brief Destroy the Utilities::Utilities object
 * 
 */
Utilities::~Utilities()
{
    ELOG(INFO, "Utilities destructor.");
}

/**
 * @brief Construct a new WebsocketUtils::WebsocketUtils object
 * 
 */
WebsocketUtils::WebsocketUtils()
{
    mWebsocketJson = mExchangeJson["websocket"];

    if (mWebsocketJson.size() == 0)
        ELOG(ERROR, "Failed to initialize WebSocketUtils constructor.");

    ELOG(INFO, "WebSocketUtils constructor initialized.");
}


/**
 * @brief Destroy the WebsocketUtils::WebsocketUtils object
 * 
 */
WebsocketUtils::~WebsocketUtils()
{
    ELOG(INFO, "WebsocketUtils destructor.");
}


/**
 * @brief Return websocket base
 * 
 * @return std::string 
 */
std::string WebsocketUtils::getBase()
{
    return mWebsocketJson["base"].asString();
}


/**
 * @brief Return port
 * 
 * @return std::string 
 */
std::string WebsocketUtils::getPort()
{
    return mWebsocketJson["port"].asString();
}


/**
 * @brief Return endpoint
 * 
 * @return std::string 
 */
std::string WebsocketUtils::getEndpoint()
{
    return mWebsocketJson["endpoint"].asString();
}


/**
 * @brief Construct a new RequestsUtils:: RequestsUtils object
 * 
 */
RequestsUtils::RequestsUtils()
{
    mRequestsJson = mExchangeJson["api"];

    if (mRequestsJson.size() == 0)
        ELOG(ERROR, "Failed to initialize RequestsUtils constructor.");

    ELOG(INFO, "RequestsUtils constructor initialized.");
}


/**
 * @brief Destroy the RequestsUtils:: RequestsUtils object
 * 
 */
RequestsUtils::~RequestsUtils()
{
    ELOG(INFO, "RequestsUtils destructor.");
}


/**
 * @brief Return api base
 * 
 * @return std::string 
 */
std::string RequestsUtils::getBase()
{
    return mRequestsJson["base"].asString();
}


/**
 * @brief Return API KEY
 * 
 * @return std::string 
 */
std::string RequestsUtils::getAPIKEY()
{
    return mRequestsJson["API_KEY"].asString();
}


/**
 * @brief Return SECRET KEY
 * 
 * @return std::string 
 */
std::string RequestsUtils::getSECRETKEY()
{
    return mRequestsJson["SECRET_KEY"].asString();
}
