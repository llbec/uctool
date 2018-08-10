# licensetool Introduction
licensetool have two functions. One is periodically scan the database and update its certificate information. And the other is clear all the certificate information of the database.
## Function 1 - Update Database
1. Configure  
	In configure path ~/.uttools, edit file ***ulordcenter.conf***, add a private key.
```
#Example:
privkey1=L1owsF7QYphRVdi8UpajX5zLiDZUbZ6oaB4VtVwUgpE7vG7Ubaqn
```
2. Run  
	Entry the project path,and run the program ***licensetool***.
```
cd
cd ulordtool/build/bin
./licensetool
```
**Optional**  
&nbsp;&nbsp;Default this program will scan the database every 1 minutes. To looking for the masternode whose license period less than 3 days(259200 seconds), and update theirs license and license period to 30 days(2592000 seconds) later.  
&nbsp;&nbsp;If you want to change this times, you can add the follow conffigures in the file ***ulordcenter.conf***  
- **periodunit** : specify the license period time. unit second, defalut:89600.  
- **needupdate** : specify the limit of time of the license need to update. It works as _needupdate*periodunit_, default:3.  
- **periodlimit**: specify the limit of time ot the license. It works as _periodlimit*periodunit_, default:30.  
- **runinterval**: specify the period ot this program scan the database. unit minutes. default:1.  
```
#Example:
#check database every 2 minutes, update license to 1800(30*60)seconds later of whoes license period is less than 600(10*60) seconds.
periodunit=60
needupdate=10
periodlimit=30
runinterval=2
```
&nbsp;&nbsp;If the private key have been stealed, you need to use a new private key, you can add the follow configures in the file ***ulordcenter.conf***  
- **privkey2**  :specify the second private key, 3td is privkey3 ...  
- **uctpubkey2**:specify the second pubkey match the second private key, 3td is uctpubkey3 ...  
```
#Example:
privkey1=L1owsF7QYphRVdi8UpajX5zLiDZUbZ6oaB4VtVwUgpE7vG7Uba1t
privkey2=L2owsF7QYphRVdi8UpajX5zLiDZUbZ6oaB4VtVwUgpE7vG7Uba2d
privkey3=L3owsF7QYphRVdi8UpajX5zLiDZUbZ6oaB4VtVwUgpE7vG7Uba3d
uctpubkey2=02e867486ebaeeadda25f1e47612cdaad3384af49fa1242c5821b424937f8ec12d
uctpubkey3=03e867486ebaeeadda25f1e47612cdaad3384af49fa1242c5821b424937f8ec13d
```
&nbsp;&nbsp;The DataBase parameters use the follow configures:
- **dbhost** : default is 127.0.0.1
- **dbport** : default is 3306
- **dbuser** : default is root
- **dbpwd**  : default is 123456
- **dbname** : default is mysql
## Function 2 - Clear License
&nbsp;&nbsp;If you want to clear all the license information, you can run as the example
```
cd
cd ulordtool/build/bin
./licensetool clear
```
