////////////////////////////////////////////////////////////////////////////////
/// @file   YtaRobotAutonomous1.cpp
/// @author David Stalter
///
/// @details
/// Implementation of autonomous routine 1 for YtaRobot.
///
/// Copyright (c) 2020 Youth Technology Academy
////////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
// <none>

// C INCLUDES
// (none)

// C++ INCLUDES
#include "RobotCamera.hpp"              // for AlignToTarget()
#include "RobotUtils.hpp"               // for DisplayMessage()
#include "YtaRobot.hpp"                 // for robot class declaration
#include "YtaRobotAutonomous.hpp"       // for autonomous declarations



////////////////////////////////////////////////////////////////
/// @method YtaRobot::AutonomousRoutine1
///
/// Autonomous routine 1.
///
////////////////////////////////////////////////////////////////
void YtaRobot::AutonomousRoutine1()
{
    //AutonomousDriveSequence(ROBOT_REVERSE, 0.25, 1.0);
    //RobotCamera::AutonomousCamera::AlignToTarget(RobotCamera::AutonomousCamera::SEEK_LEFT);

    static SendableChooser<std::string> autoSeekEnableChooser;
    const std::string AUTO_CAMERA_SEEK_ON = "Auto camera seek on";
    const std::string AUTO_CAMERA_SEEK_OFF = "Auto camera seek off";
    autoSeekEnableChooser.SetDefaultOption(AUTO_CAMERA_SEEK_ON, AUTO_CAMERA_SEEK_ON);
    autoSeekEnableChooser.AddOption(AUTO_CAMERA_SEEK_OFF, AUTO_CAMERA_SEEK_OFF);
    SmartDashboard::PutData("Auto camera seek", &autoSeekEnableChooser);
    std::string autoCameraSeekEnabled = autoSeekEnableChooser.GetSelected();

    bool bMotorSeek = false;
    if (autoCameraSeekEnabled == AUTO_CAMERA_SEEK_ON)
    {
        bMotorSeek = true;
    }

    while ( m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled() )
    {
        //RobotCamera::AutonomousCamera::AlignToTarget(RobotCamera::AutonomousCamera::SEEK_LEFT, bMotorSeek);
        //RobotCamera::AutonomousCamera::BillyTest();
        //RobotCamera::AutonomousCamera::BillyTurretControl();
        RobotCamera::AutonomousCamera::BillyTurretPControl();
    }
    
    // Returning from here will enter the idle state until autonomous is over
    RobotUtils::DisplayMessage("Auto routine 1 done.");
}
