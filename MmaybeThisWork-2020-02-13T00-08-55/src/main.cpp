/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       andrewcaplan                                              */
/*    Created:      Wed Jan 22 2020                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

  // A global instance of competition
  competition Competition;
  // define your global instances of motors and other devices here

  int autonomousSelection = -1; // storage for our auton selection

  // collect data for on screen button and include
  // off and on color feedback for
  // button prc - instead of radio approach with one button on or off at a time,
  // each button has
  //          a state.  ie shootPreload may be low yellow and high yellow when on.
  typedef struct _button {
    int xpos;
    int ypos;
    int width;
    int height;
    bool state;
    vex::color offColor;
    vex::color onColor;
    const char *label;
  } button;

  // Button array definitions for each software button. The purpose of each button
  // data structure is defined above.  The array size can be extended, so you can
  // have as many buttons as you wish as long as it fits.
  button buttons[] = {
      {30, 30, 180, 60, true, 0xE00000, 0x0000E0, "Ally"},
      {270, 30, 180, 60, true, 0xE00000, 0x00FF00, "Do Auton?"},
      {30, 150, 180, 60, true, 0x404040, 0xC0C0C0, "Front"},
      {270, 150, 180, 60, false, 0x404040, 0xC0C0C0, "Back"},
      //{  150,  30, 60, 60,  false, 0x303030, 0xD0D0D0, "Start" },
      //{  390,  30, 60, 60,  false, 0x303030, 0xDDDD00, "Shoot" },
      //{  150, 150, 60, 60,  false, 0x404040, 0xC0C0C0, "5-" },
      //{  390, 150, 60, 60,  false, 0x404040, 0xC0C0C0, "7-" }
  };

  // forward ref
  void displayButtonControls(int index, bool pressed);

  /*-----------------------------------------------------------------------------*/
  /** @brief      Check if touch is inside button */
  /*-----------------------------------------------------------------------------*/
  int findButton(int16_t xpos, int16_t ypos) {
    int nButtons = sizeof(buttons) / sizeof(button);

    for (int index = 0; index < nButtons; index++) {
      button *pButton = &buttons[index];
      if (xpos < pButton->xpos || xpos > (pButton->xpos + pButton->width))
        continue;

      if (ypos < pButton->ypos || ypos > (pButton->ypos + pButton->height))
        continue;

      return (index);
    }
    return (-1);
  }

  /*-----------------------------------------------------------------------------*/
  /** @brief      Init button states */
  /*-----------------------------------------------------------------------------*/
  void initButtons() {
    int nButtons = sizeof(buttons) / sizeof(button);

    for (int index = 0; index < nButtons; index++) {
      buttons[index].state = false;
    }
  }

  /*-----------------------------------------------------------------------------*/
  /** @brief      Screen has been touched */
  /*-----------------------------------------------------------------------------*/
  void userTouchCallbackPressed() {
    int index;
    int xpos = Brain.Screen.xPosition();
    int ypos = Brain.Screen.yPosition();

    if ((index = findButton(xpos, ypos)) >= 0) {
      displayButtonControls(index, true);
    }
  }

  /*-----------------------------------------------------------------------------*/
  /** @brief      Screen has been (un)touched */
  /*-----------------------------------------------------------------------------*/
  void userTouchCallbackReleased() {
    int index;
    int xpos = Brain.Screen.xPosition();
    int ypos = Brain.Screen.yPosition();

    if ((index = findButton(xpos, ypos)) >= 0) {
      // clear all buttons to false, ie. unselected
      //      initButtons();

      // now set this one as true
      if (buttons[index].state == true) {
        buttons[index].state = false;
      } else {
        buttons[index].state = true;
      }

      if (index == 3 && buttons[index].state == true) {
        buttons[2].state = false;
      } else if (index == 3 && buttons[index].state == false) {
        buttons[2].state = true;
      }

      if (index == 2 && buttons[index].state == true) {
        buttons[3].state = false;
      } else if (index == 2 && buttons[index].state == false) {
        buttons[3].state = true;
      }

      // save as auton selection
      autonomousSelection = index;

      displayButtonControls(index, false);
    }
  }

  /*-----------------------------------------------------------------------------*/
  /** @brief      Draw all buttons */
  /*-----------------------------------------------------------------------------*/
  void displayButtonControls(int index, bool pressed) {
    vex::color c;
    Brain.Screen.setPenColor(vex::color(0xe0e0e0));

    for (int i = 0; i < sizeof(buttons) / sizeof(button); i++) {

      if (buttons[i].state)
        c = buttons[i].onColor;
      else
        c = buttons[i].offColor;

      Brain.Screen.setFillColor(c);

      // button fill
      if (i == index && pressed == true) {
        Brain.Screen.drawRectangle(buttons[i].xpos, buttons[i].ypos,
                                  buttons[i].width, buttons[i].height, c);
      } else
        Brain.Screen.drawRectangle(buttons[i].xpos, buttons[i].ypos,
                                  buttons[i].width, buttons[i].height);

      // outline
      Brain.Screen.drawRectangle(buttons[i].xpos, buttons[i].ypos,
                                buttons[i].width, buttons[i].height,
                                vex::color::transparent);

      // draw label
      if (buttons[i].label != NULL)
        Brain.Screen.printAt(buttons[i].xpos + 4,
                            buttons[i].ypos + buttons[i].height - 4,
                            buttons[i].label);
    }
  }

  void pre_auton(void) {
    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();

    // All activities that occur before the competition starts
    // Example: clearing encoders, setting servo positions, ...
  }
