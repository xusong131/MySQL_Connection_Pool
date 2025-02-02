#pragma once
#include<iostream>
#include<string>
#include<mysql.h>
//ÿ������ʵ�����ݿ����

class Connection
{
public:
	Connection();
	~Connection();
	bool connect(std::string ip, unsigned short port, std::string usename,
		std::string passwd, std::string dbname); //���ӿ�
	bool update(std::string sql); //����
	MYSQL_RES* query(std::string sql); // ��ѯ
	void displayQueryResult(MYSQL_RES* result); //��ʾ��ѯ���
private:
	MYSQL* m_conn;
};