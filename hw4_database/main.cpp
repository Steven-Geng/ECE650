#include <iostream>
#include <pqxx/pqxx>
#include <fstream>
#include "exerciser.h"

using namespace std;
using namespace pqxx;

int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }


  //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  //      load each table with rows from the provided source txt files
  string sql, line;

  work dropTables(*C);
  sql = "DROP TABLE IF EXISTS PLAYER, TEAM, STATE, COLOR CASCADE";
  dropTables.exec(sql);
  dropTables.commit();

  work createPlayer(*C);
  sql = "CREATE TABLE PLAYER( \
  PLAYER_ID SERIAL PRIMARY KEY, \
  TEAM_ID INT, \
  UNIFORM_NUM INT, \
  FIRST_NAME VARCHAR(30), \
  LAST_NAME VARCHAR(30), \
  MPG INT, \
  PPG INT, \
  RPG INT, \
  APG INT, \
  SPG FLOAT, \
  BPG FLOAT)";
  createPlayer.exec(sql);
  createPlayer.commit();

  work createTeam(*C);
  sql = "CREATE TABLE TEAM( \
  TEAM_ID SERIAL PRIMARY KEY, \
  NAME VARCHAR(30), \
  STATE_ID INT, \
  COLOR_ID INT, \
  WINS INT, \
  LOSSES INT)";
  createTeam.exec(sql);
  createTeam.commit();

  work createState(*C);
  sql = "CREATE TABLE STATE( \
  STATE_ID SERIAL PRIMARY KEY, \
  NAME VARCHAR(30))";
  createState.exec(sql);
  createState.commit();

  work createColor(*C);
  sql = "CREATE TABLE COLOR( \
  COLOR_ID SERIAL PRIMARY KEY, \
  NAME VARCHAR(30))";
  createColor.exec(sql);
  createColor.commit();

/*
  sql = "COPY PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) FROM '/home/steven/ECE650/hw4_database/player.txt' WITH (DELIMITER ' ')";
  work copyPlayer(*C);
  copyPlayer.exec(sql);
  copyPlayer.commit();

  sql = "COPY TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) FROM '/home/steven/ECE650/hw4_database/team.txt' WITH (DELIMITER ' ')";
  work copyTeam(*C);
  copyTeam.exec(sql);
  copyTeam.commit();

  sql = "COPY STATE (NAME) FROM '/home/steven/ECE650/hw4_database/state.txt' WITH (DELIMITER ' ')";
  work copyState(*C);
  copyState.exec(sql);
  copyState.commit();

  sql = "COPY COLOR (NAME) FROM '/home/steven/ECE650/hw4_database/color.txt' WITH (DELIMITER ' ')";
  work copyColor(*C);
  copyColor.exec(sql);
  copyColor.commit();*/
  string first_name, last_name;
  int player_id, team_id, uniform_num, mpg, ppg, rpg, apg;
  double spg, bpg;
  ifstream player("player.txt");
  while(getline(player, line)){
    stringstream ss;
    ss << line;
    ss >> player_id >> team_id >> uniform_num >> first_name >> last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg;
	  add_player(C, team_id, uniform_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);
  }
  player.close();

  string name;
  int state_id, color_id, wins, losses;
  ifstream team("team.txt");
  while(getline(team, line)){
    stringstream ss;
    ss << line;
    ss >> team_id >> name >> state_id >> color_id >> wins >> losses;
	  add_team(C, name, state_id, color_id, wins, losses);
  }
  team.close();

  ifstream state("state.txt");
  while(getline(team, line)){
    stringstream ss;
    ss << line;
    ss >> state_id >> name;
    add_state(C, name);
  }
  state.close();

  ifstream color("color.txt");
  while(getline(color, line)){
    stringstream ss;
    ss << line;
    ss >> color_id >> name;
    add_color(C, name);
  }
  color.close();
  
  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


