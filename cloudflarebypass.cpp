// MIT License :: Copyright (c) 2017 Karmel0x
// https://github.com/Karmel0x/cloudflare-js-bypass

#include <string>
#include <vector>

std::string solveCFJSChallenge(std::string response, std::string url){//cloudflare
	std::size_t pos1a = response.find("jschl_vc");// get form values
	std::size_t pos1b = response.find("\"/>", pos1a);
	std::string str1 = response.substr(pos1a + 17, pos1b - pos1a - 17);//jschl_vc value
	//printf("jschl_vc[%s] %d\n", str1.c_str(), str1.length());
	pos1a = response.find("pass");//" value="
	pos1b = response.find("\"/>", pos1a);
	std::string str2 = response.substr(pos1a + 13, pos1b - pos1a - 13);//pass value
	//printf("____pass[%s] %d\n", str2.c_str(), str2.length());

	pos1a = response.find("k,i,n,g,f,");// get form values
	pos1b = response.find(";a.value", pos1a);
	std::string str3 = response.substr(pos1a + 11, pos1b - pos1a - 11);
	for (;;){
		size_t pos6a = str3.find("+!![]");
		if (pos6a == std::string::npos) break;
		str3.replace(pos6a, 5, "1");
	}
	for (;;){
		size_t pos6a = str3.find("!+[]");
		if (pos6a == std::string::npos) break;
		str3.replace(pos6a, 4, "1");
	}
	for (;;){
		size_t pos6a = str3.find("+[]");
		if (pos6a == std::string::npos) break;
		str3.replace(pos6a, 3, "0");
	}
	pos1a = str3.find("\":");//":+((110)+(11))};
	pos1b = str3.find("}", pos1a);
	std::string str4 = str3.substr(pos1a + 2, pos1b - pos1a - 2);

	int sep_num = 0;//separated variable number
	int f_amount = 0;//calculated variables

	pos1a = str3.find("challenge-form");//":+((110)+(11))};
	str3 = str3.substr(pos1a + 20);
	pos1b = 0;
	std::string str5lel = "+=" + str4 + ";" + str3;
	for (;;){
		pos1a = str5lel.find("=", pos1b);
		if (pos1a == std::string::npos) break;
		pos1b = str5lel.find(";", pos1a);
		str3 = str5lel.substr(pos1a + 1, pos1b - pos1a - 1);

		int vn = 0;//variable num
		std::vector<int> sa; sa.push_back(0);//digits amount
		std::vector<bool> sb; sb.push_back(false);
		for (size_t i = 0; i < str3.length(); ++i){
			if (str3[i] == '1'){
				sa[vn]++;
				sb[vn] = true;
			}
			else if (str3[i] == '0')
				sb[vn] = true;
			else if (str3[i] == ')'){
				vn++;
				sa.push_back(0);
				sb.push_back(false);
			}
		}
		char bufferk[11] = "";
		for (size_t i = 0; i < sa.size(); ++i)
			if (sb[i] == true) sprintf_s(bufferk, "%s\%d", bufferk, sa[i]);

		int sep_val;
		sscanf_s(bufferk, "%d", &sep_val);
		if		(str5lel[pos1a - 1] == '+') f_amount += sep_val;
		else if (str5lel[pos1a - 1] == '-') f_amount -= sep_val;
		else if (str5lel[pos1a - 1] == '*') f_amount *= sep_val;
		sep_num++;
	}
	pos1a = url.find("://");
	if (pos1a == std::string::npos) pos1a = 0;
		else pos1a += 3;
	pos1b = url.find("/", pos1a);
	str3 = url.substr(0, pos1b);

	str4 = str3.substr(pos1a);
	f_amount += str4.length();// domain lenght

	return str3 + "/cdn-cgi/l/chk_jschl?jschl_vc=" + str1 + "&pass=" + str2 + "&jschl_answer=" + std::to_string(f_amount);
}