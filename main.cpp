#include <iostream>
#include <pqxx/pqxx>
#include <Windows.h>
#include <tuple>
#include <exception>

class clientbd
{
private:
	pqxx::connection conn;
public:

clientbd(const std::string & conn_str) : conn(conn_str) { create_tables(); };

	void create_tables()
	{
		pqxx::work t(conn);
		      t.exec("CREATE TABLE IF NOT EXISTS clients ("
		         "client_id SERIAL PRIMARY KEY, "
		         "first_name TEXT, "
		         "last_name TEXT, "
		         "email TEXT)");
		      t.exec("CREATE TABLE IF NOT EXISTS phones ("
		         "phone_id SERIAL PRIMARY KEY, "
		         "client_id INT REFERENCES clients(client_id) ON DELETE CASCADE, "
		         "phone_number TEXT)");
		      t.commit();
	}

	void add_new_client(const std::string& first_name, const std::string& second_name, const std::string& email)
	{
		pqxx::work t(conn);
		t.exec("INSERT INTO clients(first_name, last_name, email) VALUES ('" + t.esc(first_name) + "', '" + t.esc(second_name) + "', '" + t.esc(email) + "')");
		t.commit();
	}

	void add_telephone(const std::string& phone_number, const std::string& client_id)
	{
		pqxx::work t(conn);
		
		t.exec("INSERT INTO phones(client_id, phone_number) VALUES ('" + t.esc(client_id) + "', '" + t.esc(phone_number) + "')");
		t.commit();
	}

	void change_inform(const std::string& client_id, const std::string& first_name, const std::string& second_name, 
		const std::string& email)
	{
		pqxx::work t(conn);
		t.exec("UPDATE clients SET first_name='" + t.esc(first_name) + "', last_name='" + t.esc(second_name) + "', email='" + t.esc(email) + "' WHERE client_id='" + t.esc(client_id) + "'");
		t.commit();
	}

	void delete_phone(const std::string& phone_number, const std::string& client_id)
	{
		pqxx::work t(conn);
		t.exec("DELETE FROM phones WHERE phone_number='" + t.esc(phone_number) + "' AND client_id='" + t.esc(client_id) + "';");
		t.commit();
	}

	void delete_client(const std::string& client_id)
	{
		pqxx::work t(conn);
		t.exec("DELETE FROM phones WHERE client_id='" + t.esc(client_id) + "';");
		t.exec("DELETE FROM clients WHERE client_id='"+ t.esc(client_id) +"';");
		t.commit();
	}

	void find_client(const std::string& select)
	{
		pqxx::transaction t(conn);

		auto result  = t.query<int, std::string, std::string, std::string, std::string>(select);

		for (std::tuple<int, std::string, std::string, std::string, std::string> value : result)
		{
			std::cout << "Id: " << std::get<0>(value);
			std::cout << "\nfirst name: " << std::get<1>(value);
			std::cout << "\nsecond name: " << std::get<2>(value);
			std::cout << "\nemail: " << std::get<3>(value);
			std::cout << "\nphone: " << std::get<4>(value) << "\n";
		}
	}

	void clear_clients_table()
	{
		pqxx::work t(conn);

		t.exec("DELETE FROM clients;");
		t.exec("ALTER SEQUENCE clients_client_id_seq RESTART WITH 1;");

		t.commit();
	}

	void clear_clients_phones()
	{
		pqxx::work t(conn);

		t.exec("DELETE FROM phones;");
		t.exec("ALTER SEQUENCE phones_phone_id_seq RESTART WITH 1;");

		t.commit();
	}
};

int main()
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, "Russian");
	
	try 
	{
		std::string connectionString = "host=127.0.0.1 port=5432 dbname=DataBaseCxx user=postgres password=1209R";
		clientbd manager(connectionString);

		manager.clear_clients_table(); // óäàëåíèå òàáë
		manager.clear_clients_phones(); // óäàëåíèå òàáë
		manager.add_new_client("Alex", "Stuart", "AS@gmail.com"); // 1 äîáàâëåíèå êëèåíòà
		manager.add_new_client("Max", "Power", "MP@gmail.com"); // 1 äîáàâëåíèå êëèåíòà

		manager.add_telephone("+7 999 568-543-33", "1"); // 2 äîáàâëåíèå íîìåðà ê ñóù-ó êëèåíòó 
		manager.add_telephone("+7 999 990-99-99", "2");// 2 äîáàâëåíèå íîìåðà ê ñóù-ó êëèåíòó 

		manager.change_inform("1", "Alex", "Stuart", "AlexStuart@gmail.com"); // 3 ñìåíà èíôîðìàöèè îá êëèåíòå

		manager.add_telephone("+7 999 534-333-21", "1"); // 4 äîáàâëåíèå íîìåðà ê ñóù-ó êëèåíòó

		manager.delete_phone("+7 999 568-543-33", "1"); // 5 óäàëåíèå íîìåðà òåëåôîíà

		manager.delete_client("2"); // óäàëåíèå êëèåíòû èç ÁÄ

		manager.find_client
		       ("SELECT c.client_id, c.first_name, c.last_name, c.email, p.phone_number "
			"FROM clients c LEFT JOIN phones p ON c.client_id = p.client_id "
			"WHERE c.first_name = 'Alex'");
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception happened: " << e.what() << std::endl;
	}

	return 0;
}
