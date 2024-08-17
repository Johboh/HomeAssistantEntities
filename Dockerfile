from ubuntu:latest

run apt-get update
run apt-get install -y wget python3 python3-serial
run wget https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
run tar xf arduino-cli_latest_Linux_64bit.tar.gz
run ./arduino-cli version
run ./arduino-cli config init
run ./arduino-cli config set board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"
run ./arduino-cli config set library.enable_unsafe_install true
run ./arduino-cli core update-index
run ./arduino-cli core install esp32:esp32@3.0.2
run ./arduino-cli lib install MQTT@2.5.1
run ./arduino-cli lib install MQTTRemote
run ./arduino-cli lib install ArduinoJson@7.1.0
COPY ./ /library
run ./arduino-cli compile --library /library --fqbn "esp32:esp32:esp32" /library/examples/arduino/actuators