//

/*----------------------------------------------------------------------------------------------------*/
/*                                            Set-up Shit */
/*----------------------------------------------------------------------------------------------------*/
void unhingeArms(void) {
  double rotflap = 2.5;
  double rotdock = 3;
  // double rotdock2 = 10;
  // Flapright.spinFor(directionType::fwd, rot, rotationUnits::rev, false);
  Flapleft.spinFor(directionType::rev, rotflap, rotationUnits::rev, 250, velocityUnits::rpm, false);
  Flapright.spinFor(directionType::fwd, rotflap, rotationUnits::rev, 250, velocityUnits::rpm, false);
  while (Flapleft.isSpinning() || Flapright.isSpinning()) {
    this_thread::sleep_for(50);
  }
  //Dock.spinFor(directionType::fwd, rotdock, rotationUnits::rev, 250, velocityUnits::rpm, false);

  // this_thread::sleep_for(1000);
  //Dock.stop();
  this_thread::sleep_for(1000);
}
//

void isDoingAuton(bool yesAuton) {
  Drivetrain.setTurnVelocity(75, velocityUnits::rpm);
  Brain.Screen.clearScreen();

  if (yesAuton) {
    unhingeArms();
    Drivetrain.setDriveVelocity(120, velocityUnits::rpm);
  }

  if (!yesAuton) {
    Brain.Screen.clearScreen();
    Brain.Screen.print("NO AUTONOMOUS");
    Drivetrain.setDriveVelocity(50, velocityUnits::rpm);
    Drivetrain.driveFor(directionType::fwd, 18, distanceUnits::in);
    Drivetrain.driveFor(directionType::rev, 12, distanceUnits::in);
    Drivetrain.setDriveVelocity(100, velocityUnits::rpm);
  }
}
//

