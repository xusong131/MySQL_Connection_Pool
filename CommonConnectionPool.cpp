#include<iostream>
#include<string>
#include<mysql.h>
#include<fstream>
#include"CommonConnectPool.h"
#include<thread>
#include<functional>
#include"public.h"

//���캯����ʵ��
ConnectionPool::ConnectionPool()
{
    if (Load_Config_File())
    {
        //������ʼ����
        for (int i = 0; i < init_size; ++i)
        {
            Connection* p = new Connection();
            p->connect(m_ip, m_port, m_uname, m_password, m_dbname);
            m_connectionQueue.push(p);
            connection_count++;
        }
    }
    //����һ�����̣߳���Ϊ����������
    std::thread productor(std::bind(&ConnectionPool::ProduceTask,this));
    productor.detach();
}

//��ȡ�̳߳ص�����ʵ��
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool; //��̬�ֲ��������������Զ��ӽ������̰߳�ȫ
	return &pool;
}

//���������ļ�����ʵ��
bool ConnectionPool::Load_Config_File()
{
    std::ifstream cfg("E:\\VS project\\MySQL_Connection_Pool\\MySQL_Connection_Pool\\mysql.ini", std::ios::in);
    if (!cfg.is_open()) {
        std::cerr << "�ļ���ʧ��" << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(cfg, line)) {
        // �ҵ��Ⱥŵ�λ��
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "��Ч��������: " << line << std::endl;
            continue; // ������Ч��
        }

        // ��ȡ����ֵ
        std::string key = line.substr(0, pos);          // �Ⱥ�ǰ�Ĳ���
        std::string value = line.substr(pos + 1);       // �Ⱥź�Ĳ���

        // ���ݼ���ֵ����Ӧ�ĳ�Ա����
        if (key == "ip") {
            m_ip = value;
        }
        else if (key == "port") {
            m_port = std::stoi(value);                 // ת��Ϊ����
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
            init_size = std::stoi(value);              // ת��Ϊ����
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
            std::cerr << "δ֪������: " << key << std::endl;
        }
    }
    cfg.close();
    return true;
}

//�������̺߳���ʵ��
void ConnectionPool::ProduceTask()
{   
    for (;;)
    {
        std::unique_lock<std::mutex> lock(QueueMutex);
        while (!m_connectionQueue.empty())
        {
            cv.wait(lock); //���в�Ϊ�գ������̵߳ȴ�
        }

        //��������δ�����ޣ�������������
        if (connection_count < max_size)
        {
            Connection* p = new Connection();
            p->connect(m_ip, m_port, m_uname, m_password, m_dbname);
            m_connectionQueue.push(p);
            connection_count++;
        }

        cv.notify_all(); //֪ͨ�������߳�
    }
}

//�������̺߳���ʵ��
std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(QueueMutex);
    if (m_connectionQueue.empty())
    {
        cv.wait_for(lock,std::chrono::milliseconds(connect_timeout));
        if (m_connectionQueue.empty())
        {
            LOG("��ȡ���ӳ�ʱ�����޿������ӣ�");
            return nullptr;
        }
    }

    std::shared_ptr<Connection> sp(m_connectionQueue.front(),
        [&](Connection* ptr) {
            //���Ƕ����̰߳�ȫ
            std::unique_lock<std::mutex> lock(QueueMutex);
            m_connectionQueue.push(ptr);
        });
    m_connectionQueue.pop();
    cv.notify_all();
    return sp;
}