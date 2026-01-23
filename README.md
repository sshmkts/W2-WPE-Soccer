# Weekly Performance Evaluator (Week 2)

The program evaluates a soccer player's weekly training performance based on:
- Training sessions (hours per session)
- Average training load
- Average sleep per night

Based on these values, the program:
- Calculates total and average training hours
- Determines the player level (Amateur, Semi-Pro, Pro)
- Generates training, recovery, and performance reports
- Writes a weekly report to a text file

## Week 2 – Unit Testing

For Week 2, unit tests were added using the **doctest** framework.

The project supports two modes:

- **Release Mode** – runs the interactive program (menu + user input)
- **Debug Mode** – runs automated unit tests only

Unit tests cover:
- Calculations (totals and averages)
- Enum decision logic (player level evaluation)
- Struct and array processing
- Non-interactive class methods

Each test includes:
- Normal cases  
- Edge cases (0 sessions, 1 session, 0.0 values)  
- Guard cases (negative input, divide-by-zero protection)

## How to Run

- Build in **Release** mode to run the program normally.
- Build in **Debug** mode to run the unit tests.


