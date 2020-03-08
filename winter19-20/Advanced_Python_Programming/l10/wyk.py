import pickle
import shelve
import sqlite3

#  pickle

print("\npickle\n")
obj1 = {'uno': [1], 'duo': [2, 3], 'tres': [4, 5, 6]}
print(obj1)

with open('object.store', 'wb') as fh:
    pickle.dump(obj1, fh)

with open('object.store', 'rb') as fh:
    obj2 = pickle.load(fh)

print(obj2)

#  shelve

print("\nshelve\n")
with shelve.open('shelve') as db:
    for i in range(10):
        db['lista%i' % i] = [1, 2, 3, i]
    db.sync()
    for k in db:
        print("{}:{}".format(k, db[k]))

#  sqlite3

db = sqlite3.connect("/tmp/temp.db")