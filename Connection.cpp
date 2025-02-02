#include<iostream>
#include<string>
#include<mysql.h>
#include"public.h"
#include"connection.h"

Connection:: Connection()
{
	m_conn = mysql_init(nullptr);
}
Connection:: ~Connection()
{
	if (m_conn != nullptr)
		mysql_close(m_conn);
}
bool Connection::connect(std::string ip, unsigned short port, std::string usename,
	std::string passwd, std::string dbname)
{
	MYSQL* p = mysql_real_connect(m_conn, ip.c_str(), usename.c_str(),
		passwd.c_str(), dbname.c_str(), port, nullptr, 0);
    if (mysql_set_character_set(m_conn, "utf8mb4"))
    {
        LOG("Failed to set character set: " + std::string(mysql_error(m_conn)));
        return false;
    }
    return true;
}
bool Connection::update(std::string sql)
{
	if (mysql_query(m_conn, sql.c_str()))
	{
		LOG("����ʧ�ܣ�" + sql);
		return 0;
	}
	return 1;
}
MYSQL_RES* Connection::query(std::string sql)
{
	if (mysql_query(m_conn, sql.c_str()))
	{
		LOG("��ѯʧ�ܣ�" + sql);
		return nullptr;
	}
	return mysql_use_result(m_conn);
}
// ��ʾ��ѯ����ĺ���
void Connection::displayQueryResult(MYSQL_RES* result) {
    if (result == nullptr) {
        return;
    }

    // ��ȡ�ֶ���Ϣ
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    unsigned int num_fields = mysql_num_fields(result);

    // ��ӡ��ͷ
    for (unsigned int i = 0; i < num_fields; ++i) {
        std::cout << fields[i].name << "\t";
    }
    std::cout << std::endl;

    // ���д�ӡ���
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        for (unsigned int i = 0; i < num_fields; ++i) {
            std::cout << (row[i] ? row[i] : "NULL") << "\t";
        }
        std::cout << std::endl;
    }

    // �ͷŽ����
    mysql_free_result(result);
}