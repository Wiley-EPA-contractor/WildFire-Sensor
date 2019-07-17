## Author: Hawes Collier
~~ Project: Wildfire Boxes

To run dencode, run decompress.exe
1.select "File from Rockblock" and select the .txt file with the UTF-8 (looks like ASCII) received message from the RockBlock.
2. select "File Output" and pick where you want the decoded numbers to go.  Note: This appends to a file, does not make overwrite a file (even though it asks you if you want to overwrite).
3. Click Append Data.  Each Time you click this (for now) it will append the data to a txt file.
Now you can see the data received from the Purple Air!

RPi config--> network name: WildfireBox.  password: purpleair
Instructions for setting up on a new device
1. Download folder of code labeled for this project
2. On the Raspberry Pi (after installation) Open Terminal
3. run: "sudo apt-get update", "sudo apt install dnsmasq hostapd", "sudo apt install isc-dhcp-server"
