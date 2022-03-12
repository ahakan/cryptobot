#include "../inc/utilities.h"
#include <string>


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

    // Get trade config
    mTradeJson      = mConfigJson["trade"];

    // Get preferred exchange name
    mExchangeName   = mConfigJson["trade"]["exchange"].asString();

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
std::string Utilities::getOldTimestamp()
{
    int mRSISize                = getRSIPeriod();

    std::string mInterval       = getInterval();

    long long int mTimestamp    = std::stol(getTimestamp());

    if (mInterval == "1m")  mTimestamp  -= 60*mRSISize*1000;
    if (mInterval == "3m")  mTimestamp  -= 60*mRSISize*3*1000;
    if (mInterval == "5m")  mTimestamp  -= 60*mRSISize*5*1000;
    if (mInterval == "15m") mTimestamp  -= 60*mRSISize*15*1000;
    if (mInterval == "30m") mTimestamp  -= 60*mRSISize*30*1000;
    if (mInterval == "1h")  mTimestamp  -= 60*mRSISize*60*1000;
    if (mInterval == "2h")  mTimestamp  -= 60*mRSISize*120*1000;
    if (mInterval == "4h")  mTimestamp  -= 60*mRSISize*240*1000;
    if (mInterval == "6h")  mTimestamp  -= 60*mRSISize*360*1000;
    if (mInterval == "8h")  mTimestamp  -= 60*mRSISize*480*1000;
    if (mInterval == "12h") mTimestamp  -= 60*mRSISize*720*1000;
    if (mInterval == "1d")  mTimestamp  -= 60*mRSISize*1440*1000;
    if (mInterval == "3d")  mTimestamp  -= 60*mRSISize*4320*1000;
    if (mInterval == "1w")  mTimestamp  -= 60*mRSISize*10080*1000;

    return std::to_string(mTimestamp);
}


/**
 * @brief Get symbol
 * 
 * @return std::string 
 */
std::string Utilities::getSymbol()
{
    return mTradeJson["symbol"].asString();
}


/**
 * @brief Get follow symbol
 * 
 * @return std::string 
 */
std::string Utilities::getFollowSymbol()
{
    return mTradeJson["follow"]["symbol"].asString();
}


/**
 * @brief Get interval
 * 
 * @return std::string 
 */
std::string Utilities::getInterval()
{
    return mTradeJson["interval"].asString();
}


/**
 * @brief Get quantity
 * 
 * @return std::string 
 */
std::string Utilities::getQuantity()
{
    return mTradeJson["quantity"].asString();
}


/**
 * @brief Get balance symbol
 * 
 * @return std::string 
 */
std::string Utilities::getBalanceSymbol()
{
    return mTradeJson["balance"]["symbol"].asString();
}


/**
 * @brief Get balance amount
 * 
 * @return std::string 
 */
std::string Utilities::getBalanceAmount()
{
    return mTradeJson["balance"]["amount"].asString();
}


/**
 * @brief Get average amount
 * 
 * @return std::string 
 */
std::string Utilities::getAverageAmount()
{
    return mTradeJson["average"]["amount"].asString();
}


/**
 * @brief Get average auto calculate
 * 
 * @return true 
 * @return false 
 */
bool Utilities::getAverageAutoCalculate()
{
    return mTradeJson["average"]["auto-calculate"].asBool();
}


/**
 * @brief Get RSI period
 * 
 * @return int 
 */
int Utilities::getRSIPeriod()
{
    return mTradeJson["RSI"]["period"].asInt();
}


/**
 * @brief Get RSI oversold
 * 
 * @return std::string 
 */
std::string Utilities::getRSIOversold()
{
    return mTradeJson["RSI"]["oversold"].asString();
}


/**
 * @brief Get RSI overbought
 * 
 * @return std::string 
 */
