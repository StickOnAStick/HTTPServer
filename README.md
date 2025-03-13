CMPE 183 - Netwowrking / Advanced CS topics

# Build and Run
**Linux:**
1. **Compile the server:**
   ```bash
   make
   ```
2. **Run the server:**
    ```bash
    ./bin/server
    ```

3. **Clean up the object files:**
   ```bash
    make clean
   ```

**Pi Pico-W:**
We use `cmakelists.txt` and `PiSDK` for running on the Pi Pico-W.

1. **Setup the Pico SDK**
    ```bash
    git clone -b master https://github.com/raspberrypi/pico-sdk.git`
    cd pico-sdk
    git submodule update --init
    export PICO_SDK_PATH=$(pwd)
    ```

2. **Compile the code**
    ```bash
    mkdir build
    cd build
    cmake ..
    make -j$(nproc)
    ```

3. **Flash to the Pico W**
- Hold the **BOOTSEL button**, connect to a computer via USB.
- Drag the .uf2 file to the **RPI-RP2** drive.

# Testing

To test the server you must have a stable connection to the device hosting it *unless running locally*.

This guide will use `localhost` for simplicity. If running on an external device replace this with the IP address of your device. ex: `172.16.80.251`.

The **port number** could also differ depending on the device. **Linux** runs on `:8080` while **Pi** devices run on `:80`

Get a basic text response:
```bash
curl http://localhost:8080/
Hello World!
```

Get a file: 
```bash
curl http://localhost:80/data/index.html
```

# References
**Primary Source:** [RFC 9910 Standard](https://www.rfc-editor.org/rfc/rfc9110.html) 

**Secondary Sources (HTTP 0.9/1 RFC 1945):** [W3 HTTP](https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html) -&- [DataTracker Documentation](https://datatracker.ietf.org/doc/html/rfc1945) 
