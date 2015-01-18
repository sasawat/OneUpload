#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include <sstream>
#include <fstream>
#include <ftw.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "onedrive.h"

#define THE_PORT 443

std::string buf = "";

static size_t  writeToBuf(char *ptr, size_t size, size_t nmemb)
{
	buf.append(ptr, nmemb);
	return size * nmemb;
}

static void clearBuf()
{
	buf = "";
}

static OneDrive::LoginData login;

static int recursiveUploadCallback(const char *fpath, const struct stat *sb, int typeflag)
{
	if(typeflag == FTW_F)
	{
		std::string fullpath(fpath);
		// Get only the file name
		int i = 0;
		while(fpath[i] != 0)
		{
			if(fpath[i] == '\\' || fpath[i] == '/')
			{
				fpath = fpath+i+1;
				i = 0;
			}
			else
			{
				i++;
			}
		}
		
		// Upload
		login.upload(cURLpp::escape(std::string(fpath)), fullpath);
//		std::cout << "\""+std::string(fpath)+"\"" << " " << fullpath << std::endl;
		std::cout << std::endl;
	}
	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		return 0;
	}
	if(argv[1][0] == '-' and argv[1][1] == 'l')
	{
		std::string code = OneDrive::LoginData::authCode();
		login = OneDrive::LoginData::authorize(code);
		std::ofstream outfile("oneuploadlogin");
		login.writeTo(outfile);
		outfile.close();
		return 0;
	}
	std::ifstream storedlogin("oneuploadlogin");
	if(storedlogin.good())
	{
		login = OneDrive::LoginData(storedlogin);
	}
	else
	{
		std::string code = OneDrive::LoginData::authCode();
		login = OneDrive::LoginData::authorize(code);
	}
	storedlogin.close();

	if(argv[1][0] == '-' and argv[1][1] == 'f')
	{
		std::cout << "File Upload" << std::endl;
		if(argc < 3)
		{
			std::cout << "Insufficient Parameter" << std::endl;
			return 0;
		}
		if(argc < 4)
		{
			std::string fullpath(argv[2]);
			// Get only the file name
			int i = 0;
			while(argv[2][i] != 0)
			{
				if(argv[2][i] == '\\' || argv[2][i] == '/')
				{
					argv[2] = argv[2]+i+1;
					i = 0;
				}
				else
				{
					i++;
				}
			}
			login.upload(cURLpp::escape(std::string(argv[2])), fullpath);
			std::cout << std::endl;
			return 0;

		}
		login.upload(cURLpp::escape(std::string(argv[3])), std::string(argv[2]));
		std::cout << std::endl;
		return 0;
	}
	if(argv[1][0] == '-' and argv[1][1] == 'd')
	{
		std::cout << "Directory Upload" << std::endl;
		if(argc < 3)
		{
			std::cout << "Insufficient Parameter" << std::endl;
			return 0;
		}
		ftw(argv[2], recursiveUploadCallback, 1);
		std::cout << std::endl;
		return 0;
	}
	return 0;
}
