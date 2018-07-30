# ulordtool

## Prepare:

### mysql
Optional, if without ulordcenter & licensetool, can skip this step.
#### install
```bash
sudo apt-get install mysql-server
sudo apt install mysql-client
sudo apt install libmysqlclient-dev
sudo apt-get install  libmysqlcppconn-dev
```
#### create table
```bash
CREATE TABLE udevforums_major_node_bind (
	id BIGINT (20) NOT NULL AUTO_INCREMENT COMMENT "主键ID",
	gmt_create BIGINT (20) NOT NULL COMMENT "创建时间",
	gmt_modify BIGINT (20) NOT NULL COMMENT "修改时间",
	user_id VARCHAR (32) DEFAULT NULL,
	major_node_nickname VARCHAR (64) DEFAULT NULL COMMENT "主节点昵称",
	trade_txid VARCHAR (64) DEFAULT NULL COMMENT "1万UT交易ID",
	trade_vout_no VARCHAR (64) DEFAULT NULL COMMENT "1万UT交易ID对应的Vout序号",
	ip_address VARCHAR (64) DEFAULT NULL COMMENT "主节点IP地址",
	special_code VARCHAR (255) DEFAULT NULL COMMENT "主节点特征码",
	status INT (3) DEFAULT "0" COMMENT "状态,0:绑定中,1:绑定确认成功,2.绑定确认失败",
	validflag INT (3) DEFAULT "0" COMMENT "unused",
	validdate BIGINT (20) DEFAULT "0" COMMENT "签证的有效期,必填",
	certificate VARCHAR (255) DEFAULT NULL COMMENT "证书",
	ut_addr VARCHAR (255) DEFAULT NULL COMMENT "Ulord地址",
	balance DECIMAL (20, 5) DEFAULT "0.00000" COMMENT "主节点锁定币的数量",
        remark VARCHAR (255) DEFAULT NULL COMMENT "绑定确认失败原因",
	audit_num INT (3) NOT NULL DEFAULT "0" COMMENT "绑定确认次数",
	auditor VARCHAR (32) DEFAULT NULL COMMENT "绑定确认审核人",
	gmt_audit BIGINT (20) DEFAULT NULL COMMENT "绑定确认审核时间",
	node_period BIGINT (20) DEFAULT NULL COMMENT "节点有效时间",
	ext_info VARCHAR (255) DEFAULT NULL COMMENT "扩展信息",
	PRIMARY KEY (id),
	UNIQUE KEY trade_vin (trade_txid, trade_vout_no)
) ENGINE = INNODB AUTO_INCREMENT = 1 DEFAULT CHARSET = utf8mb4 COMMENT = "主节点绑定表";
```

### install glog
```bash
git clone https://github.com/google/glog.git
cd glog
./autogen.sh
./configure
make
make install
```

### download UlordChain & compiles
https://github.com/UlordChain/UlordChain.git

### ulordtool
```bash
git clone https://github.com/pazzaBec/ulordtool.git
cd ulordtool
mkdir build
cd build
cmake -DCHAIN_SRC_ROOT={UlordChain Path} ..
```

### configure & log file
path: /home/*username*/.uttool