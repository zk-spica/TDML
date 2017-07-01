#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

const int N=10000000;
char a[N];
int n;

bool match(int i, char * x)
{
	int len=strlen(x);
	if (a[i-1]>='0'&&a[i-1]<='9'||a[i-1]>='a'&&a[i-1]<='z'||a[i-1]>='A'&&a[i-1]<='Z'||a[i-1]=='-'||a[i-1]=='_')return false;
	if (a[i+len]>='0'&&a[i+len]<='9'||a[i+len]>='a'&&a[i+len]<='z'||a[i+len]>='A'&&a[i+len]<='Z'||a[i+len]=='-'||a[i+len]=='_')return false;
	for (int j=0; j<len; j++)
		if (a[i+j]!=x[j])
			return false;
	return true;
}

int main(int argc, char ** argv)
{
	if (argc!=3)
	{
		printf("input infile and outfile!\n");
		return 0;
	}
	FILE *fpin1=fopen(argv[1], "r"), *fpin2=fopen("Type__", "r"), *fpout=fopen(argv[2], "w"), *fptmp=fopen("__tmp", "w");
	//fprintf(fpout, "class Script__\n{\npublic:\n\n");
	char c;
	//while (fscanf(fpin2, "%c", &c)!=EOF)
		//fprintf(fpout, "%c", c);
	
	while (fscanf(fpin1, "%c", &a[++n])!=EOF);
	n--;
	a[0]=' ';
	for (int i=n+1;i<=n+20;i++) a[i]=' ';
	int flag=0, count;
	for (int i=1; i<=n; i++)
	{
		if (a[i]=='\"'&&a[i-1]!='\\')flag=1-flag;
		if (!flag)
		{
			if (match(i, "var"))
			{
				fprintf(fptmp, "Type__");
				i+=2;
				continue;
			}
			if (match(i, "function"))
			{
				fprintf(fptmp, "Type__");
				i+=8;
				count=0;
				while (1)
				{
					fprintf(fptmp, "%c", a[i]);
					if (a[i]==',')
					{
						fprintf(fptmp, "Type__");
						if (a[i+1]!=' ')
							fprintf(fptmp, " ");
					}
					if (a[i]=='(')
					{
						int j;
						for (j=i+1; a[j]!=')'&&a[j]!=','; j++);
						if (a[j]==',')
						{
							fprintf(fptmp, "Type__");
							if (a[i+1]!=' ')
								fprintf(fptmp, " ");
						}
					}
					if (a[i]=='(')
						count++;
					if (a[i]==')')
					{
						count--;
						if (count==0)
							break;
					}
					i++;
					if (i>n)
						break;
				}
				continue;
			}
			if (match(i, "if"))
			{
				fprintf(fptmp, "if (");
				i+=2;
				count=0;
				while (1)
				{
					fprintf(fptmp, "%c", a[i]);
					if (a[i]=='(')
						count++;
					if (a[i]==')')
					{
						count--;
						if (count==0)
							break;
					}
					i++;
					if (i>n)
						break;
				}
				fprintf(fptmp, ".getbool() )");
				continue;
			}
			if (match(i, "while"))
			{
				fprintf(fptmp, "while (");
				i+=5;
				count=0;
				while (1)
				{
					fprintf(fptmp, "%c", a[i]);
					if (a[i]=='(')
						count++;
					if (a[i]==')')
					{
						count--;
						if (count==0)
							break;
					}
					i++;
					if (i>n)
						break;
				}
				fprintf(fptmp, ".getbool() )");
				continue;
			}
			if (match(i, "for"))
			{
				fprintf(fptmp, "for ");
				i+=3;
				count=0;
				int countx=0;
				while (1)
				{
					
					if (a[i]==';')
					{
						countx++;
						if (countx==2)
							fprintf(fptmp, ").getbool()");
					}
					fprintf(fptmp, "%c", a[i]);
					if (a[i]==';'&&countx==1)
						fprintf(fptmp,"(");
					if (a[i]=='(')
						count++;
					if (a[i]==')')
					{
						count--;
						if (count==0)
							break;
					}
					i++;
					if (i>n)
						break;
				}
				continue;
			}
		}
		fprintf(fptmp, "%c", a[i]);
	}
	
	fclose(fpin1);
	fclose(fpin2);
	fclose(fptmp);
	fptmp=fopen("__tmp", "r");
	n=0;
	while (fscanf(fptmp, "%c", &a[++n])!=EOF);
	n--;
	a[0]=' ';
	for (int i=n+1;i<=n+20;i++) a[i]=' ';
	flag=0;
	bool num;
	for (int i=1;i<=n;i++)
	{
		if (a[i]=='\"'&&a[i-1]!='\\')flag=1-flag;
		if (!flag&&a[i]=='.')
		{
			num=true;
			int j;
			for (j=i+1;a[j]>='0'&&a[j]<='9'||a[j]>='a'&&a[j]<='z'||a[j]>='A'&&a[j]<='Z'||a[j]=='_';j++)
			{
				if (!(a[j]>='0'&&a[j]<='9'))
					num=false;
			}
			if (!num&&!(a[i+1]=='g'&&a[i+2]=='e'&&a[i+3]=='t'&&a[i+4]=='b'&&a[i+5]=='o'&&a[i+6]=='o'&&a[i+7]=='l'))
			{
				fprintf(fpout,"[\"");
				for (int k=i+1; k<j; k++)
					fprintf(fpout, "%c", a[k]);
				fprintf(fpout,"\"]");
				i=j-1;
				continue;
			}
		}
		fprintf(fpout, "%c", a[i]);
	}
	//fprintf(fpout, "\n};\n");
	fclose(fpout);
	return 0;
}
