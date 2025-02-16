/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** This file implements the `HighScoreDB` class, which provides an interface
** for managing the high score database of the Breakout game. It includes
** methods for opening, closing, and interacting with an SQLite database to
** store, retrieve, and display high scores.
**
** This implementation is custom-made for this specific version of Breakout,
** created as part of the SNHU CS-499 Capstone Course.
**
** Author: Jeremy Snow
** Date: February 4, 2025
** Version: 1.0
*******************************************************************/


#include "high_score_DB.h"
#include <iostream>

// Constructor that initializes the database connection
HighScoreDB::HighScoreDB(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db)) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;  // Set db to nullptr if opening fails
    }
}

// Destructor that closes the database connection
HighScoreDB::~HighScoreDB() {
    if (db) {
        sqlite3_close(db);
    }
}


// Function to create a high score table for a specific level if it doesn't already exist
bool HighScoreDB::createTable(int level) {
    std::string query = "CREATE TABLE IF NOT EXISTS level_" + std::to_string(level) + "_highscores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "player_name TEXT, "
        "completion_time REAL);";
    return executeQuery(query);
}

// Function to add a new high score entry for a specific level
bool HighScoreDB::addScore(int level, const std::string& playerName, double completionTime) {
    std::string query = "INSERT INTO level_" + std::to_string(level) + "_highscores (player_name, completion_time) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind parameters to the SQL query
    sqlite3_bind_text(stmt, 1, playerName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, completionTime);

    // Execute the statement and check for success
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (!success) {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Ensure that only the top 10 scores remain in the table
    std::string cleanupQuery =
        "DELETE FROM level_" + std::to_string(level) + "_highscores "
        "WHERE completion_time = (SELECT MAX(completion_time) FROM level_" + std::to_string(level) + "_highscores) "
        "AND (SELECT COUNT(*) FROM level_" + std::to_string(level) + "_highscores) > 10;";

    return executeQuery(cleanupQuery);
}

// Function to retrieve the top 10 high scores for a given level
std::vector<HighScore> HighScoreDB::getHighScores(int level) {
    std::vector<HighScore> scores;
    std::string query = "SELECT player_name, completion_time FROM level_" + std::to_string(level) + "_highscores "
        "ORDER BY completion_time ASC LIMIT 10;";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return scores; // Return empty vector if preparation fails
    }

    // Iterate through the results and store them in the vector
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        HighScore score;
        score.playerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        score.completionTime = sqlite3_column_double(stmt, 1);
        scores.push_back(score);
    }

    sqlite3_finalize(stmt);
    return scores;
}

// Function to check if a given time qualifies as a new high score
bool HighScoreDB::isNewHighScore(int level, float playerTime)
{
    // Returns -1.0f if there are less than 10 high scores
    float slowestHighScore = getSlowestHighScore(level);

    // If there are fewer than 10 scores OR the player's time is better than the 10th fastest time, it's a high score
    return (slowestHighScore == -1.0f || playerTime < slowestHighScore);
}

// Function to retrieve the slowest (worst) high score from the leaderboard for a given level
float HighScoreDB::getSlowestHighScore(int level) {
    // Ensure that the database connection is valid
    if (db ==  nullptr) {
        std::cerr << "Error: Database connection is not initialized!" << std::endl;
        return 1.0f; // Return a default high value to indicate failure
    }

    float slowestHighScore = -1.0f; // Default value when no scores exist

    // Query to get the 10th slowest high score for the level
    std::string query = "SELECT completion_time FROM level_" + std::to_string(level) + "_highscores "
        "ORDER BY completion_time ASC LIMIT 1 OFFSET 9;"; 

    sqlite3_stmt* stmt;


    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
    }

    // Retrieve the score, if available
    if (sqlite3_step(stmt) == SQLITE_ROW) {
            slowestHighScore = static_cast<float>(sqlite3_column_double(stmt, 0)); // Retrieve time
    }

    sqlite3_finalize(stmt);
    return slowestHighScore;
}

// Function to execute a generic SQL query (used for table creation and cleanup)
bool HighScoreDB::executeQuery(const std::string& query) {
    char* errMsg = nullptr;

    // Execute the query and check for errors
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}