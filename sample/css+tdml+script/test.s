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

var cnt = 0;
var x, rot, off, len, tex;
function onRedisplay()
{
	++cnt;
	
	if(cnt >= 50) cnt = 0;
	
	printf("cnt = %d\n", cnt);
	
	x = getElementById("lu");
	rot = cnt/50.0*6.28;
	modify(x, "z-rotation", tostring(rot));
	
	x = getElementById("ru");
	off = cnt/20+2;
	modify(x, "x-offset", tostring(off));
	
	x = getElementById("ld");
	len = cnt/20+2;
	modify(x, "x-length", tostring(len));
	
	x = getElementById("rd");
	if (cnt > 25) tex = "sun.bmp";
	else tex = "earth.bmp";
	modify(x, "texture", tex);
	
	return 0;
}

function mainScript() {
//	return cal(1, 2, 5);
//	var x = getElementById("ball");
//	modify(x, "color", "rgb(1,0,0)");
	
	
	
	return 0;
}

