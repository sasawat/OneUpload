#ifndef LOGIN_DATA_H
#define LOGIN_DATA_H
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace OneDrive
{
class LoginData
{
	private:
		std::string _refreshToken;
	public:
		std::string accessToken();
		std::string refreshToken();
		LoginData(std::string json);
		LoginData(std::istream &from);
		LoginData(const LoginData& login);
		LoginData();
		void writeTo(std::ostream to);
		void upload(std::string cloudname, std::string localname);
};

class UpFile
{
	private:
		char *_data;
		int _datalen;
		std::string _onedriveLoc;
		LoginData _login;
	public:
		std::string location();
		std::string location(std::string newlocation);
		bool upload();
		bool loadData(std::istream &input);
		bool loadData(char * data, int datalen);
		UpFile(LoginData login);
		~UpFile();
};





};












#endif
