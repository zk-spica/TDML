#include <iostream>
#include <string>
#include <cstdio>
#include <list>

using namespace std;

list<string> code;

int main()
{
	freopen("homework 1/homework 1/Transform/main.cpp", "r", stdin);
	string s;
	do
	{
		getline(cin, s);
		code.push_back(s);
	}
	while (s != "#define TDML_BUILDCSSLIST");

	cerr << "alive\n";

	freopen("tmpCSS", "r", stdin);
	cin.clear();
	do
	{
		getline(cin, s);
		code.push_back("\t"+s);
	}
	while (!cin.eof());

	cerr << "alive\n";

	freopen("homework 1/homework 1/Transform/main.cpp", "r", stdin);
	cin.clear();
	do
	{
		//s = "";
		getline(cin, s);

//		cerr << "get s=" << s << endl;

		//if (s == "") break;
	}
	while (s != "#define TDML_BUILDCSSLIST_END");

	cerr << "alive\n";

	code.push_back(s);
	do
	{
		getline(cin,s);
		code.push_back(s);
	}
	while (!cin.eof());

	cerr << "alive\n";

	freopen("homework 1/homework 1/Transform/main.cpp", "w", stdout);
	for (auto it=code.begin(); it!=code.end(); it++) cout << *it << endl;
	
	return 0;
}
