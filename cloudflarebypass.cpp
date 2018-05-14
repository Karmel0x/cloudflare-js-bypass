// MIT License :: Copyright (c) 2017 Karmel0x
// https://github.com/Karmel0x/cloudflare-js-bypass

#include <string>
#include <vector>
//#include <iostream>
template<typename functor_type>
void ExtractStringBetweenDelimitersOnSameLevel(
	const std::string &original_string,
	char opening_delimiter, char closing_delimiter,
	functor_type &&functor)
{
	auto b = original_string.begin(), e = original_string.end(), p = b;
	int nesting_level = 0;

	while (b != e) {
		if (*b == closing_delimiter) {
			if (nesting_level > 0 && --nesting_level == 0) {
				functor(std::string(p, b));
			}
		}
		if (*b++ == opening_delimiter) {
			if (nesting_level++ == 0)
				p = b;
		}
	}
}
std::string solveCFJSChallenge(std::string response, std::string url){//cloudflare
	std::size_t pos1a = response.find("jschl_vc");// get form values
	std::size_t pos1b = response.find("\"/>", pos1a);
	std::string str1 = response.substr(pos1a + 17, pos1b - pos1a - 17);//jschl_vc value
	//printf("jschl_vc[%s] %d\n", str1.c_str(), str1.length());
	pos1a = response.find("pass");//" value="
	pos1b = response.find("\"/>", pos1a);
	std::string str2 = response.substr(pos1a + 13, pos1b - pos1a - 13);//pass value
	//printf("    pass[%s] %d\n", str2.c_str(), str2.length());

	pos1a = response.find("k,i,n,g,f,");// get form values
	pos1b = response.find(";a.value", pos1a);
	std::string str3 = response.substr(pos1a + 11, pos1b - pos1a - 11);
	//std::cout << std::endl << str3 << std::endl;system("pause");
	size_t pos2a;
	for (;;){
		pos2a = str3.find("+!![]");
		if (pos2a == std::string::npos) break;
		str3.replace(pos2a, 5, "1");
	}
	for (;;){
		pos2a = str3.find("!+[]");
		if (pos2a == std::string::npos) break;
		str3.replace(pos2a, 4, "1");
	}
	for (;;){
		pos2a = str3.find("+[]");
		if (pos2a == std::string::npos) break;
		str3.replace(pos2a, 3, "0");
	}
	pos1a = str3.find("\":");//":+((110)+(11))};
	pos1b = str3.find("}", pos1a);
	std::string str4 = str3.substr(pos1a + 2, pos1b - pos1a - 2);

	int sep_num = 0;//separated variable number
	double f_amount = 0;//calculated variables

	pos1a = str3.find("challenge-form");
	str3 = str3.substr(pos1a + 20);
	pos1b = 0;
	std::string str5e = "+=" + str4 + ";" + str3;
	for (;;){
		pos1a = str5e.find("=", pos1b);
		if (pos1a == std::string::npos) break;
		pos1b = str5e.find(";", pos1a);
		str3 = str5e.substr(pos1a + 1, pos1b - pos1a - 1);

		std::vector<std::string> part;

		ExtractStringBetweenDelimitersOnSameLevel(str3, '(', ')',
			[&](const std::string &strin){
				part.push_back(strin);
		});
		int chall_t[6] = { 0 };
		for (size_t i = 0; i < part.size(); ++i){

			std::string strQ = "";
			ExtractStringBetweenDelimitersOnSameLevel(part[i], '(', ')',
				[&](const std::string &strin) {
					int Q = 0;
					for (size_t l = 0; l < strin.length(); ++l) {
						if (strin[l] == '1') Q++;
					}
					strQ += std::to_string(Q);
					
			});
			chall_t[i] = stoi(strQ);
		}
		double answ = chall_t[0];
		for (int i = 1; i < part.size(); ++i)
			if(chall_t[i] > 0)
				answ /= (double)chall_t[i];

		if      (str5e[pos1a - 1] == '+') f_amount += answ;
		else if (str5e[pos1a - 1] == '-') f_amount -= answ;
		else if (str5e[pos1a - 1] == '*') f_amount *= answ;
		sep_num++;//printf("%.15f\n", answ);
	}

	pos1a = url.find("://");
	if (pos1a == std::string::npos) pos1a = 0;
		else pos1a += 3;
	pos1b = url.find("/", pos1a);
	str3 = url.substr(0, pos1b);

	str4 = str3.substr(pos1a);
	f_amount += str4.length();// domain lenght

	std::string f_amount_fixed(16, '\0');
	auto written = std::snprintf(&f_amount_fixed[0], f_amount_fixed.size(), "%.10f", f_amount);
	f_amount_fixed.resize(written);

	return str3 + "/cdn-cgi/l/chk_jschl?jschl_vc=" + str1 + "&pass=" + str2 + "&jschl_answer=" + f_amount_fixed;
}
