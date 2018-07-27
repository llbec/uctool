# ulordtool

Prepare:

install mysql
```bash
sudo apt-get install mysql-server
sudo apt install mysql-client
sudo apt install libmysqlclient-dev
sudo apt-get install  libmysqlcppconn-dev
```

install glog
```bash
git clone https://github.com/google/glog.git
cd glog
./autogen.sh
./configure
make
make install
```