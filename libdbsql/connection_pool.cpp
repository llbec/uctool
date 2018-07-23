#include <stdexcept>  
#include <exception>  
#include <stdio.h>  
#include "connection_pool.h"  
  
using  namespace std;  
using  namespace sql;  
  
ConnPool*ConnPool::_connPool=NULL;  
//连接池的构造函数  
ConnPool::ConnPool(const std::string & url,const std::string & userName,const std::string&  password, int maxSize)  
{  
    this->_maxSize=maxSize;  
    this->_curSize=0;  
    this->_username=userName;  
    this->_password=password;  
    this->_url=url;  
    _lock = PTHREAD_MUTEX_INITIALIZER;/*初始化互斥锁*/  

    try{  
        this->_driver=sql::mysql::get_driver_instance();  
    }  
    catch(sql::SQLException&e)  
    {  
       return  ;// perror("驱动连接出错;\n");  
    }  
    catch(std::runtime_error&e)  
    {  
        return ; 
       // perror("运行出错了\n");  
    }  

    this->InitConnection(maxSize);  
}  
//获取连接池对象，单例模式  
ConnPool*ConnPool::GetInstance(std::string user, std::string passwd , int nConnNum){  
    if(_connPool==NULL)  
    {  
        std::string url("tcp://127.0.0.1:3306");
        _connPool=new  ConnPool(url,user,passwd,nConnNum);  
    }  
    return  _connPool;  
}  

ConnPool*ConnPool::GetInstance()
{  
    if(_connPool==NULL)  
    {  
        return NULL;
    }  
    return _connPool;
}
//初始化连接池，创建最大连接数的一半连接数量  
void   ConnPool::InitConnection(int iInitialSize)  
{  
    Connection  * conn;  
    pthread_mutex_lock(&_lock);  
    for(int  i=0;i<iInitialSize;i++)  
    {  
        conn=this->CreateConnection();  
        if(conn){  
            _connList.push_back(conn);  
            ++(this->_curSize);  
        }  
        else  
        {  
            //perror("创建CONNECTION出错");  
        }  
    }  
    pthread_mutex_unlock(&_lock);  
}  
//创建连接,返回一个Connection  
Connection*  ConnPool::CreateConnection(){  
    Connection * conn;  
    try{  
        conn=_driver->connect(this->_url,this->_username,this->_password);//建立连接  
        return   conn;  
    }  
    catch(sql::SQLException&e)  
    {  
        //perror("创建连接出错");  
        return   NULL;  
    }  
    catch(std::runtime_error&e)  
    {  
        //perror("运行时出错");  
        return  NULL;  
    }  
}  
//在连接池中获得一个连接  
Connection *  ConnPool::GetConnection(){  
    Connection  *  con;  
    pthread_mutex_lock(&_lock);  
    if(_connList.size()>0)//连接池容器中还有连接  
    {  
        con=_connList.front();//得到第一个连接  
        _connList.pop_front();//移除第一个连接  
        if(con->isClosed())//如果连接已经被关闭，删除后重新建立一个  
        {  
            delete  con;  
            con=this->CreateConnection();  
        }  
        //如果连接为空，则创建连接出错  
        if(con==NULL)  
        {  
            --_curSize;  
        }  
        pthread_mutex_unlock(&_lock);  
        return  con;  
    }  
    else{  
        if(_curSize< _maxSize){//还可以创建新的连接  
            con= this->CreateConnection();  
            if(con){  
                ++_curSize;  
                pthread_mutex_unlock(&_lock);  
                return con;  
            }  
            else{  
                pthread_mutex_unlock(&_lock);  
                return  NULL;  
            }  
        }  
        else{//建立的连接数已经达到maxSize  
            pthread_mutex_unlock(&_lock);  
            return   NULL;  
        }  
    }  
}  
//回收数据库连接  
void   ConnPool::ReleaseConnection(sql::Connection * conn){  
    if(conn){  
        pthread_mutex_lock(&_lock);  
        _connList.push_back(conn);  
        pthread_mutex_unlock(&_lock);  
    }  
}  
//连接池的析构函数  
ConnPool::~ConnPool()  
{  
    this->DestroyConnPool();  
}  
//销毁连接池,首先要先销毁连接池的中连接  
void  ConnPool::DestroyConnPool(){  
    list<Connection*>::iterator icon;  
    pthread_mutex_lock(&_lock);  
    for(icon=_connList.begin();icon!=_connList.end();++icon)  
    {  
        this->DestroyConnection(*icon);//销毁连接池中的连接  
    }  
    _curSize=0;  
    _connList.clear();//清空连接池中的连接  
    pthread_mutex_unlock(&_lock);  
}  
//销毁一个连接  
void  ConnPool::DestroyConnection(Connection* conn)  
{  
    if(conn)  
    {  
        try{  
            conn->close();  
        }  
        catch(sql::SQLException &e)  
        {  
            //perror(e.what());  
        } 
        catch(std::exception &e)  
        {  
            //perror(e.what());  
        }  
        delete  conn;  
    }  
}  
