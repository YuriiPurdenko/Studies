import gi
from sqlalchemy import create_engine, ForeignKey, Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, relationship, validates
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk


def getbooks():
    result = session.query(Book).all()
    for x in result:
        var = []
        var.append(x.title)
        if x.author is not None:
            var.append(x.author)
        else:
            var.append('')
        if x.year is not None:
            var.append(str(x.year))
        else:
            var.append('')
        if x.borrower is not None:
            var.append(x.borrower)
        else:
            var.append('')
        print(var)
        modelb.append(var)


def getfriends():
    result = session.query(Friend).all()
    for x in result:
        var = []
        var.append(x.name)
        if x.email is not None:
            var.append(x.email)
        else:
            var.append('')
        print(var)
        modelf.append(var)


def addbook(title, year, author):
    b = Book(title=title)
    if len(author):
        b.author = author
    if len(year):
        b.year = year
    if b.title == '':
        print("In addbook title have to be a non zero length string")
        return
    book = session.query(Book).filter(Book.title == b.title).first()
    if book is not None:
        print("Book already in database")
        return
    session.add(b)
    modelb.append([title, author, year, ''])
    session.commit()


def editbook(title1, title2, year, author):
    book = session.query(Book).filter(Book.title == title1).first()
    if book is None:
        print("Book not in database")
        return
    for x in modelb:
        if x[0] == title1:
            if len(author):
                session.query(Book).filter(Book.title == title1).update({'author': author})
                x[1] = author
            if len(year):
                session.query(Book).filter(Book.title == title1).update({'year': int(year)})
                x[2] = year
            if len(title2):
                session.query(Book).filter(Book.title == title1).update({'title': title2})
                x[0] = title2
    session.commit()


def addfriend(name, email):
    f = Friend(name=name)
    if len(email):
        f.email = email
    if f.name == '':
        print("In addfriend name have to be a non zero length string")
        return
    frie = session.query(Friend).filter(Friend.name == f.name).first()
    if frie is not None:
        print("Friend already in database")
        return
    session.add(f)
    modelf.append([name, email])
    session.commit()


def editfriend(name1, name2, email):
    frie = session.query(Friend).filter(Friend.name == name1).first()
    if frie is None:
        print("Friend not in database")
        return
    for x in modelf:
        if x[0] == name1:
            if len(email):
                session.query(Friend).filter(Friend.name == name1).update({'email': email})
                x[1] = email
            if len(name2):
                session.query(Friend).filter(Friend.name == name1).update({'name': name2})
                x[0] = name2
    session.commit()


def add_book_window(selektor):
    builder = Gtk.Builder()
    builder.add_from_file('bazadanych.glade')
    window = builder.get_object('addbookwindow')
    window.show_all()

    def add_book(selektor):
        name = builder.get_object('titleentry')
        text1 = name.get_text()
        name = builder.get_object('yearentry')
        text2 = name.get_text()
        name = builder.get_object('authorentry')
        text3 = name.get_text()
        addbook(text1, text2, text3)
        window = builder.get_object('addbookwindow')
        window.close()
    sel = builder.get_object('addbook')
    sel.connect('clicked', add_book)


def add_friend_window(selektor):
    builder = Gtk.Builder()
    builder.add_from_file('bazadanych.glade')
    window = builder.get_object('addfriendwindow')
    window.show_all()

    def add_friend(selektor):
        name = builder.get_object('nameentry')
        text1 = name.get_text()
        name = builder.get_object('emailentry')
        text2 = name.get_text()
        addfriend(text1, text2)
        window = builder.get_object('addfriendwindow')
        window.close()
    sel = builder.get_object('addfriend')
    sel.connect('clicked', add_friend)


def on_select_book(selektor):
    builder = Gtk.Builder()
    builder.add_from_file('bazadanych.glade')
    modelb, treeiter = selektor.get_selected()
    label = modelb[treeiter][0]
    print(label+'\n')
    window = builder.get_object('editbookwindow')
    window.show_all()

    def edit_book(selektor):
        name = builder.get_object('titleeditentry')
        text1 = name.get_text()
        name = builder.get_object('yeareditentry')
        text2 = name.get_text()
        name = builder.get_object('authoreditentry')
        text3 = name.get_text()
        editbook(label, text1, text2, text3)
        window = builder.get_object('editbookwindow')
        window.close()

    sel = builder.get_object('editbook')
    sel.connect('clicked', edit_book)


def on_select_friend(selektor):
    builder = Gtk.Builder()
    builder.add_from_file('bazadanych.glade')
    modelf, treeiter = selektor.get_selected()
    label = modelf[treeiter][0]
    print(label+'\n')
    window = builder.get_object('editfriendwindow')
    window.show_all()

    def edit_friend(selektor):
        name = builder.get_object('nameeditentry')
        text1 = name.get_text()
        name = builder.get_object('emaileditentry')
        text2 = name.get_text()
        editfriend(label, text1, text2)
        window = builder.get_object('editfriendwindow')
        window.close()

    sel = builder.get_object('editfriend')
    sel.connect('clicked', edit_friend)


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


books = [['1984', 'Orwell', '', ''], ['abc', '', '', '']]

friends = [['Wiktor', 'wiktor@gmail.com'], ['Jan', '']]

sygnaly = {'ondestroy': Gtk.main_quit}

builder = Gtk.Builder()
builder.add_from_file('bazadanych.glade')

# model
modelb = builder.get_object('listbook')
getbooks()

modelf = builder.get_object('listfriend')
getfriends()

# Book column
view = builder.get_object('treeviewbook')

r1 = Gtk.CellRendererText()
k1 = Gtk.TreeViewColumn("Title", r1, text=0)
k1.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k1)

r2 = Gtk.CellRendererText()
k2 = Gtk.TreeViewColumn("Author", r2, text=1)
k2.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k2)

r3 = Gtk.CellRendererText()
k3 = Gtk.TreeViewColumn("Year", r3, text=2)
k3.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k3)

r4 = Gtk.CellRendererText()
k4 = Gtk.TreeViewColumn("Borrower", r4, text=3)
k4.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k4)

# Friend column
view = builder.get_object('treeviewfriend')

r1 = Gtk.CellRendererText()
k1 = Gtk.TreeViewColumn("Name", r1, text=0)
k1.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k1)

r2 = Gtk.CellRendererText()
k2 = Gtk.TreeViewColumn("Email", r2, text=1)
k2.set_sizing(Gtk.TreeViewColumnSizing.FIXED)
view.append_column(k2)

# signal handlers
sel = builder.get_object('treeviewselectionbook')
sel.connect('changed', on_select_book)

sel = builder.get_object('treeviewselectionfriend')
sel.connect('changed', on_select_friend)

sel = builder.get_object('bookbutton')
sel.connect('clicked', add_book_window)

sel = builder.get_object('friendbutton')
sel.connect('clicked', add_friend_window)

builder.connect_signals(sygnaly)

window = builder.get_object('window')
window.connect('destroy', Gtk.main_quit)
window.show_all()

Gtk.main()
session.close()
