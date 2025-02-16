/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** This file defines the `HighScoreDB` class, which provides an interface
** for managing the high score database of the Breakout game. The class
** includes methods for opening, closing, and interacting with an SQLite
** database to store, retrieve, and display high scores.
**
** This implementation is custom-made for this specific version of Breakout,
** created as part of the SNHU CS-499 Capstone Course.
**
** Author: Jeremy Snow
** Date: February 4, 2025
** Version: 1.0
*******************************************************************/




#ifndef HIGHSCOREDB_H
#define HIGHSCOREDB_H

#include <sqlite3.h>
#include <string>
#include <vector>

// Structure to store high score entries, containing player name and completion time.
struct HighScore {
    std::string playerName;   // Player's username
    double completionTime;    // Time taken to complete the level.
};

// Class for managing the high score database using SQLite.
class HighScoreDB {
public:
    // Constructor: Opens (or creates) the database file with the given name.
    HighScoreDB(const std::string& dbName);

    // Destructor: Closes the database connection.
    ~HighScoreDB();

    // Creates a high score table for a specific level if it does not already exist.
    bool createTable(int level);

    // Adds a new high score entry to the specified level's table.
    bool addScore(int level, const std::string& playerName, double completionTime);

    // Retrieves the top 10 high scores for a given level.
    std::vector<HighScore> getHighScores(int level);

    // Checks if a given completion time qualifies as a new high score for the level.
    bool isNewHighScore(int level, float playerTime);

private:
    sqlite3* db;  // Pointer to the SQLite database connection.

    // Executes a given SQL query that does not return results (e.g., CREATE, DELETE).
    bool executeQuery(const std::string& query);

    // Retrieves the slowest high score (i.e., the 10th best time) for a level.
    float getSlowestHighScore(int level);
};

#endif 