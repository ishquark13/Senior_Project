--mqtt.lua
--Adrian Alvarez
-- Connects to MQTT Broker

m = mqtt.Client(MQTT_CLIENTID, 120)

tmr.alarm(0, 1000, 1, function()
    if m:connect("192.168.1.1", 1883, 0) == false then
       print("\nConnecting to MQTT Broker...\n")
    else
        tmr.stop(0) 
        --m:publish("IOT", "ESP connected to Broker", 0, 0, function(client) print("connected") end)      
        print("\nFinished initializing MQTT....Starting main\n")

        dofile("main.lua")
        print("\nOpened main.lua\n")
    end
end)