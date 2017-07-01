function cal(a123, b123b, c) {
	var a = new Object();
	a[1] = 10;
	a[a[1]] = a[1];
	var b=new Object();
	b[1]=new Object();
	b[1].c=new Object();
	b[1].c.d=a123++;
	return a[10]+b[1].c.d;
}

function mainScript() {
	return cal(1, 2, 5);
}

