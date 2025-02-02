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

//ʵ�����ӳع���ģ��
class ConnectionPool
{
public:
	static ConnectionPool* getConnectionPool();	//��ȡ���ӳض���
	std::shared_ptr<Connection> getConnection(); //��ȡһ������
private:

	ConnectionPool();
	bool Load_Config_File();//���������ļ�
	void ProduceTask(); //���������ӵ��̺߳���
	std::string m_ip; //mysql ip 
	unsigned short m_port; //mysql port
	std::string m_uname; //��¼�û���
	std::string m_password; //��½����
	std::string m_dbname; //���������ݿ���
	int init_size; //���ӳس�ʼ������
	int max_size; //���ӳ����������
	int max_spare_time; //������ʱ��
	int connect_timeout; //���ӳ�ʱʱ��
	std::atomic_int connection_count; //��¼��������
	std::queue<Connection*> m_connectionQueue; //������ӵĶ���
	std::mutex QueueMutex; //������
	std::condition_variable cv; //�������������������������߳�
};