#pragma once

#include "config.h"

void runCourse() {
  // Home to reload 1

  driveForward(130, 130, 6);
  followLineForTicks(200, 5);
  waitCrossings(CENTER_EDGE_MASK, 1, 200);
  turnLeft(200, 200, 4);
  stopMotors();
  turnRight(150, 150, 2);
  followLineForTicks(200, 5);
  followLineForTicks(250, 22);
  followLineForTicks(150, 5);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  stopMotors();
  turnLeft(220, 220, 20);
  followLineForTicks(150, 5);
  followLineForTicks(220, 20);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  followLineForTicks(80, 19); // park
  stopMotors();
  liftDown(10); gripper.write(GRIPPER_CLOSED); 
  stopMotors();
  
  // Reload to basket 2 >> carry ball
  
  delay(5000);
  liftUp(20);
  driveBackward(130, 130, 10);
  brake(150);
  turnLeft(220, 220, 38);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 9);
  waitCrossings(LEFT_EDGE_MASK, 1, 150);
  brake(50);
  turnLeft(220, 220, 18);
  brake(150);
   followLineForTicks(150, 3);
  followLineForTicks(220, 8);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 5);
  driveForward(150, 150, 4);
  followLineForTicks(150, 3);
  followLineForTicks(220, 12);
  waitCrossings(CENTER_EDGE_MASK, 1, 220);
  followLineForTicks(81, 20); // slow approach before placing the ball
  lift.write(LIFT_PLACE);
  brake(100);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  lift.write(LIFT_PLACE);
  gripper.write(GRIPPER_OPEN); delay(500);
  lift.write(LIFT_UP); delay(500);
  driveBackward(130, 130, 8);
  brake(100);
  gripper.write(GRIPPER_CLOSED); delay(500);

  // Basket 2 back to reload 1
  delay(2000);
  turnLeft(220, 220, 27);
  followLineForTicks(150, 3);
  followLineForTicks(220, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 5);
  driveForward(150, 150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  brake(100);   
  followLineForTicks(150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnRight(220, 220, 20);
  followLineForTicks(150, 6);
  followLineForTicks(220, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  followLineForTicks(82, 19);
  stopMotors();
  liftDown(10); gripper.write(GRIPPER_CLOSED); 
  stopMotors();

  // Reload to basket 3 >> carry ball

  delay(5000);
  liftUp(20);
  driveBackward(130, 130, 8);
  brake(150);
  turnLeft(220, 220, 37);
  brake(150);
  followLineForTicks(150, 7);
  followLineForTicks(220, 14);
  waitCrossings(LEFT_EDGE_MASK, 1, 150);
  brake(50);
  turnLeft(220, 220, 18);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 8);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 5);
  driveForward(170, 150, 15);
  followLineForTicks(150, 3);
  followLineForTicks(220, 3);
  waitCrossings(CENTER_EDGE_MASK, 1, 220);
  followLineForTicks(220, 5);
  followLineForTicks(150, 2);
  followLineForTicks(90, 2);
  lift.write(LIFT_PLACE); // lower the lift slightly
  brake(100);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  lift.write(LIFT_PLACE);
  gripper.write(GRIPPER_OPEN); delay(500);
  lift.write(LIFT_UP); delay(500);
  driveBackward(130, 130, 8);
  brake(100);
  gripper.write(GRIPPER_CLOSED); delay(500);

  // Basket 3 back to reload 1
  
  delay(2000);
  turnLeft(220, 220, 27);
  followLineForTicks(150, 3);
  followLineForTicks(220, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 5);
  driveForward(150, 150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  brake(100);   
  followLineForTicks(150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnRight(220, 220, 20);
  followLineForTicks(150, 6);
  followLineForTicks(250, 10);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  followLineForTicks(82, 19);
  stopMotors();
  liftDown(10); gripper.write(GRIPPER_CLOSED); 
  stopMotors();

  // Reload to basket 4 >> carry ball

  delay(5000);
  liftUp(20);
  driveBackward(130, 130, 9);
  brake(150);
  turnLeft(220, 220, 37);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 22);
  waitCrossings(LEFT_EDGE_MASK, 1, 150);
  brake(50);
  turnLeft(220, 220, 18);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 8);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 8);
  driveForward(170, 100, 9);
  followLineForTicks(150, 3);
  followLineForTicks(220, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 220);
  followLineForTicks(220, 5);
  followLineForTicks(150, 2);
  followLineForTicks(90, 2);
  lift.write(LIFT_PLACE); // lower the lift slightly
  brake(100);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  lift.write(LIFT_PLACE);
  gripper.write(GRIPPER_OPEN); delay(500);
  lift.write(LIFT_UP); delay(500);
  driveBackward(130, 130, 8);
  brake(100);
  gripper.write(GRIPPER_CLOSED); delay(500);

  // Basket 4 back to reload 1
  
  delay(2000);
  turnLeft(220, 220, 27);
  brake(100);
  followLineForTicks(150, 3);
  followLineForTicks(220, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 9);
  driveForward(150, 150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  brake(100);   
  followLineForTicks(150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnRight(220, 220, 20);
  followLineForTicks(150, 8);
  followLineForTicks(250, 16);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  followLineForTicks(82, 10); // park
  liftDown(10); gripper.write(GRIPPER_CLOSED); 
  stopMotors();

  // Reload to basket 1 >> carry ball

  delay(5000);
  liftUp(20);
  driveBackward(130, 130, 9);
  brake(150);
  turnLeft(220, 220, 37);
  brake(150);
  followLineForTicks(150, 6);
  waitCrossings(LEFT_EDGE_MASK, 1, 150);
  brake(50);
  turnLeft(220, 220, 20);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 8);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 5);
  driveForward(150, 150, 4);
  followLineForTicks(150,4 );
  followLineForTicks(220, 12);
  waitCrossings(CENTER_EDGE_MASK, 1, 220);
  followLineForTicks(81, 20); // slow approach before placing the ball
  lift.write(LIFT_PLACE);
  brake(100);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  lift.write(LIFT_PLACE);
  gripper.write(GRIPPER_OPEN); delay(500);
  lift.write(LIFT_UP); delay(500);
  driveBackward(130, 130, 8);
  brake(100);
  gripper.write(GRIPPER_CLOSED); delay(500);

  // // Basket 1 back to reload 1

  delay(2000);
  turnRight(220, 220, 27);
  followLineForTicks(150, 3);
  followLineForTicks(220, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnLeft(220, 220, 5);
  driveForward(150, 150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  brake(100);   
  followLineForTicks(150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnRight(220, 220, 20);
  followLineForTicks(150, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  followLineForTicks(82, 19);
  stopMotors();
  liftDown(10); gripper.write(GRIPPER_CLOSED); 
  stopMotors();

  // Reload to basket 5 >> carry ball

  delay(5000);
  liftUp(20);
  driveBackward(130, 130, 9);
  brake(150);
  turnLeft(220, 220, 37);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 24);
  waitCrossings(LEFT_EDGE_MASK, 1, 150);
  brake(50);
  turnLeft(220, 220, 18);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 8);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnRight(220, 220,8);
  driveForward(110, 170, 9);
  followLineForTicks(150, 3);
  followLineForTicks(220, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 220);
  followLineForTicks(220, 5);
  followLineForTicks(150, 2);
  followLineForTicks(90, 2);
  lift.write(LIFT_PLACE); // lower the lift slightly
  brake(100);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  gripper.write(GRIPPER_OPEN); delay(75); gripper.write(GRIPPER_CLOSED); delay(750);
  lift.write(LIFT_PLACE);
  gripper.write(GRIPPER_OPEN); delay(500);
  lift.write(LIFT_UP); delay(500);
  driveBackward(130, 130, 8);
  brake(100);
  gripper.write(GRIPPER_CLOSED); delay(500);

   
   // // Basket 5 back to reload 1


  delay(2000);
  turnLeft(220, 220, 27);
  brake(100);
  followLineForTicks(150, 3);
  followLineForTicks(220, 6);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnRight(220, 220, 9);
  driveForward(150, 150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  brake(100);   
  followLineForTicks(150, 4);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  turnRight(220, 220, 20);
  followLineForTicks(150, 8);
  followLineForTicks(250, 16);
  waitCrossings(CENTER_EDGE_MASK, 1, 150);
  followLineForTicks(82, 10); // park
  liftDown(10); gripper.write(GRIPPER_CLOSED); 
  stopMotors();

  // Reload to fortune ball >> carry ball

  delay(5000);
  liftUp(20);
  driveBackward(130, 130, 9);
  brake(150);
  turnLeft(220, 220, 35);
  brake(150);
  followLineForTicks(150, 3);
  followLineForTicks(220, 15);
  waitCrossings(LEFT_EDGE_MASK, 1, 150);
  brake(50);
  turnRight(220, 220, 17);
  brake(150);
  followLineForTicks(150, 4);
  followLineForTicks(220, 13);
  waitCrossings(LEFT_EDGE_MASK, 1, 150);
  brake(50);
  turnLeft(220, 220, 18);
  brake(150);
  followLineForTicks(220, 5);
  followLineForTicks(150, 2);
  followLineForTicks(81, 13); // slow approach before placing the ball
  lift.write(LIFT_PLACE);
  brake(100);
  gripper.write(GRIPPER_OPEN); delay(250); gripper.write(GRIPPER_CLOSED); delay(750);
  driveBackward(130, 130, 6);
  brake(150);
  turnLeft(220, 220, 36);
  brake(150);
  followLineForTicks(150, 5);
  followLineForTicks(220, 28);
  

  

}
