FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y wget python3 python3-serial
RUN wget https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
RUN tar xf arduino-cli_latest_Linux_64bit.tar.gz
RUN ./arduino-cli version
RUN ./arduino-cli config init
RUN ./arduino-cli config set board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"
RUN ./arduino-cli config set library.enable_unsafe_install true
RUN ./arduino-cli core update-index
RUN ./arduino-cli core install esp32:esp32@3.2.0
RUN ./arduino-cli lib install MQTT@2.5.2
RUN ./arduino-cli lib install MQTTRemote@6.0.2
RUN ./arduino-cli lib install ArduinoJson@7.1.0
COPY ./ /library
RUN ./arduino-cli compile --library /library --fqbn "esp32:esp32:esp32" /library/examples/arduino/actuators