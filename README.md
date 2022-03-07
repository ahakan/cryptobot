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

fast, secure and open source crypto trading bot

[Installation](#installation) •
[Configuration](#configuration) •
[Usage](#usage) •
[Licence](#licence) 
</div>


## Installation

### *Step 1: Install Boost*

crytobot use Boost/Beast library for websocket. You must has Boost library.
Please install it. [Boost C++ Libraries][boost].


### *Step 2: Clone cryptobot*

Clone [cryptobot][cryptobot].

```sh
git clone https://github.com/ahakan/cryptobot.git   
```

### *Step 3: Build cryptobot*
```sh
cd cryptobot
mkdir build && cd build
cmake ..
sudo make  
```

## Configuration

### Config.json

When use the crypto bot, you have to make some configurations.

```
{
    "user": {
        "interval": "15m",
        "trade": {
            "symbol": "ETHUSDT",
            "balance": {
                "symbol": "USDT",
                "amount": "400"
            },
            "average": {
                "auto-calculate": 0,
                "amount": "0.20"
            }
        },
        "follow": {
            "symbol": "BTCUSDT"
        },
        "exchange": {
            "name": "binance"
        }
    },
    "binance": {
        "websocket": {
            "base": "stream.binance.com",
            "port": "9443"
        },
        "api": {
            "base": "https://api.binance.com",
            "API_KEY": "1",
            "SECRET_KEY": "1"
        }
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
    | `balance symbol`  | Wallet balance symbol (USDT)      |
    | `balance amount`  | Wallet balance amount (400$)      |
    | `follow symbol`   | Follow coin symbol (BTC/USDT)     |
    | `average amount`  | Average amount for trade (0.25)   |
    | `RSI`             | RSI type                          |

- `binance`
  - .
    | Configuration     | Description                       |
    | ------------------| --------------------------------- |
    | `websocket base`  | Binance websocket address         |
    | `websocket port`  | Binance websocket port            |
    | `api base`        | Binance api address               |
    | `api key`         | Binance api key                   |
    | `secret key`      | Binance secret key                |


## Usage

```sh
cd build           
./cryptobot        
```

## Licence

[cryptobot]: https://github.com/ahakan/cryptobot.git
[boost]: https://www.boost.org/
