#include<iostream>
#include<string>
#include<mysql.h>
#include<fstream>
#include"CommonConnectPool.h"
#include<thread>
#include<functional>
#include"public.h"

//构造函数的实现
ConnectionPool::ConnectionPool()
{
    if (Load_Config_File())
    {
        //创建初始连接
        for (int i = 0; i < init_size; ++i)
        {
            Connection* p = new Connection();
            p->connect(m_ip, m_port, m_uname, m_password, m_dbname);
            m_connectionQueue.push(p);
            connection_count++;
        }
    }
    //启动一个新线程，作为连接生产者
    std::thread productor(std::bind(&ConnectionPool::ProduceTask,this));
    productor.detach();
}

//获取线程池单例的实现
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool; //静态局部变量，编译器自动加解锁，线程安全
	return &pool;
}

//载入配置文件函数实现
bool ConnectionPool::Load_Config_File()
{
    std::ifstream cfg("E:\\VS project\\MySQL_Connection_Pool\\MySQL_Connection_Pool\\mysql.ini", std::ios::in);
    if (!cfg.is_open()) {
        std::cerr << "文件打开失败" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(cfg, line)) {
        // 找到等号的位置
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "无效的配置行: " << line << std::endl;
            continue; // 跳过无效行
        }

        // 提取键和值
        std::string key = line.substr(0, pos);          // 等号前的部分
        std::string value = line.substr(pos + 1);       // 等号后的部分

        // 根据键赋值到相应的成员变量
        if (key == "ip") {
            m_ip = value;
        }
        else if (key == "port") {
            m_port = std::stoi(value);                 // 转换为整数
        }
        else if (key == "user_name") {
            m_uname = value;
        }
        else if (key == "password") {
            m_password = value;
        }
        else if (key == "database") {
            m_dbname = value;
        }
        else if (key == "init_size") {
            init_size = std::stoi(value);              // 转换为整数
        }
        else if (key == "max_size") {
            max_size = std::stoi(value);
        }
        else if (key == "max_spare_time") {
            max_spare_time = std::stoi(value);
        }
        else if (key == "connect_timeout") {
            connect_timeout = std::stoi(value);
        }
        else {
            std::cerr << "未知配置项: " << key << std::endl;
        }
    }
    cfg.close();
    return true;
}

//生产者线程函数实现
void ConnectionPool::ProduceTask()
{   
    for (;;)
    {
        std::unique_lock<std::mutex> lock(QueueMutex);
        while (!m_connectionQueue.empty())
        {
            cv.wait(lock); //队列不为空，生产线程等待
        }

        //连接数量未达上限，继续创建连接
        if (connection_count < max_size)
        {
            Connection* p = new Connection();
            p->connect(m_ip, m_port, m_uname, m_password, m_dbname);
            m_connectionQueue.push(p);
            connection_count++;
        }

        cv.notify_all(); //通知消费者线程
    }
}

//消费者线程函数实现
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(QueueMutex);
    if (m_connectionQueue.empty())
    {
        cv.wait_for(lock,std::chrono::milliseconds(connect_timeout));
        if (m_connectionQueue.empty())
        {
            LOG("获取连接超时，暂无可用连接！");
            return nullptr;
        }
    }

    std::shared_ptr<Connection> sp(m_connectionQueue.front(),
        [&](Connection* ptr) {
            //考虑队列线程安全
            std::unique_lock<std::mutex> lock(QueueMutex);
            m_connectionQueue.push(ptr);
        });
    m_connectionQueue.pop();
    cv.notify_all();
    return sp;
}