// Weekly Performance Evaluator - Week 2
// Added unit tests with doctest + test-friendly class methods

#ifdef _DEBUG
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

// Constants
const double PRO_TRAIN_HRS = 6.0;
const double INT_TRAIN_HRS = 3.0;
const double SLEEP_MIN_OK = 7.0;
const double SLEEP_GOOD_MIN = 7.5;
const double SLEEP_GOOD_MAX = 9.0;
const double W_TRAIN = 10.0;
const double W_SLEEP = 5.0;

const int INPUT_FLUSH = 10000;
const int MAX_SESSIONS = 5;
const double SHIFT_MIN = 10.0;

const int MIN_AGE = 1;
const double MIN_TRAINING_HOURS = 0.0;
const double MIN_SLEEP_HOURS = 0.01;

const int MENU_MIN_CHOICE = 0;
const int MENU_MAX_CHOICE = 3;

const double PRO_TECH_MINS = 50.0;
const double PRO_COND_MINS = 30.0;
const double INT_TECH_MINS = 45.0;
const double INT_COND_MINS = 25.0;
const double BEG_TECH_MINS = 35.0;
const double BEG_COND_MINS = 20.0;

const int REST_DAYS_HIGH = 2;
const int REST_DAYS_MODERATE = 1;
const int REST_DAYS_LOW = 0;

// Enum
enum PlayerLevel { LEVEL_AMATEUR, LEVEL_SEMI_PRO, LEVEL_PRO };

// Struct
struct PlayerWeek
{
    string name;
    int age;
    int sessionCount;
    double totalTraining;
    double avgTraining;
    double sleepHours;
    PlayerLevel level;
    double readinessScore;
    string advice;
};

// Function prototypes
void setConsoleColor();
void showBanner();

int getValidInt(const string& prompt, int minValue);
double getValidDouble(const string& prompt, double minValue);

void collectPlayerBasics(PlayerWeek& pw);
void fillTrainingSessions(double sessions[], int& sessionCount, int maxSessions);
void computeTrainingStats(const double sessions[], int sessionCount, PlayerWeek& pw);
void evaluateLevel(PlayerWeek& pw);

int getMenuChoice();
void printSessionsTable(const double sessions[], int sessionCount);

void handleLevelReport(const PlayerWeek& pw, const double sessions[], int sessionCount);
void handleTrainingPlanReport(const PlayerWeek& pw);
void handleRecoveryReport(const PlayerWeek& pw);

string levelToString(PlayerLevel level);

// ===============================================
// CLASS TrainingLog
// ===============================================
class TrainingLog
{
private:
    PlayerWeek playerWeek;
    double trainingSessions[MAX_SESSIONS];

    void updateStats()
    {
        computeTrainingStats(trainingSessions, playerWeek.sessionCount, playerWeek);
    }

public:
    TrainingLog()
    {
        playerWeek.name = "";
        playerWeek.age = 0;
        playerWeek.sessionCount = 0;
        playerWeek.totalTraining = 0.0;
        playerWeek.avgTraining = 0.0;
        playerWeek.sleepHours = 0.0;
        playerWeek.readinessScore = 0.0;
        playerWeek.level = LEVEL_AMATEUR;
        playerWeek.advice = "";

        for (int i = 0; i < MAX_SESSIONS; ++i)
            trainingSessions[i] = 0.0;
    }

    // Interactive setup (Program Mode)
    void setup()
    {
        collectPlayerBasics(playerWeek);
        fillTrainingSessions(trainingSessions, playerWeek.sessionCount, MAX_SESSIONS);
        updateStats();

        playerWeek.sleepHours = getValidDouble("Avg sleep hours per night: ", MIN_SLEEP_HOURS);
        evaluateLevel(playerWeek);
    }

    // Menu loop
    void runMenu()
    {
        int choice;
        do
        {
            choice = getMenuChoice();

            switch (choice)
            {
            case 1:
                showLevelReport();
                break;
            case 2:
                handleTrainingPlanReport(playerWeek);
                break;
            case 3:
                handleRecoveryReport(playerWeek);
                break;
            case 0:
                cout << "\nExiting...\n";
                break;
            }

        } while (choice != 0);
    }

