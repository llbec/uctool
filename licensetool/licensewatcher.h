#ifndef LICENSE_WATCHER_H
#define LICENSE_WATCHER_H

#ifdef MYSQL_ENABLE
#include "MySQLConnection.h"
#include "ulordDB.h"

/*
1    `id` BIGINT (20) NOT NULL AUTO_INCREMENT COMMENT '主键ID',
2	 `gmt_create` BIGINT (20) NOT NULL COMMENT '创建时间',
3	 `gmt_modify` BIGINT (20) NOT NULL COMMENT '修改时间',
4	 `user_id` VARCHAR (32) DEFAULT NULL,
5	 `major_node_nickname` VARCHAR (64) DEFAULT NULL COMMENT '主节点昵称',
6	 `trade_txid` VARCHAR (64) DEFAULT NULL COMMENT '1万UT交易ID',
7	 `trade_vout_no` VARCHAR (64) DEFAULT NULL COMMENT '1万UT交易ID对应的Vout序号',
8	 `ip_address` VARCHAR (64) DEFAULT NULL COMMENT '主节点IP地址',
9	 `special_code` VARCHAR (255) DEFAULT NULL COMMENT '主节点特征码',
10	 `status` INT (3) DEFAULT '0' COMMENT '状态,0:绑定中,1:绑定确认成功,2.绑定确认失败',
11	 `validflag` INT (3) DEFAULT '0' COMMENT '有效标志位,0为无效标志，1为有效，不由用户填写' 
12	 `validdate` BIGINT (20) DEFAULT '0' COMMENT '签证的有效期 validflag=1有效 必填',  
13	 `certificate` VARCHAR (255) DEFAULT NULL COMMENT '证书',
14	 `ut_addr` VARCHAR (255) DEFAULT NULL COMMENT 'Ulord地址',
15   'balance' DECIMAL (20, 5) DEFAULT "0.00000" COMMENT '主节点锁定币的数量',
16	 `remark` VARCHAR (255) DEFAULT NULL COMMENT '绑定确认失败原因',
17	 `audit_num` INT (3) NOT NULL DEFAULT '0' COMMENT '绑定确认次数',
18	 `auditor` VARCHAR (32) DEFAULT NULL COMMENT '绑定确认审核人',
19	 `gmt_audit` BIGINT (20) DEFAULT NULL COMMENT '绑定确认审核时间',
20   `node_period' BIGINT (20) DEFAULT NULL COMMENT '节点有效时间',
21	 `ext_info` VARCHAR (255) DEFAULT NULL COMMENT '扩展信息',
 */

class CLicenseWatcher {
private:
    typedef std::map <int, CKey> map_int_key_t;
    typedef std::pair <int, CKey> pair_int_key_t;
    typedef typename map_int_key_t::iterator map_int_key_it;
    typedef typename map_int_key_t::const_iterator map_int_key_cit;

    const int licversion_;
    const int64_t licPeriodLimit_;
    const int64_t needUpdatePeriod_;
    const int64_t runInterval_;
    const std::string tablename_;
    CUlordDb db_;
    map_int_key_t mapWatcherkey_;
    int keyVersion_;
public:
    CLicenseWatcher();
    void Run();
    void ClearDB();
    void SelectNeedUpdateMNData(std::vector<CMNode> & vecnode);
    void UpdateDB(std::vector<CMNode> & vecnode);
private:
    bool InitWatcherKey();
    bool SignMNLicense(CMNode & mn);
};

#endif // MYSQL_ENABLE
#endif //LICENSE_WATCHER_H