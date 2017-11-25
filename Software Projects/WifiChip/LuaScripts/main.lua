
--main.lua
--Edited By Adrian Alvarez

--This contains all the bits and pieces to connect to the broker, subscribe and
--and publish to topics, and listen to the UART Port
--on.

--m:on("connect", function(client) print("connected") end)
--m:on("offline", function(client) print("offline") end)
--m:on("message", function(client, topic, data) 
--  print(topic .. ":" ) 
--  if data ~= nil then
--    print(data)
--  end
--end)

uart.on("data", "\r", 
    function(data)
    print("receive from uart:", data)
    m:publish("IOT", data, 0, 0, function(client) print("sent") end)
   
    if data=="quit\r" then
      uart.on("data") -- unregister callback function
    end

end, 0)


--Disconnect from the broker
--m:close();