    void showLevelReport() const
    {
        handleLevelReport(playerWeek, trainingSessions, playerWeek.sessionCount);
    }

    // ------------------------------------------------
    // Test-friendly methods (NO cin)  <-- for Week 2
    // ------------------------------------------------

    bool addSession(double hours)
    {
        // Guard cases
        if (hours < 0.0) return false;
        if (playerWeek.sessionCount >= MAX_SESSIONS) return false;

        trainingSessions[playerWeek.sessionCount] = hours;
        playerWeek.sessionCount++;

        // Keep unused spots clean
        for (int i = playerWeek.sessionCount; i < MAX_SESSIONS; ++i)
            trainingSessions[i] = 0.0;

        updateStats();
        return true;
    }

    int getSessionCount() const { return playerWeek.sessionCount; }
    double getTotalHours() const { return playerWeek.totalTraining; }
    double getAverageHours() const { return playerWeek.avgTraining; }

    void clearSessions()
    {
        playerWeek.sessionCount = 0;
        for (int i = 0; i < MAX_SESSIONS; ++i)
            trainingSessions[i] = 0.0;
        updateStats();
    }
};

// ====================
// Functions
// ====================

void setConsoleColor()
{
    system("color 1E");  // Windows only
}

void showBanner()
{
    cout << "===================================\n";
    cout << "    Weekly Performance Evaluator\n";
    cout << "===================================\n\n";
}

int getValidInt(const string& prompt, int minValue)
{
    int value;
    cout << prompt;

    while (!(cin >> value) || value < minValue)
    {
        cin.clear();
        cin.ignore(INPUT_FLUSH, '\n');
        cout << "Invalid input. " << prompt;
    }

    cin.ignore(INPUT_FLUSH, '\n');
    return value;
}

double getValidDouble(const string& prompt, double minValue)
{
    double value;
    cout << prompt;

    while (!(cin >> value) || value < minValue)
    {
        cin.clear();
        cin.ignore(INPUT_FLUSH, '\n');
        cout << "Invalid input. " << prompt;
    }

    cin.ignore(INPUT_FLUSH, '\n');
    return value;
}

void collectPlayerBasics(PlayerWeek& pw)
{
    cout << "Enter player's full name: ";
    getline(cin, pw.name);

    pw.age = getValidInt("Enter player's age: ", MIN_AGE);

    cout << fixed << showpoint << setprecision(2);
}

void fillTrainingSessions(double sessions[], int& sessionCount, int maxSessions)
{
    cout << "\nHow many training sessions did you have this week (1-" << maxSessions << ")? ";

    while (!(cin >> sessionCount) || sessionCount < 1 || sessionCount > maxSessions)
    {
        cin.clear();
        cin.ignore(INPUT_FLUSH, '\n');
        cout << "Invalid. Enter a number between 1 and " << maxSessions << ": ";
    }

    cin.ignore(INPUT_FLUSH, '\n');

    cout << "\nEnter training hours for each session:\n";

    for (int i = 0; i < sessionCount; ++i)
    {
        sessions[i] = getValidDouble("  Session " + to_string(i + 1) + ": ", MIN_TRAINING_HOURS);
    }

    for (int i = sessionCount; i < maxSessions; ++i)
        sessions[i] = 0.0;
}

void computeTrainingStats(const double sessions[], int sessionCount, PlayerWeek& pw)
{
    pw.totalTraining = 0.0;

    // Guard: if sessionCount is negative, treat it as 0
    if (sessionCount < 0) sessionCount = 0;

    for (int i = 0; i < sessionCount; ++i)
        pw.totalTraining += sessions[i];

    // Guard: avoid divide-by-zero
    pw.avgTraining = (sessionCount > 0 ? pw.totalTraining / sessionCount : 0.0);
    pw.sessionCount = sessionCount;
}

