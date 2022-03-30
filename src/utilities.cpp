#include "../inc/utilities.h"


/**
 * @brief Construct a new Utilities::Utilities object
 * 
 */
Utilities::Utilities()
{
    // Log file init
    mConfigFile.open("config.json", std::ifstream::binary);

    if (!mConfigFile.is_open())
    {
        ELOG(ERROR, "Failed to open Config file.");

        setExitSignal(0);
    }

    ELOG(INFO, "Configuration file opened successfully.");

    mConfigFile >> mConfigJson;

    if (mConfigJson.size() == 0)
    {
        ELOG(ERROR, "Failed to initialize Utilities constructor.");

        setExitSignal(0);
    }

    // Get trade config
    mTradeJson      = mConfigJson["trade"];

    if (mTradeJson.size() == 0)
    {
        ELOG(ERROR, "Failed to parse Trade JSON");
        
        setExitSignal(0);
    }

    // Get preferred exchange name
    mExchangeName   = mConfigJson["trade"]["exchange"].asString();

    if (mExchangeName.size() == 0)
    {
        ELOG(ERROR, "Failed to parse Exchange Name");
        
        setExitSignal(0);
    }

    // Get exchange config
    mExchangeJson   = mConfigJson[mExchangeName];

    if (mExchangeJson.size() == 0)
    {
        ELOG(ERROR, "Failed to parse Exchange JSON");
        
        setExitSignal(0);
    }

    // Get server config
    mWebserverJson   = mConfigJson["server"];

    if (mWebserverJson.size() == 0)
    {
        ELOG(ERROR, "Failed to parse Exchange JSON");
        
        setExitSignal(0);
    }

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
 * @brief Set exit signal
 * 
 * @param signal 
 */
void Utilities::setExitSignal(bool signal)
{
    Opel *iOpel = Opel::instance();

    if (iOpel->getExitSignal() != 0)
    {
        iOpel->setExitSignal(signal);

        ELOG(WARNING, "Sending a exit signal.");
    }

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

    if (mInterval == "1m")  return std::to_string(mTimestamp  -= 60*mRSISize*1000);
    if (mInterval == "3m")  return std::to_string(mTimestamp  -= 60*mRSISize*3*1000);
    if (mInterval == "5m")  return std::to_string(mTimestamp  -= 60*mRSISize*5*1000);
    if (mInterval == "15m") return std::to_string(mTimestamp  -= 60*mRSISize*15*1000);
    if (mInterval == "30m") return std::to_string(mTimestamp  -= 60*mRSISize*30*1000);
    if (mInterval == "1h")  return std::to_string(mTimestamp  -= 60*mRSISize*60*1000);
    if (mInterval == "2h")  return std::to_string(mTimestamp  -= 60*mRSISize*120*1000);
    if (mInterval == "4h")  return std::to_string(mTimestamp  -= 60*mRSISize*240*1000);
    if (mInterval == "6h")  return std::to_string(mTimestamp  -= 60*mRSISize*360*1000);
    if (mInterval == "8h")  return std::to_string(mTimestamp  -= 60*mRSISize*480*1000);
    if (mInterval == "12h") return std::to_string(mTimestamp  -= 60*mRSISize*720*1000);
    if (mInterval == "1d")  return std::to_string(mTimestamp  -= 60*mRSISize*1440*1000);
    if (mInterval == "3d")  return std::to_string(mTimestamp  -= 60*mRSISize*4320*1000);
    if (mInterval == "1w")  return std::to_string(mTimestamp  -= 60*mRSISize*10080*1000);
    
    ELOG(ERROR, "Failed to calculate old timestamp.");

    return std::to_string(mTimestamp);
}


/**
 * @brief Get coin symbol
 * 
 * @return std::string 
 */
std::string Utilities::getCoinSymbol()
{
    std::string mTradeSymbol    = getTradeSymbol();

    std::string mBalanceSymbol  = getBalanceSymbol();

    std::string coinSymbol      = mTradeSymbol.substr(0, mTradeSymbol.find(mBalanceSymbol));

    if (coinSymbol.length() == 0)
    {
        ELOG(ERROR, "Failed to parse Coin Symbol.");

        setExitSignal(0);
    }

    return coinSymbol;
}


/**
 * @brief Get trade symbol
 * 
 * @return std::string 
 */
std::string Utilities::getTradeSymbol()
{
    std::string mTradeSymbol = mTradeJson["symbol"].asString();
    
    if (mTradeSymbol.length() == 0)
    {
        ELOG(ERROR, "Failed to parse Trade Symbol.");

        setExitSignal(0);
    }

    return mTradeSymbol;
}


/**
 * @brief Get follow symbol
 * 
 * @return std::string 
 */
std::string Utilities::getFollowSymbol()
{
    std::string mFollowSymbol = mTradeJson["follow"]["symbol"].asString();

    if (mFollowSymbol.length() == 0)
    {
        ELOG(ERROR, "Failed to parse Follow Symbol.");

        setExitSignal(0);
    }

    return mFollowSymbol;
}


/**
 * @brief Get interval
 * 
 * @return std::string 
 */
std::string Utilities::getInterval()
{
    std::string mInterval = mTradeJson["interval"].asString();

    if (mInterval.length() == 0)
    {
        ELOG(ERROR, "Failed to parse interval.");

        setExitSignal(0);
    }

    return mInterval;
}


/**
 * @brief Get quantity
 * 
 * @return std::string 
 */
std::string Utilities::getQuantity()
{
    std::string mQuantity = mTradeJson["quantity"].asString();

    if (mQuantity.length() == 0)
    {
        ELOG(ERROR, "Failed to parse quantity.");

        setExitSignal(0);
    }

    return mQuantity;
}


/**
 * @brief Get balance symbol
 * 
 * @return std::string 
 */
std::string Utilities::getBalanceSymbol()
{
    std::string mBalanceSymbol = mTradeJson["balance"]["symbol"].asString();

    if (mBalanceSymbol.length() == 0)
    {
        ELOG(ERROR, "Failed to parse balance symbol.");

        setExitSignal(0);
    }

    return mBalanceSymbol;
}


/**
 * @brief Get balance amount
 * 
 * @return std::string 
 */
std::string Utilities::getBalanceAmount()
{
    std::string mBalanceAmount = mTradeJson["balance"]["amount"].asString();

    if (mBalanceAmount.length() == 0)
    {
        ELOG(ERROR, "Failed to parse balance amount.");

        setExitSignal(0);
    }

    return mBalanceAmount;
}


/**
 * @brief Get average amount
 * 
 * @return std::string 
 */
std::string Utilities::getAverageAmount()
{
    std::string mAverageAmount = mTradeJson["average"]["amount"].asString();

    if (mAverageAmount.length() == 0)
    {
        ELOG(ERROR, "Failed to parse average amount.");

        setExitSignal(0);
    }

    return mAverageAmount;
}


/**
 * @brief Get average auto calculate
 * 
 * @return true 
 * @return false 
 */
bool Utilities::getAverageAutoCalculate()
{
    bool mAutoCalculate = mTradeJson["average"]["auto-calculate"].asBool();

    if (mAutoCalculate != 0 && mAutoCalculate != 1)
    {
        ELOG(ERROR, "Failed to parse average auto calculate.");

        setExitSignal(0);
    }

    return mAutoCalculate;
}


/**
 * @brief Get RSI period
 * 
 * @return int 
 */
int Utilities::getRSIPeriod()
{
    int mPeriod = mTradeJson["RSI"]["period"].asInt();

    if (mPeriod < 0 || mPeriod > 97)
    {
        ELOG(ERROR, "Failed to parse RSI period. Period, must be between (1-96).");

        setExitSignal(0);
    }

    return mPeriod;
}


/**
 * @brief Get RSI oversold
 * 
 * @return std::string 
 */
std::string Utilities::getRSIOversold()
{
    std::string mOversold = mTradeJson["RSI"]["oversold"].asString();

    if (mOversold.length() == 0)
    {
        ELOG(ERROR, "Failed to parse RSI oversold.");

        setExitSignal(0);
    }

    return mOversold;
}


/**
 * @brief Get RSI overbought
 * 
 * @return std::string 
 */
std::string Utilities::getRSIOverbought()
{
    std::string mOverbought = mTradeJson["RSI"]["overbought"].asString();

    if (mOverbought.length() == 0)
    {
        ELOG(ERROR, "Failed to parse RSI overebought.");

        setExitSignal(0);
    }

    return mOverbought;
}


/**
 * @brief Get server base
 * 
 * @return std::string 
 */
std::string Utilities::getWebserverBase()
{
    std::string mWebserverBase = mWebserverJson["base"].asString();

    if (mWebserverBase.length() == 0)
    {
        ELOG(ERROR, "Failed to parse server base.");

        setExitSignal(0);
    }

    return mWebserverBase;
}


/**
 * @brief Get server socket
 * 
 * @return unsigned short 
 */
unsigned short Utilities::getWebserverPort()
{
    unsigned short mWebserverPort = mWebserverJson["port"].asUInt();

    if (mWebserverPort <= 0)
    {
        ELOG(ERROR, "Failed to parse server port.");

        setExitSignal(0);
    }

    return mWebserverPort;
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
        
        setExitSignal(0);
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
        
        setExitSignal(0);
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
std::string Utilities::roundString(std::string price, int tickSize)
{
    int size = 0;

    for (int i = static_cast<int>(price.size())-1; i >= 0; i--)
    {
        if (price[i] == 46)                                     // 46=>ASCII = . 
            break;

        size++;
    }

    if (size <= tickSize)
    {
        for (int i = tickSize; i > size; i--)
        {
            price = price + "0";
        }
    }
    else
    {
        for (int i = static_cast<int>(price.size())-1; i >=0 ; i--)
        {
            if (price[i-tickSize] == 46)                        // 46=>ASCII = . 
                break;

            price[i] = 48;                                      // 48=>ASCII = 0 
        }
    }

    return price;
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

    ELOG(ERROR, "Add two strings. Some parameters is empty. Number 1: %S, Number 2: %s.", number1.c_str(), number2.c_str());
    
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
    
    ELOG(ERROR, "Subs two strings. Some parameters is empty. Number 1: %S, Number 2: %s.", number1.c_str(), number2.c_str());
    
    return "ERROR";
}


/**
 * @brief Multiply two strings
 * 
 * @param number1 
 * @param number2 
 * @return std::string 
 */
std::string Utilities::multiplyTwoStrings(std::string number1, std::string number2)
{
    if (number1.length() > 0 && number2.length() > 0)
    {
        float num1          = std::stof(number1);
        float total         = 0;

        for (int i=0; i<std::stoi(number2); i++)
        {
            total = total + num1;
        }

        return  std::to_string(total);
    }
    
    ELOG(ERROR, "Multiply two strings. Some parameters is empty. Number 1: %S, Number 2: %s.", number1.c_str(), number2.c_str());
    
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
bool Utilities::compareTwoStrings(std::string firstPrice, std::string secondPrice)
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
    int size = 0;

    for (int i = static_cast<int>(data.size())-1; i >= 0; i--)
    {
        if (data[i] == 46)                                      // 46=>ASCII = . 
            break;

        size++;
    }

    for (int i=static_cast<int>(data.size())-1; i>=0; i--)
    {
        if (data[i] == 49)                                      // 49=>ASCII = 1 
            break;

        size--;
    }

    return size;
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

        setExitSignal(0);
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
    std::string mWebsocketBase = mWebsocketJson["base"].asString();

    if (mWebsocketBase.length() == 0)
    {
        ELOG(ERROR, "Failed to parse websocket base.");

        setExitSignal(0);
    }

    return mWebsocketBase;
}


/**
 * @brief Return port
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketPort()
{
    std::string mWebsocketPort = std::to_string(mWebsocketJson["port"].asInt());

    if (mWebsocketPort.length() == 0)
    {
        ELOG(ERROR, "Failed to parse websocket base.");

        setExitSignal(0);
    }

    return mWebsocketPort;
}


/**
 * @brief Return trade symbol endpoint
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketEndpointT()
{
    std::string symbol      = upperToLower(getTradeSymbol());

    std::string endpoint    = "/ws/" + symbol + "@kline_" + getInterval();
        
    return endpoint;
}


/**
 * @brief Return follow symbol endpoint
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getWebsocketEndpointF()
{
    std::string symbol      = upperToLower(getFollowSymbol());

    std::string endpoint    = "/ws/" + symbol + "@kline_" + getInterval();
        
    return endpoint;
}


/**
 * @brief Return api base
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getAPIBase()
{
    std::string mAPIBase = mAPIJson["base"].asString();

    if (mAPIBase.length() == 0)
    {
        ELOG(ERROR, "Failed to parse API base.");

        setExitSignal(0);
    }

    std::string mAPIBaseWithHttps = "https://" + mAPIBase;

    return mAPIBaseWithHttps;
}


/**
 * @brief Return API KEY
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getAPIKEY()
{
    std::string mAPIKey = mAPIJson["API_KEY"].asString();

    if (mAPIKey.length() == 0)
    {
        ELOG(ERROR, "Failed to parse API_KEY.");

        setExitSignal(0);
    }

    return mAPIKey;
}


/**
 * @brief Return SECRET KEY
 * 
 * @return std::string 
 */
std::string BinanceUtilities::getAPISECRETKEY()
{
    std::string mAPISecretKey = mAPIJson["SECRET_KEY"].asString();

    if (mAPISecretKey.length() == 0)
    {
        ELOG(ERROR, "Failed to parse SECRET_KEY.");

        setExitSignal(0);
    }
    return mAPISecretKey;
}


/**
 * @brief encrypt with hmac
 * 
 * @param key 
 * @param data 
 * @return std::string 
 */
std::string BinanceUtilities::encryptWithHMAC(const char* key, const char* data) 
{
    int             result_len = 32;
    unsigned char   *result;
    static char     res_hexstring[64];
    std::string     signature;

    result = HMAC(EVP_sha256(), key, strlen((char *)key), const_cast<unsigned char *>(reinterpret_cast<const unsigned char*>(data)), strlen((char *)data), NULL, NULL);
  	
    for (int i = 0; i < result_len; i++) 
    {
    	sprintf(&(res_hexstring[i * 2]), "%02x", result[i]);
  	}

  	for (int i = 0; i < 64; i++) 
    {
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
std::string BinanceUtilities::getSignature(std::string query) 
{
    std::string mSecretKey  = getAPISECRETKEY();
	std::string signature   = encryptWithHMAC(mSecretKey.c_str(), query.c_str());

    // ELOG(INFO, "Generated a signature: %s.", signature.c_str());

	return signature;
}