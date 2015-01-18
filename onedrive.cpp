#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "onedrive.h"

#include <iostream>
#include <fstream>

using namespace OneDrive;


static std::string buf = "";

static size_t  writeToBuf(char *ptr, size_t size, size_t nmemb)
{
	buf.append(ptr, nmemb);
	return size * nmemb;
}

static void clearBuf()
{
	buf = "";
}

LoginData::LoginData()
{
}

LoginData::LoginData(std::string json)
{
	//std::cout << json << std::endl << std::endl << std::endl;
	rapidjson::Document doc;
	doc.Parse(json.c_str());
	
	rapidjson::Value& val = doc["refresh_token"];
	_refreshToken = std::string(val.GetString());
	//std::cout << _refreshToken << std::endl;
}

LoginData::LoginData(std::istream &from)
{
	getline(from, _refreshToken);
}

LoginData::LoginData(const LoginData &login)
{
	_refreshToken = login._refreshToken;
}

void LoginData::writeTo(std::ostream &to)
{
	to << _refreshToken;
}

std::string LoginData::authCode()
{
	std::cout << "Please navigate to: https://login.live.com/oauth20_authorize.srf?client_id="+CLIENTID+"&scope=wl.signin%20wl.basic%20wl.skydrive_update%20wl.skydrive%20wl.offline_access&client_secret="+CLIENTSECRET+"&response_type=code&redirect_uri=https://login.live.com/oauth20_desktop.srf" << std::endl;
	std::cout << "After completing the authentication, please enter the auth code." << 
		std::endl <<
		"> ";
	std::string retval;
	getline(std::cin, retval);
	return retval;

}

LoginData LoginData::authorize(std::string code)
{
	try{
	// Connection options to connect to the MS auth server
	cURLpp::Options::Url optUrl("https://login.live.com/oauth20_token.srf");
	cURLpp::Options::Port optPort(443);
	cURLpp::Options::Post optPost(true);
	cURLpp::Options::HttpHeader optHeaders(std::list<std::string>(1, "Content-type: application/x-www-form-urlencoded"));
	cURLpp::Options::PostFields optFields("client_id="+CLIENTID+"&redirect_uri=https://login.live.com/oauth20_desktop.srf&client_secret="+CLIENTSECRET+"&code="+code+"&grant_type=authorization_code");
	cURLpp::Options::WriteFunction optWFunc(writeToBuf);
	cURLpp::Options::Timeout optTimeout(30);
	cURLpp::Easy request;
	request.setOpt(optUrl.clone());
	request.setOpt(optPort.clone());
	request.setOpt(optPost.clone());
	request.setOpt(optHeaders.clone());
	request.setOpt(optFields.clone());
	request.setOpt(optWFunc.clone());
	request.setOpt(optTimeout.clone());
	clearBuf();

	// Connect to the server
	request.perform();
	}
	catch(cURLpp::RuntimeError exception)
	{
		std::cout << "Network error authorizing" << std::endl;
		std::cout << exception.what() << std::endl;
		exit(0);
	}
	// Return a LoginData
	return LoginData(buf);
}

std::string LoginData::accessToken()
{
	try{
	// Connection options to connect to the MS auth server
	cURLpp::Options::Url optUrl("https://login.live.com/oauth20_token.srf");
	cURLpp::Options::Port optPort(443);
	cURLpp::Options::Post optPost(true);
	cURLpp::Options::HttpHeader optHeaders(std::list<std::string>(1, "Content-type: application/x-www-form-urlencoded"));
	cURLpp::Options::PostFields optFields("client_id="+CLIENTID+"&redirect_uri=https://login.live.com/oauth20_desktop.srf&client_secret="+CLIENTSECRET+"&refresh_token=" + refreshToken() + "&grant_type=refresh_token");
	cURLpp::Options::WriteFunction optWFunc(writeToBuf);
	cURLpp::Options::Timeout optTimeout(30);
	
	cURLpp::Easy request;
	request.setOpt(optUrl.clone());
	request.setOpt(optPort.clone());
	request.setOpt(optPost.clone());
	request.setOpt(optHeaders.clone());
	request.setOpt(optFields.clone());
	request.setOpt(optWFunc.clone());
	request.setOpt(optTimeout.clone());

	clearBuf();
	request.perform();
	}
	catch(cURLpp::RuntimeError exception)
	{
		std::cout << "Network error obtaining access token" << std::endl;
		std::cout << exception.what() << std::endl;
		exit(0);
	}

	rapidjson::Document doc;
	doc.Parse(buf.c_str());
	
	rapidjson::Value& val = doc["access_token"];
	return std::string(val.GetString());

}