void evaluateLevel(PlayerWeek& pw)
{
    pw.readinessScore = (pw.avgTraining * W_TRAIN)
        + ((pw.sleepHours - SLEEP_MIN_OK) * W_SLEEP);

    if (pw.avgTraining >= PRO_TRAIN_HRS &&
        (pw.sleepHours >= SLEEP_GOOD_MIN && pw.sleepHours <= SLEEP_GOOD_MAX))
    {
        pw.level = LEVEL_PRO;
    }
    else if (pw.avgTraining >= INT_TRAIN_HRS && pw.sleepHours >= SLEEP_MIN_OK)
    {
        pw.level = LEVEL_SEMI_PRO;
    }
    else
    {
        pw.level = LEVEL_AMATEUR;
    }

    if (pw.sleepHours < SLEEP_MIN_OK || pw.avgTraining < INT_TRAIN_HRS)
        pw.advice = "Build base: 3 sessions + 8h sleep.";
    else if (pw.avgTraining >= PRO_TRAIN_HRS &&
        !(pw.sleepHours >= SLEEP_GOOD_MIN && pw.sleepHours <= SLEEP_GOOD_MAX))
        pw.advice = "Improve sleep routine.";
    else
        pw.advice = "Good balance.";
}

int getMenuChoice()
{
    int choice;

    cout << "\nMenu:\n";
    cout << "  1) Evaluate Level\n";
    cout << "  2) Training Plan\n";
    cout << "  3) Recovery\n";
    cout << "  0) Quit\n";
    cout << "Enter choice: ";

    while (!(cin >> choice) || choice < MENU_MIN_CHOICE || choice > MENU_MAX_CHOICE)
    {
        cin.clear();
        cin.ignore(INPUT_FLUSH, '\n');
        cout << "Invalid. Enter " << MENU_MIN_CHOICE << "-" << MENU_MAX_CHOICE << ": ";
    }

    cin.ignore(INPUT_FLUSH, '\n');
    return choice;
}

void printSessionsTable(const double sessions[], int sessionCount)
{
    cout << "\nSession Breakdown:\n";
    cout << left << setw(10) << "#" << setw(12) << "Hours" << "\n";

    for (int i = 0; i < sessionCount; ++i)
    {
        cout << left << setw(10) << (i + 1)
            << setw(12) << sessions[i] << "\n";
    }
}

void handleLevelReport(const PlayerWeek& pw, const double sessions[], int sessionCount)
{
    cout << "\n----- LEVEL REPORT -----\n";
    cout << "Player: " << pw.name << "\n";
    cout << "Age: " << pw.age << "\n";
    cout << "Level: " << levelToString(pw.level) << "\n";
    cout << "Total training this week: " << pw.totalTraining << "\n";
    cout << "Average training per session: " << pw.avgTraining << "\n";
    cout << "Average sleep: " << pw.sleepHours << "\n";
    cout << "Readiness Score: " << pw.readinessScore << "\n";
    cout << "Advice: " << pw.advice << "\n";

    printSessionsTable(sessions, sessionCount);

    ofstream out("report.txt");
    if (out)
    {
        out << "WEEKLY PERFORMANCE REPORT (Week 2)\n";
        out << "Section: LEVEL\n";
        out << "----------------------------------\n";
        out << left << setw(22) << "Player:" << right << setw(20) << pw.name << "\n";
        out << left << setw(22) << "Age:" << right << setw(20) << pw.age << "\n";
        out << left << setw(22) << "Total training:" << right << setw(20) << pw.totalTraining << "\n";
        out << left << setw(22) << "Avg training:" << right << setw(20) << pw.avgTraining << "\n";
        out << left << setw(22) << "Avg sleep:" << right << setw(20) << pw.sleepHours << "\n";
        out << left << setw(22) << "Level:" << right << setw(20) << levelToString(pw.level) << "\n";
        out << left << setw(22) << "Readiness:" << right << setw(20) << pw.readinessScore << "\n";
        out << left << setw(22) << "Advice:" << right << setw(20) << pw.advice << "\n\n";

        out << "Session details:\n";
        out << left << setw(10) << "Session"
            << setw(15) << "Hours" << "\n";

        for (int i = 0; i < sessionCount; ++i)
        {
            out << left << setw(10) << (i + 1)
                << setw(15) << sessions[i] << "\n";
        }
    }
    else
    {
        cout << "Could not write report.txt\n";
    }
}

