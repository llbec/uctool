# Tools Introduction
Set of some tools. This program configure file is ***ulordtool.conf***.
## asklicense
Send a requset message to server for a masternode's license.
```
cd ulordtool/build/bin
./tools asklicense 8ce506c13737091e5fb341cc4a58b0acc52155f527e331bdcf2b1d12723b2aac 0
```
## askkeyversion
Send a request message to server for the publickey and license versions.
```
cd ulordtool/build/bin
./tools askkeyversion
```
## keynew
Create a private key, and show it's public and address.
```
cd ulordtool/build/bin
./tools keynew
```
## keymasternode
Create a masternode key, and show it's public.
```
cd ulordtool/build/bin
./tools keymasternode
```
## keyshow
Show the specify private key's public key and address.
```
cd ulordtool/build/bin
./tools keyshow
```
## keymatch
Check wether the private key and the public key is matched.
```
cd ulordtool/build/bin
./tools keymatch KykTrtW5czrxg5ArdxzhnmpaxP8UKj5p8ix2RtPp7okuqkHntuic 03cb10b2fa7edbbc7f6ebc28c472aa9025a27823ab421e4e3b831bedcd667f3a67
```
## keyencode
Encode the private key to protect it
```
cd ulordtool/build/bin
./tools keyencode KykTrtW5czrxg5ArdxzhnmpaxP8UKj5p8ix2RtPp7okuqkHntuic
```
## keydecode
Decode the string to private key
```
cd ulordtool/build/bin
./tools keydecode 7LvU469GBZththJzNtdaStRaBq5oPy4ZvPZsj4nKfkqv4SkwQxmx
```
