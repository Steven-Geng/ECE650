from sqlalchemy import create_engine, Column, Integer, String, Float
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from query_funcs import *
from models import Base
from models import Player, Team, State, Color
from exerciser import *

import csv


engine = create_engine('postgresql://postgres:passw0rd@localhost/ACC_BBALL')
Session = sessionmaker(bind=engine)
session = Session()

def main():
    engine = create_engine('postgresql://postgres:passw0rd@localhost/ACC_BBALL')
    Base.metadata.drop_all(engine)
    Base.metadata.create_all(engine)
    Session = sessionmaker(bind=engine)
    session = Session()

    load_data_from_txt(session)

    session.commit()

    exercise(session)
    session.close()

if __name__ == "__main__":
    main()