
def vat_faktura(lis):
	x = sum(lis)
	return round(x*0.23,2)
	
def vat_paragon(lis):
	x = sum(x*0.23 for x in lis)
	return round(x,2)


zakupy = [0.2, 0.5, 4.59,6]

print(vat_paragon(zakupy))
print(vat_faktura(zakupy))

print(vat_faktura(zakupy) == vat_paragon(zakupy))
