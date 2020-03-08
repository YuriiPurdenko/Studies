import requests
from bs4 import BeautifulSoup
import re

head1 = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36'
head2 = ' (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'
header = {'User-Agent': head1 + head2}
h = "href"


def crawl(start_page, distance, action):
    vis = {}

    def crawl_recursive(start_page, distance, action):
        vis[start_page] = True
        r = requests.get(start_page, headers=header)
        soup = BeautifulSoup(r.text, "lxml")
        yield(start_page, action(soup))
        if distance == 1:
            return
        for link in soup.findAll('a', attrs={h: re.compile("^http://")}):
            if link not in vis:
                yield from crawl_recursive(link.get(h), distance-1, action)
    yield from crawl_recursive(start_page, distance, action)


# funkcja liczaca wystapienia slowa Python, ulatwia testowanie
# def find_python(page):
#    return len(re.findall(r'\WPython\W', requests.get(page).text))

def find_python(soup):
    text = soup.get_text()
    z = []
    #  dzieli na linie i usuwa spacje przy liniach
    lines = (line.strip() for line in text.splitlines())
    #  naglowki zbija do jednej linii
    chunks = (phrase.strip() for line in lines for phrase in line.split("  "))
    #  tworzy jednolity tekst
    text = '\n'.join(chunk for chunk in chunks if chunk)
    text += '.'
    sentences = re.split(r'[.!?\n]', text)
    for x in sentences:
        if 'Python' in x:
            z.append(x)
    return z


#url = "https://www.python.org/"
url = "https://www.ii.uni.wroc.pl/~marcinm/dyd/python/"

for x in crawl(url, 2, find_python):
    print(x,'\n')
