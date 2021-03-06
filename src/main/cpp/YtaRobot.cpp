////////////////////////////////////////////////////////////////////////////////
/// @file   YtaRobot.cpp
/// @author David Stalter
///
/// @details
/// Implementation of the YtaRobot class.  This file contains the functions for
/// full robot operation in FRC.  It contains the autonomous and operator
/// control routines as well as all necessary support for interacting with all
/// motors, sensors and input/outputs on the robot.
///
/// Copyright (c) 2020 Youth Technology Academy
////////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
#include <cstddef>                      // for nullptr
#include <cstring>                      // for memset

// C INCLUDES
// (none)

// C++ INCLUDES
#include "YtaRobot.hpp"                 // for class declaration (and other headers)
#include "RobotCamera.hpp"              // for interacting with cameras
#include "RobotI2c.hpp"                 // for I2cThread()
#include "RobotUtils.hpp"               // for Trim(), Limit() and DisplayMessage()

// STATIC MEMBER VARIABLES
YtaRobot * YtaRobot::m_pThis;


////////////////////////////////////////////////////////////////
/// @method YtaRobot::YtaRobot
///
/// Constructor.  Instantiates all robot control objects.
///
////////////////////////////////////////////////////////////////
YtaRobot::YtaRobot() :
    m_AutonomousChooser                 (),
    m_pDriverStation                    (&DriverStation::GetInstance()),
    m_pDriveJoystick                    (nullptr),
    m_pControlJoystick                  (nullptr),
    m_pDriveCustomController            (new YtaController(DRIVE_CUSTOM_CONTROLLER_TYPE, DRIVE_JOYSTICK_PORT, true)),
    m_pControlCustomController          (new YtaController(CONTROL_CUSTOM_CONTROLLER_TYPE, CONTROL_JOYSTICK_PORT, false)),
    m_pDriveLogitechExtreme             (new Joystick(DRIVE_JOYSTICK_PORT)),
    m_pControlLogitechExtreme           (new Joystick(CONTROL_JOYSTICK_PORT)),
    m_pDriveXboxGameSir                 (new XboxController(DRIVE_JOYSTICK_PORT)),
    m_pControlXboxGameSir               (new XboxController(CONTROL_JOYSTICK_PORT)),
    m_pLeftDriveMotors                  (new TalonMotorGroup<TalonFX>(NUMBER_OF_LEFT_DRIVE_MOTORS, LEFT_MOTORS_CAN_START_ID, MotorGroupControlMode::FOLLOW, FeedbackDevice::CTRE_MagEncoder_Relative)),
    m_pRightDriveMotors                 (new TalonMotorGroup<TalonFX>(NUMBER_OF_RIGHT_DRIVE_MOTORS, RIGHT_MOTORS_CAN_START_ID, MotorGroupControlMode::FOLLOW, FeedbackDevice::CTRE_MagEncoder_Relative)),
    m_pShooterMotors                    (new TalonMotorGroup<TalonFX>(NUMBER_OF_SHOOTER_MOTORS, SHOOTER_MOTORS_CAN_START_ID, MotorGroupControlMode::INVERSE, FeedbackDevice::None)),
    m_pWinchMotor                       (new TalonFX(WINCH_MOTOR_CAN_ID)),
    m_pIntakeMotor                      (new TalonSRX(INTAKE_MOTOR_CAN_ID)),
    m_pTurretMotor                      (new TalonSRX(TURRET_MOTOR_CAN_ID)),
    m_pColorWheelMotor                  (new TalonSRX(COLOR_WHEEL_MOTOR_CAN_ID)),
    m_pLedsEnableRelay                  (new Relay(LEDS_ENABLE_RELAY_ID)),
    m_pRedLedRelay                      (new Relay(RED_LED_RELAY_ID)),
    m_pGreenLedRelay                    (new Relay(GREEN_LED_RELAY_ID)),
    m_pBlueLedRelay                     (new Relay(BLUE_LED_RELAY_ID)),
    m_pTurretLeftHallSensor             (new DigitalInput(TURRET_LEFT_HALL_SENSOR_DIO_CHANNEL)),
    m_pTurretCenterHallSensor           (new DigitalInput(TURRET_CENTER_HALL_SENSOR_DIO_CHANNEL)),
    m_pTurretRightHallSensor            (new DigitalInput(TURRET_RIGHT_HALL_SENSOR_DIO_CHANNEL)),
    m_pDebugOutput                      (new DigitalOutput(DEBUG_OUTPUT_DIO_CHANNEL)),
    m_pIntakeSolenoid                   (new DoubleSolenoid(INTAKE_SOLENOID_FORWARD_CHANNEL, INTAKE_SOLENOID_REVERSE_CHANNEL)),
    m_pShooterSolenoid                  (new DoubleSolenoid(SHOOTER_SOLENOID_FORWARD_CHANNEL, SHOOTER_SOLENOID_REVERSE_CHANNEL)),
    m_pHangerRaiseSolenoid              (new DoubleSolenoid(HANGER_RAISE_SOLENOID_FORWARD_CHANNEL, HANGER_RAISE_SOLENOID_REVERSE_CHANNEL)),
    m_pHangerExtendSolenoid             (new DoubleSolenoid(HANGER_EXTEND_SOLENOID_FORWARD_CHANNEL, HANGER_EXTEND_SOLENOID_REVERSE_CHANNEL)),
    m_pIntakeSolenoidTrigger            (nullptr),
    m_pShooterSolenoidTrigger           (nullptr),
    m_pHangerRaiseSolenoidTrigger       (nullptr),
    m_pHangerExtendSolenoidTrigger      (nullptr),
    m_pAutonomousTimer                  (new Timer()),
    m_pInchingDriveTimer                (new Timer()),
    m_pDirectionalAlignTimer            (new Timer()),
    m_pSafetyTimer                      (new Timer()),
    m_pAccelerometer                    (new BuiltInAccelerometer),
    m_pAdxrs450Gyro                     (nullptr),
    m_Bno055Angle                       (),
    m_CameraThread                      (RobotCamera::LimelightThread),
    m_pToggleFullProcessingTrigger      (nullptr),
    m_pToggleProcessedImageTrigger      (nullptr),
    m_SerialPortBuffer                  (),
    m_pSerialPort                       (new SerialPort(SERIAL_PORT_BAUD_RATE, SerialPort::kMXP, SERIAL_PORT_NUM_DATA_BITS, SerialPort::kParity_None, SerialPort::kStopBits_One)),
    m_I2cThread                         (RobotI2c::I2cThread),
    m_pColorSensor                      (new rev::ColorSensorV3(I2C::Port::kOnboard)),
    m_pColorMatcher                     (new rev::ColorMatch()),
    m_RobotMode                         (ROBOT_MODE_NOT_SET),
    m_RobotDriveState                   (MANUAL_CONTROL),
    m_AllianceColor                     (m_pDriverStation->GetAlliance()),
    m_bDriveSwap                        (false),
    m_HeartBeat                         (0U),
    m_GameData                          ("")
{
    RobotUtils::DisplayMessage("Robot constructor.");
    
    // Set the autonomous options
    m_AutonomousChooser.SetDefaultOption(AUTO_ROUTINE_1_STRING, AUTO_ROUTINE_1_STRING);
    m_AutonomousChooser.AddOption(AUTO_ROUTINE_2_STRING, AUTO_ROUTINE_2_STRING);
    m_AutonomousChooser.AddOption(AUTO_ROUTINE_3_STRING, AUTO_ROUTINE_3_STRING);
    m_AutonomousChooser.AddOption(AUTO_TEST_ROUTINE_STRING, AUTO_TEST_ROUTINE_STRING);
    SmartDashboard::PutData("Autonomous Modes", &m_AutonomousChooser);
    
    // Set the driver input to the correct object
    switch (DRIVE_CONTROLLER_TYPE)
    {
        case CUSTOM_CONTROLLER:
        {
            switch (DRIVE_CUSTOM_CONTROLLER_TYPE)
            {
                case YtaController::LOGITECH:
                {
                    RobotUtils::DisplayMessage("Driver controller: Custom Logitech");
                    break;
                }
                case YtaController::PLAY_STATION:
                {
                    RobotUtils::DisplayMessage("Driver controller: Custom Play Station");
                    break;
                }
                default:
                {
                    ASSERT(false);
                    break;
                }
            }
            m_pDriveJoystick = m_pDriveCustomController;
            break;
        }
        case LOGITECH_EXTREME:
        {
            RobotUtils::DisplayMessage("Driver controller: Logitech Extreme");
            m_pDriveJoystick = m_pDriveLogitechExtreme;
            break;
        }
        case LOGITECH_GAMEPAD:
        case XBOX_GAMESIR:
        {
            RobotUtils::DisplayMessage("Driver controller: Xbox");
            m_pDriveJoystick = m_pDriveXboxGameSir;
            break;
        }
        default:
        {
            // Deliberately crash - fix the configuration in the header and try again
            ASSERT(false);
            break;
        }
    }
    
    // Set the controller input to the correct object
    switch (CONTROL_CONTROLLER_TYPE)
    {
        case CUSTOM_CONTROLLER:
        {
            switch (DRIVE_CUSTOM_CONTROLLER_TYPE)
            {
                case YtaController::LOGITECH:
                {
                    RobotUtils::DisplayMessage("Control controller: Custom Logitech");
                    break;
                }
                case YtaController::PLAY_STATION:
                {
                    RobotUtils::DisplayMessage("Control controller: Custom Play Station");
                    break;
                }
                default:
                {
                    ASSERT(false);
                    break;
                }
            }
            m_pControlJoystick = m_pControlCustomController;
            break;
        }
        case LOGITECH_EXTREME:
        {
            RobotUtils::DisplayMessage("Control controller: Logitech Extreme");
            m_pControlJoystick = m_pControlLogitechExtreme;
            break;
        }
        case LOGITECH_GAMEPAD:
        case XBOX_GAMESIR:
        {
            RobotUtils::DisplayMessage("Control controller: Xbox");
            m_pControlJoystick = m_pControlXboxGameSir;
            break;
        }
        default:
        {
            // Deliberately crash - fix the configuration in the header and try again
            ASSERT(false);
            break;
        }
    }

    RobotUtils::DisplayFormattedMessage("The drive forward axis is: %d\n", YtaController::GetControllerMapping(DRIVE_CUSTOM_CONTROLLER_TYPE)->AXIS_MAPPINGS.RIGHT_TRIGGER);
    RobotUtils::DisplayFormattedMessage("The drive reverse axis is: %d\n", YtaController::GetControllerMapping(DRIVE_CUSTOM_CONTROLLER_TYPE)->AXIS_MAPPINGS.LEFT_TRIGGER);
    RobotUtils::DisplayFormattedMessage("The drive left/right axis is: %d\n", YtaController::GetControllerMapping(DRIVE_CUSTOM_CONTROLLER_TYPE)->AXIS_MAPPINGS.LEFT_X_AXIS);
    
    // @todo: Figure out how to assign these sooner to a valid joystick (pass by reference?).
    // Since the triggers use a joystick object, they can't be created until the joysticks are assigned
    m_pToggleFullProcessingTrigger  = new TriggerChangeValues(m_pDriveJoystick, CAMERA_TOGGLE_FULL_PROCESSING_BUTTON);
    m_pToggleProcessedImageTrigger  = new TriggerChangeValues(m_pDriveJoystick, CAMERA_TOGGLE_PROCESSED_IMAGE_BUTTON);
    m_pIntakeSolenoidTrigger = new TriggerChangeValues(m_pControlJoystick, INTAKE_SOLENOID_CHANGE_STATE_BUTTON);
    m_pShooterSolenoidTrigger = new TriggerChangeValues(m_pControlJoystick, SHOOTER_SOLENOID_CHANGE_STATE_BUTTON);
    m_pHangerRaiseSolenoidTrigger = new TriggerChangeValues(m_pDriveJoystick, HANG_RAISE_SOLENOID_CHANGE_STATE_BUTTON);
    m_pHangerExtendSolenoidTrigger = new TriggerChangeValues(m_pDriveJoystick, HANG_EXT_SOLENOID_CHANGE_STATE_BUTTON);
    
    // Construct the ADXRS450 gyro if configured
    if (ADXRS450_GYRO_PRESENT)
    {
        m_pAdxrs450Gyro = new ADXRS450_Gyro();
    }

    // Reset the serial port and clear buffer
    m_pSerialPort->Reset();
    std::memset(&m_SerialPortBuffer, 0U, sizeof(m_SerialPortBuffer));
    
    // Spawn the vision and I2C threads
    m_CameraThread.detach();
    m_I2cThread.detach();

    // Add the target color values to the matcher
    m_pColorMatcher->AddColorMatch(BLUE_TARGET_COLOR);
    m_pColorMatcher->AddColorMatch(GREEN_TARGET_COLOR);
    m_pColorMatcher->AddColorMatch(RED_TARGET_COLOR);
    m_pColorMatcher->AddColorMatch(YELLOW_TARGET_COLOR);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::RobotInit
///
/// This method is run when initializing the robot.
///
////////////////////////////////////////////////////////////////
void YtaRobot::RobotInit()
{
    RobotUtils::DisplayMessage("RobotInit called.");
    SetStaticThisInstance();
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::RobotPeriodic
///
/// This method is run in all robot states.  It is called each
/// time a new packet is received from the driver station.
///
////////////////////////////////////////////////////////////////
void YtaRobot::RobotPeriodic()
{
    static bool bRobotPeriodicStarted = false;
    if (!bRobotPeriodicStarted)
    {
        RobotUtils::DisplayMessage("RobotPeriodic called.");
        bRobotPeriodicStarted = true;
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::InitialStateSetup
///
/// This method contains the work flow for putting motors,
/// solenoids, etc. into a known state.  It is intended to be
/// used by both autonomous and user control.
///
////////////////////////////////////////////////////////////////
void YtaRobot::InitialStateSetup()
{
    // Start with motors off
    m_pLeftDriveMotors->Set(OFF);
    m_pRightDriveMotors->Set(OFF);
    m_pShooterMotors->Set(OFF);
    m_pWinchMotor->Set(ControlMode::PercentOutput, OFF);
    m_pIntakeMotor->Set(ControlMode::PercentOutput, OFF);
    m_pTurretMotor->Set(ControlMode::PercentOutput, OFF);
    m_pColorWheelMotor->Set(ControlMode::PercentOutput, OFF);
    
    // Configure brake or coast for the drive motors
    m_pLeftDriveMotors->SetBrakeMode();
    m_pRightDriveMotors->SetBrakeMode();
    
    // Tare encoders
    m_pLeftDriveMotors->TareEncoder();
    m_pRightDriveMotors->TareEncoder();

    // Solenoids
    m_pIntakeSolenoid->Set(DoubleSolenoid::kOff);
    m_pShooterSolenoid->Set(DoubleSolenoid::kOff);
    m_pHangerRaiseSolenoid->Set(DoubleSolenoid::kOff);
    m_pHangerExtendSolenoid->Set(DoubleSolenoid::kOff);
    
    // Enable LEDs, but keep them off for now
    m_pLedsEnableRelay->Set(LEDS_ENABLED);
    m_pRedLedRelay->Set(LEDS_OFF);
    m_pGreenLedRelay->Set(LEDS_OFF);
    m_pBlueLedRelay->Set(LEDS_OFF);
    
    // Stop/clear any timers, just in case
    m_pInchingDriveTimer->Stop();
    m_pInchingDriveTimer->Reset();
    m_pDirectionalAlignTimer->Stop();
    m_pDirectionalAlignTimer->Reset();
    m_pSafetyTimer->Stop();
    m_pSafetyTimer->Reset();
    
    // Just in case constructor was called before these were set (likely the case)
    m_AllianceColor = m_pDriverStation->GetAlliance();
    
    // Clear the debug output pin
    m_pDebugOutput->Set(false);

    // Reset the heartbeat
    m_HeartBeat = 0U;
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::TeleopInit
///
/// The teleop init method.  This method is called once each
/// time the robot enters teleop control.
///
////////////////////////////////////////////////////////////////
void YtaRobot::TeleopInit()
{
    RobotUtils::DisplayMessage("TeleopInit called.");
    
    // Autonomous should have left things in a known state, but
    // just in case clear everything.
    InitialStateSetup();
    
    // Tele-op won't do detailed processing of the images unless instructed to
    RobotCamera::SetFullProcessing(false);
    RobotCamera::SetLimelightMode(RobotCamera::DRIVER_CAMERA);
    
    // Indicate to the I2C thread to get data less often
    RobotI2c::SetThreadUpdateRate(I2C_RUN_INTERVAL_MS);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::TeleopPeriodic
///
/// The teleop control method.  This method is called
/// periodically while the robot is in teleop control.
///
////////////////////////////////////////////////////////////////
void YtaRobot::TeleopPeriodic()
{
    // Log a mode change if one occurred
    CheckAndUpdateRobotMode(ROBOT_MODE_TELEOP);

    HeartBeat();

    DriveControlSequence();

    IntakeSequence();

    TurretSequence();

    ShooterSequence();

    //ColorSequence();

    //LedSequence();

    PneumaticSequence();

    HangSequence();

    //SerialPortSequence();
    
    //I2cSequence();
    
    //CameraSequence();
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::IntakeSequence
///
/// This method contains the main workflow for the intake.
///
////////////////////////////////////////////////////////////////
void YtaRobot::IntakeSequence()
{
    double intakeMotorSpeed = OFF;

    if (m_pControlJoystick->GetRawButton(INTAKE_FORWARD_BUTTON))
    {
        intakeMotorSpeed = INTAKE_MOTOR_SPEED;
    }
    else if (m_pControlJoystick->GetRawButton(INTAKE_REVERSE_BUTTON))
    {
        intakeMotorSpeed = -INTAKE_MOTOR_SPEED;
    }
    else
    {
        intakeMotorSpeed = OFF;
    }

    m_pIntakeMotor->Set(ControlMode::PercentOutput, intakeMotorSpeed);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::TurretSequence
///
/// This method contains the main workflow for the turret.
///
////////////////////////////////////////////////////////////////
void YtaRobot::TurretSequence()
{
    bool bLeftMovementAllowed = true;
    bool bRightMovementAllowed = true;

    // Hall sensors read true until the field is introduced (i.e. false = magnet present, true = magnet not present)

    if (!m_pTurretLeftHallSensor->Get())
    {
        bRightMovementAllowed = false;
    }

    if (!m_pTurretRightHallSensor->Get())
    {
        bLeftMovementAllowed = false;
    }

    double turretControlValue = m_pControlJoystick->GetRawAxis(TURRET_CONTROL_AXIS);

    if ((turretControlValue < 0.0) && bRightMovementAllowed)
    {
        m_pTurretMotor->Set(ControlMode::PercentOutput, turretControlValue * TURRET_MOTOR_SCALING_VALUE);
    }
    else if ((turretControlValue > 0.0) && bLeftMovementAllowed)
    {
        m_pTurretMotor->Set(ControlMode::PercentOutput, turretControlValue * TURRET_MOTOR_SCALING_VALUE);
    }
    else
    {
        m_pTurretMotor->Set(ControlMode::PercentOutput, OFF);
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::ShooterSequence
///
/// This method contains the main workflow for the shooter.
///
////////////////////////////////////////////////////////////////
void YtaRobot::ShooterSequence()
{
    double shooterMotorSpeed = 0.0;
    
    if (m_pControlJoystick->GetRawButton(SHOOTER_FAST_BUTTON))
    {
        // Negative motor value spins in the desired direction
        shooterMotorSpeed = -SHOOTER_FAST_MOTOR_SPEED;
    }
    else if (m_pControlJoystick->GetRawButton(SHOOTER_SLOW_BUTTON))
    {
        // Negative motor value spins in the desired direction
        shooterMotorSpeed = -SHOOTER_SLOW_MOTOR_SPEED;
    }
    else
    {
        shooterMotorSpeed = OFF;
    }

    m_pShooterMotors->Set(shooterMotorSpeed);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::HangSequence
///
/// This method contains the main workflow for hanging.
///
////////////////////////////////////////////////////////////////
void YtaRobot::HangSequence()
{
    double winchMotorSpeed = OFF;

    if (m_pDriveJoystick->GetRawButton(WINCH_FORWARD_BUTTON))
    {
        winchMotorSpeed = WINCH_MOTOR_SPEED;
    }
    else if (m_pDriveJoystick->GetRawButton(WINCH_REVERSE_BUTTON))
    {
        winchMotorSpeed = -WINCH_MOTOR_SPEED;
    }
    else
    {
        winchMotorSpeed = OFF;
    }

    m_pWinchMotor->Set(ControlMode::PercentOutput, winchMotorSpeed);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::ColorSequence
///
/// This method contains the main workflow for interacting with
/// the color sensor and associated field elements.
///
////////////////////////////////////////////////////////////////
void YtaRobot::ColorSequence()
{
    m_GameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
    
    // Sample code uses 'if m_GameData.length() > 0' too.
    frc::Color targetColor = {0.0, 0.0, 0.0};
    switch (m_GameData[0])
    {
        case GAME_DATA_BLUE:
        {
            targetColor = BLUE_TARGET_COLOR;
            break;
        }
        case GAME_DATA_GREEN:
        {
            targetColor = GREEN_TARGET_COLOR;
            break;
        }
        case GAME_DATA_RED:
        {
            targetColor = RED_TARGET_COLOR;
            break;
        }
        case GAME_DATA_YELLOW:
        {
            targetColor = YELLOW_TARGET_COLOR;
            break;
        }
        default:
        {
            break;
        }
    }

    frc::Color detectedColor = m_pColorSensor->GetColor();
    double irDistance = m_pColorSensor->GetIR();
    uint32_t proximity = m_pColorSensor->GetProximity();

    double confidence = 0.0;
    frc::Color matchedColor = m_pColorMatcher->MatchClosestColor(detectedColor, confidence);

    std::string colorString = "";
    if (matchedColor == BLUE_TARGET_COLOR)
    {
      colorString = "Blue";
    }
    else if (matchedColor == RED_TARGET_COLOR)
    {
      colorString = "Red";
    }
    else if (matchedColor == GREEN_TARGET_COLOR)
    {
      colorString = "Green";
    }
    else if (matchedColor == YELLOW_TARGET_COLOR)
    {
      colorString = "Yellow";
    }
    else
    {
      colorString = "Unknown";
    }
    
    if (RobotUtils::DEBUG_PRINTS)
    {
        SmartDashboard::PutNumber("Color sensor red", detectedColor.red);
        SmartDashboard::PutNumber("Color sensor green", detectedColor.green);
        SmartDashboard::PutNumber("Color sensor blue", detectedColor.blue);
        SmartDashboard::PutNumber("Color sensor IR distance", irDistance);
        SmartDashboard::PutNumber("Color sensor proximity", proximity);
        SmartDashboard::PutString("Color sensor detected color", colorString);
        SmartDashboard::PutNumber("Color sensor match confidence", confidence);
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::LedSequence
///
/// This method contains the main workflow for controlling
/// any LEDs on the robot.
///
////////////////////////////////////////////////////////////////
void YtaRobot::LedSequence()
{
    LedsTest();
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::PneumaticSequence
///
/// This method contains the main workflow for updating the
/// state of the pnemuatics on the robot.
///
////////////////////////////////////////////////////////////////
void YtaRobot::PneumaticSequence()
{
    static DoubleSolenoid::Value intakeSolenoidState = m_pIntakeSolenoid->Get();

    if (m_pIntakeSolenoidTrigger->DetectChange())
    {
        switch (intakeSolenoidState)
        {
            case DoubleSolenoid::kForward:
            {
                
                m_pIntakeSolenoid->Set(DoubleSolenoid::kReverse);
                intakeSolenoidState = DoubleSolenoid::kReverse;
                break;
            }
            // @todo: Remove kOff after InitialStateSetup() sets direction.
            case DoubleSolenoid::kReverse:
            case DoubleSolenoid::kOff:
            {
                m_pIntakeSolenoid->Set(DoubleSolenoid::kForward);
                intakeSolenoidState = DoubleSolenoid::kForward;
                break;
            }
            default:
            {
                break;
            }
        }
    }


    static DoubleSolenoid::Value shooterSolenoidState = m_pShooterSolenoid->Get();

    if (m_pShooterSolenoidTrigger->DetectChange())
    {
        switch (shooterSolenoidState)
        {
            case DoubleSolenoid::kForward:
            {
                
                m_pShooterSolenoid->Set(DoubleSolenoid::kReverse);
                shooterSolenoidState = DoubleSolenoid::kReverse;
                break;
            }
            // @todo: Remove kOff after InitialStateSetup() sets direction.
            case DoubleSolenoid::kReverse:
            case DoubleSolenoid::kOff:
            {
                m_pShooterSolenoid->Set(DoubleSolenoid::kForward);
                shooterSolenoidState = DoubleSolenoid::kForward;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    
    static DoubleSolenoid::Value hangRaiseSolenoidState = m_pHangerRaiseSolenoid->Get();

    if (m_pHangerRaiseSolenoidTrigger->DetectChange())
    {
        switch (hangRaiseSolenoidState)
        {
            case DoubleSolenoid::kForward:
            {
                
                m_pHangerRaiseSolenoid->Set(DoubleSolenoid::kReverse);
                hangRaiseSolenoidState = DoubleSolenoid::kReverse;
                break;
            }
            // @todo: Remove kOff after InitialStateSetup() sets direction.
            case DoubleSolenoid::kReverse:
            case DoubleSolenoid::kOff:
            {
                m_pHangerRaiseSolenoid->Set(DoubleSolenoid::kForward);
                hangRaiseSolenoidState = DoubleSolenoid::kForward;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    
    static DoubleSolenoid::Value hangExtendSolenoidState = m_pHangerExtendSolenoid->Get();

    if (m_pHangerExtendSolenoidTrigger->DetectChange())
    {
        switch (hangExtendSolenoidState)
        {
            case DoubleSolenoid::kForward:
            {
                
                m_pHangerExtendSolenoid->Set(DoubleSolenoid::kReverse);
                hangExtendSolenoidState = DoubleSolenoid::kReverse;
                break;
            }
            // @todo: Remove kOff after InitialStateSetup() sets direction.
            case DoubleSolenoid::kReverse:
            case DoubleSolenoid::kOff:
            {
                m_pHangerExtendSolenoid->Set(DoubleSolenoid::kForward);
                hangExtendSolenoidState = DoubleSolenoid::kForward;
                break;
            }
            default:
            {
                break;
            }
        }
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::SerialPortSequence
///
/// This method contains the main workflow for interaction with
/// the serial port.
///
////////////////////////////////////////////////////////////////
void YtaRobot::SerialPortSequence()
{
    /*
    // Check for any incoming transmissions, limit it to our read buffer size
    int32_t bytesReceived = m_pSerialPort->GetBytesReceived();
    bytesReceived = (bytesReceived > SERIAL_PORT_BUFFER_SIZE_BYTES) ? SERIAL_PORT_BUFFER_SIZE_BYTES : bytesReceived;

    // If we got data, read it
    if (bytesReceived > 0)
    {
        static_cast<void>(m_pSerialPort->Read(m_SerialPortBuffer, bytesReceived));

        // See if its a packet intended for us
        if (memcmp(m_SerialPortBuffer, SERIAL_PORT_PACKET_HEADER, SERIAL_PORT_PACKET_HEADER_SIZE_BYTES) == 0)
        {
            // Next character is the command.  Array indexing starts at zero, thus no +1 on the size bytes constant
            int32_t command = static_cast<int32_t>(m_SerialPortBuffer[SERIAL_PORT_PACKET_HEADER_SIZE_BYTES]) - ASCII_0_OFFSET;

            // Sanity check it
            if (command >= 0 && command <= 9)
            {
                RobotUtils::DisplayFormattedMessage("Received a valid packet, command: %d\n", command);
            }
            else
            {
                RobotUtils::DisplayFormattedMessage("Invalid command received: %d\n", command);
            }
        }

        RobotUtils::DisplayFormattedMessage(m_SerialPortBuffer);
    }
    m_SerialPortBuffer[0] = NULL_CHARACTER;
    */
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::I2cSequence
///
/// This method contains the main workflow for interaction with
/// the I2C bus.
///
////////////////////////////////////////////////////////////////
void YtaRobot::I2cSequence()
{
    static std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
    static std::chrono::time_point<std::chrono::high_resolution_clock> oldTime;
    currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = currentTime - oldTime;
    if (elapsed.count() > I2C_RUN_INTERVAL_MS)
    {
        RobotI2c::ManualTrigger();
        //static_cast<void>(GetGyroValue(BNO055));
        
        oldTime = currentTime;
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::CameraSequence
///
/// This method handles camera related behavior.  See the
/// RobotCamera class for full details.
///
////////////////////////////////////////////////////////////////
void YtaRobot::CameraSequence()
{
    static bool bFullProcessing = false;
    
    // @note: Use std::chrono is precise time control is needed.
    
    // Check for any change in camera
    if (m_pDriveJoystick->GetRawButton(SELECT_FRONT_CAMERA_BUTTON))
    {
        RobotCamera::SetCamera(RobotCamera::FRONT_USB);
    }
    else if (m_pDriveJoystick->GetRawButton(SELECT_BACK_CAMERA_BUTTON))
    {
        RobotCamera::SetCamera(RobotCamera::BACK_USB);
    }
    else
    {
    }
    
    // Look for full processing to be enabled/disabled
    if (m_pToggleFullProcessingTrigger->DetectChange())
    {
        // Change state first, because the default is set before this code runs
        bFullProcessing = !bFullProcessing;
        RobotCamera::SetFullProcessing(bFullProcessing);
    }
    
    // Look for the displayed processed image to be changed
    if (m_pToggleProcessedImageTrigger->DetectChange())
    {
        RobotCamera::ToggleCameraProcessedImage();
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::DriveControlSequence
///
/// This method contains the main workflow for drive control.
/// It will gather input from the drive joystick and then filter
/// those values to ensure they are past a certain threshold and
/// within range to send to the speed controllers.  Lastly it
/// will actually set the speed values.
///
////////////////////////////////////////////////////////////////
void YtaRobot::DriveControlSequence()
{
    if (DIRECTIONAL_ALIGN_ENABLED)
    {
        // Check for a directional align first
        DirectionalAlign();
        
        // If an align is in progress, do not accept manual driver input
        if (m_RobotDriveState == DIRECTIONAL_ALIGN)
        {
            return;
        }
    }

    if (DIRECTIONAL_INCH_ENABLED)
    {
        // If a directional inch occurred, just return
        if (DirectionalInch())
        {
            return;
        }
    }

    //CheckForDriveSwap();
    
    // Computes what the maximum drive speed could be.
    // It's a little unfortunate we have to handle throttle this way,
    // but GetThrottle is not a member of the GenericHID base class,
    // so we can't use the generic objects since the v-table layout
    // is not the same.  This means we have to manually get the throttle
    // based on the driver input type.
    double throttleControl = 0.0;
    switch (DRIVE_CONTROLLER_TYPE)
    {
        case CUSTOM_CONTROLLER:
        {
            throttleControl = GetThrottleControl(m_pDriveCustomController);
            break;
        }
        case LOGITECH_EXTREME:
        {
            throttleControl = GetThrottleControl(m_pDriveLogitechExtreme);
            break;
        }
        case LOGITECH_GAMEPAD:
        case XBOX_GAMESIR:
        {
            // Xbox controllers have no GetThrottle method, default to max
            throttleControl = 1.0;
            break;
        }
        default:
        {
            // Deliberately crash - fix the configuration in the header and try again
            ASSERT(false);
            break;
        }
    }

    // All the controllers are normalized
    // to represent the x and y axes with
    // the following values:
    //   -1
    //    |
    // -1---+1
    //    |
    //   +1
    
    // Get driver X/Y inputs
    double xAxisDrive = m_pDriveJoystick->GetX();
    double yAxisDrive = m_pDriveJoystick->GetY();

    if (RobotUtils::DEBUG_PRINTS)
    {
        SmartDashboard::PutNumber("x-axis input", xAxisDrive);
        SmartDashboard::PutNumber("y-axis input", yAxisDrive);
    }
    
    // Make sure axes inputs clear a certain threshold.  This will help to drive straight.
    xAxisDrive = RobotUtils::Trim((xAxisDrive * throttleControl), JOYSTICK_TRIM_UPPER_LIMIT, JOYSTICK_TRIM_LOWER_LIMIT);
    yAxisDrive = RobotUtils::Trim((yAxisDrive * throttleControl), JOYSTICK_TRIM_UPPER_LIMIT, JOYSTICK_TRIM_LOWER_LIMIT);

    // If the swap direction button was pressed, negate y value
    if (m_bDriveSwap)
    {
        yAxisDrive *= -1.0;
    }

    // By default, the drive equations cause the x-axis input
    // to be flipped when going backward.  Correct that here,
    // if configured.  Remember, y-axis full forward is negative.
    if ((!USE_INVERTED_REVERSE_CONTROLS) && (yAxisDrive > 0.0))
    {
        xAxisDrive *= -1.0;
    }
    
    if (SLOW_DRIVE_ENABLED)
    {
        // Get the slow drive control joystick input
        double xAxisSlowDrive = m_pDriveJoystick->GetRawAxis(DRIVE_SLOW_X_AXIS);
        xAxisSlowDrive = RobotUtils::Trim((xAxisSlowDrive * DRIVE_SLOW_THROTTLE_VALUE), JOYSTICK_TRIM_UPPER_LIMIT, JOYSTICK_TRIM_LOWER_LIMIT);
        
        // If the normal x-axis drive is non-zero, use it.  Otherwise use the slow drive input, which could also be zero.
        xAxisDrive = (xAxisDrive != 0.0) ? xAxisDrive : xAxisSlowDrive;
    }
    
    // Filter motor speeds
    double leftSpeed = RobotUtils::Limit((LeftDriveEquation(xAxisDrive, yAxisDrive)), DRIVE_MOTOR_UPPER_LIMIT, DRIVE_MOTOR_LOWER_LIMIT);
    double rightSpeed = RobotUtils::Limit(RightDriveEquation(xAxisDrive, yAxisDrive), DRIVE_MOTOR_UPPER_LIMIT, DRIVE_MOTOR_LOWER_LIMIT);
    
    // Set motor speed
    m_pLeftDriveMotors->Set(leftSpeed);
    m_pRightDriveMotors->Set(rightSpeed);

    // Retrieve motor temperatures
    double leftTemp = ConvertCelsiusToFahrenheit(m_pLeftDriveMotors->GetMotorObject()->GetTemperature());
    double rightTemp = ConvertCelsiusToFahrenheit(m_pRightDriveMotors->GetMotorObject()->GetTemperature());

    if (RobotUtils::DEBUG_PRINTS)
    {
        SmartDashboard::PutNumber("Left drive speed", leftSpeed);
        SmartDashboard::PutNumber("Right drive speed", rightSpeed);
        SmartDashboard::PutNumber("Left temperature (F)", leftTemp);
        SmartDashboard::PutNumber("Right temperature (F)", rightTemp);
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::DirectionalInch
///
/// This method contains the main workflow for drive directional
/// inching.  Based on input direction, it will briefly move the
/// robot a slight amount in that direction.
///
////////////////////////////////////////////////////////////////
bool YtaRobot::DirectionalInch()
{
    double leftSpeed = 0.0;
    double rightSpeed = 0.0;

    if (m_pDriveJoystick->GetRawButton(DRIVE_CONTROLS_INCH_FORWARD_BUTTON))
    {
        leftSpeed = INCHING_DRIVE_SPEED * LEFT_DRIVE_FORWARD_SCALAR;
        rightSpeed = INCHING_DRIVE_SPEED * RIGHT_DRIVE_FORWARD_SCALAR;
    }
    else if (m_pDriveJoystick->GetRawButton(DRIVE_CONTROLS_INCH_BACKWARD_BUTTON))
    {
        leftSpeed = INCHING_DRIVE_SPEED * LEFT_DRIVE_REVERSE_SCALAR;
        rightSpeed = INCHING_DRIVE_SPEED * RIGHT_DRIVE_REVERSE_SCALAR;
    }
    else if (m_pDriveJoystick->GetRawButton(DRIVE_CONTROLS_INCH_LEFT_BUTTON))
    {
        leftSpeed = INCHING_DRIVE_SPEED * LEFT_DRIVE_REVERSE_SCALAR;
        rightSpeed = INCHING_DRIVE_SPEED * RIGHT_DRIVE_FORWARD_SCALAR;
    }
    else if (m_pDriveJoystick->GetRawButton(DRIVE_CONTROLS_INCH_RIGHT_BUTTON))
    {
        leftSpeed = INCHING_DRIVE_SPEED * LEFT_DRIVE_FORWARD_SCALAR;
        rightSpeed = INCHING_DRIVE_SPEED * RIGHT_DRIVE_REVERSE_SCALAR;
    }
    else
    {
    }
    
    if ((leftSpeed == 0.0) && (rightSpeed == 0.0))
    {
        // No directional inch input, just return
        return false;
    }
    
    // Start the timer
    m_pInchingDriveTimer->Reset();
    m_pInchingDriveTimer->Start();
    
    // Motors on
    m_pLeftDriveMotors->Set(leftSpeed);
    m_pRightDriveMotors->Set(rightSpeed);
    
    while (m_pInchingDriveTimer->Get() < INCHING_DRIVE_DELAY_S)
    {
    }
    
    // Motors back off
    m_pLeftDriveMotors->Set(OFF);
    m_pRightDriveMotors->Set(OFF);
    
    // Stop the timer
    m_pInchingDriveTimer->Stop();
    m_pInchingDriveTimer->Reset();

    return true;
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::DirectionalAlign
///
/// This method contains the main workflow for automatically
/// aligning the robot to an angle based on input from the
/// driver.  The angles are relative to the robot at the start
/// of the match (when power is applied to the gyro and zero
/// is set).  The robot angle is reported as follows:
///
///     0
///     |
/// 270---90
///     |
///    180
///
/// The POV input is used to pick the angle to align to.  The
/// corresponding input on the d-pad maps 1:1 to the drawing.
///
////////////////////////////////////////////////////////////////
void YtaRobot::DirectionalAlign()
{
    // Retain the last POV value between function invocations
    static int lastPovValue = -1;
    
    // Indicate whether or not a change between align/no align is allowed
    static bool bStateChangeAllowed = false;
    
    // Get the current POV value
    int povValue = m_pDriveJoystick->GetPOV();
    
    // Check if it changed since last function call
    if (povValue != lastPovValue)
    {
        // Something changed, figure out what
        
        // POV button was released
        if (povValue == -1)
        {
            // State change not allowed until next button press
            bStateChangeAllowed = false;
        }
        // POV button was pressed
        else if (lastPovValue == -1)
        {
            // State change allowed since button is now pressed
            bStateChangeAllowed = true;
        }
        // There was some change in the already pressed POV value, which doesn't matter
        else
        {
        }
    }
    
    const int POV_NORMALIZATION_ANGLE = 45;
    
    // Save off a new last POV value
    lastPovValue = povValue;
    
    // This alignment uses the following from the POV input:
    //
    // ///////////////////////
    // //   315      45     //
    // //     \  up  /      //
    // // left |    | right //
    // //     / down \      //
    // //   225      135    //
    // ///////////////////////
    //
    // The input value (0 -> 360) will be normalized such that
    // angle 315 is interpreted as zero.
    static int destinationAngle = -1;
    
    switch (m_RobotDriveState)
    {
        case MANUAL_CONTROL:
        {
            // Only start an align if a state change is allowed
            if (bStateChangeAllowed)
            {                
                // This gives a value between 45 -> 405
                povValue += POV_NORMALIZATION_ANGLE;
                
                // Normalize between 0 -> 360 (maps 0:360 in to 45:360:0:45 out)
                if (povValue >= ANGLE_360_DEGREES)
                {
                    povValue -= ANGLE_360_DEGREES;
                }
                
                // Now at value between 0 -> 360, where:
                // 0 -> 89 = align up
                // 90 -> 179 = align right
                // 180 -> 269 = align down
                // 270 -> 359 = align left
                // Get a scalar multiplier to find the destination angle.
                // Making this volatile to prevent the compiler from trying
                // to optimize the division followed by multliplication of
                // the same constant.  Integer division is deliberate.
                // This gives a scalar multiplier of 0 -> 3
                volatile int degreeMultiplier = (povValue / ANGLE_90_DEGREES);
                
                // Find the destination angle.
                // This gives a value of 0, 90, 180 or 270
                destinationAngle = ANGLE_90_DEGREES * degreeMultiplier;
                
                // Read the starting angle
                RobotI2c::ManualTrigger();
                int startingAngle = static_cast<int>(GetGyroValue(BNO055));
                
                // Do some angle math to figure out which direction is faster to turn.
                // Examples:
                // Starting: 45, 180    Destination: 0, 90, 180, 270
                // 45 - 0 = 45          180 - 0 = 180
                // 45 - 90 = -45        180 - 90 = 90
                // 45 - 180 = -135      180 - 180 = 0
                // 45 - 270 = -225      180 - 270 = -90
                int angleDistance = startingAngle - destinationAngle;
                int absValueAngleDistance = std::abs(angleDistance);
                
                // Variables to indicate which way to turn
                bool bTurnLeft = false;
                bool bTurnRight = false;
                
                // Figure out which way to turn
                if (angleDistance > 0)
                {
                    // Target is to the left of where we are
                    bTurnLeft = true;
                }
                else
                {
                    // Target is to the right of where we are
                    bTurnRight = true;
                }

                // If the target distance is more than halfway around, it's actually faster to turn the other way 
                if (absValueAngleDistance > ANGLE_180_DEGREES)
                {
                    bTurnLeft = !bTurnLeft;
                    bTurnRight = !bTurnRight;
                }
                
                // The destination angle and direction is now known, time to do the move
                if (bTurnLeft)
                {
                    m_pLeftDriveMotors->Set(DIRECTIONAL_ALIGN_DRIVE_SPEED * LEFT_DRIVE_REVERSE_SCALAR);
                    m_pRightDriveMotors->Set(DIRECTIONAL_ALIGN_DRIVE_SPEED * RIGHT_DRIVE_FORWARD_SCALAR);
                }
                if (bTurnRight)
                {
                    m_pLeftDriveMotors->Set(DIRECTIONAL_ALIGN_DRIVE_SPEED * LEFT_DRIVE_FORWARD_SCALAR);
                    m_pRightDriveMotors->Set(DIRECTIONAL_ALIGN_DRIVE_SPEED * RIGHT_DRIVE_REVERSE_SCALAR);
                }
                
                // Start the safety timer
                m_pDirectionalAlignTimer->Start();

                // Indicate a state change is not allowed until POV release
                bStateChangeAllowed = false;
                
                // Indicate a directional align is in process
                m_RobotDriveState = DIRECTIONAL_ALIGN;
            }
            
            break;
        }
        case DIRECTIONAL_ALIGN:
        {   
            // Force update gyro value
            RobotI2c::ManualTrigger();
            
            // Three conditions for stopping the align:
            // 1. Destination angle is reached
            // 2. Safety timer expires
            // 3. User cancels the operation
            // @todo: Is it a problem that (destinationAngle - 1) can be negative when angle == zero?
            int currentAngle = static_cast<int>(GetGyroValue(BNO055));
            if (((currentAngle >= (destinationAngle - 1)) && (currentAngle <= (destinationAngle + 1))) ||
                (m_pDirectionalAlignTimer->Get() > DIRECTIONAL_ALIGN_MAX_TIME_S) ||
                (bStateChangeAllowed))
            {
                // Motors off
                m_pLeftDriveMotors->Set(OFF);
                m_pRightDriveMotors->Set(OFF);
                
                // Reset the safety timer
                m_pDirectionalAlignTimer->Stop();
                m_pDirectionalAlignTimer->Reset();
                
                // Clear this just to be safe
                destinationAngle = -1;
                
                // Indicate a state change is not allowed until POV release
                bStateChangeAllowed = false;
                
                // Align done, back to manual control
                m_RobotDriveState = MANUAL_CONTROL;
            }
            
            break;
        }
        default:
        {
            break;
        }
    }
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::DisabledInit
///
/// The disabled init method.  This method is called once each
/// time the robot enters disabled mode.
///
////////////////////////////////////////////////////////////////
void YtaRobot::DisabledInit()
{
    RobotUtils::DisplayMessage("DisabledInit called.");
    
    // All motors off
    m_pLeftDriveMotors->Set(OFF);
    m_pRightDriveMotors->Set(OFF);
    
    // Even though 'Disable' shuts off the relay signals, explitily turn the LEDs off
    m_pLedsEnableRelay->Set(LEDS_DISABLED);
    m_pRedLedRelay->Set(LEDS_OFF);
    m_pGreenLedRelay->Set(LEDS_OFF);
    m_pBlueLedRelay->Set(LEDS_OFF);
}



////////////////////////////////////////////////////////////////
/// @method YtaRobot::DisabledPeriodic
///
/// The disabled control method.  This method is called
/// periodically while the robot is disabled.
///
////////////////////////////////////////////////////////////////
void YtaRobot::DisabledPeriodic()
{
    // Log a mode change if one occurred
    CheckAndUpdateRobotMode(ROBOT_MODE_DISABLED);
}



////////////////////////////////////////////////////////////////
/// @method main
///
/// Execution start for the robt.
///
////////////////////////////////////////////////////////////////
#ifndef RUNNING_FRC_TESTS
int main()
{
    return frc::StartRobot<YtaRobot>();
}
#endif
