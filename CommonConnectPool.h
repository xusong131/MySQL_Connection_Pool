#pragma once
#include<string>
#include<iostream>
#include"connection.h"
#include<queue>
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<condition_variable>

//实现连接池功能模块
class ConnectionPool
{
public:
	static ConnectionPool* getConnectionPool();	//获取连接池对象
	std::shared_ptr<Connection> getConnection(); //获取一个连接
private:

	ConnectionPool();
	bool Load_Config_File();//载入配置文件
	void ProduceTask(); //生产新连接的线程函数
	std::string m_ip; //mysql ip 
	unsigned short m_port; //mysql port
	std::string m_uname; //登录用户名
	std::string m_password; //登陆密码
	std::string m_dbname; //连接至数据库名
	int init_size; //连接池初始连接量
	int max_size; //连接池最大连接量
	int max_spare_time; //最大空闲时间
	int connect_timeout; //连接超时时间
	std::atomic_int connection_count; //记录连接数量
	std::queue<Connection*> m_connectionQueue; //存放连接的队列
	std::mutex QueueMutex; //队列锁
	std::condition_variable cv; //条件变量，连接生产与消费线程
};