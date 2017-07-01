function mainScript() {
	var a="abc", b="a", c=new Object(), d=1, e=2;
	c.a = a;
	c.b = b;
	d = (++d)+d;
	e = (e++)+e;
	if (c.a + c.b == "abca")
		c.a = 1;
	else c.a = 2;
	return c.a*100+d*10+e;
}

