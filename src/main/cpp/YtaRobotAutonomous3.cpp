////////////////////////////////////////////////////////////////////////////////
/// @file   YtaRobotAutonomous3.cpp
/// @author David Stalter
///
/// @details
/// Implementation of autonomous routine 3 for YtaRobot.
///
/// Copyright (c) 2021 Youth Technology Academy
////////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
// <none>

// C INCLUDES
// (none)

// C++ INCLUDES
#include "RobotUtils.hpp"               // for DisplayMessage()
#include "YtaRobot.hpp"                 // for robot class declaration
#include "YtaRobotAutonomous.hpp"       // for autonomous declarations



////////////////////////////////////////////////////////////////
/// @method YtaRobot::AutonomousRoutine3
///
/// Autonomous routine 3.
///
////////////////////////////////////////////////////////////////
void YtaRobot::AutonomousRoutine3()
{
    // 12.7 battery, turn .95s @ 20% is about 90 degrees

    // Get to and knock down the first marker
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 1.30);
    AutonomousDriveSequence(ROBOT_LEFT, 0.20, 0.90);
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 1.25);
    AutonomousDriveSequence(ROBOT_REVERSE, 0.20, 1.6);

    // Navigate to the next marker
    AutonomousDriveSequence(ROBOT_RIGHT, 0.20, 0.80);
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 0.70);
    AutonomousDriveSequence(ROBOT_RIGHT, 0.20, 0.75);
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 1.20);
    AutonomousDriveSequence(ROBOT_LEFT, 0.20, 0.85);

    // These are along the baseline
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 1.3);
    AutonomousDriveSequence(ROBOT_LEFT, 0.20, 0.90);

    // To the marker and back
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 3.1);
    AutonomousDriveSequence(ROBOT_REVERSE, 0.20, 3.1);

    // Navigate to the final marker
    AutonomousDriveSequence(ROBOT_RIGHT, 0.20, 0.75);
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 2.35);
    AutonomousDriveSequence(ROBOT_LEFT, 0.20, 0.90);
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 3.3);
    AutonomousDriveSequence(ROBOT_REVERSE, 0.20, 1.65);

    // Finish
    AutonomousDriveSequence(ROBOT_RIGHT, 0.20, 0.80);
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 1.0);

    // Returning from here will enter the idle state until autonomous is over
    RobotUtils::DisplayMessage("Auto routine 3 done.");
}
