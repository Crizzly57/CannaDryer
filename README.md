# CannaDryer

## Introduction
CannaDryer is a project designed for efficient and controlled cannabis drying. This repository contains all the necessary code and configurations to set up and run the system.

## Features
- **Precise Drying Control**: Optimize drying conditions for the best results.
- **Real-Time Monitoring**: Monitor the drying process wirelessly.
- **Extendable**: Easily modify and extend functionalities.

## Dependencies
To ensure proper functionality, the following dependencies are required:

- **Mongoose v7.16**: Provides the network stack for the project. You can download the necessary files from the [Mongoose v7.16 release](https://github.com/cesanta/mongoose/releases/tag/7.16).

## Setup Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/Crizzly57/CannaDryer.git
   cd CannaDryer
   ```

2. Download the `mongoose.c` and `mongoose.h` files from the [Mongoose v7.16 release](https://github.com/cesanta/mongoose/releases/tag/7.16).

3. Place the downloaded files into the `lib/mongoose` directory:
   ```
   lib/mongoose/mongoose.c
   lib/mongoose/mongoose.h
   ```

4. Build the project using your preferred build system (e.g., PlatformIO).

## Configuration
- Create a file named `config_secrets.hpp` in the `include` directory.  
  - This file should contain your Wi-Fi SSID and password as shown below:  

```cpp
#pragma once

#define WIFI_SSID_SECRET  "YourWiFiSSID"
#define WIFI_PASS_SECRET  "YourWiFiPassword"
```

## Usage
After setup, flash the firmware to your device and start the system. Access the web interface or monitor the output to control and observe the drying process.

## Contributing
Contributions are welcome! Feel free to open an issue or submit a pull request if you have ideas or improvements.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Support
For questions or support, open an issue in this repository.