std::string Utilities::getRSIOverbought()
{
    return mTradeJson["RSI"]["overbought"].asString();
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
 * @brief Round price
 * 
 * @param price 
 * @param tickSize 
 * @return std::string 
 */
std::string Utilities::roundPrice(std::string price, int tickSize)
{
    for (int i=static_cast<int>(price.size())-1; i>=0; i--)
    {
        if (price[i-tickSize] == 46)                        // 46=>. ASCII
            break;

        price[i] = 48;                                      // 48=>0 ASCII
    }

    return price;
}


/**
 * @brief Calculate average
 * 
 * @param vector 
 * @return std::string 
 */
std::string Utilities::calculateAverage(std::vector<std::string> vector)
{
    if (vector.empty())
    {
        ELOG(ERROR, "Average Vector is empty.");
        return 0;
    }

    int size        = vector.size();
    float average   = 0;

    for (int i=0; i<size; i++)
    {
        average += std::stof(vector[i]);
    }

    average = average / size;

    return std::to_string(average);
}


/**
 * @brief Calculate RSI
 * 
 * @param vector 
 * @param period 
 * @return std::string 
 */
std::string Utilities::calculateRSI(std::vector<std::string> vector)
{
    if (vector.empty())
    {
        ELOG(ERROR, "RSI Vector is empty.");
        return 0;
    }

    int size            = vector.size();
    float sumGain       = 0;
    float sumLoss       = 0;

    for (int i=size-1; i>0; i--)
    {
        float difference = std::stof(vector[i]) - std::stof(vector[i-1]);

        if (difference >= 0)
        {
            sumGain += difference;
        }
        else
        {
            sumLoss -= difference;
        }
    }

    if (sumGain == 0) return "0";

    float relativeStrength = sumGain / sumLoss;

    std::string RSI = std::to_string(100.0 - (100.0 / (1 + relativeStrength)));

    return RSI;
}


/**
 * @brief Add two strings
 * 
 * @param number1 
 * @param number2 
 * @return std::string 
 */
std::string Utilities::addTwoStrings(std::string number1, std::string number2)
{
    if (number1.length() > 0 && number2.length() > 0)
    {
        float num1          = std::stof(number1);
        float num2          = std::stof(number2);

        return std::to_string(num1+num2);
    }

    ELOG(ERROR, "Some parameters is empty. Number 1: %S, Number 2: %s.", number1.c_str(), number2.c_str());
    
    return "ERROR";
}


/**
 * @brief Subs two strings
 * 
 * @param number1 
 * @param number2 
 * @return std::string 
 */
std::string Utilities::subTwoStrings(std::string number1, std::string number2)
{
    if (number1.length() > 0 && number2.length() > 0)
    {
        float num1          = std::stof(number1);
        float num2          = std::stof(number2);

        return (num1 >= num2) ? std::to_string(num1-num2) : std::to_string(num2-num1);
    }
    
    ELOG(ERROR, "Some parameters is empty. Number 1: %S, Number 2: %s.", number1.c_str(), number2.c_str());
    
    return "ERROR";
}


/**
 * @brief Compare two price if firstPrice high return true, if secondPrice high return false
 * 
 * @param firstPrice 
 * @param secondPrice 
 * @return true 
 * @return false 
 */
bool Utilities::comparePrice(std::string firstPrice, std::string secondPrice)
{
    if (firstPrice.length() > 0 && secondPrice.length() > 0)
    {
        float first     = std::stof(firstPrice);

        float second    = std::stof(secondPrice);

        return (first >= second) ? true : false;
    }
    
    ELOG(ERROR, "Some parameters is empty. First Price: %S, Second Price: %s.", firstPrice.c_str(), secondPrice.c_str());

    return false;
}


/**
 * @brief Get tick size
 * 
 * @param data 
 * @return int 
 */
int Utilities::getTickSize(std::string data)
{
    int tickSize = 10;                  // Why 10? Because data has \n and " => +2 

    for (int i=static_cast<int>(data.size())-1; i>=0; i--)
    {
        if (data[i] == 49)              // 49=>1 ASCII
            break;

        tickSize--;
    }

    return tickSize;
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
 * @brief Return trade symbol endpoint
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketEndpointT()
{
    std::string symbol = upperToLower(getSymbol());
    std::string endpoint = "/ws/" + symbol + "@kline_" + getInterval();
        
    return endpoint;
}


/**
 * @brief Return follow symbol endpoint
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketEndpointF()
{
    std::string symbol = upperToLower(getFollowSymbol());
    std::string endpoint = "/ws/" + symbol + "@kline_" + getInterval();
        
    return endpoint;
}


/**
 * @brief Return api base
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getAPIBase()
{
    std::string mAPIBase = "https://" + mAPIJson["base"].asString();

    return mAPIBase;
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