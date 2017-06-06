#include "cloudflarebypass.h"
#include <curl/curl.h>
#include <iostream>

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s){//curl
	size_t newLength = size*nmemb;
	size_t oldLength = s->size();
	try{ s->resize(oldLength + newLength); }
	catch (std::bad_alloc &e){ return 0; }

	std::copy((char*)contents, (char*)contents + newLength, s->begin() + oldLength);
	return size*nmemb;
}
std::string CurlRequest(std::string url_, std::string ref = "", bool ssl = true){
	CURL *curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	std::string s;
	if (curl){
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:52.0) Gecko/20100101 Firefox/52.0");
		if (ref != "")curl_easy_setopt(curl, CURLOPT_REFERER, ref.c_str());//not necessary
		if (ssl){
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);//only for https
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);//only for https
		}
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L);	   //reconnect timeout
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8L);//connect timeout
		curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 8L);//receiving data timeout
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
		curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());

	retr:s = "";
		res = curl_easy_perform(curl);//Perform the request, res will get the return code

		if (res != CURLE_OK){//Check for errors
			fprintf(stderr, "curl_easy_perform() failed: %d %s\n", res, curl_easy_strerror(res));
			system("pause");
			goto retr;
		}
		if (s.find("Attention Required") != std::string::npos || //ip banned by CloudFlare
			s.find("Access denied") != std::string::npos ||
			s.find("restrict access") != std::string::npos){
			system("pause");
			goto retr;
		}
		else if (s.find("503 Service Temporarily Unavailable") != std::string::npos ||
			s.find("408 Request Time-out") != std::string::npos){
			system("pause");
			goto retr;
		}
		curl_easy_cleanup(curl);
	}
	return s;
}

int main(int argc, char* argv[]){
	std::string url = "https://cloudflareprotected.website/";
retry:
	std::string r1 = CurlRequest(url);//ssl = true
	if (r1.find("Just a moment") != std::string::npos){
		std::string JS = solveCFJSChallenge(r1, url); std::cout << JS << std::endl;
		Sleep(4000);// mimic waiting process
		CurlRequest(JS, url);
		goto retry;
	}

	std::cout << r1 << std::endl;
	system("pause");
	return 0;
}