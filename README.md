### Agric Machinery Runtime Sensing ###
#### Sensing LoRa ####
>1. An MCU Sensing Prototype Module.
>2. Include GPS, Flow, Temperature, Humidity, Speed.
>3. Data via LoRa to communicate with backend.

#### LocalQuery.py ####
>1. An RPI Zero and MCU combine, RPI use Serial Port.
>2. Query Data and Save as CSV File.

#### Query_Push.py ####
>1. Query Data via LoRA Gateway and MQTT .
>2. Push Data to Mediatek MCS.
>3. Data Save as CSV File.