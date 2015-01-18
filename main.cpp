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

std::string getAuthCode()
{
	std::cout << "Please navigate to: https://login.live.com/oauth20_authorize.srf?client_id=000000004413537F&scope=wl.signin%20wl.basic%20wl.skydrive_update%20wl.skydrive%20wl.offline_access&client_secret=VFPG2Fh7L1Ja4vNlC1QX2OemaTP7mvij&response_type=code&redirect_uri=https://login.live.com/oauth20_desktop.srf" << std::endl;
	std::cout << "After completing the authentication, please enter the auth code." << 
		std::endl <<
		"> ";
	std::string retval;
	getline(std::cin, retval);
	return retval;
}

std::string getTokens(std::string authCode)
{
	// Connection options to connect to the MS auth server
	cURLpp::Options::Url optUrl("https://login.live.com/oauth20_token.srf");
	cURLpp::Options::Port optPort(THE_PORT);
	cURLpp::Options::SslVerifyHost optSsl(false);
	cURLpp::Options::Post optPost(true);
	cURLpp::Options::HttpHeader optHeaders(std::list<std::string>(1, "Content-type: application/x-www-form-urlencoded"));
	cURLpp::Options::PostFields optFields("client_id=000000004413537F&redirect_uri=https://login.live.com/oauth20_desktop.srf&client_secret=VFPG2Fh7L1Ja4vNlC1QX2OemaTP7mvij&code="+authCode+"&grant_type=authorization_code");
	cURLpp::Options::WriteFunction optWFunc(writeToBuf);
	cURLpp::Easy request;
	request.setOpt(optUrl.clone());
	request.setOpt(optPort.clone());
	request.setOpt(optSsl.clone());
	request.setOpt(optPost.clone());
	request.setOpt(optHeaders.clone());
	request.setOpt(optFields.clone());
	request.setOpt(optWFunc.clone());
	clearBuf();
	request.perform();
	return buf;
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
	}
	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		return 0;
	}
	//std::string loginjson = getTokens(getAuthCode());
	std::istringstream strum("CkSsSeVaYFYG7!oqNF6R5*st*GIVckdz9VNlcU8guzNHhjGmBmks9E00flkj37OomYeXYr!wjZQjlG!vkIlI2Sq2gdBec9QG!iyzzrZ!23GKG3O7P593bz8dkuSWh9ATViPzro6bNz4TL78VSkJNV8MvwPRTlikqveG3ZvIj0vJGh1VGOsL!Waup7jipOrnKQJVy*kHzqQfVOSsc*PChB4qhhavKkPE2Mm2AYgMxcBJpZFhWmwRR84gJOn940VmS2UYcOeC5m9595ieu4AqjmTDDLnxGzsbKdjcKKUMt6dNkwxxd2*W*Av83X4WZ0mHSLYOQQ3tTU*JBuaQ4xaRD4onrS*aEdax5rDH!ZAhn3Te9PbN35DI9F1HKIrq26qp4qSYgCSt5tDQlOTUZ3RO2Dly63JDqyCOEZVeus7dr!Mq7BjNXkKrou87ETYc3l68QoA$$");
	
	OneDrive::LoginData loginner(strum);
	login = loginner;

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