void handleTrainingPlanReport(const PlayerWeek& pw)
{
    string planFocus;
    double techMins;
    double condMins;

    switch (pw.level)
    {
    case LEVEL_PRO:
        techMins = PRO_TECH_MINS;
        condMins = PRO_COND_MINS;
        planFocus = "High Tempo";
        break;
    case LEVEL_SEMI_PRO:
        techMins = INT_TECH_MINS;
        condMins = INT_COND_MINS;
        planFocus = "Balanced";
        break;
    default:
        techMins = BEG_TECH_MINS;
        condMins = BEG_COND_MINS;
        planFocus = "Fundamentals";
        break;
    }

    if (pw.sleepHours < SLEEP_MIN_OK && condMins >= SHIFT_MIN)
    {
        techMins += SHIFT_MIN;
        condMins -= SHIFT_MIN;
    }

    cout << "\n------- TRAINING PLAN -------\n";
    cout << left << setw(20) << "Player:" << right << setw(20) << pw.name << "\n";
    cout << left << setw(20) << "Level:" << right << setw(20) << levelToString(pw.level) << "\n";
    cout << left << setw(20) << "Focus:" << right << setw(20) << planFocus << "\n";
    cout << left << setw(20) << "Technical work:" << right << setw(20) << techMins << " min\n";
    cout << left << setw(20) << "Conditioning:" << right << setw(20) << condMins << " min\n";
}

void handleRecoveryReport(const PlayerWeek& pw)
{
    string fatigue = "Low";
    int rest = REST_DAYS_LOW;
    string tip;

    if (pw.sleepHours < SLEEP_MIN_OK)
        fatigue = "High";
    else if (pw.totalTraining >= PRO_TRAIN_HRS || pw.totalTraining < INT_TRAIN_HRS)
        fatigue = "Moderate";

    if (fatigue == "High")
        rest = REST_DAYS_HIGH;
    else if (fatigue == "Moderate")
        rest = REST_DAYS_MODERATE;

    if (pw.sleepHours < SLEEP_MIN_OK)
        tip = "Aim for 8h sleep.";
    else if (pw.totalTraining >= PRO_TRAIN_HRS)
        tip = "Foam roll.";
    else
        tip = "Light stretching.";

    cout << "\n---- RECOVERY ----\n";
    cout << left << setw(18) << "Fatigue:" << right << setw(20) << fatigue << "\n";
    cout << left << setw(18) << "Rest Days:" << right << setw(20) << rest << "\n";
    cout << left << setw(18) << "Tip:" << right << setw(20) << tip << "\n";
}

string levelToString(PlayerLevel level)
{
    switch (level)
    {
    case LEVEL_PRO: return "Pro";
    case LEVEL_SEMI_PRO: return "Semi-Pro";
    case LEVEL_AMATEUR: return "Amateur";
    default: return "Unknown";
    }
}

#ifndef _DEBUG
// ====================
// MAIN (Program Mode)
// ====================
int main()
{
    setConsoleColor();
    showBanner();

    TrainingLog tracker;
    tracker.setup();
    tracker.runMenu();

    return 0;
}
#endif

#ifdef _DEBUG
// ====================
// UNIT TESTS (Test Mode)
// ====================

// A) Calculations — 4 tests

TEST_CASE("Calc: total and average with 3 sessions")
{
    PlayerWeek pw{};
    double sessions[MAX_SESSIONS] = { 1.5, 2.0, 2.5, 0.0, 0.0 };

    computeTrainingStats(sessions, 3, pw);

    CHECK(pw.totalTraining == doctest::Approx(6.0)); // 1.5+2.0+2.5
    CHECK(pw.avgTraining == doctest::Approx(2.0));   // 6/3
    CHECK(pw.sessionCount == 3);
}

TEST_CASE("Calc: one session only")
{
    PlayerWeek pw{};
    double sessions[MAX_SESSIONS] = { 4.0, 999.0, 999.0, 999.0, 999.0 };

    computeTrainingStats(sessions, 1, pw);

    CHECK(pw.totalTraining == doctest::Approx(4.0));
    CHECK(pw.avgTraining == doctest::Approx(4.0));
    CHECK(pw.sessionCount == 1);
}

TEST_CASE("Calc: zero sessions gives zero average")
{
    PlayerWeek pw{};
    double sessions[MAX_SESSIONS] = { 3.0, 3.0, 3.0, 3.0, 3.0 };

    computeTrainingStats(sessions, 0, pw);

    CHECK(pw.totalTraining == doctest::Approx(0.0));
    CHECK(pw.avgTraining == doctest::Approx(0.0)); // no divide by zero
    CHECK(pw.sessionCount == 0);
}

