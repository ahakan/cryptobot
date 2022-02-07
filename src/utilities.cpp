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
 * @brief Return timestamp
 * 
 * @return std::string 
 */
std::string Utilities::getTimestamp() {
	long long ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return std::to_string(ms_since_epoch);
}


/**
 * @brief Construct a new BinanceUtilities::BinanceUtilities object
 * 
 */
BinanceUtilities::BinanceUtilities()
{
    mWebsocketJson  = mExchangeJson["websocket"];
    mAPIJson        = mExchangeJson["api"];

    if (mWebsocketJson.size() == 0 || mAPIJson.size() == 0)
    {
        ELOG(ERROR, "Failed to initialize BinanceUtilities constructor.");
        exit(0);
    }
    ELOG(INFO, "BinanceUtilities constructor initialized.");
}


/**
 * @brief Destroy the BinanceUtilities::BinanceUtilities object
 * 
 */
BinanceUtilities::~BinanceUtilities()
{
    ELOG(INFO, "BinanceUtilities destructor.");
}


/**
 * @brief Return websocket base
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketBase()
{
    return mWebsocketJson["base"].asString();
}


/**
 * @brief Return port
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketPort()
{
    return mWebsocketJson["port"].asString();
}


/**
 * @brief Return endpoint
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketEndpoint()
{
    return mWebsocketJson["endpoint"].asString();
}

/**
 * @brief Return api base
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getAPIBase()
{
    return mAPIJson["base"].asString();
}


/**
 * @brief Return API KEY
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getAPIKEY()
{
    return mAPIJson["API_KEY"].asString();
}


/**
 * @brief Return SECRET KEY
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getAPISECRETKEY()
{
    return mAPIJson["SECRET_KEY"].asString();
}


/**
 * @brief encrypt with hmac
 * 
 * @param key 
 * @param data 
 * @return std::string 
 */
std::string BinanceUtilities::encryptWithHMAC(const char* key, const char* data) {
    int result_len = 32;
    unsigned char *result;
    static char res_hexstring[64];
    std::string signature;

    result = HMAC(EVP_sha256(), key, strlen((char *)key), const_cast<unsigned char *>(reinterpret_cast<const unsigned char*>(data)), strlen((char *)data), NULL, NULL);
  	for (int i = 0; i < result_len; i++) {
    	sprintf(&(res_hexstring[i * 2]), "%02x", result[i]);
  	}

  	for (int i = 0; i < 64; i++) {
  		signature += res_hexstring[i];
  	}

  	return signature;
}


/**
 * @brief generate a signature
 * 
 * @param query 
 * @return std::string 
 */
std::string BinanceUtilities::getSignature(std::string query) {
    std::string mSecretKey  = getAPISECRETKEY();
	std::string signature   = encryptWithHMAC(mSecretKey.c_str(), query.c_str());

    ELOG(INFO, "Generated a signature: %s.", signature.c_str());

	return signature;
}