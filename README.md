<!-- markdownlint-configure-file {
  "MD013": {
    "code_blocks": false,
    "tables": false
  },
  "MD033": false,
  "MD041": false
} -->

<div align="center">

# cryptobot

fast, secure, lightweight and open source crypto trading bot

[Installation](#installation) •
[Usage](#usage) •
[Configuration](#configuration) •
[Licence](#licence) 
</div>

This bot run on unix-like systems. You can use easily on your linux system. Just clone and build. That's all.
Also if you want to use localhost you can use easily. When you run the crypto bot, localhost will open. 
Attention! Don't forget to edit the config file.

<p float="left">
    <img src="https://github.com/AHakan/cryptobot/blob/master/local.png"/>
</p>

## Installation

### *Installing Prerequisites: CMake and Boost*

crytobot use CMake for building the app and Boost/Beast library for websocket. You must has CMake and Boost library.

Please install [CMake][cmake] and [Boost Libraries][boost].

### *Step 1: Clone cryptobot*

Clone [cryptobot][cryptobot].

```sh
git clone https://github.com/ahakan/cryptobot.git   
```

### *Step 2.1: Build cryptobot with bash script*
```sh
cd cryptobot
chmod +x build.sh
./build.sh
```

### *Step 2.2: Build cryptobot with CMake*
```sh
cd cryptobot
mkdir build && cd build
cmake .. 
sudo make  
```
If you don't want build some libraries with cmake you can use these configurations,

 -DBUILD_LIBCAP=FALSE 
 -DBUILD_SQLITE3=FALSE 
 -DBUILD_CURL=FALSE

## Usage

```sh
cd build           
./cryptobot        
```
or
```sh
./cryptobot.sh      
```

## Configuration

### Config.json

When use the crypto bot, you have to make some configurations.

```
{
    "trade": {
        "exchange": "binance",
        "symbol"  : "SOLBUSD",
        "interval": "4h",
        "quantity": "2.00",
        "balance": {
            "symbol": "BUSD",
            "amount": "400"
        },
        "follow": {
            "symbol": "BTCBUSD"
        },
        "average": {
            "auto-calculate": 0,
            "amount": "0.20"
        },
        "RSI": {
            "period": 14,
            "oversold"  : "40.00",
            "overbought": "55.00"
        }
    },
    "binance": {
        "websocket": {
            "base": "stream.binance.com",
            "port": 9443
        },
        "api": {
            "base": "api.binance.com",
            "API_KEY": "YOUR_API_KEY",
            "SECRET_KEY": "YOUR_SECRET_KEY"
        }
    },
    "webserver": {
        "base": "127.0.0.1",
        "port": 8080
    }
}
```

- `user`
  - .
    | Configuration         | Description                       |
    | --------------------- | --------------------------------- |
    | Exchange`str`         | Exchange name (Binance)           |
    | Symbol`str`           | Trade coin symbol (ETH/USDT)      |
    | Interval`str`         | Klines interval type (15m)        |
    | Quantity`str`         | Trade coin quantity               |
    | Balance symbol`str`   | Wallet balance symbol (USDT)      |
    | Balance amount`str`   | Wallet balance amount (400$)      |
    | Follow symbol`str`    | Follow coin symbol (BTC/USDT)     |
    | Average auto-cal`bool`| Average auto calculate            |
    | Average amount`str`   | Average amount for trade (0.25)   |
    | RSI period`int`       | RSI period                        |
    | RSI oversold`str`     | RSI oversold signal level         |
    | RSI overbought`str`   | RSI overbought signal level       |

- `binance`
  - .
    | Configuration         | Description                       |
    | --------------------- | --------------------------------- |
    | Websocket base`str`   | Binance websocket address         |
    | Websocket port`int`   | Binance websocket port            |
    | Api base`str`         | Binance api address               |
    | Api key`str`          | Binance api key                   |
    | Secret key`str`       | Binance secret key                |

- `webserver`
  - .
    | Configuration         | Description                       |
    | --------------------- | --------------------------------- |
    | Base`str`             | Webserver address                 |
    | Port`int`             | Webserver port                    |


## Licence

[cryptobot]: https://github.com/ahakan/cryptobot.git
[cmake]: https://cmake.org/
[boost]: https://www.boost.org/
