#pragma once
#include<iostream>
#include<string>
#include<mysql.h>
//每条连接实现数据库操作

class Connection
{
public:
	Connection();
	~Connection();
	bool connect(std::string ip, unsigned short port, std::string usename,
		std::string passwd, std::string dbname); //连接库
	bool update(std::string sql); //更新
	MYSQL_RES* query(std::string sql); // 查询
	void displayQueryResult(MYSQL_RES* result); //显示查询结果
private:
	MYSQL* m_conn;
};