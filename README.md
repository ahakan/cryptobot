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

### *Step 1: Install Boost*

crytobot use Boost/Beast library for websocket. You must has Boost library.
Please install it. [Boost C++ Libraries][boost].


### *Step 2: Clone cryptobot*

Clone [cryptobot][cryptobot].

```sh
git clone https://github.com/ahakan/cryptobot.git   
```

### *Step 3.1: Build cryptobot with bash script*
```sh
cd cryptobot
chmod +x build.sh
./build.sh
```

### *Step 3.2: Build cryptobot with CMake*
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
            "port": "9443"
        },
        "api": {
            "base": "api.binance.com",
            "API_KEY": "1",
            "SECRET_KEY": "1"
        }
    },
    "webserver": {
        "base": "127.0.0.1",
        "socket": 8080
    }
}


```

- `user`
  - .
    | Configuration     | Description                       |
    | ------------------| --------------------------------- |
    | `exchange`        | Exchange name (Binance)           |
    | `interval`        | Klines interval type (15m)        |
    | `symbol`          | Trade coin symbol (ETH/USDT)      |
    | `quantity`        | Trade coin quantity               |
    | `balance symbol`  | Wallet balance symbol (USDT)      |
    | `balance amount`  | Wallet balance amount (400$)      |
    | `follow symbol`   | Follow coin symbol (BTC/USDT)     |
    | `average amount`  | Average amount for trade (0.25)   |
    | `RSI period`      | RSI period                        |
    | `RSI oversold`    | RSI oversold signal level         |
    | `RSI overbought`  | RSI overbought signal level       |

- `binance`
  - .
    | Configuration     | Description                       |
    | ------------------| --------------------------------- |
    | `websocket base`  | Binance websocket address         |
    | `websocket port`  | Binance websocket port            |
    | `api base`        | Binance api address               |
    | `api key`         | Binance api key                   |
    | `secret key`      | Binance secret key                |

- `webserver`
  - .
    | Configuration     | Description                       |
    | ------------------| --------------------------------- |
    | `base`            | Webserver address                 |
    | `port`            | Webserver port                    |


## Licence

[cryptobot]: https://github.com/ahakan/cryptobot.git
[boost]: https://www.boost.org/
