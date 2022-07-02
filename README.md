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
[Licence](#licence) •
[Contribution](#contribution)
</div>

This bot run on unix-like systems. You can use easily on your linux system.
Just clone and build. That's all.
Also if you want to use localhost you can use easily.
When you run the crypto bot, localhost will open.
Attention! Don't forget to edit the config file.

<p float="left">
    <img src="https://github.com/AHakan/cryptobot/blob/master/local.png"/>
</p>

## Installation

### *Dependencies*

- [CMake][cmake]
- [Boost Libraries][boost]
- [OpenSSL][openssl]
- [Curl][curl]
- [SQLite][sqlite]

You must install these dependencies for cryptobot to run.

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
    "server": {
        "base": "127.0.0.1",
        "port": 8080
    }
}
```

- `user`
  - If you fill in the Calculate average section with 1,
  the bot will automatically calculate the average with candlesticks data.

    | Configuration                         | Description                       |
    | ------------------------------------- | --------------------------------- |
    | `Exchange` <sub>(str)</sub>           | Exchange name (Binance)           |
    | `Symbol` <sub>(str)</sub>             | Trade coin symbol (ETH/USDT)      |
    | `Interval` <sub>(str)</sub>           | Klines interval type (15m)        |
    | `Quantity` <sub>(str)</sub>           | Trade coin quantity               |
    | `Balance symbol` <sub>(str)</sub>     | Wallet balance symbol (USDT)      |
    | `Balance amount` <sub>(str)</sub>     | Wallet balance amount (400$)      |
    | `Follow symbol` <sub>(str)</sub>      | Follow coin symbol (BTC/USDT)     |
    | `Average calculate` <sub>(bool)</sub> | Average auto calculate            |
    | `Average amount` <sub>(str)</sub>     | Average amount for trade (0.25)   |
    | `RSI period` <sub>(int)</sub>         | RSI period                        |
    | `RSI oversold` <sub>(str)</sub>       | RSI oversold signal level         |
    | `RSI overbought` <sub>(str)</sub>     | RSI overbought signal level       |

- `binance`
  - You must fill in API Key and SECRET Key.

    | Configuration                         | Description                       |
    | ------------------------------------- | --------------------------------- |
    | `Websocket base` <sub>(str)</sub>     | Binance websocket address         |
    | `Websocket port` <sub>(int)</sub>     | Binance websocket port            |
    | `Api base` <sub>(str)</sub>           | Binance api address               |
    | `Api key` <sub>(str)</sub>            | Binance api key                   |
    | `Secret key` <sub>(str)</sub>         | Binance secret key                |

- `server`
  - If you want to see orders with browser you can use the web server. (New feature)

    | Configuration                         | Description                       |
    | ------------------------------------- | --------------------------------- |
    | `Base` <sub>(str)</sub>               | Server address                 |
    | `Port` <sub>(int)</sub>               | Server port                    |

## Licence

GNU General Public License v3.0

## Contribution

Pull requests are welcome. For major changes, please open an issue first to
discuss what you would like to change.

[cryptobot]: https://github.com/ahakan/cryptobot.git
[cmake]: https://cmake.org/
[boost]: https://www.boost.org/
[openssl]: https://www.openssl.org/
[curl]: https://curl.se/
[sqlite]: https://www.sqlite.org/index.html
