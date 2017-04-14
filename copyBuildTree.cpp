#include <iostream>
#include <string>
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
	while (s != "#define TDML_BUILDTREE");

	freopen("tmp", "r", stdin);
	cin.clear();
	do
	{
		getline(cin, s);
		code.push_back("\t"+s);
	}
	while (!cin.eof());

	freopen("homework 1/homework 1/Transform/main.cpp", "w", stdout);
	for (auto it=code.begin(); it!=code.end(); it++) cout << *it << endl;
	cout << "}\n";
	
	return 0;
}