/*----------------------------------------------------------------------------------------------------*/
/*                                         Autonomous Check */
/*----------------------------------------------------------------------------------------------------*/
void autonomous(void) {
  Brain.Screen.clearScreen();
    Brain.Screen.print("NO AUTONOMOUS");
    Drivetrain.setDriveVelocity(50, velocityUnits::rpm);
    Drivetrain.driveFor(directionType::fwd, 18, distanceUnits::in);
    Drivetrain.driveFor(directionType::rev, 12, distanceUnits::in);
    Drivetrain.setDriveVelocity(250, velocityUnits::rpm);
  /*
  bool allianceBlue = buttons[0].state;
  bool doAuton = buttons[1].state;
  bool startFront = buttons[2].state;
  bool startBack = buttons[3].state;
  bool blueFront = (doAuton && allianceBlue && startFront);
  bool redFront = (doAuton && !allianceBlue && startFront);
  bool blueBack = (doAuton && allianceBlue && startBack);
  bool redBack = (doAuton && !allianceBlue && startBack);
  isDoingAuton(doAuton);
  // isDoingAuton(buttons[1].state);
  if (doAuton) {
    //-----------------------BLUE FRONT (small goal)---------BLUE FRONT (small
    //goal)---------BLUE FRONT (small goal)-----------------------
    if (blueFront) {
      Brain.Screen.print("FRONT BLUE");
      //--------------------------------------------------------------------------drive
      //forward and collect and then drive backwards
      Drivetrain.setDriveVelocity(80, velocityUnits::rpm);
      Drivetrain.driveFor(directionType::fwd, 45, distanceUnits::in, false);
      while (Drivetrain.isMoving()) {
        Flapleft.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
        Flapright.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
      }
      Drivetrain.setDriveVelocity(100, velocityUnits::rpm);
      Drivetrain.driveFor(directionType::rev, 25, distanceUnits::in, false);
      while (Drivetrain.isMoving()) {
        Flapleft.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
        Flapright.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
      }
      //--------------------------------------------------------------------------turn
      //towards goal
      Flapleft.spinFor(directionType::fwd, 0.5, timeUnits::sec, 250,
                       velocityUnits::rpm);
      Flapright.spinFor(directionType::rev, 0.5, timeUnits::sec, 250,
                        velocityUnits::rpm);
      Drivetrain.turnFor(-110, rotationUnits::deg, true);
      //--------------------------------------------------------------------------move
      //towards goal
      Drivetrain.setDriveVelocity(50, velocityUnits::rpm);
      Drivetrain.driveFor(directionType::fwd, 11.5, distanceUnits::in, true);
      //--------------------------------------------------------------------------unload
      //blocks
      Dock.spin(directionType::fwd, 50, vex::velocityUnits::rpm);
      // Leftdock.spin(directionType::fwd, 50, vex::velocityUnits::rpm);
      // Rightdock.spin(directionType::rev, 50, vex::velocityUnits::rpm);
      Flapleft.spinFor(directionType::rev, 0.5, timeUnits::sec, 25,
                       velocityUnits::rpm);
      Flapright.spinFor(directionType::fwd, 0.5, timeUnits::sec, 25,
                        velocityUnits::rpm);
      wait(1, timeUnits::sec);
      Drivetrain.driveFor(directionType::rev, 10, distanceUnits::in, false);
      while (Drivetrain.isMoving()) {
        Flapleft.spin(vex::directionType::rev, 15, vex::velocityUnits::rpm);
        Flapright.spin(vex::directionType::fwd, 15, vex::velocityUnits::rpm);
      }
    }

    if (redFront) {
      Brain.Screen.print("FRONT RED");
      Drivetrain.driveFor(directionType::fwd, 45, distanceUnits::in, false);
      while (Drivetrain.isMoving()) {
        Flapleft.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
        Flapright.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
      }
      Drivetrain.driveFor(directionType::rev, 40, distanceUnits::in, true);
      // while (Drivetrain.isMoving()) {
      //   Flapleft.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
      //   Flapright.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
      // }
      Flapleft.spinFor(directionType::fwd, 0.5, timeUnits::sec, 250,
                       velocityUnits::rpm);
      Flapright.spinFor(directionType::rev, 0.5, timeUnits::sec, 250,
                        velocityUnits::rpm);
      // Drivetrain.driveFor(directionType::fwd, 5, distanceUnits::in, false);
      Flapleft.spinFor(directionType::fwd, 2.0, timeUnits::sec, 250,
                       velocityUnits::rpm);
      Flapright.spinFor(directionType::rev, 2.0, timeUnits::sec, 250,
                        velocityUnits::rpm);
      Drivetrain.driveFor(directionType::fwd, 5, distanceUnits::in, false);
      Drivetrain.turnFor(-100, rotationUnits::deg, true);
      Drivetrain.driveFor(directionType::fwd, 5, distanceUnits::in, false);
      Dock.spin(directionType::fwd, 50, vex::velocityUnits::rpm);
      // Leftdock.spin(directionType::fwd, 50, vex::velocityUnits::rpm);
      // Rightdock.spin(directionType::rev, 75, vex::velocityUnits::rpm);
    }

    if (blueBack) {
      Brain.Screen.print("BACK BLUE");
    }

    if (redBack) {
      Brain.Screen.print("BACK RED");
    }
  }
*/
}

