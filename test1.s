
var cam = new Object();
var camDOM;
var velo = 0.1;
var lastX, lastY;

function onRedisplay()
{
	if (isKeyboardDown('w'))
	{
		cam.pos.y = cam.pos.y+velo;
	}
	modify(camDOM, "y-offset", cam.pos.y);
	cam.look.x = cam.pos.x + cam.dir.x;
	cam.look.y = cam.pos.y + cam.dir.y;
	cam.look.z = cam.pos.z + cam.dir.z;
	
	var tmp = "(";
	tmp = tmp+cam.look.x+","+cam.look.y+","+cam.look.z+")";
	modify(camDOM, "look-at", tmp);
	return 0;
}

function onMouseDown(button, x, y)
{
	if (button == 0)
	{
		lastX = x;
		lastY = y;
	}
	return 0;
}

function onMouseUp(button, x, y)
{
	return 0;
}

function onMouseMove(x, y)
{
	if (isMouseDown(0))
	{
		var theta = (x-lastX)*0.001;
		var tmpX = cam.dir.x*Cos(theta)-cam.dir.y*Sin(theta);
		var tmpY = cam.dir.y*Cos(theta)+cam.dir.x*Sin(theta);
		cam.dir.x = tmpX;
		cam.dir.y = tmpY;
		lastX = x;
		lastY = y;
	}
	return 0;
}

function mainScript()
{
	cam.pos = new Object();
	cam.pos.x = 0;
	cam.pos.y = -20;
	cam.pos.z = 0;
	cam.dir = new Object();
	cam.dir.x = 0;
	cam.dir.y = 1;
	cam.dir.z = 0;
	cam.look = new Object();
	cam.look.x = 0;
	cam.look.y = 0;
	cam.look.z = 0;
	camDOM = getElementById("camera");
	
	return 0;
}

