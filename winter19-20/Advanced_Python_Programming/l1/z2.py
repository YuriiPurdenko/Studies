
def zaplac(kwota):
	count = 0
	print("Z portfela nalezy wyjac")
	lis = [20,10,5,2,1]
	for x in lis:
		while kwota >= x:
			count += 1
			kwota -= x
		if count != 0: 
			if x > 5:
				print "banknotow", x, "zl:",count
				count = 0
			else:
				print "monet", x, "zl:",count
				count = 0
'''
	print("Z portfela nalezy wyjac")
	while kwota >= 20:
		count += 1
		kwota -= 20;
	if count != 0:
		print "banknotow 20 zl:",count
		count = 0
	
	while kwota >= 10:
		count += 1
		kwota -= 10;
	if count != 0:
		print "banknotow 10 zl:",count
		count = 0 
		
	while kwota >= 5:
		count += 1
		kwota -= 5;
	if count != 0:
		print " monet 5 zl",count
		count = 0 
		
	while kwota >= 2:
		count += 1
		kwota -= 2;
	if count != 0:
		print "monet 2 zl:",count
		count = 0 
		
	while kwota >= 1:
		count += 1
		kwota -= 1;
	if count != 0:
		print "monet 1 zl:",count
		count = 0
	'''
	return

zaplac(123)
		
