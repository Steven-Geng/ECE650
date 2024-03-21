from sqlalchemy import Column, Integer, String, Float
from sqlalchemy.ext.declarative import declarative_base

Base = declarative_base()

class Player(Base):
    __tablename__ = 'player'

    player_id = Column(Integer, primary_key=True)
    team_id = Column(Integer)
    uniform_num = Column(Integer)
    first_name = Column(String)
    last_name = Column(String)
    mpg = Column(Integer)
    ppg = Column(Integer)
    rpg = Column(Integer)
    apg = Column(Integer)
    spg = Column(Float)
    bpg = Column(Float)

class Team(Base):
    __tablename__ = 'team'

    team_id = Column(Integer, primary_key=True)
    name = Column(String)
    state_id = Column(Integer)
    color_id = Column(Integer)
    wins = Column(Integer)
    losses = Column(Integer)

class State(Base):
    __tablename__ = 'state'

    state_id = Column(Integer, primary_key=True)
    name = Column(String)

class Color(Base):
    __tablename__ = 'color'

    color_id = Column(Integer, primary_key=True)
    name = Column(String)
