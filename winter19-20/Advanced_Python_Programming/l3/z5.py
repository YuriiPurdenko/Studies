def kompresja(tex):
    new = ""
    count = 1
    i = 0
    le = len(tex)
    while i < le:
        while i < le-1 and tex[i] == tex[i+1]:
            count += 1
            i += 1
        if count > 1:
            new += str(count)
        new += tex[i]
        count = 1
        i += 1
    return new


def dekompresja(tex):
    new = ""
    i = 0
    le = len(tex)
    count = 0
    while i < le:
        while tex[i] >= '0' and tex[i] <= '9':
            count *= 10
            count += int(tex[i])
            i += 1
        if count > 0:
            while count > 0:
                count -= 1
                new += tex[i]
        else:
            new += tex[i]
        count = 0
        i += 1
    return new


print(kompresja("abbcccddddeeeee"))
print(dekompresja(kompresja("abbcccddddeeeee")))
