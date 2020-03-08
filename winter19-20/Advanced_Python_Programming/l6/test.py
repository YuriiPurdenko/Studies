import requests #pobranie strony internwetowej

from collections import Counter
from string import punctuation
from bs4 import BeautifulSoup #polecane przez wykladowce
import urllib
import re
import time 

url = "https://en.wikipedia.org/wiki/Python_%28programming_language%29"

r = requests.get(url)
soup = BeautifulSoup(r.text, "lxml")
for link in soup.findAll('a', attrs={'href': re.compile("^http://")}):
    print(link.get("href"))
#print(soup.prettify())
print(len(re.findall(r'Python', requests.get(url).text)))

print(len(soup.find(text=lambda text: text and "Python" in text)))

for script in soup(["script", "style"]):
    script.extract()



#text = soup.find_all(text=True)
text = soup.get_text()

lines = (line.strip() for line in text.splitlines())
chunks = (phrase.strip() for line in lines for phrase in line.split("  "))
chunks = (phrase.strip() for line in lines for phrase in line.split("  "))
text = '\n'.join(chunk for chunk in chunks if chunk)
text += '.'
sentences = re.split(r'[.!?\n]', text)
print(sentences)
for x in sentences:
    if 'Python' in x:
        print(x, "HALO")
'''
chunks = (phrase.strip() for line in lines for phrase in line.split("  "))
text = '\n'.join(chunk for chunk in chunks if chunk)
#print(text)
sentences = re.split(r' *[\.\?!][\'"\)\]]* *', text)
for x in sentences:
    print(x,"HALO")
    '''