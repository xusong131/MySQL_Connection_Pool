#include<iostream>
#include<string>
#include<mysql.h>
#include"connection.h"
#include"commonconnectpool.h"

int main()
{
	#if 0
		Connection c;
		int ret = c.connect("127.0.0.1",3306,"root","xusong123456.","test_lib");
		std::string ret_s = (ret == 1 ? "连接成功!" : "连接失败!");
		std::cout << ret_s <<std::endl;
		c.update("update employees_info set salary = 28000 where eid = 1");
		c.displayQueryResult(c.query("select * from test_lib.employees_info;"));
		std::cout <<std::endl;
	
		std::cout<< "init_size = " << cp->init_size << std::endl << "max_size = " << cp->max_size << std::endl
				 << "max_spare_time = " << cp->max_spare_time << std::endl 
				 << "connect_timeout = " << cp->connect_timeout << std::endl
				 << "ip = " << cp->m_ip << std::endl << "port = " << cp->m_port << std::endl 
				 << "username = " << cp->m_uname << std::endl << "password = " << cp->m_password <<std::endl;
	#endif

	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	std::shared_ptr<Connection> p1 = cp->getConnection();
	p1->displayQueryResult(p1->query("select * from employees_info"));
	return 0;
}