TEST_CASE("Calc: sessions with zero hours")
{
    PlayerWeek pw{};
    double sessions[MAX_SESSIONS] = { 0.0, 2.0, 0.0, 0.0, 0.0 };

    computeTrainingStats(sessions, 2, pw);

    CHECK(pw.totalTraining == doctest::Approx(2.0));
    CHECK(pw.avgTraining == doctest::Approx(1.0));
    CHECK(pw.sessionCount == 2);
}

// B) Enum decision logic — 3 tests

TEST_CASE("Level: pro player case")
{
    PlayerWeek pw{};
    pw.avgTraining = 6.0;
    pw.sleepHours = 8.0;

    evaluateLevel(pw);

    CHECK(pw.level == LEVEL_PRO);
    CHECK(levelToString(pw.level) == "Pro");
}

TEST_CASE("Level: semi-pro player case")
{
    PlayerWeek pw{};
    pw.avgTraining = 3.0;
    pw.sleepHours = 7.0;

    evaluateLevel(pw);

    CHECK(pw.level == LEVEL_SEMI_PRO);
    CHECK(levelToString(pw.level) == "Semi-Pro");
}

TEST_CASE("Level: amateur player case")
{
    PlayerWeek pw{};
    pw.avgTraining = 2.9;
    pw.sleepHours = 7.0;

    evaluateLevel(pw);

    CHECK(pw.level == LEVEL_AMATEUR);
    CHECK(levelToString(pw.level) == "Amateur");
}

// C) Struct/array processing — 3 tests

TEST_CASE("Struct: values written into PlayerWeek")
{
    PlayerWeek pw{};
    double sessions[MAX_SESSIONS] = { 1.0, 1.0, 1.0, 0.0, 0.0 };

    computeTrainingStats(sessions, 3, pw);

    CHECK(pw.sessionCount == 3);
    CHECK(pw.totalTraining == doctest::Approx(3.0));
    CHECK(pw.avgTraining == doctest::Approx(1.0));
}

TEST_CASE("Struct: uses only first N sessions")
{
    PlayerWeek pw{};
    double sessions[MAX_SESSIONS] = { 2.0, 2.0, 100.0, 100.0, 100.0 };

    computeTrainingStats(sessions, 2, pw);

    CHECK(pw.totalTraining == doctest::Approx(4.0)); // only 2 + 2
    CHECK(pw.avgTraining == doctest::Approx(2.0));
    CHECK(pw.sessionCount == 2);
}

TEST_CASE("Struct: negative session count treated as zero")
{
    PlayerWeek pw{};
    double sessions[MAX_SESSIONS] = { 5.0, 5.0, 5.0, 5.0, 5.0 };

    computeTrainingStats(sessions, -3, pw);

    CHECK(pw.sessionCount == 0);
    CHECK(pw.totalTraining == doctest::Approx(0.0));
    CHECK(pw.avgTraining == doctest::Approx(0.0));
}

// D) Class methods — 2 tests

TEST_CASE("Class: addSession normal case")
{
    TrainingLog log;

    CHECK(log.getSessionCount() == 0);
    CHECK(log.getTotalHours() == doctest::Approx(0.0));
    CHECK(log.getAverageHours() == doctest::Approx(0.0));

    CHECK(log.addSession(2.0) == true);
    CHECK(log.addSession(4.0) == true);

    CHECK(log.getSessionCount() == 2);
    CHECK(log.getTotalHours() == doctest::Approx(6.0));  // 2+4
    CHECK(log.getAverageHours() == doctest::Approx(3.0)); // 6/2
}

TEST_CASE("Class: addSession guard cases")
{
    TrainingLog log;

    // Guard 1: negative hours
    CHECK(log.addSession(-1.0) == false);
    CHECK(log.getSessionCount() == 0);

    // Guard 2: cannot exceed MAX_SESSIONS
    for (int i = 0; i < MAX_SESSIONS; ++i)
        CHECK(log.addSession(1.0) == true);

    CHECK(log.getSessionCount() == MAX_SESSIONS);
    CHECK(log.addSession(1.0) == false); // full
}
#endif
