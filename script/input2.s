function findmax(a, n) {
	var ans1=a[0], i, ans2;
	for (i=1;i<n;++i)
		if (ans1<a[i])
			ans1=a[i];
	i=n-2;
	ans2=a[n-1];
	while (i>=0)
	{
		if (a[i]>ans2)
			ans2=a[i];
		i--;
	}
	if (ans1==ans2)
		return ans1;
	else
		return "impossible";
}

function mainScript() {
	var a = new Object();
	a[0] = 14;
	a[1] = 10;
	a[2] = 12;
	a[3] = 15;
	a[4] = 15;
	a[5] = 13;
	return findmax(a, 6);
}

