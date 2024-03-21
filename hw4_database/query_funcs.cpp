#include "query_funcs.h"


void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg){
    work addPlayer(*C);
    stringstream sql;

    sql << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG)"
        " VALUES (" << team_id << ", " << jersey_num << ", " << addPlayer.quote(first_name) << ", " 
        << addPlayer.quote(last_name) << ", " << mpg << ", " << ppg << ", " << rpg << ", "
        << apg << ", " << spg << ", " << bpg << ");";
    addPlayer.exec(sql.str());
    addPlayer.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses){
    work addTeam(*C);
    stringstream sql;

    sql << "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES)"
        "VALUES (" << addTeam.quote(name) << ", " << state_id << ", " << color_id << ", "
        << wins << ", " << losses << ");";
    addTeam.exec(sql.str());
    addTeam.commit();
}


void add_state(connection *C, string name){
    work addState(*C);
    stringstream sql;

    sql << "INSERT INTO STATE (NAME)"
        "VALUES (" << addState.quote(name) << ");";
    addState.exec(sql.str());
    addState.commit();
}


void add_color(connection *C, string name){
    work addColor(*C);
    stringstream sql;

    sql << "INSERT INTO COLOR (NAME)"
        "VALUES (" << addColor.quote(name) << ");";
    addColor.exec(sql);
    addColor.commit();
}

/*
 * All use_ params are used as flags for corresponding attributes
 * a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * a 0 for a use_ param means this attribute is disabled
 */
void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            ){
    nontransaction q1(*C);
    stringstream sql;
    int previousUse = 0;

    sql << "SELECT * FROM PLAYER";
    if(use_mpg){
        sql << " WHERE MPG BETWEEN " << min_mpg << " AND " << max_mpg;
        previousUse = 1;
    }
    if(use_ppg){
        if(previousUse){
            sql << " AND";
        }
        else{
            sql << " WHERE";
        }
        sql << " PPG BETWEEN " << min_ppg << " AND " << max_ppg;
        previousUse = 1;
    }
    if(use_rpg){
        if(previousUse){
            sql << " AND";
        }
        else{
            sql << " WHERE";
        }
        sql << " RPG BETWEEN " << min_rpg << " AND " << max_rpg;
        previousUse = 1;
    }
    if(use_apg){
        if(previousUse){
            sql << " AND";
        }
        else{
            sql << " WHERE";
        }
        sql << " APG BETWEEN " << min_apg << " AND " << max_apg;
        previousUse = 1;
    }
    if(use_spg){
        if(previousUse){
            sql << " AND";
        }
        else{
            sql << " WHERE";
        }
        sql << " SPG BETWEEN " << min_spg << " AND " << max_spg;
        previousUse = 1;
    }
    if(use_bpg){
        if(previousUse){
            sql << " AND";
        }
        else{
            sql << " WHERE";
        }
        sql << " BPG BETWEEN " << min_bpg << " AND " << max_bpg;
        previousUse = 1;
    }

    result res = q1.exec(sql.str());

    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << endl;
    for (result::const_iterator row = res.begin(); row != res.end(); ++row) {
        cout << row[0].as<int>() << " " 
        << row[1].as<int>() << " " 
        << row[2].as<int>() << " " 
        << row[3].as<string>() << " "
        << row[4].as<string>() << " "
        << row[5].as<int>() << " "
        << row[6].as<int>() << " "
        << row[7].as<int>() << " "
        << row[8].as<int>() << " "
        << fixed << setprecision(1)
        << row[9].as<double>() << " "
        << row[10].as<double>() << endl;
    }

    q1.commit();
}


void query2(connection *C, string team_color){
    nontransaction q2(*C);
    string sql;
    sql = "SELECT TEAM.name FROM TEAM, COLOR WHERE COLOR.NAME = '" + team_color + "' AND TEAM.COLOR_ID = COLOR.COLOR_ID";
    result res = q2.exec(sql);
    cout << "NAME" << endl;
    for (result::const_iterator row = res.begin(); row != res.end(); ++row) {
        cout << row[0].as<string>() << endl;
    }

    q2.commit();
}

void query3(connection *C, string team_name){
  nontransaction q3(*C);
  string sql;
  sql = "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM TEAM, PLAYER WHERE TEAM.NAME = '" + team_name + "' AND TEAM.TEAM_ID = PLAYER.TEAM_ID ORDER BY PPG DESC";
  result res = q3.exec(sql);
  cout << "FIRST_NAME LAST_NAME" << endl;
  for (result::const_iterator row = res.begin(); row != res.end(); ++row) {
    cout << row[0].as<string>() << " " << row[1].as<string>() << endl;
  }
  
  q3.commit();
}

void query4(connection *C, string team_state, string team_color){
  nontransaction q4(*C);
  string sql;
  sql = "SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM, STATE, COLOR WHERE STATE.NAME = '" + team_state + "' AND COLOR.NAME = '" + team_color + "' AND TEAM.STATE_ID = STATE.STATE_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID AND PLAYER.TEAM_ID = TEAM.TEAM_ID";
  result res = q4.exec(sql);
  cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
  for (result::const_iterator row = res.begin(); row != res.end(); ++row) {
    cout << row[0].as<int>() << " " << row[1].as<string>() << " " << row[2].as<string>() << endl;
  }
  
  q4.commit();
}

void query5(connection *C, int num_wins){
  nontransaction q5(*C);
  string sql;
  sql = "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, TEAM.WINS FROM TEAM, PLAYER WHERE TEAM.WINS > '" + to_string(num_wins) + "' AND TEAM.TEAM_ID = PLAYER.TEAM_ID";
  result res = q5.exec(sql);
  cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
  for (result::const_iterator row = res.begin(); row != res.end(); ++row) {
    cout << row[0].as<string>() << " " << row[1].as<string>() << " " << row[2].as<string>() << " " << row[3].as<int>() << endl;
  }

  q5.commit();
}
