# JHE42B and JHE42B open firmware

Both  are controlled by a STM8S0003F3

# Goal
Replace the default firmware in order to add common feature found on other autonomous buzzer

# Features
AUTO activates after 30s lost of electric supply
AUTO disable if power supply is removed 10-12s
BEEPS every 30s
LIGHT flash at every beeps

# How to flash

You need a ST-link v2, so you can write firmware onto the STM8 flash memory

# Boards pictures
![Commercial shot](/img/both_buzzer_full.jpg)
## JHE42B
![JHE42B front](/img/JHE42B_front.jpg) ![JHE42B back](/img/JHE42B_back.jpg)
## JHE42B_S
![JHE42B_S front](/img/JHE42B_S_front.jpg) ![JHE42B_S back](/img/JHE42B_S_back.jpg)

# Resources
[STM8S0003F3 DataSheet](/doc/STM8S0003F3_datasheet_dm00024550.pdf)