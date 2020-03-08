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

    RobotCamera::AutonomousCamera::BillyReset();


    YtaRobot * pRobotObj = YtaRobot::GetRobotInstance();


    
    RobotCamera::SetFullProcessing(true);
    RobotCamera::SetLimelightMode(RobotCamera::VISION_PROCESSOR);

    while ( !m_pDriverStation->IsAutonomous() || !m_pDriverStation->IsEnabled() )
    {};

    // Back Up Sequence
    for(int i = 0; i < 50000; i++)
    {
        if(m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
        {                    
            pRobotObj->m_pLeftDriveMotors->Set(-0.2);
            pRobotObj->m_pRightDriveMotors->Set(0.2);
        }
        else
        {
            pRobotObj->m_pLeftDriveMotors->Set(0);
            pRobotObj->m_pRightDriveMotors->Set(0);
            return;
        }

        SmartDashboard::PutNumber("Auto 1 Backward Count",i);
    }
    
    pRobotObj->m_pLeftDriveMotors->Set(0);
    pRobotObj->m_pRightDriveMotors->Set(0);

    
    bool targetInView = false;
    bool targetLock = false;

    // Target Searching and Aiming Sequnce
    while (!targetInView)
    {
        if(m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
            return;
    }

    /*
    // Target Search and Lock On Sequence
    while ( m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled() )
    {
        
        // Move Backward for 10000 count
        if(ranOnce == false)
        {
            for(int i = 0; i < 1000000; i++)
            {
                if(m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
                {                    
                    pRobotObj->m_pLeftDriveMotors->Set(-0.2);
                    pRobotObj->m_pRightDriveMotors->Set(0.2);
                }
                else
                {
                    pRobotObj->m_pLeftDriveMotors->Set(0);
                    pRobotObj->m_pLeftDriveMotors->Set(0);
                }
            }
            ranOnce = true;
        }
        
        if(!targetInView)
            targetInView = RobotCamera::AutonomousCamera::BillyTargetSearch(0.1);
        else       
            targetLock = RobotCamera::AutonomousCamera::BillyBasePControl(.015,.000115,.01);
        
        SmartDashboard::PutBoolean("Target In View", targetInView);
        SmartDashboard::PutBoolean("Target Lock", targetLock);
        
        if(targetLock)
        {
            if(m_pDriverStation->IsEnabled() && m_pDriverStation->IsAutonomous())
                pRobotObj->m_pShooterMotors->Set(1);
            else
                pRobotObj->m_pShooterMotors->Set(0);
        }
    }*/
    
    // Returning from here will enter the idle state until autonomous is over
    RobotUtils::DisplayMessage("Auto routine 1 done.");
}
