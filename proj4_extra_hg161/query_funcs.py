from sqlalchemy import create_engine, Column, Integer, String, Float
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from models import Player, Team, Color, State
import os


def add_player(session, player_id, team_id, jersey_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg):
    new_player = Player(player_id=player_id, team_id=team_id, uniform_num=jersey_num, first_name=first_name, last_name=last_name,
                        mpg=mpg, ppg=ppg, rpg=rpg, apg=apg, spg=spg, bpg=bpg)
    session.add(new_player)
    session.commit()

def add_team(session, team_id, name, state_id, color_id, wins, losses):
    new_team = Team(team_id=team_id, name=name, state_id=state_id, color_id=color_id, wins=wins, losses=losses)
    session.add(new_team)
    session.commit()

def add_state(session, state_id, name):
    new_state = State(state_id=state_id, name=name)
    session.add(new_state)
    session.commit()

def add_color(session, color_id, name):
    new_color = Color(color_id=color_id, name=name)
    session.add(new_color)
    session.commit()

def load_data_from_txt(session):
    base_dir = os.path.dirname(os.path.abspath(__file__))
    files = ['player.txt', 'team.txt', 'state.txt', 'color.txt']

    for file_name in files:
        with open(os.path.join(base_dir, file_name), 'r') as file:
            lines = file.readlines()
            for line in lines:
                data = line.strip().split(' ')
                if file_name == 'player.txt':
                    add_player(session, *data)
                elif file_name == 'team.txt':
                    add_team(session, *data)
                elif file_name == 'state.txt':
                    add_state(session, *data)
                elif file_name == 'color.txt':
                    add_color(session, *data)

def query1(session, use_mpg, min_mpg, max_mpg,
           use_ppg, min_ppg, max_ppg,
           use_rpg, min_rpg, max_rpg,
           use_apg, min_apg, max_apg,
           use_spg, min_spg, max_spg,
           use_bpg, min_bpg, max_bpg):
    query = session.query(Player)
    conditions = []

    if use_mpg:
        conditions.append(Player.mpg.between(min_mpg, max_mpg))
    if use_ppg:
        conditions.append(Player.ppg.between(min_ppg, max_ppg))
    if use_rpg:
        conditions.append(Player.rpg.between(min_rpg, max_rpg))
    if use_apg:
        conditions.append(Player.apg.between(min_apg, max_apg))
    if use_spg:
        conditions.append(Player.spg.between(min_spg, max_spg))
    if use_bpg:
        conditions.append(Player.bpg.between(min_bpg, max_bpg))

    if conditions:
        query = query.filter(*conditions)

    results = query.all()

    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    for player in results:
        print(player.player_id, player.team_id, player.uniform_num, player.first_name,
              player.last_name, player.mpg, player.ppg, player.rpg, player.apg,
              player.spg, player.bpg)


def query2(session, team_color):
    query = session.query(Team.name).join(Color, Team.color_id == Color.color_id).filter(Color.name == team_color)
    results = query.all()

    print("NAME")
    for team in results:
        print(team.name)


def query3(session, team_name):
    query = session.query(Player.first_name, Player.last_name).join(Team, Team.team_id == Player.team_id) \
        .filter(Team.name == team_name).order_by(Player.ppg.desc())
    results = query.all()

    print("FIRST_NAME LAST_NAME")
    for player in results:
        print(player.first_name, player.last_name)


def query4(session, team_state, team_color):
    query = session.query(Player.uniform_num, Player.first_name, Player.last_name).join(Team, Player.team_id == Team.team_id) \
        .join(State, Team.state_id == State.state_id).join(Color, Team.color_id == Color.color_id) \
        .filter(State.name == team_state, Color.name == team_color)
    results = query.all()

    print("UNIFORM_NUM FIRST_NAME LAST_NAME")
    for player in results:
        print(player.uniform_num, player.first_name, player.last_name)


def query5(session, num_wins):
    query = session.query(Player.first_name, Player.last_name, Team.name, Team.wins).join(Team, Player.team_id == Team.team_id) \
        .filter(Team.wins > num_wins)
    results = query.all()

    print("FIRST_NAME LAST_NAME NAME WINS")
    for player in results:
        print(player.first_name, player.last_name, player.name, player.wins)