//
void usercontrol(void) {
  
  Drivetrain.setDriveVelocity(250, velocityUnits::rpm);

  while (1) {
    Drivetrain.setDriveVelocity(250, rpm);
    //LeftDriveSmart.setVelocity(250, rpm);
    //RightDriveSmart.setVelocity(250, rpm);
    LeftDriveSmart.setVelocity(250 * Controller1.Axis3.position(), velocityUnits::rpm);
    RightDriveSmart.setVelocity(250 * Controller1.Axis2.position(), velocityUnits::rpm);
    // Flapper                                                                                        Flapper
    if (Controller1.ButtonR2.pressing()) {
      Flapleft.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
      Flapright.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
    } else if (Controller1.ButtonR1.pressing()) {
      Flapleft.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
      Flapright.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
    } else {
      Flapleft.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
      Flapright.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
    }
    // Slow Outtake                                                                                     Slow Outtake
    if (Controller1.ButtonY.pressing()) {
      Flapleft.spin(vex::directionType::rev, 50, vex::velocityUnits::rpm);
      Flapright.spin(vex::directionType::fwd, 50, vex::velocityUnits::rpm);
    } else if (Controller1.ButtonB.pressing()) {
      Flapleft.spin(vex::directionType::rev, 20, vex::velocityUnits::rpm);
      Flapright.spin(vex::directionType::fwd, 20, vex::velocityUnits::rpm);
    }

    // Dock                                                                                              Dock
    if (Controller1.ButtonDown.pressing()) {
      //Dock1.torque(torqueUnits::Nm);
      Dock1.setMaxTorque(2.1, Nm);
      Dock2.setMaxTorque(2.1,Nm);
      Dock1.spin(vex::directionType::rev, 30, vex::velocityUnits::rpm);
      Dock2.spin(vex::directionType::fwd, 30, vex::velocityUnits::rpm);
      // Leftdock.spin(vex::directionType::rev, 30, vex::velocityUnits::rpm);
      // Rightdock.spin(vex::directionType::fwd, 30, vex::velocityUnits::rpm);
    } else if (Controller1.ButtonUp.pressing()) {
      vex::timer totalTime;
      totalTime.reset();
      double speed = (30.0 / ((totalTime + 1)));
      Dock1.spin(vex::directionType::fwd, speed, vex::velocityUnits::rpm);
      Dock2.spin(vex::directionType::rev, speed, vex::velocityUnits::rpm);

      // Leftdock.spin(vex::directionType::fwd, speed, vex::velocityUnits::rpm);
      // Rightdock.spin(vex::directionType::rev, speed,
      // vex::velocityUnits::rpm);
    } else {
      //Dock.stop(brakeType::hold);
      Dock1.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
      Dock2.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
      // Leftdock.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
      // Rightdock.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
    }

    // Arms                                                                                              Arms
    if (Controller1.ButtonL2.pressing()) {
      ArmRight.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
      ArmLeft.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
      //Dock.spin(vex::directionType::fwd, 30, vex::velocityUnits::rpm);

    } else if (Controller1.ButtonL1.pressing()) {
      // Dock.spin(vex::directionType::fwd, 50, vex::velocityUnits::rpm);
      ArmRight.spin(vex::directionType::fwd, 250, vex::velocityUnits::rpm);
      ArmLeft.spin(vex::directionType::rev, 250, vex::velocityUnits::rpm);
      Dock1.spinFor(fwd, 1000, rotationUnits::rev, 250, velocityUnits::rpm, false);
      Dock2.spinFor(fwd, 1000, rotationUnits::rev, 250, velocityUnits::rpm, false);
      //Dock.spinFor(1, sec, 250, velocityUnits::rpm);
      // Leftdock.spin(vex::directionType::fwd, 30, vex::velocityUnits::rpm);
      // Rightdock.spin(vex::directionType::rev, 30, vex::velocityUnits::rpm);
    } else {
      //ArmRight.spin(vex::directionType::fwd, 18, vex::velocityUnits::rpm);
      //ArmLeft.spin(vex::directionType::rev, 18, vex::velocityUnits::rpm);
      ArmRight.stop(brakeType::hold);
      ArmLeft.stop(brakeType::hold);
    }

    // else {
    // ArmLeft.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
    // ArmRight.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
    //}

    /*if (Controller1.ButtonDown.pressing()) {
      Leftarm.spin(vex::directionType::fwd, 500, vex::velocityUnits::rpm);
      Rightarm.spin(vex::directionType::rev, 500, vex::velocityUnits::rpm);
      } else if (Controller1.ButtonUp.pressing()) {
        Leftarm.spin(vex::directionType::rev, 500, vex::velocityUnits::rpm);
        Rightarm.spin(vex::directionType::fwd, 500, vex::velocityUnits::rpm);
      } else {
        Leftarm.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
        Rightarm.spin(vex::directionType::rev, 0, vex::velocityUnits::rpm);
    }*/

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

int main() {

  // Run the pre-autonomous function.
  pre_auton();

  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // register events for button selection
  Brain.Screen.pressed(userTouchCallbackPressed);
  Brain.Screen.released(userTouchCallbackReleased);

  // make nice background
  Brain.Screen.setFillColor(vex::color(0x404040));
  Brain.Screen.setPenColor(vex::color(0x404040));
  Brain.Screen.drawRectangle(0, 0, 480, 120);
  Brain.Screen.setFillColor(vex::color(0x808080));
  Brain.Screen.setPenColor(vex::color(0x808080));
  Brain.Screen.drawRectangle(0, 120, 480, 120);

  // initial display
  displayButtonControls(0, false);

  buttons[0].state = true;
  buttons[1].state = true;
  buttons[2].state = true;
  buttons[3].state = false;

  while (true) {
    // Allow other tasks to run
    if (!Competition.isEnabled())
      Brain.Screen.setFont(fontType::mono40);
    Brain.Screen.setFillColor(vex::color(0xFFFFFF));

    Brain.Screen.setPenColor(vex::color(0xc11f27));
    Brain.Screen.printAt(0, 135, "     12465B HAWKEN       ");

    this_thread::sleep_for(10);
  }
}
