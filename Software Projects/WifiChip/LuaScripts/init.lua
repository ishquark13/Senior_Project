--
--init.lua
--by Adrian Alvarez

--NodeMCU automatically runs init.lua on bootup 
--

--print("\nInitiating UART...\n")
uart.setup(0, 9600, 8, uart.PARITY_NONE, uart.STOPBITS_1)
tmr.delay(3000000)
--print("Finished\n")

print("Entering init.lua")

print("\nLoading Global Variables...")

--UART
UART_ID = 0
UART_BAUD = 9600
UART_DATA = 8
UART_ECHO = 0

-- WiFi
WIFI_SSID = "IshPi"
WIFI_PASS = "raspberrypi"

-- MQTT
MQTT_CLIENTID = "ESP_DSP"
MQTT_HOST = "192.168.1.1"
MQTT_PORT = 1883

print("Finished\n")

-- Put radio into station mode to connect to network
--connect to Access Point (DO save config to flash)
wifi.setmode(wifi.STATION, true)

-- Debug info
print('\n\nSTATION Mode:',  'mode='..wifi.getmode(), '\n')
print('\n\nMAC Address: ',      wifi.sta.getmac(), '\n')
print('\n\nChip ID: ',          node.chipid(), '\n')
print('\n\nHeap Size: ',        node.heap(),'\n')


-- Start the connection attempt
station_cfg={}
station_cfg.ssid="IshPi"
station_cfg.pwd="raspberrypi"
station_cfg.save=true
wifi.sta.config(station_cfg)


-- Count how many times you tried to connect to the network
local wifi_counter = 0


-- Create an alarm to poll the wifi.sta.getip() function once a second
tmr.alarm(0, 1000, 1, function()
    if wifi.sta.getip() == nil then
        print("\nConnecting to AP...\n")
        
        wifi_counter = wifi_counter + 1;
    else

        tmr.stop(0)     -- Stop the polling loop
    
        ip, nm, gw = wifi.sta.getip()
        
        -- Debug info
        print("\nConnected to ",WIFI_SSID)
        print("\n\nIP Info: \nIP Address: ",ip, '\n')
        print("\nNetmask: ",nm,'\n')
        print("\nGateway Addr: ",gw,'\n')
        
        -- Continue to main function after network connection
        print("\nFinished initializing Wifi....Connecting to broker\n")
        dofile("mqtt.lua")
        
    end
end)



