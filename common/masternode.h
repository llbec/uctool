#ifndef MST_NODE_DATA_H
#define MST_NODE_DATA_H  

#include <iostream>
#include <string>
#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using namespace std;


// master node  quest  master register center  about master node info
enum MST_QUEST  
{
    MST_QUEST_ONE=1,
    MST_QUEST_ALL=2

};

// master node quest version 
class  mstnodequest
{
public:
    mstnodequest(int version, MST_QUEST  type  ):_msgversion(version), _questtype(type)
    {
       //_verfyflag=std::string("#$%@");  
    }  

    mstnodequest(){}
    int             _msgversion; 	
    int             _questtype;
	int64_t         _timeStamps;	
    //std::string     _verfyflag;
    std::string     _txid;
	unsigned int    _voutid;
    friend class boost::serialization::access;
    
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {  
        //ar & _verfyflag;
        ar & _msgversion;
		ar & _timeStamps;
        ar & _questtype;
        ar & _txid;
		ar & _voutid;
        //ar & _llAmount;  
    }  
    int   GetVersion() const {return _msgversion;}  
    int   GetQuestType() const {return _questtype;}  
    //void  SetMasterAddr(std::string addr){ _masteraddr=addr;}    

};
// master node quest version 
class  mstnoderes
{
public:
    mstnoderes(int version  ):_msgversion(version)
    {
       //_verfyflag=std::string("#$%@");
       _num=1;
    }

    mstnoderes(){}

    int             _msgversion;
    int             _num;
    //std::string     _verfyflag;
    //int64_t         _licperiod;  //licence period
    //std::string     _licence;    //licence
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        //ar & _verfyflag;
        ar & _msgversion;
        ar & _num;
        //ar & _signstr;  // 使用 查询的第一个地址来签名  。 
        //ar & _llAmount;  
    }
    int GetVersion() const {return _msgversion;}
    int GetNum() const {return _num;}
};

class test_mstnodequest
{
public:
// test
   void testAr() 
   {
      mstnodequest  d1(1111, MST_QUEST::MST_QUEST_ONE);  
      std::ostringstream os;  
      boost::archive::binary_oarchive oa(os);  
      oa << d1;//序列化到一个ostringstream里面  
  
      std::string content = os.str();//content保存了序列化后的数据。  
  
      mstnodequest  d2;  
      std::istringstream is(content);  
      boost::archive::binary_iarchive ia(is);  
      ia >> d2;//从一个保存序列化数据的string里面反序列化，从而得到原来的对象。  
  
      std::cout << "CMyData tag: " << d2.GetVersion() << ",  text: " << d2.GetQuestType() << "\n";
   }
};

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
class CMstNodeData  
{  
private:  
    friend class boost::serialization::access;  
  
    template<class Archive>  
    void serialize(Archive& ar, const unsigned int version)  
    {  
        ar & _version;
        ar & _txid;
		ar & _voutid;
        ar & _status;
        ar & _validflag;
		ar & _licperiod;
		ar & _licence;
        ar & _nodeperiod;
    }
      
public:  
    CMstNodeData():_version(0), _txid(""), _voutid(0), _validflag(0){}  
  
    CMstNodeData(int version, std::string txid, unsigned int voutid):_version(version), _txid(txid), _voutid(voutid){}  
  
    int GetVersion() const {return _version;}  
    int GetValidFlag() const {return _validflag;}  
    //std::string GetMasterAddr() const {return _masteraddr;}  

    CMstNodeData & operator=(CMstNodeData &b)
    {
        _version   = b._version;
        _txid      = b._txid;
		_voutid    = b._voutid;
        _status    = b._status;
        _validflag = b._validflag;
		_licperiod = b._licperiod;
		_licence   = b._licence;
        _nodeperiod= b._nodeperiod;
        return * this;
    }
public:  
    int _version;  
    std::string  _txid;       //
    unsigned int _voutid;
    int          _status;
    int          _validflag;  //
    int64_t      _licperiod;  //licence period
    std::string  _licence;    //licence
    int64_t      _nodeperiod;
    unsigned int _time;       //read db time
};  

#endif
