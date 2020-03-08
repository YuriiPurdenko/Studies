from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Table, Column, Integer, create_engine, ForeignKey, String, DateTime
import datetime
from sqlalchemy.orm import validates, relationship, sessionmaker

Base = declarative_base()

class Osoba(Base):
    __tablename__ = 'Osoba'
    id = Column(Integer, primary_key=True)
    imie = Column(String(20), nullable=False)
    wiek = Column(Integer, default=18)
    created = Column(DateTime,default=datetime.datetime.utcnow)
    adresy = relationship('Adres')


class Adres(Base):
    __tablename__ = 'Adres'
    id = Column(Integer, primary_key=True)
    email = Column(String)
    miasto = Column(String)
    
    @validates('email')
    def validate_email(self, key, value):
        assert "@" in value
        return value
    mieszkaniec = Column(Integer, ForeignKey('Osoba.id'))

engine = create_engine('sqlite:///wyklad.db', echo=True)
#Base.metadata.createall(engine)
Session = sessionmaker(bind=engine)
sesja = Session()

o = Osoba(imie='Debsciak')
adr1 = Adres(email='Joliot@Curie', miasto='Wroclaw')
o.adresy = [adr1]
sesja.add(o)

sesja.add(adr1)
sesja.commit()

print("dziwne")