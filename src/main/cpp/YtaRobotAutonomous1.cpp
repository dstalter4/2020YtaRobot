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
#include "frc/Timer.h"

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

    /*
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
    */

    RobotCamera::AutonomousCamera::Reset();

    RobotCamera::SetFullProcessing(true);
    RobotCamera::SetLimelightMode(RobotCamera::VISION_PROCESSOR);

    // This loop shouldn't be needed
    /*
    while ( !m_pDriverStation->IsAutonomous() || !m_pDriverStation->IsEnabled() )
    {
    }
    */

    // If a timer is needed, use m_pAutonomousTimer.
    // Calling new without delete is a resource leak.
    //m_pAutonomousTimer->Start();

    // Back Up Sequence
    // This needs to call AutonomousDriveSequence() instead of using a loop
    for (int i = 0; i < 50000; i++)
    {
        if (m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
        {                    
            m_pLeftDriveMotors->Set(-0.2);
            m_pRightDriveMotors->Set(0.2);
        }
        else
        {
            m_pLeftDriveMotors->Set(0);
            m_pRightDriveMotors->Set(0);
            return;
        }

        //SmartDashboard::PutNumber("Auto 1 Backward Count", i);
        //SmartDashboard::PutNumber("Time", m_pAutonomousTimer->Get());
    }
    
    m_pLeftDriveMotors->Set(0);
    m_pRightDriveMotors->Set(0);
    
    bool targetInView = false;
    bool targetLock = false;

    // Target Searching and Aiming Sequnce
    while (!targetInView)
    {
        if (m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
        {
            targetInView = RobotCamera::AutonomousCamera::TargetSearch(0.2);
            SmartDashboard::PutBoolean("Target In View", targetInView);
        }
        else
        {
            m_pLeftDriveMotors->Set(0);
            m_pRightDriveMotors->Set(0);
            return;
        }
            
    }

    // Target Searching and Aiming Sequnce
    while (!targetLock)
    {
        if (m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
        {
            //.00015 / 4.0
            targetLock = RobotCamera::AutonomousCamera::BasePControl(.01, 0.0001, .01);
            SmartDashboard::PutBoolean("Target Lock", targetLock);
        }
        else
        {
            m_pLeftDriveMotors->Set(0);
            m_pRightDriveMotors->Set(0);
            return;
        }
    }

    // Enable Firing Motor
    m_pShooterMotors->Set(-1.0);

    for (int i = 0; i < 100000; i++)
    {
        if (m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
        {  
            m_pShooterMotors->Set(-1.0);
            m_pIntakeMotor->Set(ControlMode::PercentOutput, 1);
        }
        else
        {
            m_pLeftDriveMotors->Set(0);
            m_pRightDriveMotors->Set(0);
            return;
        }
    }

    // Autonomous Sequence Completed
    m_pShooterMotors->Set(0);
    m_pLeftDriveMotors->Set(0);
    m_pRightDriveMotors->Set(0);
    m_pIntakeMotor->Set(ControlMode::PercentOutput, 0);

    /*
    // Target Search and Lock On Sequence
    while ( m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled() )
    {
        static bool ranOnce = false;

        // Move Backward for 10000 count
        if (ranOnce == false)
        {
            for (int i = 0; i < 1000000; i++)
            {
                if (m_pDriverStation->IsAutonomous() && m_pDriverStation->IsEnabled())
                {                    
                    m_pLeftDriveMotors->Set(-0.2);
                    m_pRightDriveMotors->Set(0.2);
                }
                else
                {
                    m_pLeftDriveMotors->Set(0);
                    m_pLeftDriveMotors->Set(0);
                }
            }

            ranOnce = true;
        }
        
        if (!targetInView)
        {
            targetInView = RobotCamera::AutonomousCamera::TargetSearch(0.1);
        }
        else       
        {
            targetLock = RobotCamera::AutonomousCamera::BasePControl(.015,.000115,.01);
        }
        
        SmartDashboard::PutBoolean("Target In View", targetInView);
        SmartDashboard::PutBoolean("Target Lock", targetLock);
        
        if (targetLock)
        {
            if (m_pDriverStation->IsEnabled() && m_pDriverStation->IsAutonomous())
            {
                m_pShooterMotors->Set(1);
            }
            else
            {
                m_pShooterMotors->Set(0);
            }
        }
    }
    */

    //m_pAutonomousTimer->Stop();
    //m_pAutonomousTimer->Reset();
    
    // Returning from here will enter the idle state until autonomous is over
    RobotUtils::DisplayMessage("Auto routine 1 done.");
}
