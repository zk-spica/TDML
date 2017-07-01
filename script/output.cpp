class Script__
{
public:

class Type__;

class Object
{
public:
	int num;
	map<string, Type__> t;
	Object() {num=1;}
};

class Type__
{
public:
	int val1;
	double val2;
	string val3;
	Object * val4;
	int now;
	Type__()
	{
		now=1; val1=0;
	}
	Type__(const int & x)
	{
		now=1; val1=x;
	}
	Type__(const double & x)
	{
		now=2; val2=x;
	}
	Type__(const char * x)
	{
		now=3; val3=string(x);
	}
	Type__(const string & x)
	{
		now=3; val3=x;
	}
	Type__(Object * x)
	{
		now=4; val4=x;
	}
	Type__(const Type__ & x)
	{
		now=x.now;
		if (now==1) val1=x.val1;
		if (now==2) val2=x.val2;
		if (now==3) val3=x.val3;
		if (now==4) {val4=x.val4; val4->num++;}
	}
	Type__ operator= (const int & x)
	{
		if (now==4){val4->num--; if (val4->num==0)delete val4;}
		now=1; val1=x;
		return *this;
	}
	Type__ operator= (const double & x)
	{
		if (now==4){val4->num--; if (val4->num==0)delete val4;}
		now=2; val2=x;
		return *this;
	}
	Type__ operator= (const char * x)
	{
		if (now==4){val4->num--; if (val4->num==0)delete val4;}
		now=3; val3=string(x);
		return *this;
	}
	Type__ operator= (const string & x)
	{
		if (now==4){val4->num--; if (val4->num==0)delete val4;}
		now=3; val3=x;
		return *this;
	}
	Type__ operator= (Object * x)
	{
		if (now==4){val4->num--; if (val4->num==0&&val4!=x)delete val4;}
		now=4; val4=x;
		val4->num++;
		return *this;
	}
	
	Type__ operator= (const Type__ & x)
	{
		if (now==4){val4->num--; if (val4->num==0&&val4!=x.val4)delete val4;}
		now=x.now;
		if (now==1) val1=x.val1;
		if (now==2) val2=x.val2;
		if (now==3) val3=x.val3;
		if (now==4) {val4=x.val4; val4->num++;}
		return *this;
	}
	Type__ operator+ (const Type__ & x) const
	{
		Type__ tmp;
		if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1+x.val1; return tmp;}
		if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1+x.val2; return tmp;}
		if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2+x.val1; return tmp;}
		if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2+x.val2; return tmp;}
		if (now==3 && x.now==3) { tmp.now=3; tmp.val3=val3+x.val3; return tmp;}
		tmp.now=1; tmp.val1=0; return tmp;
	}
	Type__ operator- (const Type__ & x) const
	{
		Type__ tmp;
		if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1-x.val1; return tmp;}
		if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1-x.val2; return tmp;}
		if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2-x.val1; return tmp;}
		if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2-x.val2; return tmp;}
		tmp.now=1; tmp.val1=0; return tmp;
	}
	Type__ operator* (const Type__ & x) const
	{
		Type__ tmp;
		if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1*x.val1; return tmp;}
		if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1*x.val2; return tmp;}
		if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2*x.val1; return tmp;}
		if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2*x.val2; return tmp;}
		tmp.now=1; tmp.val1=0; return tmp;
	}
	Type__ operator/ (const Type__ & x) const
	{
		Type__ tmp;
		if (now==1 && x.now==1) 
		{ 
			tmp.now=2; tmp.val2=(double)val1/x.val1; 
			if (tmp.val2==int(tmp.val2)) {tmp.now=1;tmp.val1=int(tmp.val2);}
			return tmp;
		}
		if (now==1 && x.now==2) { tmp.now=2; tmp.val2=val1/x.val2; return tmp;}
		if (now==2 && x.now==1) { tmp.now=2; tmp.val2=val2/x.val1; return tmp;}
		if (now==2 && x.now==2) { tmp.now=2; tmp.val2=val2/x.val2; return tmp;}
		tmp.now=1; tmp.val1=0; return tmp;
	}
	Type__ operator& (const Type__ & x) const
	{
		Type__ tmp;
		if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1&x.val1; return tmp;}
		tmp.now=1; tmp.val1=0; return tmp;
	}
	Type__ operator| (const Type__ & x) const
	{
		Type__ tmp;
		if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1|x.val1; return tmp;}
		tmp.now=1; tmp.val1=0; return tmp;
	}
	Type__ operator^ (const Type__ & x) const
	{
		Type__ tmp;
		if (now==1 && x.now==1) { tmp.now=1; tmp.val1=val1^x.val1; return tmp;}
		tmp.now=1; tmp.val1=0; return tmp;
	}
	Type__ operator>(const Type__ & x) const
	{
		if (now==1 && x.now==1) return Type__(val1>x.val1?1:0);
		if (now==1 && x.now==2) return Type__(val1>x.val2?1:0);
		if (now==2 && x.now==1) return Type__(val2>x.val1?1:0);
		if (now==2 && x.now==2) return Type__(val2>x.val2?1:0);
		return Type__(0);
	}
	Type__ operator<(const Type__ & x) const
	{
		if (now==1 && x.now==1) return Type__(val1<x.val1?1:0);
		if (now==1 && x.now==2) return Type__(val1<x.val2?1:0);
		if (now==2 && x.now==1) return Type__(val2<x.val1?1:0);
		if (now==2 && x.now==2) return Type__(val2<x.val2?1:0);
		return Type__(0);
	}
	Type__ operator>=(const Type__ & x) const
	{
		if (now==1 && x.now==1) return Type__(val1>=x.val1?1:0);
		if (now==1 && x.now==2) return Type__(val1>=x.val2?1:0);
		if (now==2 && x.now==1) return Type__(val2>=x.val1?1:0);
		if (now==2 && x.now==2) return Type__(val2>=x.val2?1:0);
		return Type__(0);
	}
	Type__ operator<=(const Type__ & x) const
	{
		if (now==1 && x.now==1) return Type__(val1<=x.val1?1:0);
		if (now==1 && x.now==2) return Type__(val1<=x.val2?1:0);
		if (now==2 && x.now==1) return Type__(val2<=x.val1?1:0);
		if (now==2 && x.now==2) return Type__(val2<=x.val2?1:0);
		return Type__(0);
	}
	Type__ operator==(const Type__ & x) const
	{
		if (now==1 && x.now==1) return Type__(val1==x.val1?1:0);
		if (now==1 && x.now==2) return Type__(val1==x.val2?1:0);
		if (now==2 && x.now==1) return Type__(val2==x.val1?1:0);
		if (now==2 && x.now==2) return Type__(val2==x.val2?1:0);
		if (now==3 && x.now==3) return Type__(val3==x.val3?1:0);
		return Type__(0);
	}
	Type__ operator++(int)
	{
		if (now==1) {Type__ tmp(val1); val1++; return tmp;}
		return *this;
	}
	const Type__ & operator++()
	{
		if (now==1) val1++;
		return *this;
	}
	Type__ operator--(int)
	{
		if (now==1) {Type__ tmp(val1); val1--; return tmp;}
		return *this;
	}
	const Type__ & operator--()
	{
		if (now==1) val1--;
		return *this;
	}
	bool getbool() const
	{
		if (now==1&&val1==0 || now==2&&val2==0.0 || now==3&&val3==string(""))
			return false;
		return true;
	}
	Type__ operator&&(const Type__ & x) const
	{
		return Type__( (this->getbool()&&x.getbool())?1:0);
	}
	Type__ operator||(const Type__ & x) const
	{
		return Type__( (this->getbool()||x.getbool())?1:0);
	}
	string Tostring(int x)
	{
		ostringstream ss;
		ss<<x;
		return ss.str();
	}
	string Tostring(double x)
	{
		ostringstream ss;
		ss<<x;
		return ss.str();
	}
	Type__ & operator[](const Type__ & x)
	{
		if (now!=4) return *(new Type__(0));
		if (x.now==1) return val4->t[Tostring(x.val1)];
		if (x.now==2) return val4->t[Tostring(x.val2)];
		if (x.now==3) return val4->t[x.val3];
		return *(new Type__(0));
	}
	Type__ & operator[](int x)
	{
		if (now!=4) return *(new Type__(0));
		return val4->t[Tostring(x)];
	}
	
};
Type__ cal(Type__ a123,Type__ b123b,Type__ c) {
	Type__ a = new Object();
	a[1] = 10;
	a[a[1]] = a[1];
	Type__ b=new Object();
	b[1]=new Object();
	b[1]["c"]=new Object();
	b[1]["c"]["d"]=a123++;
	return a[10]+b[1]["c"]["d"];
}

Type__ mainScript() {
	return cal(1, 2, 5);
}


};
