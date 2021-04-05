////////////////////////////////////////////////////////////////////////////////
/// @file   YtaRobotAutonomous2.cpp
/// @author David Stalter
///
/// @details
/// Implementation of autonomous routine 2 for YtaRobot.
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
/// @method YtaRobot::AutonomousRoutine2
///
/// Autonomous routine 2.
///
////////////////////////////////////////////////////////////////
void YtaRobot::AutonomousRoutine2()
{
    // Go forward
    AutonomousDriveSequence(ROBOT_FORWARD, 0.30, 1.75);

    // First loop
    // 6s @ base 10%, diff 15% is pretty clean
    // 4s @ base 20%, diff 20% is iffy
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 3.5, true, (0.20 * LEFT_DRIVE_FORWARD_SCALAR), 0.0);

    // Go forward
    AutonomousDriveSequence(ROBOT_FORWARD, 0.30, 1.4);

    // Second loop
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 3.0, true, 0.0, (0.20 * RIGHT_DRIVE_FORWARD_SCALAR));

    // Go forward
    AutonomousDriveSequence(ROBOT_FORWARD, 0.30, 1.5);

    // Final turn
    AutonomousDriveSequence(ROBOT_FORWARD, 0.20, 2.1, true, 0.0, (0.20 * RIGHT_DRIVE_FORWARD_SCALAR));

    // Return home
    AutonomousDriveSequence(ROBOT_FORWARD, 0.30, 4.0);
    //AutonomousDriveSequence(ROBOT_FORWARD, 0.30, 1.7, true, (0.01 * LEFT_DRIVE_FORWARD_SCALAR), 0.0);
    //AutonomousDriveSequence(ROBOT_FORWARD, 0.50, 2.3, true, (0.01 * LEFT_DRIVE_FORWARD_SCALAR), 0.0);
    //AutonomousDriveSequence(ROBOT_FORWARD, 0.50, 2.3, true, 0.0, (0.01 * RIGHT_DRIVE_FORWARD_SCALAR));

    // Returning from here will enter the idle state until autonomous is over
    RobotUtils::DisplayMessage("Auto routine 2 done.");
}