std::string LoginData::refreshToken()
{
	return _refreshToken;
}

void LoginData::upload(std::string cloudname, std::string localname)
{
	std::ifstream infile(localname.c_str(), std::ios::binary);
	UpFile upfile(*this);
	upfile.loadData(infile);
	upfile.location(cloudname);
	upfile.upload();
	infile.close();
}

UpFile::UpFile(LoginData login)
{
	_data = (char*)malloc(5);
	_datalen = 0;
	_onedriveLoc = "";
	_login = login;
}

std::string UpFile::location()
{
	return _onedriveLoc;
}

std::string UpFile::location(std::string newlocation)
{
	_onedriveLoc = newlocation;
	return _onedriveLoc;
}

static char *updat;
static int uplen;
static int upindex;

static size_t readFunction(char *buffer, size_t size, size_t nmemb)
{
	size_t len = size * nmemb;
	if(len > uplen - upindex)
	{
		len = uplen - upindex;
	}
	memcpy((void*)buffer, (void*)(updat+upindex), len);
	upindex += len;
	std::cout << upindex/(double)uplen << "\r";
	std::cout.flush();
	return len;
}

bool UpFile::upload()
{	
	try{
	if(_datalen == 0)
	{
		return false;
	}
	upindex = 0;
	updat = _data;
	uplen = _datalen;

	// Connection options to connect to OneDrive server
	cURLpp::Options::Url optUrl("https://apis.live.net/v5.0/me/skydrive/files/"+_onedriveLoc+"?access_token="+_login.accessToken());
	cURLpp::Options::Port optPort(443);
	cURLpp::Options::WriteFunction optWFunc(writeToBuf);
	cURLpp::Options::Upload optPut(true);
	cURLpp::Options::InfileSize optSz(_datalen);
	cURLpp::Options::ReadFunction optRFunc(readFunction);
	cURLpp::Options::LowSpeedLimit optTimeout1(1);
	cURLpp::Options::LowSpeedTime optTimeout2(30);
	cURLpp::Easy request;
	request.setOpt(optUrl.clone());
	request.setOpt(optPort.clone());
	request.setOpt(optPut.clone());
	request.setOpt(optRFunc.clone());
	request.setOpt(optSz.clone());
	request.setOpt(optWFunc.clone());
	request.setOpt(optTimeout1.clone());
	request.setOpt(optTimeout2.clone());
	clearBuf();
	request.perform();
	//std::cout << std::endl;
	//std::cout << buf;
	}
	catch(cURLpp::RuntimeError exception)
	{
		std::cout << "Upload failed" << std::endl;
		std::cout << exception.what() << std::endl;
		return false;
	}
	return true;
}

bool UpFile::loadData(char *data, int datalen)
{
	free(_data);
	_data = (char*)malloc(datalen);
	memcpy(_data, data, datalen);
	_datalen = datalen;
	return true;
}

bool UpFile::loadData(std::istream &input)
{
	free(_data);
	input.seekg(0, std::ios::end);
	_datalen = input.tellg();
	_data = (char*)malloc(_datalen);
	input.seekg(0, std::ios::beg);
	input.read(_data, _datalen);
	return true;
}

UpFile::~UpFile()
{
	free(_data);
}
