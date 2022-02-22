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

    // Get user config

    mUserJson       = mConfigJson["user"];

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
	long long ms_since_epoch = std::chrono::duration_cast
                                <std::chrono::milliseconds>
                                (std::chrono::system_clock::now().time_since_epoch())
                                .count();
                                
	return std::to_string(ms_since_epoch);
}


/**
 * @brief Get old timestamp
 * 
 * @param day 
 * @param hour 
 * @param minute 
 * @param second 
 * @param millisecond 
 * @return std::string 
 */
std::string Utilities::getOldTimestamp(int day, int hour, int minute, int second, int millisecond)
{
    long long int mTimestamp = std::stol(getTimestamp());

    if (day != 0)
        mTimestamp = mTimestamp - day*24*60*60*1000;

    if (hour != 0)
        mTimestamp = mTimestamp - hour*60*60*1000;

    if (minute != 0)
        mTimestamp = mTimestamp - minute*60*1000;

    if (second != 0)
        mTimestamp = mTimestamp - second*1000;

    if (millisecond != 0)
        mTimestamp = mTimestamp - millisecond;

    return std::to_string(mTimestamp);
}


/**
 * @brief Get symbol
 * 
 * @return std::string 
 */
std::string Utilities::getSymbol()
{
    return mUserJson["symbol"].asString();
}


/**
 * @brief Get interval
 * 
 * @return std::string 
 */
std::string Utilities::getInterval()
{
    return mUserJson["interval"].asString();
}


/**
 * @brief Get balance symbol
 * 
 * @return std::string 
 */
std::string Utilities::getBalanceSymbol()
{
    return mUserJson["balance"]["symbol"].asString();
}


/**
 * @brief Get balance amount
 * 
 * @return std::string 
 */
std::string Utilities::getBalanceAmount()
{
    return mUserJson["balance"]["amount"].asString();
}


/**
 * @brief Upper to lower
 * 
 * @param data 
 * @return std::string 
 */
std::string Utilities::upperToLower(std::string data)
{
    for (int i = 0; i < static_cast<int>(data.length()); i++)
        data[i] = tolower(data[i]);

    return data;
}


/**
 * @brief Get average
 * 
 * @param vector 
 * @return float 
 */
float Utilities::getAverage(std::vector<float> vector)
{
    if (vector.empty())
    {
        ELOG(ERROR, "Vector is empty.");
        return 0;
    }

    int size = vector.size();
    float average;

    for (int i=0; i<size; i++)
    {
        average = average + vector[i];
    }

    average = average / size;

    return average;
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
    std::string symbol = upperToLower(mUserJson["symbol"].asString());
    std::string endpoint = "/ws/" + symbol + "@kline_" + mUserJson["interval"].asString();
        
    return endpoint;
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

    // ELOG(INFO, "Generated a signature: %s.", signature.c_str());

	return signature;
}