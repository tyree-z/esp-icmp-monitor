# esp-icmp-monitor

## Introduction
esp-icmp-monitor is an IoT application developed for ESP8266 microcontrollers. It's designed to monitor network connectivity by pinging pre-defined services and reporting the results to a specified API. This system is useful for network diagnostics and performance monitoring.

## Features
- **Automated Ping Tests:** Periodically pings a list of services and records the latency metrics.
- **Dynamic Service List:** Fetches the list of services to ping from an external API.
- **Reporting:** Sends the ping statistics to a server in a structured JSON format.
- **WiFi Connectivity:** Utilizes ESP8266's WiFi capabilities for network operations.

## System Requirements
- An ESP8266 microcontroller.
- Arduino IDE for code compilation and uploading.
- Access to a WiFi network.
- A server endpoint to receive ping statistics.

## Dependencies
This project requires the following libraries:
- ESP8266WiFi
- ESPping
- ESP8266HTTPClient
- ArduinoJson

## Installation
1. Install the Arduino IDE.
2. Add the ESP8266 board to your Arduino IDE.
3. Install the required libraries using the Arduino Library Manager.
4. Clone this repository or download the source code.

## Configuration
Update the following parameters in the code to match your environment:
- `ssid`: Your WiFi network SSID.
- `password`: Your WiFi network password.
- `serverName`: API endpoint to send ping reports.
- `serviceFetchURL`: API endpoint to fetch the list of services to ping.

## How It Works
### Ping Data Collection
- The ESP8266 fetches a list of IP addresses to ping from `serviceFetchURL`.
- It performs ping operations on each service, collecting metrics like minimum, average, and maximum response times.

### Data Format Sent to API
```json
{
  "1.1.1.1": [{"avg": 18, "max": 20, "min": 17}],
  "10.0.0.1": [{"avg": 3, "max": 5, "min": 2}],
  "device_id": "98:CD:AC:2F:XX:xx"
}
```
### Expected Format for Service List
```json
{
  "ShawRouter": "10.0.0.1",
  "Cloudflare": "1.1.1.1"
}
```

## Usage
1. Power up your ESP8266 module.
2. The device automatically connects to the WiFi network.
3. It fetches the list of services and begins the ping process.
4. Ping statistics are compiled and sent to the specified server API.

## Troubleshooting
- Ensure your WiFi credentials are correctly set.
- Verify the API endpoints are reachable and correctly configured.
- Check serial output for debugging information.

## Contributing
Contributions to improve the Ping Report System are welcome. Please ensure to follow the existing code style and add comments where necessary.

## License
[MIT License](LICENSE)

## Contact
For any inquiries or suggestions, please open an issue in this repository.

---

**Note:** This project is a demonstration and should be adapted for production use with necessary security and reliability enhancements.

