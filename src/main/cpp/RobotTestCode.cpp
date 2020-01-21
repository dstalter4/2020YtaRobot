////////////////////////////////////////////////////////////////////////////////
/// @file   RobotTestCode.cpp
/// @author David Stalter
///
/// @details
/// Implementation of the YtaRobot test functions.  This keeps official stable
/// robot code isolated.
///
/// Copyright (c) 2020 Youth Technology Academy
////////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
// <none>

// C INCLUDES
#include "networktables/NetworkTable.h"         // for network tables
#include "networktables/NetworkTableInstance.h" // for network table instance

// C++ INCLUDES
#include "RobotUtils.hpp"       // for DisplayMessage(), DisplayFormattedMessage()
#include "YtaRobot.hpp"         // for robot class declaration


static void LimelightTest()
{
    // Reference: http://docs.limelightvision.io/en/latest/getting_started.html#basic-programming
    std::shared_ptr<NetworkTable> table = nt::NetworkTableInstance::GetDefault().GetTable("limelight");
    double targetOffsetAngle_Horizontal = table->GetNumber("tx",0.0);
    double targetOffsetAngle_Vertical = table->GetNumber("ty",0.0);
    double targetArea = table->GetNumber("ta",0.0);
    double targetSkew = table->GetNumber("ts",0.0);

    // Just to silence warnings
    (void)targetOffsetAngle_Horizontal;
    (void)targetOffsetAngle_Vertical;
    (void)targetArea;
    (void)targetSkew;
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::TestInit
///
/// The test init method.  This method is called once each time
/// the robot enters test mode.
///
////////////////////////////////////////////////////////////////
void YtaRobot::TestInit()
{
    RobotUtils::DisplayMessage("TestInit called.");
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::TestPeriodic
///
/// The test control method.  This method is called
/// periodically while the robot is in test mode.
///
////////////////////////////////////////////////////////////////
void YtaRobot::TestPeriodic()
{
    // Log a mode change if one occurred
    CheckAndUpdateRobotMode(ROBOT_MODE_TEST);

    //AutonomousTestCode();
    //TeleopTestCode();

    LimelightTest();
    return;
    
    // Example code using standard library delays and time tracking
    static std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> oldTime;
    
    currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = currentTime - oldTime;
    
    // Run for 100ms, sleep for 100ms
    const unsigned RUN_SLEEP_INTERVAL_MS = 100U;
    if (elapsed.count() > RUN_SLEEP_INTERVAL_MS)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(RUN_SLEEP_INTERVAL_MS));
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Slept for " << elapsed.count() << " ms." << std::endl;
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::AutonomousTestCode
///
/// Test code to try out for autonomous mode.
///
////////////////////////////////////////////////////////////////
void YtaRobot::AutonomousTestCode()
{
    // Motors off
    m_pLeftDriveMotors->Set(OFF);
    m_pRightDriveMotors->Set(OFF);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::TeleopTestCode
///
/// Test code to try out for operator control mode.
///
////////////////////////////////////////////////////////////////
void YtaRobot::TeleopTestCode()
{
    // Test code for reading the built in accelerometer
    double x = m_pAccelerometer->GetX();
    double y = m_pAccelerometer->GetY();
    double z = m_pAccelerometer->GetZ();
    RobotUtils::DisplayFormattedMessage("x: %f, y: %f, z: %f\n", x, y, z);

    // Sample code for testing the detect trigger change code
    TriggerChangeValues testValues(m_pControlJoystick, 10);
    if ( testValues.DetectChange() )
    {
        RobotUtils::DisplayMessage("Trigger change detected!");
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::MotorTest
///
/// Motor test code to make sure they aren't driving against
/// each other.
///
////////////////////////////////////////////////////////////////
Joystick * pDriveJoystick;
Joystick * pControlJoystick;
TalonSRX * pLeft1;
TalonSRX * pLeft2;
TalonSRX * pRight1;
TalonSRX * pRight2;
void YtaRobot::MotorTest()
{
    static bool bInitialized = false;
    if (!bInitialized)
    {
        pDriveJoystick = new Joystick(DRIVE_JOYSTICK_PORT);
        pControlJoystick = new Joystick(CONTROL_JOYSTICK_PORT);
        
        pLeft1 = new TalonSRX(LEFT_MOTORS_CAN_START_ID);
        pLeft2 = new TalonSRX(LEFT_MOTORS_CAN_START_ID + 1);
        pRight1 = new TalonSRX(RIGHT_MOTORS_CAN_START_ID);
        pRight2 = new TalonSRX(RIGHT_MOTORS_CAN_START_ID + 1);
        
        //pLeft1->SetControlMode(CANSpeedController::kPercentVbus);
        //pLeft2->SetControlMode(CANSpeedController::kPercentVbus);
        //pRight1->SetControlMode(CANSpeedController::kPercentVbus);
        //pRight2->SetControlMode(CANSpeedController::kPercentVbus);
        
        //pLeft1->ConfigNeutralMode(CANSpeedController::kNeutralMode_Coast);
        //pLeft2->ConfigNeutralMode(CANSpeedController::kNeutralMode_Coast);
        //pRight1->ConfigNeutralMode(CANSpeedController::kNeutralMode_Coast);
        //pRight2->ConfigNeutralMode(CANSpeedController::kNeutralMode_Coast);
        
        bInitialized = true;
    }
    
    while (pDriveJoystick->GetRawButton(6))
    {
        pLeft1->Set(ControlMode::PercentOutput, 1);
    }
    while (pDriveJoystick->GetRawButton(7))
    {
        pLeft1->Set(ControlMode::PercentOutput, -1);
    }
    while (pDriveJoystick->GetRawButton(8))
    {
        pLeft2->Set(ControlMode::PercentOutput, 1);
    }
    while (pDriveJoystick->GetRawButton(9))
    {
        pLeft2->Set(ControlMode::PercentOutput, -1);
    }
    while (pControlJoystick->GetRawButton(6))
    {
        pRight1->Set(ControlMode::PercentOutput, 1);
    }
    while (pControlJoystick->GetRawButton(7))
    {
        pRight1->Set(ControlMode::PercentOutput, -1);
    }
    while (pControlJoystick->GetRawButton(8))
    {
        pRight2->Set(ControlMode::PercentOutput, 1);
    }
    while (pControlJoystick->GetRawButton(9))
    {
        pRight2->Set(ControlMode::PercentOutput, -1);
    }
    
    pLeft1->Set(ControlMode::PercentOutput, 0);
    pLeft2->Set(ControlMode::PercentOutput, 0);
    pRight1->Set(ControlMode::PercentOutput, 0);
    pRight2->Set(ControlMode::PercentOutput, 0);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::TankDrive
///
/// Test code for tank drive of the robot.
///
////////////////////////////////////////////////////////////////
void YtaRobot::TankDrive()
{
    m_pLeftDriveMotors->Set(-m_pDriveJoystick->GetY());
    m_pRightDriveMotors->Set(m_pControlJoystick->GetY());
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::LedsTest
///
/// Test code to verify functionality of RGB LED strips.
///
////////////////////////////////////////////////////////////////
void YtaRobot::LedsTest()
{
    enum LedDisplayState
    {
        NONE,
        RED_ONLY,
        GREEN_ONLY,
        BLUE_ONLY,
        RED_GREEN,
        RED_BLUE,
        GREEN_BLUE,
        RED_GREEN_BLUE
    };
    static LedDisplayState displayState = NONE;
    
    static std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> oldTime;
    currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = currentTime - oldTime;
    if (elapsed.count() > 1000)
    {
        // kForward turns the LEDs off (voltage difference is zero)
        // kOff turns the LEDs on (voltage difference is +12V)
        switch (displayState)
        {
            case NONE:
            {
                m_pRedLedRelay->Set(LEDS_OFF);
                m_pGreenLedRelay->Set(LEDS_OFF);
                m_pBlueLedRelay->Set(LEDS_OFF);
                displayState = RED_ONLY;
                break;
            }
            case RED_ONLY:
            {
                m_pRedLedRelay->Set(LEDS_ON);
                m_pGreenLedRelay->Set(LEDS_OFF);
                m_pBlueLedRelay->Set(LEDS_OFF);
                displayState = GREEN_ONLY;
                break;
            }
            case GREEN_ONLY:
            {
                m_pRedLedRelay->Set(LEDS_OFF);
                m_pGreenLedRelay->Set(LEDS_ON);
                m_pBlueLedRelay->Set(LEDS_OFF);
                displayState = BLUE_ONLY;
                break;
            }
            case BLUE_ONLY:
            {
                m_pRedLedRelay->Set(LEDS_OFF);
                m_pGreenLedRelay->Set(LEDS_OFF);
                m_pBlueLedRelay->Set(LEDS_ON);
                displayState = RED_GREEN;
                break;
            }
            case RED_GREEN:
            {
                m_pRedLedRelay->Set(LEDS_ON);
                m_pGreenLedRelay->Set(LEDS_ON);
                m_pBlueLedRelay->Set(LEDS_OFF);
                displayState = RED_BLUE;
                break;
            }
            case RED_BLUE:
            {
                m_pRedLedRelay->Set(LEDS_ON);
                m_pGreenLedRelay->Set(LEDS_OFF);
                m_pBlueLedRelay->Set(LEDS_ON);
                displayState = GREEN_BLUE;
                break;
            }
            case GREEN_BLUE:
            {
                m_pRedLedRelay->Set(LEDS_OFF);
                m_pGreenLedRelay->Set(LEDS_ON);
                m_pBlueLedRelay->Set(LEDS_ON);
                displayState = RED_GREEN_BLUE;
                break;
            }
            case RED_GREEN_BLUE:
            {
                m_pRedLedRelay->Set(LEDS_ON);
                m_pGreenLedRelay->Set(LEDS_ON);
                m_pBlueLedRelay->Set(LEDS_ON);
                displayState = NONE;
                break;
            }
            default:
            {
                break;
            }
        }

        oldTime = currentTime;
    }
}
