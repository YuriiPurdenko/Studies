import requests
import threading
from bs4 import BeautifulSoup
import re
import queue

head1 = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36'
head2 = ' (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'
header = {'User-Agent': head1 + head2}
h = "href"
lock = threading.Condition()


def crawl(start_page, distance, action):
    vis = {}
    odp = queue.Queue()

    def crawl_rec(start_page, dist, act):
        watki = []
        print(threading.active_count())
        lock.acquire()
        if start_page not in vis:
            vis[start_page] = True
        else:
            return
        lock.release()
        r = requests.get(start_page, headers=header)
        soup = BeautifulSoup(r.text, "lxml")
        odp.put((start_page, act(soup)))
        if dist == 1:
            return
        for link in soup.findAll('a', attrs={h: re.compile("^http://")}):
            thread = threading.Thread(target=crawl_rec, args=(link.get(h), dist - 1, act))
            watki.append(thread)
            thread.start()
        for x in watki:
            x.join()
    crawl_rec(start_page, distance, action)
    return odp

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


# url = "https://www.python.org/"
url = "https://www.ii.uni.wroc.pl/~marcinm/dyd/python/"
res = crawl(url, 2, find_python)
for x in range(res.qsize()):
    print(res.get())
