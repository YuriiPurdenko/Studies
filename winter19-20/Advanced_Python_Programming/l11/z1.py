from sqlalchemy import create_engine, ForeignKey, Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, relationship, validates
import sys


def prefix(s1, s2):
    ok = True
    if len(s1) > len(s2):
        return False
    for x in range(len(s1)):
        if s1[x] != s2[x]:
            ok = False
            break
    return ok


def addbook():
    args = sys.argv
    b = Book(title='')
    for x in range(3, len(args)):
        if prefix('--title=', args[x]):
            b.title = args[x][8:len(args[x])]
        if prefix('--author=', args[x]):
            b.author = args[x][9:len(args[x])]
        if prefix('--year=', args[x]):
            b.year = args[x][7:len(args[x])]
    if b.title == '':
        print("In addbook title have to be a non zero length string")
        session.close()
        return
    book = session.query(Book).filter(Book.title == b.title).first()
    if book is not None:
        print("Book already in database")
        session.close()
        return
    session.add(b)
    session.commit()
    session.close()


def editbook():
    args = sys.argv
    if prefix('--title=', args[3]):
        key = args[3][8:len(args[3])]
        book = session.query(Book).filter(Book.title == key).first()
        if book is None:
            print("Book not in database")
            session.close()
            return
        for x in range(4, len(args)):
            if prefix('--title=', args[x]):
                word = args[x][8:len(args[x])]
                session.query(Book).filter(Book.title == key).update({'title': word})
            if prefix('--author=', args[x]):
                word = args[x][9:len(args[x])]
                session.query(Book).filter(Book.title == key).update({'author': word})
            if prefix('--year=', args[x]):
                word = args[x][7:len(args[x])]
                session.query(Book).filter(Book.title == key).update({'year': word})
        session.commit()
        session.close()
    else:
        print("In editbook false argument")
        session.close()
        return


def addfriend():
    args = sys.argv
    f = Friend(name='')
    for x in range(3, len(args)):
        if prefix('--name=', args[x]):
            f.name = args[x][7:len(args[x])]
        if prefix('--email=', args[x]):
            f.email = args[x][8:len(args[x])]
    if f.name == '':
        print("In addfriend name have to be a non zero length string")
        session.close()
        return
    frie = session.query(Friend).filter(Friend.name == f.name).first()
    if frie is not None:
        print("Friend already in database")
        session.close()
        return
    session.add(f)
    session.commit()
    session.close()


def editfriend():
    args = sys.argv
    if prefix('--name=', args[3]):
        key = args[3][7:len(args[3])]
        frie = session.query(Friend).filter(Friend.name == key).first()
        if frie is None:
            print("Friend not in database")
            session.close()
            return
        for x in range(4, len(args)):
            if prefix('--name=', args[x]):
                word = args[x][7:len(args[x])]
                session.query(Friend).filter(Friend.name == key).update({'name': word})
            if prefix('--email=', args[x]):
                word = args[x][8:len(args[x])]
                session.query(Friend).filter(Friend.name == key).update({'email': word})
        session.commit()
        session.close()
    else:
        print("In editfriend false argument")
        session.close()
        sys.exit(0)


def borrow():
    args = sys.argv
    if not(prefix('--title=', args[3])) or not(prefix('--name=', args[4])):
        print("To borrow book u have give --title= and --name=")
        session.close()
        return
    key = args[3][8:len(args[3])]
    book = session.query(Book).filter(Book.title == key).first()
    key = args[4][7:len(args[4])]
    frie = session.query(Friend).filter(Friend.name == key).first()
    frie.borrowed.append(book)
    session.commit()
    session.close()


def returnbook():
    args = sys.argv
    if not(prefix('--title=', args[3])):
        print("To return book u have give --title=")
        session.close()
        return
    key = args[3][8:len(args[3])]
    book = session.query(Book).filter(Book.title == key).first()
    if book is None:
        print("Book not in database")
        session.close()
        return
    if book.borrower is None:
        print("Can't return not borrowed book")
        session.close()
        return
    frie = session.query(Friend).filter(Friend.name == book.borrower).first()
    frie.borrowed.remove(book)
    session.commit()
    session.close()


def getborrower():
    args = sys.argv
    if not(prefix('--title=', args[3])):
        print("To getborrower u have give --title=")
        session.close()
        return
    key = args[3][8:len(args[3])]
    book = session.query(Book).filter(Book.title == key).first()
    if book is None:
        print("Book not in database")
        session.close()
        return
    if book.borrower is None:
        print("In getborrower book not borrowed")
        return
    print(book.borrower)
    session.close()


def arg_handle():
    arg = sys.argv[1]
    if arg == 'book':
        arg = sys.argv[2]
        if arg == '--add':
            addbook()
            return
        if arg == '--edit':
            editbook()
            return
        if arg == '--borrow':
            borrow()
            return
        if arg == '--return':
            returnbook()
            return
        if arg == '--getborrower':
            getborrower()
            return

    if arg == 'friend':
        arg = sys.argv[2]
        if arg == '--add':
            addfriend()
            return
        if arg == '--edit':
            editfriend()
            return

    print("Incorrect argument")
    sys.exit(0)


engine = create_engine('sqlite:///people.db', echo=False)
Base = declarative_base()


class Book(Base):
    __tablename__ = 'Books'
    title = Column(String, primary_key=True)
    author = Column(String)
    year = Column(Integer)
    borrower = Column(String, ForeignKey('Friends.name'))


class Friend(Base):
    __tablename__ = 'Friends'
    name = Column(String, primary_key=True)
    email = Column(String)
    borrowed = relationship('Book')
    @validates('email')
    def validateemail(self, key, value):
        assert '@' in value
        return value
        

Base.metadata.create_all(engine)
Session = sessionmaker(bind=engine)
session = Session()

print("BOOKS:\n")
result = session.query(Book).all()
for x in result:
    print(x.title)

print("\nFRIENDS:\n")
result = session.query(Friend).all()
for x in result:
    print(x.name)

if len(sys.argv) < 4:
    print("Number of arguments to small")
    sys.exit(0)

arg_handle()
